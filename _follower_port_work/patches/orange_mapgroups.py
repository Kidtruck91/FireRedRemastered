#!/usr/bin/env python3
# Split the flat gMapGroup_OrangeIslands into outdoor + per-island interior groups.
#
# The outdoor maps (islands, beaches, routes) stay in gMapGroup_OrangeIslands so its
# group index does not move. Each island's interiors go into gMapGroup_Orange_<Island>_Interior,
# appended after the existing groups so no existing group index shifts either -- only the
# Orange interior maps change their MAP_ constant, and those are referenced symbolically.
#
# Route caves, when they exist, go in gMapGroup_Orange_Route_<N>_Interior on the same pattern.
# Empty groups break the build, so a route group is only created once it has a map.
#
# include/constants/map_groups.h and data/maps/groups.inc are generated from map_groups.json
# by tools/mapjson at build time, so this edits the json only.
#
# Run from the repo root.
import json, collections

PATH = 'data/maps/map_groups.json'

# island -> interior maps, in the order the player meets them
INTERIORS = collections.OrderedDict([
    ('Valencia', ['OrangeIslands_Valencia_IvysLab']),
    ('Mikan',    ['OrangeIslands_Mikan_Gym']),
    ('Navel',    ['OrangeIslands_Navel_Gym']),
    ('Trovita',  ['OrangeIslands_Trovita_Gym']),
    ('Kumquat',  ['OrangeIslands_Kumquat_Gym']),
    ('Pummelo',  ['OrangeIslands_Pummelo_Stadium', 'OrangeIslands_Pummelo_Cave']),
    ('Sunburst', ['OrangeIslands_Sunburst_Cave']),
    ('Moro',     ['OrangeIslands_Moro_GhostShip']),
])

d = json.load(open(PATH))
order = d['group_order']
outdoor = d['gMapGroup_OrangeIslands']

moved = {m for maps in INTERIORS.values() for m in maps}
missing = sorted(m for m in moved if m not in outdoor)
if missing:
    raise SystemExit('not in gMapGroup_OrangeIslands: %s' % missing)

# already split?
if any(g.startswith('gMapGroup_Orange_') and g.endswith('_Interior') for g in order):
    print('orange mapgroups ok (already split)')
    raise SystemExit(0)

d['gMapGroup_OrangeIslands'] = [m for m in outdoor if m not in moved]

for island, maps in INTERIORS.items():
    g = 'gMapGroup_Orange_%s_Interior' % island
    d[g] = maps
    order.append(g)

json.dump(d, open(PATH, 'w'), indent=2)
open(PATH, 'a').write('\n')

print('orange mapgroups ok: %d outdoor, %d interior groups (%d maps)'
      % (len(d['gMapGroup_OrangeIslands']), len(INTERIORS), len(moved)))
