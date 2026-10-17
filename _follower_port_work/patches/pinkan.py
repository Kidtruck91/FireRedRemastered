#!/usr/bin/env python3
# Pinkan: a heart-shaped nature reserve (60x52, OrangeGeneral + OrangeSevii67). No Pokemon Center and
# no buildings: woods crowning both lobes with tall-grass glades, meadows down to the point.
# Reached from Route 13's west edge: a 20-row channel is opened in Route 13's rock rim and
# edge_links.py ties it to Pinkan's east edge. Pinkan Beach stays unhooked.
import json, re, shutil, struct
X = '_follower_port_work/xfer/'
def set_layout(s, lid, **kv):
    i = s.index('"id": "%s"' % lid); j = s.index('}', i); blk = s[i:j]
    for k, v in kv.items(): blk = re.sub(r'"%s": [^,\n]+' % k, '"%s": %s' % (k, json.dumps(v)), blk)
    return s[:i] + blk + s[j:]
s = open('data/layouts/layouts.json').read()
s = set_layout(s, 'LAYOUT_ORANGE_ISLANDS_PINKAN', width=60, height=52)
open('data/layouts/layouts.json', 'w', newline='\n').write(s)
shutil.copy(X + 'pinkan.bin', 'data/layouts/OrangeIslands_Pinkan/map.bin')
open('data/layouts/OrangeIslands_Pinkan/border.bin', 'wb').write(struct.pack('<4H', *[0x12b | (1 << 12)] * 4))
q = 'data/maps/OrangeIslands_Pinkan/map.json'; j = json.load(open(q))
j.update(connections=0, object_events=[], warp_events=[], bg_events=[], show_map_name=True)
j['coord_events'] = [c for c in j.get('coord_events', []) if 'SeaEdge' in c['script']]
open(q, 'w', newline='\n').write(json.dumps(j, indent=2) + '\n')
# Route 13: open the west rim, rows 30-49
q = 'data/layouts/OrangeIslands_Route_13/map.bin'; b = bytearray(open(q, 'rb').read())
for y in range(30, 50):
    for x in (0, 1): struct.pack_into('<H', b, (y * 88 + x) * 2, 0x170 | (1 << 12))
open(q, 'wb').write(b)
print('pinkan placed')
