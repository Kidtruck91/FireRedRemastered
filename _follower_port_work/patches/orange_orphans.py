#!/usr/bin/env python3
"""Reconnect the Orange Islands maps that were drawn but unreachable.

TROVITA FALLS is a finished 48x56 map -- river, waterfall, a MART, tall grass,
a hiker -- with no way in at all. Its only open edge is the bottom at x=19..25,
which is the river running south, and TROVITA's whole north edge is ocean. So it
was drawn to be surfed into from the island's north coast, and that is what the
connection does. The offset lines the river mouth up with Trovita x=31..37.

Seven beaches had neither connections nor warps; KUMQUAT's and PUMMELO's
are reachable through the caves added earlier, and the other five get warps here. Unlike Mikan,
Navel and Trovita, their beaches are wider than their islands, so they were never
going to join by a map edge; they get warp pairs on real land instead -- out of
the island's north shore, in at the beach's south.

PINKAN also gets the thing it is named for: a warden on the north shore who
explains that the island is a reserve.

Run from the repo root.
"""
import json, os, sys

# island warp <-> beach warp, both on land, verified before writing
PAIRS = [
    ('OrangeIslands_Pinkan', (34, 27), 'OrangeIslands_Pinkan_Beach', (18, 38)),
    ('OrangeIslands_Moro', (26, 4), 'OrangeIslands_Moro_Beach', (22, 29)),
    ('OrangeIslands_Kinnow', (30, 4), 'OrangeIslands_Kinnow_Beach', (24, 28)),
    ('OrangeIslands_Mandarin_South', (48, 2), 'OrangeIslands_Mandarin_South_Beach', (34, 48)),
    ('OrangeIslands_Sunburst', (34, 6), 'OrangeIslands_Sunburst_Beach', (26, 34)),
]
# Trovita Falls sits above Trovita; offset puts the Falls' river over Trovita
# x=31..37, which is open ocean you can surf to.
FALLS_OFFSET = 12

WARDEN = ('OrangeIslands_Pinkan', 34, 28, 'OBJ_EVENT_GFX_POLICEMAN',
          'OrangeIslands_Pinkan_EventScript_Warden')
WARDEN_SCRIPT = '''
@ ---- Pinkan reserve (orange_orphans.py) ----
OrangeIslands_Pinkan_EventScript_Warden::
\tlock
\tfaceplayer
\tmsgbox OrangeIslands_Pinkan_Text_Warden
\trelease
\tend

OrangeIslands_Pinkan_Text_Warden::
\t.string "Hold it. This whole island is a\\n"
\t.string "reserve, and I'm its warden.\\p"
\t.string "The berries here turn whatever\\n"
\t.string "eats them pink. Every POKeMON on\\l"
\t.string "PINKAN is pink to the tail.\\p"
\t.string "You can look. You can battle.\\n"
\t.string "You do not take one off this\\l"
\t.string "island. Understood?\\p"
\t.string "The shore path north runs down to\\n"
\t.string "the beach if you want the long\\l"
\t.string "way round.$"
'''


def main(repo='.'):
    def load(m):
        return json.load(open(os.path.join(repo, 'data/maps/%s/map.json' % m)))

    def save(m, j):
        p = os.path.join(repo, 'data/maps/%s/map.json' % m)
        json.dump(j, open(p, 'w'), indent=2)
        open(p, 'a').write('\n')

    # --- Trovita <-> Trovita Falls, by river ---------------------------------
    t = load('OrangeIslands_Trovita')
    conns = [c for c in (t.get('connections') or [])
             if 'TROVITA_FALLS' not in c['map']]
    conns.append({'map': 'MAP_ORANGE_ISLANDS_TROVITA_FALLS',
                  'offset': FALLS_OFFSET, 'direction': 'up'})
    t['connections'] = conns
    save('OrangeIslands_Trovita', t)
    f = load('OrangeIslands_Trovita_Falls')
    f['connections'] = [{'map': 'MAP_ORANGE_ISLANDS_TROVITA',
                         'offset': -FALLS_OFFSET, 'direction': 'down'}]
    save('OrangeIslands_Trovita_Falls', f)
    print('  Trovita <-up/down-> Trovita Falls, offset %+d/%+d'
          % (FALLS_OFFSET, -FALLS_OFFSET))

    # --- island <-> beach warp pairs -----------------------------------------
    for isl, ip, beach, bp in PAIRS:
        ji, jb = load(isl), load(beach)
        bid = jb['id']
        iid = ji['id']
        ji['warp_events'] = [w for w in (ji.get('warp_events') or [])
                             if w['dest_map'] != bid]
        jb['warp_events'] = [w for w in (jb.get('warp_events') or [])
                             if w['dest_map'] != iid]
        ii, bi = len(ji['warp_events']), len(jb['warp_events'])
        ji['warp_events'].append({'x': ip[0], 'y': ip[1], 'elevation': 3,
                                  'dest_map': bid, 'dest_warp_id': str(bi)})
        jb['warp_events'].append({'x': bp[0], 'y': bp[1], 'elevation': 3,
                                  'dest_map': iid, 'dest_warp_id': str(ii)})
        save(isl, ji)
        save(beach, jb)
        print('  %-28s %s  <->  %-28s %s'
              % (isl.replace('OrangeIslands_', ''), ip,
                 beach.replace('OrangeIslands_', ''), bp))

    # --- the warden ----------------------------------------------------------
    m, x, y, gfx, script = WARDEN
    j = load(m)
    j['object_events'] = [o for o in j['object_events'] if o['script'] != script] + [{
        'type': 'object', 'graphics_id': gfx, 'x': x, 'y': y, 'elevation': 3,
        'movement_type': 'MOVEMENT_TYPE_FACE_DOWN', 'movement_range_x': 1,
        'movement_range_y': 1, 'trainer_type': 'TRAINER_TYPE_NONE',
        'trainer_sight_or_berry_tree_id': '0', 'script': script, 'flag': '0'}]
    save(m, j)
    sp = os.path.join(repo, 'data/maps/%s/scripts.inc' % m)
    s = open(sp).read()
    if 'Pinkan reserve' not in s:
        open(sp, 'w').write(s.rstrip('\n') + '\n' +
                            WARDEN_SCRIPT.replace('POKeMON', 'POKéMON'))
    print('  Pinkan warden at (%d,%d)' % (x, y))


if __name__ == '__main__':
    main(sys.argv[1] if len(sys.argv) > 1 else '.')
