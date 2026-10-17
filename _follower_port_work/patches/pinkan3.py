#!/usr/bin/env python3
# Pinkan, bigger: one heart-shaped island (~100x80) split over three seamless maps, so the shape only shows when
# all three are seen together: Orange Route 15 (west lobe, 36x92) | Pinkan (cleft, central meadow, the point; 40x92)
# | Orange Route 16 (east lobe, 36x92). All OrangeGeneral + OrangeSevii67, no buildings. Surfing west off Route 13
# lands on Route 16's east coast. Needs land_routes_scaffold.py first. Run from the repo root.
import json, os, re, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from land_common import *
X = '_follower_port_work/xfer/'
full = read_bin(X + 'pinkan3.bin', 112)
s = open('data/layouts/layouts.json').read()
i = s.index('"id": "LAYOUT_ORANGE_ISLANDS_PINKAN"'); j = s.index('}', i)
blk = re.sub(r'"width": \d+', '"width": 40', s[i:j]); blk = re.sub(r'"height": \d+', '"height": 92', blk)
open('data/layouts/layouts.json', 'w', newline='\n').write(s[:i] + blk + s[j:])
write_bin('data/layouts/OrangeIslands_Route_15/map.bin', [r[:36] for r in full])
write_bin('data/layouts/OrangeIslands_Pinkan/map.bin', [r[36:76] for r in full])
write_bin('data/layouts/OrangeIslands_Route_16/map.bin', [r[76:] for r in full])
strip_link('Pinkan', 'Route_13'); strip_link('Route_13', 'Pinkan')
connect('Route_15', 'Pinkan', 'right', 0)
connect('Pinkan', 'Route_16', 'right', 0)
for m in ('Route_15', 'Route_16', 'Pinkan'):
    p = 'data/maps/OrangeIslands_%s/map.json' % m; jj = json.load(open(p))
    jj.update(music='MUS_SEVII_67', weather='WEATHER_SUNNY', show_map_name=True)
    open(p, 'w', newline='\n').write(json.dumps(jj, indent=2) + '\n')
edge_entries([('Route_16', 'right', 'Route_13', -8, (38, 57), 2), ('Route_13', 'left', 'Route_16', 8, (30, 49), 34)],
             remove=[('Pinkan', 'right', 'Route_13'), ('Route_13', 'left', 'Pinkan')])
print('pinkan3 ok; now run edge_links.py Route_16 Route_13')
