# Run from the repo root, after orange_ivy_mikan.py.
# The Mikan Gym building is the one already on Mikan Beach (door at 46,17), not a new one on
# Mikan: put Mikan's grass back, drop Mikan's gym warp, and wire the gym to Mikan Beach.
import json, struct
def rd(p): return open(p, encoding='utf-8').read()
def wr(p, s): open(p, 'w', newline='\n', encoding='utf-8').write(s)
L = {l['id']: l for l in json.loads(rd('data/layouts/layouts.json'))['layouts'] if 'id' in l}

ml = L['LAYOUT_ORANGE_ISLANDS_MIKAN']; md = bytearray(open(ml['blockdata_filepath'], 'rb').read())
if struct.unpack_from('<H', md, ((35 + 4) * ml['width'] + 22 + 3) * 2)[0] & 0x3ff == 0x15b:
    for dy in range(5):
        for dx in range(7):
            struct.pack_into('<H', md, ((35 + dy) * ml['width'] + 22 + dx) * 2, 0x3001)  # grass, elevation 3
    open(ml['blockdata_filepath'], 'wb').write(bytes(md))

p = 'data/maps/OrangeIslands_Mikan/map.json'; m = json.loads(rd(p))
m['warp_events'] = [w for w in m['warp_events'] if w['dest_map'] != 'MAP_ORANGE_ISLANDS_MIKAN_GYM']
wr(p, json.dumps(m, indent=2) + '\n')

p = 'data/maps/OrangeIslands_Mikan_Beach/map.json'; b = json.loads(rd(p))
bl = L[b['layout']]; bd = open(bl['blockdata_filepath'], 'rb').read()
assert struct.unpack_from('<H', bd, (17 * bl['width'] + 46) * 2)[0] & 0x3ff == 0x15b, 'gym door moved?'
idx = [i for i, w in enumerate(b['warp_events']) if w['dest_map'] == 'MAP_ORANGE_ISLANDS_MIKAN_GYM']
if not idx:
    b['warp_events'].append({"x": 46, "y": 17, "elevation": 3, "dest_map": "MAP_ORANGE_ISLANDS_MIKAN_GYM", "dest_warp_id": "1"})
    idx = [len(b['warp_events']) - 1]
for w in b['warp_events']:
    if w['dest_map'] == 'MAP_ORANGE_ISLANDS_MIKAN_GYM':
        w['dest_warp_id'] = "1"  # gym warp 1 is the exit mat at (8,18)
wr(p, json.dumps(b, indent=2) + '\n')

p = 'data/maps/OrangeIslands_Mikan_Gym/map.json'; g = json.loads(rd(p))
for w in g['warp_events']:
    w['dest_map'] = 'MAP_ORANGE_ISLANDS_MIKAN_BEACH'; w['dest_warp_id'] = str(idx[0])
wr(p, json.dumps(g, indent=2) + '\n')
print('mikan gym on the beach ok')
