"""Check Orange Islands exterior data and reachable landmarks (no emulator needed)."""
from pathlib import Path
from collections import deque
import json, struct, sys
ROOT = Path(__file__).resolve().parents[2]
layouts = {l['id']: l for l in json.loads((ROOT/'data/layouts/layouts.json').read_text())['layouts']}
attrs = []
for p in ['primary/general', 'secondary/orange_islands']:
    b = (ROOT/'data/tilesets'/p/'metatile_attributes.bin').read_bytes()
    attrs.extend(struct.unpack('<'+'I'*(len(b)//4), b))
water = {0x10, 0x11, 0x12, 0x13, 0x15, 0x17, 0x50, 0x51, 0x52, 0x53}
checks = {
    'Valencia': ((20,39), [(20,0),(0,19),(9,9),(11,20),(8,28),(26,17),(28,27)], False),
    'Route_1': ((15,0), [(19,34),(8,19)], False),
    'Route_2': ((49,19), [(8,9),(15,34),(35,9),(25,9)], False),
    'Mandarin_South': ((30,49), [(30,0),(0,24),(59,25),(12,11),(40,10),(32,33)], False),
    'Trovita': ((22,39), [(20,0),(0,22),(28,12),(8,13),(31,25)], False),
    'Kumquat': ((24,39), [(47,20),(24,12),(8,13),(8,37)], False),
    'Pinkan': ((39,21), [(10,22),(18,10),(26,28)], False),
    'Pummelo': ((20,39), [(20,23),(20,15),(13,33)], False),
    'Route_9': ((20,49), [(20,0),(10,25),(29,37),(25,21)], True),
    'Route_10': ((0,17), [(59,17),(18,11),(40,27)], True),
    'Route_11': ((20,49), [(20,0),(22,27)], True),
    'Route_12': ((0,15), [(69,15),(22,7),(48,24)], True),
    'Route_13': ((20,55), [(20,0),(0,29),(29,18),(30,44)], True),
    'Route_14': ((0,16), [(28,35),(23,10),(48,20)], True),
    'Navel': ((25,59), [(25,13),(12,40),(37,29),(49,45),(25,0)], False),
    'Moro': ((18,29), [(19,13),(27,22),(6,12),(35,14)], False),
    'Kinnow': ((0,15), [(11,8),(25,9),(30,27),(18,0)], False),
    'Route_5': ((18,49), [(18,0),(8,27),(29,15)], True),
    'Route_7': ((18,0), [(30,59),(10,19),(27,34),(12,48)], True),
    'Route_8': ((0,15), [(59,15),(29,16),(43,24)], True),
    'Mikan': ((20,35), [(0,20),(27,12),(11,10),(30,27)], False),
    'Sunburst': ((43,16), [(32,0),(8,10),(27,10),(26,27)], False),
    'Route_4': ((0,13), [(69,14),(19,10),(39,20),(52,10)], True),
    'Route_6': ((21,35), [(49,20),(20,12),(31,18),(8,9),(41,29)], True),
    'Route_3': ((20,59), [(20,0),(11,19),(29,35),(10,46)], True),
}
data = {}
for suffix, (start, goals, surf) in checks.items():
    name = 'OrangeIslands_'+suffix
    header = json.loads((ROOT/'data/maps'/name/'map.json').read_text())
    l = layouts[header['layout']]; w,h = l['width'],l['height']
    assert (w+15)*(h+14) <= 10240, name+' exceeds engine map buffer'
    b = (ROOT/l['blockdata_filepath']).read_bytes()
    assert len(b) == 2*w*h, name+' has wrong block data length'
    blocks = struct.unpack('<'+'H'*(len(b)//2), b)
    assert all((v&1023)<len(attrs) for v in blocks), name+' has missing metatiles'
    assert all(v&0xc00 or not 0x60<=attrs[v&1023]&511<=0x7f for v in blocks), name+': unconfigured walkable warp tile'
    def walk(x,y):
        if not (0<=x<w and 0<=y<h): return False
        v=blocks[y*w+x]
        return not(v&0xc00) and (surf or attrs[v&1023]&511 not in water)
    seen={start}; q=deque([start]); assert walk(*start)
    while q:
        x,y=q.popleft()
        for point in [(x-1,y),(x+1,y),(x,y-1),(x,y+1)]:
            if point not in seen and walk(*point):seen.add(point);q.append(point)
    for x,y in goals:assert (x,y) in seen, f'{name}: unreachable landmark {x,y}'
    for event in header['bg_events']:
        x,y=event['x'],event['y']
        assert any(p in seen for p in [(x-1,y),(x+1,y),(x,y-1),(x,y+1)]), name+': inaccessible sign'
    # Forced movement must terminate in calm Surf water, never a wall or a loop.
    directions={0x50:(1,0),0x51:(-1,0),0x52:(0,-1),0x53:(0,1)}
    for index,v in enumerate(blocks):
        if attrs[v&1023]&511 not in directions:continue
        x,y=index%w,index//w;visited=set()
        while attrs[blocks[y*w+x]&1023]&511 in directions:
            assert (x,y) not in visited, name+': current loop'
            visited.add((x,y));dx,dy=directions[attrs[blocks[y*w+x]&1023]&511]
            x,y=x+dx,y+dy
            assert walk(x,y), name+': current hits a wall or map edge'
        assert attrs[blocks[y*w+x]&1023]&511 in water, name+': current ends on land'
    data[name]=(header,l,blocks)
    print(f'{name}: dimensions, metatiles, {len(goals)} landmarks and {len(header["bg_events"])} signs passed')
for name,(header,l,blocks) in data.items():
    # Same-side connections must not compete for the same coordinate range.
    ranges=[]
    for connection in header['connections']:
        destination=next(n for n in data if data[n][0]['id']==connection['map'])
        dl=data[destination][1]
        limit=l['width'] if connection['direction'] in ['up','down'] else l['height']
        size=dl['width'] if connection['direction'] in ['up','down'] else dl['height']
        span=(max(0,connection['offset']),min(limit,connection['offset']+size))
        for direction,other_span in ranges:
            if direction==connection['direction']:
                assert max(span[0],other_span[0])>=min(span[1],other_span[1]), name+': overlapping same-side connections'
        ranges.append((connection['direction'],span))
    for c in header['connections']:
        other=next(n for n in data if data[n][0]['id']==c['map'])
        oh,ol,ob=data[other]
        opposite={'up':'down','down':'up','left':'right','right':'left'}[c['direction']]
        assert any(o['map']==header['id'] and o['direction']==opposite and o['offset']==-c['offset'] for o in oh['connections'])
        matched=0
        for t in range(l['width'] if c['direction'] in ['up','down'] else l['height']):
            ot=t-c['offset']
            if c['direction'] in ['up','down']:
                if not 0<=ot<ol['width']:continue
                v=blocks[(0 if c['direction']=='up' else l['height']-1)*l['width']+t]
                ov=ob[(ol['height']-1 if c['direction']=='up' else 0)*ol['width']+ot]
            else:
                if not 0<=ot<ol['height']:continue
                v=blocks[t*l['width']+(0 if c['direction']=='left' else l['width']-1)]
                ov=ob[ot*ol['width']+(ol['width']-1 if c['direction']=='left' else 0)]
            if not(v&0xc00 or ov&0xc00) and attrs[v&1023]&511 not in water and attrs[ov&1023]&511 not in water:
                assert v==ov, f'{name}: mismatched walkable seam at {t}'
                matched+=1
        assert matched>=3, name+': connection has no usable land crossing'
print('All directional connections have reciprocal offsets and matching walkable seams.')

if "--rom" in sys.argv:
    rom = (ROOT/"pokefirered.gba").read_bytes()
    for name, (_, layout, _) in data.items():
        expected = (ROOT/layout["blockdata_filepath"]).read_bytes()
        assert expected in rom, name+": ROM contains stale or missing layout data"
    print(f"ROM contains the exact binary layouts for all {len(data)} designed maps.")
