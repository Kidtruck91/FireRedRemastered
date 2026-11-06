"""Check Orange Islands exterior data and reachable landmarks (no emulator needed)."""
from pathlib import Path
from collections import deque
import json, struct, sys, traceback
ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(Path(__file__).resolve().parent))
from render_maps import tileset_path
layouts = {l['id']: l for l in json.loads((ROOT/'data/layouts/layouts.json').read_text())['layouts']}
attrs = []
for p in ['primary/orange_general', 'secondary/orange_islands']:
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
    'Mikan': ((11,47), [(15,12),(16,22),(15,36),(6,13),(6,26),(16,27),(6,40)], False),
    'Mikan_Beach': ((20,43), [(0,20),(20,0),(51,0),(32,21),(29,35)], False),
    'Sunburst': ((43,16), [(32,0),(8,10),(27,10),(26,27)], False),
    'Route_4': ((0,13), [(69,14),(19,10),(39,20),(52,10)], True),
    'Route_6': ((21,35), [(49,20),(20,12),(31,18),(8,9),(41,29)], True),
    'Route_3': ((20,59), [(20,0),(11,19),(29,35),(10,46)], True),
}
landmarks_path = ROOT/'docs/orange-islands/island_landmarks.json'
if landmarks_path.exists():
    checks.update({name:(tuple(value[0]), [tuple(g) for g in value[1]], value[2]) for name,value in json.loads(landmarks_path.read_text()).items()})
data = {}
problems = []
for suffix, (start, goals, surf) in checks.items():
    name = 'OrangeIslands_'+suffix
    try:
        header = json.loads((ROOT/'data/maps'/name/'map.json').read_text())
        l = layouts[header['layout']]; w,h = l['width'],l['height']
        # Resolve whatever tilesets the layout actually names, so new or condensed
        # tilesets do not need a table here.
        secondary = l['secondary_tileset']
        pb=(tileset_path(l['primary_tileset'])/'metatile_attributes.bin').read_bytes()
        sb=(tileset_path(secondary)/'metatile_attributes.bin').read_bytes()
        attrs=list(struct.unpack('<'+'I'*((len(pb)+len(sb))//4),pb+sb))
        assert (w+15)*(h+14) <= 10240, name+' exceeds engine map buffer'
        b = (ROOT/l['blockdata_filepath']).read_bytes()
        assert len(b) == 2*w*h, name+' has wrong block data length'
        blocks = struct.unpack('<'+'H'*(len(b)//2), b)
        assert all((v&1023)<len(attrs) for v in blocks), name+' has missing metatiles'
        if secondary!='gTileset_OrangeIslands':
            for c in (header['connections'] or []):
                neighbor=next(p for p in (ROOT/'data/maps').glob('*/map.json') if json.loads(p.read_text())['id']==c['map'])
                nh=json.loads(neighbor.read_text());nl=layouts[nh['layout']]
                if nl['secondary_tileset']==l['secondary_tileset']:continue
                vertical=c['direction'] in ('up','down')
                lo=max(0,c['offset']);hi=min(w if vertical else h,c['offset']+(nl['width'] if vertical else nl['height']))
                cells=((x,y) for y in range(h) for x in range(w) if lo<=(x if vertical else y)<hi and (
                    (c['direction']=='up' and y<7) or (c['direction']=='down' and y>=h-7) or
                    (c['direction']=='left' and x<7) or (c['direction']=='right' and x>=w-7)))
                assert all(blocks[y*w+x]&1023<736 for x,y in cells), name+': imported metatile visible across a different secondary tileset'
        if secondary.startswith('orange_sevii_'):
            assert all(v&0xc00 or attrs[v&1023]&511 in water or v>>12==3 for v in blocks), name+': dry authored path has an abrupt elevation change'
        # A warp-behaviour tile fires when the player steps onto it, so it is
        # meant to be walkable: 142 of vanilla's 147 are. What makes one a trap
        # is being walkable with no warp event on it -- the player steps in and
        # nothing happens -- so the warp events are what this has to be read
        # against, and naming the cell saves hunting for it.
        doors={(e['x'],e['y']) for e in header['warp_events']}
        stray=[(i%w,i//w) for i,v in enumerate(blocks)
               if not v&0xc00 and 0x60<=attrs[v&1023]&511<=0x7f and (i%w,i//w) not in doors]
        assert not stray, '%s: walkable warp tile with no warp event on it at %s%s'%(
            name, stray[0], '' if len(stray)==1 else ' (and %d more)'%(len(stray)-1))
        def walk(x,y):
            if not (0<=x<w and 0<=y<h): return False
            v=blocks[y*w+x]
            return not(v&0xc00) and (surf or attrs[v&1023]&511 not in water)
        assert walk(*start), ('%s: flood-fill start %s is no longer walkable; update the '
                              'landmark fixture for the current terrain'%(name,start))
        seen={start}; q=deque([start])
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
        data[name]=(header,l,blocks,attrs)
        print(f'{name}: dimensions, metatiles, {len(goals)} landmarks and {len(header["bg_events"])} signs passed')
    except AssertionError as problem:
        problems.append(str(problem) or '%s: failed check at line %d: %s'
                        %(name, traceback.extract_tb(sys.exc_info()[2])[-1].lineno,
                          (traceback.extract_tb(sys.exc_info()[2])[-1].line or '').strip()[:90]))
    except Exception as problem:
        problems.append('%s: %s: %s' % (name, type(problem).__name__, problem))

group=json.loads((ROOT/'data/maps/map_groups.json').read_text())['gMapGroup_OrangeIslands']
# Coverage is about which maps this checker has landmarks for, not which ones
# happened to pass; a map that failed above is already reported on its own.
attempted={'OrangeIslands_'+suffix for suffix in checks}
uncovered=sorted(set(group)-attempted)
if uncovered:
    problems.append('no landmark coverage for %d of %d exterior map(s): %s'
                    %(len(uncovered),len(group),' '.join(uncovered)))
covered_ids=set()
for suffix in checks:
    p=ROOT/'data/maps'/('OrangeIslands_'+suffix)/'map.json'
    if p.exists(): covered_ids.add(json.loads(p.read_text())['id'])
for name,(header,l,blocks,attrs) in data.items():
  try:
    # Same-side connections must not compete for the same coordinate range.
    # A neighbour that failed its own checks is not in `data`, and reaching
    # past it used to raise a bare StopIteration that named neither map. Say
    # which seam went unchecked and why, and carry on with the rest.
    # Only a map this checker holds landmarks for can be said to have failed;
    # a dive map or one with no fixture is reported elsewhere, not here.
    skipped=set()
    def neighbour(id):
        for n in data:
            if data[n][0]['id']==id: return n
        if id in covered_ids: skipped.add(id)
    ranges=[]
    for connection in (header['connections'] or []):
        destination=neighbour(connection['map'])
        if destination is None: continue
        dl=data[destination][1]
        limit=l['width'] if connection['direction'] in ['up','down'] else l['height']
        size=dl['width'] if connection['direction'] in ['up','down'] else dl['height']
        span=(max(0,connection['offset']),min(limit,connection['offset']+size))
        for direction,other_span in ranges:
            if direction==connection['direction']:
                assert max(span[0],other_span[0])>=min(span[1],other_span[1]), name+': overlapping same-side connections'
        ranges.append((connection['direction'],span))
    for c in (header['connections'] or []):
        other=neighbour(c['map'])
        if other is None: continue
        oh,ol,ob,other_attrs=data[other]
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
            if not(v&0xc00 or ov&0xc00) and attrs[v&1023]&511 not in water and other_attrs[ov&1023]&511 not in water:
                assert v==ov, f'{name}: mismatched walkable seam at {t}'
                matched+=1
        assert matched>=3, name+': connection has no usable land crossing'
    assert not skipped, '%s: seam unchecked, %s failed its own checks first'%(
        name, ' and '.join(sorted(i.replace('MAP_ORANGE_ISLANDS_','') for i in skipped)))
  except AssertionError as problem:
    problems.append(str(problem) or '%s: connection check failed at line %d: %s'
                    %(name, traceback.extract_tb(sys.exc_info()[2])[-1].lineno,
                      (traceback.extract_tb(sys.exc_info()[2])[-1].line or '').strip()[:90]))
  except Exception as problem:
    problems.append('%s: %s: %s'%(name,type(problem).__name__,problem))
if problems:
    print('\n%d problem(s):'%len(problems))
    for problem in problems: print('  '+problem)
    sys.exit(1)
print('All directional connections have reciprocal offsets and matching walkable seams.')

if "--rom" in sys.argv:
    rom = (ROOT/"pokefirered.gba").read_bytes()
    for name, (_, layout, _, _) in data.items():
        expected = (ROOT/layout["blockdata_filepath"]).read_bytes()
        assert expected in rom, name+": ROM contains stale or missing layout data"
    print(f"ROM contains the exact binary layouts for all {len(data)} designed maps.")
