"""Check painted route data, current outcomes, maze gates and existing seams."""
from collections import Counter,deque
from pathlib import Path
import argparse
import json
import struct
import sys

HERE=Path(__file__).resolve().parent
ROOT=HERE.parents[2]
sys.path.insert(0,str(HERE.parent))
from render_maps import tileset_path
SURF={0x10,0x11,0x12,0x13,0x15,0x1A,0x1B,0x50,0x51,0x52,0x53}
CURRENT={0x50:(1,0),0x51:(-1,0),0x52:(0,-1),0x53:(0,1)}


def flood(start,edges,removed=frozenset()):
    seen=set(start)-removed;queue=deque(seen);distance={i:0 for i in seen}
    while queue:
        i=queue.popleft()
        for n in edges.get(i,[]):
            if n not in seen and n not in removed:
                seen.add(n);distance[n]=distance[i]+1;queue.append(n)
    return seen,distance


def main():
    parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('--rom',action='store_true');args=parser.parse_args()
    rom=(ROOT/'pokefirered.gba').read_bytes() if args.rom else None
    layouts={l['id']:l for l in json.loads((ROOT/'data/layouts/layouts.json').read_text())['layouts']}
    headers={j['id']:j for p in (ROOT/'data/maps').glob('*/map.json') if (j:=json.loads(p.read_text()))}
    designs=json.loads((HERE/'designs.json').read_text());landmarks=json.loads((HERE/'landmarks.json').read_text());report={}
    frlg_beaches=json.loads((HERE/'native-beaches.json').read_text())
    hoenn_beaches={k:v for k,v in json.loads((HERE/'hoenn-motifs.json').read_text()).items() if '_beach_' in k}
    def beach_grammar(templates,ocean):
        counts=[Counter(),Counter()];vocabulary={ocean}
        for sample in templates.values():
            patch=sample['blocks'];sw,sh=len(patch[0]),len(patch)
            vocabulary.update(v&1023 for row in patch for v in row)
            def tile(x,y):return patch[y][x]&1023 if 0<=x<sw and 0<=y<sh else ocean
            for y in range(-1,sh+1):
                for x in range(-1,sw+1):
                    for axis,(dx,dy) in enumerate([(1,0),(0,1)]):counts[axis][tile(x,y),tile(x+dx,y+dy)]+=1
        return counts,vocabulary
    grammars={False:beach_grammar(frlg_beaches,0x12B),True:beach_grammar(hoenn_beaches,0x170)}
    native_shores=json.loads((HERE/'native-hoenn-shore-grammar.json').read_text())
    for axis,pairs in enumerate(native_shores['directional_pairs']):
        grammars[True][0][axis].update({tuple(map(int,key.split(','))):count for key,count in pairs.items()})
    def load(header):
        layout=layouts[header['layout']];w,h=layout['width'],layout['height'];raw=(ROOT/layout['blockdata_filepath']).read_bytes();assert len(raw)==2*w*h
        blocks=[v[0] for v in struct.iter_unpack('<H',raw)];attrs=[]
        for symbol in [layout['primary_tileset'],layout['secondary_tileset']]:attrs.extend(v[0] for v in struct.iter_unpack('<I',(tileset_path(symbol)/'metatile_attributes.bin').read_bytes()))
        return layout,blocks,attrs
    for number,d in designs.items():
        header=headers['MAP_ORANGE_ISLANDS_ROUTE_'+number];name=header['name'];layout,blocks,attrs=load(header);w,h=layout['width'],layout['height'];targets=landmarks[number]
        assert [w,h]==d['size'] and (w+15)*(h+14)<=10240,(name,'map buffer')
        if not d.get('preserve_layout'):assert layout['primary_tileset']==d['primary'] and layout['secondary_tileset']==d['secondary'],(name,'incompatible tilesets')
        assert all(v&1023<len(attrs) for v in blocks),(name,'missing metatile')
        def behavior(i):return attrs[blocks[i]&1023]&511
        def neighbors(i):
            x,y=i%w,i//w
            return [ny*w+nx for nx,ny in [(x-1,y),(x+1,y),(x,y-1),(x,y+1)] if 0<=nx<w and 0<=ny<h]
        passable={i for i,v in enumerate(blocks) if not v&0xC00}
        for i in passable:
            assert blocks[i]>>12==(1 if behavior(i) in SURF else 3),(name,'bad land/Surf elevation',i%w,i//w,hex(blocks[i]))
            assert not 0x60<=behavior(i)<=0x7F,(name,'unconfigured warp')
        edges={};reverse={i:[] for i in passable}
        for i in passable:
            if behavior(i) in CURRENT:
                dx,dy=CURRENT[behavior(i)];x,y=i%w+dx,i//w+dy
                assert 0<=x<w and 0<=y<h and y*w+x in passable,(name,'current strikes wall/edge')
                ns=[y*w+x]
            else:ns=[j for j in neighbors(i) if j in passable]
            edges[i]=ns
            for j in ns:reverse[j].append(i)
        start=d['entrance'][1]*w+d['entrance'][0];goal=d['exit'][1]*w+d['exit'][0]
        assert start in passable and goal in passable,(name,'blocked entrance/exit anchor')
        seen,distance=flood([start],edges);assert passable<=seen,(name,'unreachable water or land',[(i%w,i//w) for i in passable-seen][:8])
        can_exit,_=flood([goal],reverse);assert passable<=can_exit,(name,'terrain/current trap')
        for x,y in targets['sign_approaches']:assert y*w+x in seen and behavior(y*w+x) not in SURF,(name,'no sign approach')
        expected_sign=3 if layout['primary_tileset']=='gTileset_HoennGeneral' else 2
        for event in header['bg_events']:
            x,y=event['x'],event['y'];assert 0<=x<w and 0<=y<h and blocks[y*w+x]==0x3400|expected_sign,(name,'bad sign')
            assert any(i in seen and behavior(i) not in SURF for i in neighbors(y*w+x)),(name,'sign only readable from water')
        # Check directional shore joins against complete native motifs, keeping
        # the Hoenn and FRLG vocabularies separate. Restore only the source sand
        # hidden beneath a sign for this visual-join check.
        hoenn=layout['primary_tileset']=='gTileset_HoennGeneral'
        counts,vocabulary=grammars[hoenn];terrain=[v&1023 for v in blocks]
        for i,mid in enumerate(terrain):
            if not hoenn and mid in {2,0x98,0x99,0x9A,0xA0,0xA1,0xA2,0xA8,0xA9,0xAA}:terrain[i]=0x115
        if hoenn:
            for sample,ix,iy in d['beaches']:
                patch=hoenn_beaches[sample]['blocks']
                for event in header['bg_events']:
                    x,y=event['x']-ix,event['y']-iy
                    if 0<=y<len(patch) and 0<=x<len(patch[0]):terrain[(iy+y)*w+ix+x]=patch[y][x]&1023
        for y in range(h-1):
            for x in range(w-1):
                if number=='3' and x>=w-8:continue
                i=y*w+x
                for axis,j in enumerate([i+1,i+w]):
                    if terrain[i] in vocabulary and terrain[j] in vocabulary:
                        assert counts[axis][terrain[i],terrain[j]],(name,'unseen native shoreline join',x,y,hex(terrain[i]),hex(terrain[j]))
        # Follow EVERY forced tile, not just the authored current entry points.
        trace_ends={}
        for i in passable:
            if behavior(i) not in CURRENT:continue
            visited=set();j=i
            while behavior(j) in CURRENT:
                assert j not in visited,(name,'current loop',j%w,j//w);visited.add(j);j=edges[j][0]
            assert behavior(j) in SURF,(name,'current ends on land');trace_ends[i]=j
        runs=[]
        for run in targets.get('current_runs',[]):
            x,y=run['entry'];i=y*w+x;assert i in trace_ends,(name,'missing current entry')
            j=trace_ends[i];ex,ey=run['end'];assert abs(j%w-ex)+abs(j//w-ey)<=run['width'],(name,'current takes wrong exit',run['name'],[j%w,j//w])
            authored=next(r for r in d['current_runs'] if r['name']==run['name'])
            points=authored['points'];dx=points[1][0]-x;dy=points[1][1]-y
            last_dx=points[-1][0]-points[-2][0];last_dy=points[-1][1]-points[-2][1]
            radius=run['width']//2;offsets=range(-radius,run['width']-radius)
            expected={(ey+(offset if last_dx else 0))*w+ex+(offset if last_dy else 0) for offset in offsets}
            for offset in offsets:
                lane=(y+(offset if dx else 0))*w+x+(offset if dy else 0)
                assert trace_ends.get(lane) in expected,(name,'outer current lane misses its exit',run['name'],offset)
            runs.append({'name':run['name'],'entry':[x,y],'actual_end':[j%w,j//w]})
        # Broad calm navigation remains available, including bypasses around
        # optional current courses. Expand each obstacle by two tiles.
        safe=set()
        for y in range(2,h-2):
            for x in range(2,w-2):
                if all((j:=(y+dy)*w+x+dx) in passable and behavior(j) in SURF-CURRENT.keys() for dy in range(-2,3) for dx in range(-2,3)):safe.add(y*w+x)
        safe_edges={i:[j for j in neighbors(i) if j in safe] for i in safe}
        entry=min(safe,key=lambda i:abs(i%w-d['entrance'][0])+abs(i//w-d['entrance'][1]));exit_node=min(safe,key=lambda i:abs(i%w-d['exit'][0])+abs(i//w-d['exit'][1]))
        broad,_=flood([entry],safe_edges);assert exit_node in broad,(name,'no five-tile-wide calm crossing')
        gate_checks=[]
        if d.get('maze'):
            manhattan=abs(start%w-goal%w)+abs(start//w-goal//w)
            assert distance[goal]>manhattan*1.25,(name,'maze can be crossed directly')
            for y,lo,hi in d['maze_gates']:
                cut={y*w+x for x in range(lo,hi)};assert goal not in flood([start],edges,cut)[0],(name,'maze gate can be bypassed',y)
                gate_checks.append([y,lo,hi])
        # Catch the original failure mode: clipped primitive reef objects.
        hoenn=layout['primary_tileset']=='gTileset_HoennGeneral'
        motifs=[(0x150,0x151,0x158,0x159),(0x175,0x176,0x17D,0x17E)] if hoenn else [(0x110,0x111,0x118,0x119),(0x1DB,0x1DC,0x1E3,0x1E4)]
        for i,v in enumerate(blocks):
            for motif in motifs:
                if v&1023 not in motif:continue
                k=motif.index(v&1023);x=i%w-k%2;y=i//w-k//2
                assert 0<=x<w-1 and 0<=y<h-1,(name,'clipped reef')
                actual=tuple(blocks[(y+dy)*w+x+dx]&1023 for dy in range(2) for dx in range(2))
                assert actual==motif or hoenn and motif[0]==0x175 and actual==(0x329,0x32B,0x17D,0x17E),(name,'incomplete reef',x,y,actual)
                assert all(blocks[(y+dy)*w+x+dx]&0xC00 for dy in range(2) for dx in range(2)),(name,'unblocked reef')
        # Land rest stops must remain small; solid rock shelves are not counted
        # as traversable islands.
        land={i for i in passable if behavior(i) not in SURF};remaining=set(land);largest=0
        while remaining:
            c,_=flood([next(iter(remaining))],{i:[j for j in neighbors(i) if j in land] for i in land});remaining-=c;largest=max(largest,len(c))
        assert len(land)/len(blocks)<0.1 and largest<=200,(name,'oversized walkable island')
        for c in header['connections'] if isinstance(header['connections'],list) else []:
            other=headers[c['map']];ol,ob,oa=load(other);ow,oh=ol['width'],ol['height'];opposite={'left':'right','right':'left','up':'down','down':'up'}[c['direction']]
            assert any(r['map']==header['id'] and r['direction']==opposite and r['offset']==-c['offset'] for r in other['connections'])
            matched=0
            for t in range(h):
                ot=t-c['offset']
                if not 0<=ot<oh:continue
                i=t*w+(0 if c['direction']=='left' else w-1);j=ot*ow+(ow-1 if c['direction']=='left' else 0)
                if blocks[i]&0xC00 or ob[j]&0xC00:continue
                a,b=behavior(i),oa[ob[j]&1023]&511
                if (a in SURF)==(b in SURF):assert blocks[i]>>12==ob[j]>>12;matched+=1
            assert matched>=3,(name,'blocked existing connection')
            if rom is not None:assert (ROOT/ol['blockdata_filepath']).read_bytes() in rom,(name,'stale connected beach')
        if rom is not None:assert (ROOT/layout['blockdata_filepath']).read_bytes() in rom,(name,'stale ROM layout')
        counts=Counter('blocked' if v&0xC00 else 'surf' if attrs[v&1023]&511 in SURF else 'land' for v in blocks)
        report[number]={'size':[w,h],'flow':d['flow'],'primary':layout['primary_tileset'],'secondary':layout['secondary_tileset'],'surf_percent':round(100*counts['surf']/len(blocks),1),'walkable_land_percent':round(100*counts['land']/len(blocks),1),'largest_walkable_island':largest,'shortest_crossing_steps':distance[goal],'current_tiles_checked':len(trace_ends),'current_runs':runs,'required_maze_gates':gate_checks}
        print(f'{name}: {w}x{h}, {d["flow"]}, {distance[goal]} crossing steps; {len(trace_ends)} current tiles / {len(gate_checks)} maze gates passed')
    (HERE/'metrics.json').write_text(json.dumps(report,ensure_ascii=False,indent=2)+'\n')
    if rom is not None:print('Exact current route and connected beach binaries are present in the ROM.')


if __name__=='__main__':main()
