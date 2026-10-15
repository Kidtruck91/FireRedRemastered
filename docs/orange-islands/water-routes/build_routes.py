"""Build distinct Orange sea routes from authored placements and native motifs.

Route 3 is retained for its existing connected beach seams. Other layouts use
complete FRLG/Hoenn motifs with explicit ID mappings, never swapped raw IDs.
"""
from pathlib import Path
import json
import struct
import sys
from PIL import Image, ImageDraw

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[2]
sys.path.insert(0,str(HERE.parent))
from render_maps import render,tileset_path

DESIGNS = json.loads((HERE/'designs.json').read_text())
FRLG = json.loads((HERE/'native-beaches.json').read_text())
HOENN = json.loads((HERE/'hoenn-motifs.json').read_text())
SURF = {0x10,0x11,0x12,0x13,0x15,0x1A,0x1B,0x50,0x51,0x52,0x53}
CURRENTS = {(1,0):0x2E4,(-1,0):0x2E2,(0,-1):0x2DB,(0,1):0x2EB}


def extended_patch(name,extra_x=0,extra_y=0):
    patch=[row[:] for row in FRLG[name]['blocks']]
    if extra_x:
        candidates=[x for x in range(len(patch[0])) if all(row[x]&1023 in {0x10D,0x115,0x11D,0x12B} for row in patch)]
        assert candidates,(name,'no straight column')
        x=min(candidates,key=lambda x:abs(x-len(patch[0])/2))
        patch=[row[:x]+[row[x]]*extra_x+row[x:] for row in patch]
    if extra_y:
        candidates=[y for y,row in enumerate(patch) if all(v&1023 in {0x114,0x115,0x116,0x12B} for v in row)]
        assert candidates,(name,'no straight row')
        y=min(candidates,key=lambda y:abs(y-len(patch)/2))
        patch=patch[:y]+[patch[y][:] for _ in range(extra_y)]+patch[y:]
    return patch


def build():
    path=ROOT/'data/layouts/layouts.json';table=json.loads(path.read_text())
    layouts={l['id']:l for l in table['layouts']};landmarks={}
    old_landmarks=json.loads((HERE/'landmarks.json').read_text())
    for number,d in DESIGNS.items():
        name='OrangeIslands_Route_'+number;hp=ROOT/'data/maps'/name/'map.json';header=json.loads(hp.read_text());layout=layouts[header['layout']]
        if d.get('preserve_layout'):
            landmarks[number]={**old_landmarks[number],'entrance':d['entrance'],'exit':d['exit'],'flow':d['flow']}
            continue
        w,h=d['size'];assert (w+15)*(h+14)<=10240
        hoenn=d['primary']=='gTileset_HoennGeneral';water=0x1170 if hoenn else 0x112B
        attrs=[]
        for symbol in [d['primary'],d['secondary']]:attrs.extend(v[0] for v in struct.iter_unpack('<I',(tileset_path(symbol)/'metatile_attributes.bin').read_bytes()))
        grid=[[water]*w for _ in range(h)];islands=[]
        blue=((0x550,0x551),(0x558,0x559)) if hoenn else ((0x510,0x511),(0x518,0x519))
        brown=((0x575,0x576),(0x57D,0x57E)) if hoenn else ((0x5DB,0x5DC),(0x5E3,0x5E4))
        def behavior(value):return attrs[value&1023]&511
        def stamp(x,y,patch):
            assert 0<=x and 0<=y and x+len(patch[0])<=w and y+len(patch)<=h,(name,'clipped motif',x,y)
            for dy,row in enumerate(patch):
                for dx,value in enumerate(row):grid[y+dy][x+dx]=value
        def rock(x,y,brown_rock=False,force=False):
            x,y=round(x),round(y)
            if not (0<=x<w-1 and 0<=y<h-1):return
            if not force and any(grid[y+dy][x+dx]!=water for dy in range(2) for dx in range(2)):return
            stamp(x,y,brown if brown_rock else blue)
        def chain(points,salt):
            index=0
            for (x0,y0),(x1,y1) in zip(points,points[1:]):
                steps=max(1,round(max(abs(x1-x0),abs(y1-y0))/2.5))
                for i in range(steps):
                    t=i/steps;rock(x0+(x1-x0)*t,y0+(y1-y0)*t,(index+salt)%3==0);index+=1
            rock(*points[-1],brown_rock=(index+salt)%3==0)
        # Larger complete rock shelves define bays and forks. Their source
        # graphics and converted IDs are recorded together in hoenn-motifs.json.
        for motif,x,y in d.get('motifs',[]):
            t=HOENN[motif];patch=t['blocks']
            assert hoenn
            assert t.get('primary_only') or t['secondary']==d['secondary'],(name,'incompatible motif',motif)
            stamp(x,y,patch)
        for feature in d.get('beaches',[]):
            motif,x,y,*extra=feature
            if hoenn:
                t=HOENN[motif];assert all(v&1023<512 for row in t['blocks'] for v in row)
                patch=[[((v&0xFFF)|(0x1000 if behavior(v) in SURF else 0x3000)) for v in row] for row in t['blocks']]
            else:patch=extended_patch(motif,*extra)
            stamp(x,y,patch);islands.append((x,y,patch))
        for island_index in d.get('rock_islets',[]):
            ix,iy,patch=islands[island_index]
            candidates=[(x,y) for y in range(1,len(patch)-3) for x in range(1,len(patch[0])-3)
                        if all(patch[y+dy][x+dx]&1023==0x115 for dy in range(3) for dx in range(3))]
            assert candidates,(name,'no outcrop base');x,y=candidates[0]
            outcrop=((0x3498,0x3499,0x349A),(0x34A0,0x34A1,0x34A2),(0x34A8,0x34A9,0x34AA))
            stamp(ix+x,iy+y,outcrop)
            for dy,row in enumerate(outcrop):
                for dx,value in enumerate(row):patch[y+dy][x+dx]=value
        for i,points in enumerate(d.get('reefs',[])):chain(points,i+int(number))
        if d.get('maze'):
            # Only this map is a closed reef puzzle. Alternating broad gates
            # force a switchback; dead-end fingers can always be backed out of.
            for x in range(0,w,2):rock(x,0,force=True);rock(x,h-2,force=True)
            for y in range(2,h-2,2):rock(0,y,force=True);rock(w-2,y,force=True)
            for wall_y,lo,hi in d['maze_gates']:
                for x in range(2,w-2,2):
                    if lo<=x<hi:continue
                    rock(x,wall_y,brown_rock=(x//2)%4==0,force=True)
                    rock(x,wall_y+2,brown_rock=(x//2)%4==0,force=True)
                # Broad native cliff shelves break up each boulder barrier.
                # Even widths replace whole reef objects without clipping them.
                for x0,x1 in [(2,lo),(hi,w-2)]:
                    for x in range(x0,x1-5,16):
                        width=min(12,x1-x)
                        width-=width%2
                        if width<6:continue
                        slab=[[0x729]+[0x72A]*(width-2)+[0x72B],
                              [0x57A]+[0x471]*(width-2)+[0x57C],
                              [0x57A]+[0x471]*(width-2)+[0x57C],
                              [0x582]+[0x479]*(width-2)+[0x584]]
                        stamp(x,wall_y,slab)
            for x,y0,y1 in [(38,24,32),(42,44,52),(58,64,70)]:
                for y in range(y0,y1,2):rock(x,y,brown_rock=True,force=True)
        run_checks=[]
        for run in d.get('current_runs',[]):
            assert d['secondary']=='gTileset_HoennPacifidlog'
            radius=run['width']//2;offsets=range(-radius,run['width']-radius);points=run['points']
            directions=[]
            def current_cell(x,y,mid):
                assert 0<=x<w and 0<=y<h and not grid[y][x]&0xC00 and behavior(grid[y][x]) in SURF,(name,'current overlaps land',run['name'],x,y)
                grid[y][x]=0x1000|mid
            for (x0,y0),(x1,y1) in zip(points,points[1:]):
                assert (x0==x1)!=(y0==y1)
                dx=(x1>x0)-(x1<x0);dy=(y1>y0)-(y1<y0);directions.append((dx,dy));mid=CURRENTS[dx,dy]
                for step in range(abs(x1-x0)+abs(y1-y0)+1):
                    for offset in offsets:current_cell(x0+dx*step+(offset if dy else 0),y0+dy*step+(offset if dx else 0),mid)
            # Fill the whole elbow with the outgoing direction so outside lanes
            # make the turn too. Merely crossing two ribbons leaves broken lanes.
            for (x,y),(dx,dy) in zip(points[1:-1],directions[1:]):
                for oy in offsets:
                    for ox in offsets:current_cell(x+ox,y+oy,CURRENTS[dx,dy])
            x,y=points[-1];dx,dy=directions[-1]
            for offset in offsets:grid[y+(offset if dx else 0)][x+(offset if dy else 0)]=water
            run_checks.append({'name':run['name'],'entry':points[0],'end':points[-1],'width':run['width']})
        sign_goals=[]
        for event,(ix,iy,patch) in zip(header['bg_events'],islands):
            candidates=[]
            for y in range(1,len(patch)-1):
                for x in range(1,len(patch[0])-1):
                    if grid[iy+y][ix+x]&0xC00 or behavior(grid[iy+y][ix+x]) not in {0x21,0x17}:continue
                    approaches=[(ix+nx,iy+ny) for nx,ny in [(x,y+1),(x-1,y),(x+1,y),(x,y-1)]
                                if not grid[iy+ny][ix+nx]&0xC00 and behavior(grid[iy+ny][ix+nx]) not in SURF]
                    if approaches:candidates.append((x,y,approaches[0]))
            assert candidates,(name,'no dry sign approach')
            x,y,approach=min(candidates,key=lambda t:abs(t[0]-len(patch[0])/2)+abs(t[1]-len(patch)/2))
            grid[iy+y][ix+x]=0x3403 if hoenn else 0x3402
            event.update(x=ix+x,y=iy+y,elevation=3);sign_goals.append(list(approach))
        assert len(sign_goals)==len(header['bg_events'])
        layout.update(width=w,height=h,primary_tileset=d['primary'],secondary_tileset=d['secondary'])
        (ROOT/layout['blockdata_filepath']).write_bytes(struct.pack('<'+'H'*(w*h),*(v for row in grid for v in row)))
        (ROOT/layout['border_filepath']).write_bytes(struct.pack('<4H',*sum(blue,())))
        hp.write_text(json.dumps(header,indent=2)+'\n')
        texts=[]
        for i,event in enumerate(header['bg_events']):
            lines=['ROUTE '+number+' - SEA PASSAGE']
            if number=='5':lines+=['Watch which way the water flows.','The other lane returns south.']
            elif number=='13':lines+=['Ride the turning current west.','The outer hook returns safely.']
            elif number=='11':lines+=['REEF MAZE','Find the gaps between the walls.']
            else:lines+=['Follow the open water channels.','Small beaches offer a rest stop.']
            body='\n'.join('\t.string '+json.dumps(line+('\\n' if k<len(lines)-1 else '$')).replace('\\\\n','\\n') for k,line in enumerate(lines))
            texts.append(event['script'].replace('_EventScript_','_Text_')+'::\n'+body+'\n')
        (hp.parent/'text.inc').write_text('\n'.join(texts))
        render(layout).save(HERE.parent/(name+'.png'))
        landmarks[number]={'size':[w,h],'sign_approaches':sign_goals,'entrance':d['entrance'],'exit':d['exit'],'flow':d['flow'],'current_runs':run_checks}
    path.write_text(json.dumps(table,indent=2)+'\n');(HERE/'landmarks.json').write_text(json.dumps(landmarks,indent=2)+'\n')
    from add_boundaries import build as seal_boundaries
    seal_boundaries()
    for number in DESIGNS:
        render(layouts['LAYOUT_ORANGE_ISLANDS_ROUTE_'+number]).save(HERE.parent/f'OrangeIslands_Route_{number}.png')
    overview()
    print('Built eleven varied routes; Route 3 and all connection records retained.')


def overview():
    out=Image.new('RGB',(1800,2280),(29,38,43));draw=ImageDraw.Draw(out)
    for i,(number,d) in enumerate(DESIGNS.items()):
        im=Image.open(HERE.parent/f'OrangeIslands_Route_{number}.png');im.thumbnail((870,320),Image.Resampling.NEAREST)
        x,y=i%2*900,i//2*380
        draw.text((x+10,y+6),f'Route {number}: {d["title"]}',fill='white')
        # Default Pillow font omits arrows; retain readable direction labels.
        flow=d['flow'].replace('→','to')
        draw.text((x+10,y+21),f'{d["size"][0]} x {d["size"][1]} / {flow} / '+d.get('secondary','OrangeIslands').replace('gTileset_',''),fill=(190,210,215))
        px,py=x+10+(870-im.width)//2,y+48
        out.paste(im,(px,py))
        for label,key,color in [('A','entrance',(100,240,150)),('B','exit',(255,215,80))]:
            tx,ty=d[key]
            cx=px+round((tx+.5)/d['size'][0]*im.width)
            cy=py+round((ty+.5)/d['size'][1]*im.height)
            draw.ellipse((cx-7,cy-7,cx+7,cy+7),fill=color,outline='black')
            draw.text((cx-3,cy-5),label,fill='black')
    out.save(HERE/'overview.png')


if __name__=='__main__':build()
