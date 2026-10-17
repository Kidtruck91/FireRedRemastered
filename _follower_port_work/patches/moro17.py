#!/usr/bin/env python3
# Moro + Orange Route 17: Moro's east beach now runs by land into Route 17 (40x56, OrangeGeneral + OrangeSevii45),
# a misty wooded trail ending at an eastern shore. Seamless left/right connection; the sea link to Route 9
# moves from Moro's east edge to Route 17's. Moro outside x 50-63 / rows 14-41 is untouched.
# Needs land_routes_scaffold.py first. Run from the repo root.
import os, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from land_common import *
X = '_follower_port_work/xfer/'
full = read_bin(X + 'moro17.bin', 104)
write_bin('data/layouts/OrangeIslands_Moro/map.bin', [r[:64] for r in full])
write_bin('data/layouts/OrangeIslands_Route_17/map.bin', [r[64:] for r in full])
strip_link('Moro', 'Route_9'); strip_link('Route_9', 'Moro')
connect('Moro', 'Route_17', 'right', 0)
p = 'data/maps/OrangeIslands_Route_17/map.json'; j = json.load(open(p))
j.update(weather='WEATHER_FOG_HORIZONTAL', music='MUS_LAVENDER')
open(p, 'w', newline='\n').write(json.dumps(j, indent=2) + '\n')
edge_entries([('Route_17', 'right', 'Route_9', 30, (4, 49), 2), ('Route_9', 'left', 'Route_17', -30, (34, 79), 38)],
             remove=[('Moro', 'right', 'Route_9'), ('Route_9', 'left', 'Moro')])
print('moro17 ok; now run edge_links.py Route_17 Route_9')
