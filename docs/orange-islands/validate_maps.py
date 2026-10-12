"""Check Orange Islands exterior data and reachable landmarks (no emulator needed)."""
from pathlib import Path
from collections import deque
import json, struct
ROOT = Path(__file__).resolve().parents[2]
layouts = {l['id']: l for l in json.loads((ROOT/'data/layouts/layouts.json').read_text())['layouts']}
attrs = []
for p in ['primary/general', 'secondary/orange_islands']:
    b = (ROOT/'data/tilesets'/p/'metatile_attributes.bin').read_bytes()
    attrs.extend(struct.unpack('<'+'I'*(len(b)//4), b))
water = {0x10, 0x11, 0x12, 0x13, 0x15, 0x17}
checks = {
    'Valencia': ((20,39), [(20,0),(0,19),(9,9),(11,20),(8,28),(26,17),(28,27)], False),
    'Route_1': ((15,0), [(19,34),(8,19)], False),
    'Route_2': ((49,19), [(8,9),(15,34),(35,9),(25,9)], False),
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
    data[name]=(header,l,blocks)
    print(f'{name}: dimensions, metatiles, {len(goals)} landmarks and {len(header["bg_events"])} signs passed')
for name,(header,l,blocks) in data.items():
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
print('All 6 directional connections have reciprocal offsets and matching walkable seams.')
