"""Validate the current sea-route pass, without stale town landmark assumptions."""
from collections import Counter, deque
from pathlib import Path
import argparse
import json
import struct
import sys

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[2]
sys.path.insert(0, str(HERE.parent))
from render_maps import tileset_path

# Match sBehaviorSurfable in src/metatile_behavior.c. Shallow water (0x17)
# is a walkable beach fringe, not Surf water.
SURF = {0x10,0x11,0x12,0x13,0x15,0x1A,0x1B,0x50,0x51,0x52,0x53}
CURRENT = {0x50:(1,0),0x51:(-1,0),0x52:(0,-1),0x53:(0,1)}
BLUE = (0x110,0x111,0x118,0x119)
BROWN = (0x1DB,0x1DC,0x1E3,0x1E4)
SAND = {0x10C,0x10D,0x10E,0x114,0x115,0x116,0x11C,0x11D,0x11E,0x125,0x126,0x12D,0x12E}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--rom', action='store_true')
    args = parser.parse_args()
    layouts = {l['id']:l for l in json.loads((ROOT/'data/layouts/layouts.json').read_text())['layouts']}
    headers = {j['id']:j for p in (ROOT/'data/maps').glob('*/map.json') if (j:=json.loads(p.read_text()))}
    goals = json.loads((HERE/'landmarks.json').read_text())
    rom = (ROOT/'pokefirered.gba').read_bytes() if args.rom else None
    patterns = json.loads((HERE/'native-beaches.json').read_text())
    adjacency = [Counter(),Counter()]
    for template in patterns.values():
        patch = template['blocks'];w,h=len(patch[0]),len(patch)
        def mid(x,y):
            return patch[y][x]&1023 if 0<=x<w and 0<=y<h else 0x12B
        for y in range(-1,h+1):
            for x in range(-1,w+1):
                for axis,(dx,dy) in enumerate([(1,0),(0,1)]):
                    adjacency[axis][mid(x,y),mid(x+dx,y+dy)] += 1

    def load(header):
        l=layouts[header['layout']];w,h=l['width'],l['height']
        raw=(ROOT/l['blockdata_filepath']).read_bytes()
        assert len(raw)==w*h*2, (header['name'],'binary size')
        blocks=[v[0] for v in struct.iter_unpack('<H',raw)]
        attrs=[]
        for symbol in [l['primary_tileset'],l['secondary_tileset']]:
            attrs.extend(v[0] for v in struct.iter_unpack('<I',(tileset_path(symbol)/'metatile_attributes.bin').read_bytes()))
        return l,blocks,attrs

    report={}
    for number, targets in goals.items():
        name='OrangeIslands_Route_'+number
        header=headers['MAP_ORANGE_ISLANDS_ROUTE_'+number]
        layout,blocks,attrs=load(header);w,h=layout['width'],layout['height']
        assert (w+15)*(h+14)<=10240, (name,'map buffer overflow')
        assert all(v&1023<len(attrs) for v in blocks), (name,'missing metatile')
        def behavior(i):return attrs[blocks[i]&1023]&511
        def neighbors(i):
            x,y=i%w,i//w
            return [ny*w+nx for nx,ny in [(x-1,y),(x+1,y),(x,y-1),(x,y+1)] if 0<=nx<w and 0<=ny<h]
        def passable(i):return not blocks[i]&0xC00
        for i,v in enumerate(blocks):
            if not passable(i):continue
            assert v>>12 == (1 if behavior(i) in SURF else 3), (name,'incorrect water/land elevation',i%w,i//w)
            assert not 0x60<=behavior(i)<=0x7F, (name,'unconfigured warp')
        start=next(i for i in range(len(blocks)) if passable(i) and behavior(i) in SURF)
        seen={start};queue=deque([start])
        while queue:
            i=queue.popleft()
            if behavior(i) in CURRENT:
                dx,dy=CURRENT[behavior(i)];next_cells=[i+dy*w+dx]
            else:next_cells=neighbors(i)
            for n in next_cells:
                if n not in seen and 0<=n<len(blocks) and passable(n):seen.add(n);queue.append(n)
        assert all(not passable(i) or i in seen for i in range(len(blocks))), (name,'isolated water or beach')
        for x,y in targets['sign_approaches']:
            assert y*w+x in seen and behavior(y*w+x) not in SURF, (name,'unreachable dry sign approach')
        for event in header['bg_events']:
            x,y=event['x'],event['y'];assert 0<=x<w and 0<=y<h, (name,'event out of bounds')
            assert blocks[y*w+x]==0x3402, (name,'missing blocked sign')
            assert event['elevation']==3
            assert any(n in seen and behavior(n) not in SURF for n in neighbors(y*w+x))
        # Each reef is a whole native 2x2 object, with all four pieces blocked.
        for i,v in enumerate(blocks):
            for motif in [BLUE,BROWN]:
                if v&1023 not in motif:continue
                position=motif.index(v&1023);x=i%w-position%2;y=i//w-position//2
                assert 0<=x<w-1 and 0<=y<h-1, (name,'clipped reef')
                cells=[blocks[(y+dy)*w+x+dx] for dy in range(2) for dx in range(2)]
                assert tuple(z&1023 for z in cells)==motif and all(z&0xC00 for z in cells), (name,'incomplete reef',x,y)
        # Learn local beach joins from the native samples. Signs and outcrops
        # replace interior sand; the connected Mikan landing is checked below.
        def terrain(i):
            m=blocks[i]&1023
            return 0x115 if m in {2,0x98,0x99,0x9A,0xA0,0xA1,0xA2,0xA8,0xA9,0xAA} else m
        for y in range(h-1):
            for x in range(w-1):
                if number=='3' and x>=w-8:continue
                i=y*w+x;m=terrain(i)
                for axis,j in enumerate([i+1,i+w]):
                    n=terrain(j)
                    if m in SAND|{0x12B} and n in SAND|{0x12B}:
                        assert adjacency[axis][m,n], (name,'unseen beach join',x,y,hex(m),hex(n),axis)
        # Forced movement must terminate in calm water, without walls or loops.
        for i in range(len(blocks)):
            if behavior(i) not in CURRENT:continue
            visited=set();n=i
            while behavior(n) in CURRENT:
                assert n not in visited, (name,'current loop');visited.add(n)
                dx,dy=CURRENT[behavior(n)];x,y=n%w+dx,n//w+dy
                assert 0<=x<w and 0<=y<h and passable(y*w+x), (name,'current strikes wall or edge')
                n=y*w+x
            assert behavior(n) in SURF, (name,'current ends on land')
        # A five-tile-wide clear Surf crossing must survive obstacle expansion.
        safe=set()
        for y in range(2,h-2):
            for x in range(2,w-2):
                if all(passable((y+dy)*w+x+dx) and behavior((y+dy)*w+x+dx) in SURF-CURRENT.keys()
                       for dy in range(-2,3) for dx in range(-2,3)):
                    safe.add(y*w+x)
        vertical=h>w
        entry={i for i in safe if (i//w if vertical else i%w)<=8}
        exit_cells={i for i in safe if (i//w if vertical else i%w)>=(h if vertical else w)-11}
        if number=='3':entry={i for i in safe if i%w==8 and 24<=i//w<=31}
        reached=set(entry);queue=deque(entry)
        while queue:
            for j in neighbors(queue.popleft()):
                if j in safe and j not in reached:reached.add(j);queue.append(j)
        assert entry and exit_cells and reached&exit_cells, (name,'no broad calm-water crossing')
        # Connections can be zero (Porymap's native empty representation).
        for c in header['connections'] if isinstance(header['connections'],list) else []:
            other=headers[c['map']];ol,ob,oa=load(other);ow,oh=ol['width'],ol['height']
            if rom is not None:
                assert (ROOT/ol['blockdata_filepath']).read_bytes() in rom, (other['name'],'stale connected layout in ROM')
            if other['name']=='OrangeIslands_Mikan_Beach':
                assert all(v&0xC00 or oa[v&1023]&511 not in SURF or v>>12==1 for v in ob), 'Mikan Beach Surf elevations'
            opposite={'left':'right','right':'left','up':'down','down':'up'}[c['direction']]
            assert any(r['map']==header['id'] and r['direction']==opposite and r['offset']==-c['offset'] for r in other['connections'])
            matches=0;wet=0;dry=0
            for t in range(h):
                ot=t-c['offset']
                if not 0<=ot<oh:continue
                i=t*w+(0 if c['direction']=='left' else w-1)
                j=ot*ow+(ow-1 if c['direction']=='left' else 0)
                if blocks[i]&0xC00 or ob[j]&0xC00:continue
                a,b=behavior(i),oa[ob[j]&1023]&511
                if a in SURF and b in SURF:
                    assert blocks[i]>>12==ob[j]>>12==1, (name,'Surf seam elevation mismatch')
                    wet+=1
                elif a not in SURF and b not in SURF:
                    assert blocks[i]>>12==ob[j]>>12==3, (name,'dry seam elevation mismatch')
                    dry+=1
                else:continue
                matches+=1
            assert matches>=3, (name,'no usable seam')
            print(f'{name} -> {other["name"]}: {wet} Surf / {dry} dry matching edge cells')
        counts=Counter('blocked' if v&0xC00 else 'surf' if attrs[v&1023]&511 in SURF else 'land' for v in blocks)
        assert counts['land']/len(blocks)<0.1, (name,'too much land')
        if rom is not None:assert (ROOT/layout['blockdata_filepath']).read_bytes() in rom, (name,'stale ROM layout')
        report[number]={'size':[w,h],'surf_percent':round(100*counts['surf']/len(blocks),1),'walkable_land_percent':round(100*counts['land']/len(blocks),1),'reachable_cells':len(seen),'signs':len(header['bg_events'])}
        print(f'{name}: {w}x{h}, {report[number]["surf_percent"]}% Surf, {report[number]["walkable_land_percent"]}% land; terrain, navigation, signs, currents passed')
    (HERE/'metrics.json').write_text(json.dumps(report,indent=2)+'\n')
    if args.rom:print('ROM contains the exact current twelve sea layouts and their connected beaches.')


if __name__=='__main__':main()
