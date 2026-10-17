#!/usr/bin/env python3
# Mandarin South + Orange Route 18: a wooded headland (96x40, General + CeladonCity) running north from Mandarin's
# north tip. Only Mandarin's sea/sand above the top row of buildings changes (rows 0-9, x 24-71); the city is
# untouched. Seamless up/down connection; the sea link to Route 13 moves from Mandarin's top edge to Route 18's.
# Needs land_routes_scaffold.py first. Run from the repo root.
import json, os, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from land_common import *
X = '_follower_port_work/xfer/'
r18 = read_bin(X + 'route18.bin', 96); top = read_bin(X + 'mandarin_top.bin', 96)
man = read_bin('data/layouts/OrangeIslands_Mandarin_South/map.bin', 96)
for y in range(10):
    for x in range(24, 72): man[y][x] = top[y][x]
write_bin('data/layouts/OrangeIslands_Mandarin_South/map.bin', man)
write_bin('data/layouts/OrangeIslands_Route_18/map.bin', r18)
strip_link('Mandarin_South', 'Route_13'); strip_link('Route_13', 'Mandarin_South')
connect('Mandarin_South', 'Route_18', 'up', 0)
edge_entries([('Route_18', 'top', 'Route_13', -4, (24, 71), 77), ('Route_13', 'bottom', 'Route_18', 4, (20, 67), 1)],
             remove=[('Mandarin_South', 'top', 'Route_13'), ('Route_13', 'bottom', 'Mandarin_South')])
print('mandarin18 ok; now run edge_links.py Route_18 Route_13')
