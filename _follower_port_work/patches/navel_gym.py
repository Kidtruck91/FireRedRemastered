#!/usr/bin/env python3
"""Danny's Navel Gym: a Rock Smash / Strength chamber.

The gym leader and his crew sit behind a rockfall gate. Three floor plates hold
it shut, and each one has to be weighed down with a boulder. The native Victory
Road mechanic does the work: a coord event sitting on an MB_STRENGTH_BUTTON tile
fires when a boulder is pushed onto it, because
HandleBoulderActivateVictoryRoadSwitch matches coord events by position alone and
ignores the var guard. Giving each event an impossible var_value (99) means the
player walking over the plate does nothing -- only a boulder can trip it.

Three alcoves, three different ideas:

  left    (x=4)      the rock guards the mouth; then two pushes north
  centre  (x=7..10)  the rock sits in the push lane; east, east, north
  right   (x=14)     the plate is BELOW the boulder, so it cannot be pushed up:
                     the way in is a rock off the upper hall, and you push down

Run from the repo root. Re-running rebuilds the same map.
"""
import json, os, struct

W, H = 20, 23

FLOOR, WALL, FILL, BUTTON = 0x281, 0x283, 0x291, 0x2C0
WALL_V = 0x282                      # wall face beside floor
GATE_TOP, GATE_BOT = 0x307, 0x317   # drawn by ON_LOAD, not stored here
DOOR = 0x2CC                        # cave exit (behavior 0x62)

ELEV_FLOOR, ELEV_WALL = 3, 0

# --- the room -------------------------------------------------------------
CHAMBER = (2, 17, 2, 6)       # Danny and his crew
GATE    = (9, 10, 7, 8)       # the rockfall
UPPER   = (2, 17, 9, 10)
LOWER   = (2, 17, 17, 18)
VEST    = (2, 17, 19, 21)
CORRIDORS = [(2, 2, 11, 16), (17, 17, 11, 16)]
ALCOVES = [(4, 4, 12, 15), (7, 10, 12, 15), (14, 14, 12, 15)]
# the centre alcove needs a two-tile mouth: with a one-tile mouth the player
# can push its boulder into the gap and seal themselves in.
MOUTHS = [(4, 16), (8, 16), (9, 16), (14, 11)]
DOORS = [(8, 22), (9, 22), (10, 22)]

BUTTONS = [(4, 12), (10, 12), (14, 15)]
BOULDERS = [(4, 14), (8, 13), (14, 13)]
ROCKS = [(4, 15), (9, 13), (14, 11)]


def build_blocks():
    g = {}
    for x in range(W):
        for y in range(H):
            g[(x, y)] = (FILL, 1, ELEV_WALL)

    def carve(r):
        x0, x1, y0, y1 = r
        for x in range(x0, x1 + 1):
            for y in range(y0, y1 + 1):
                g[(x, y)] = (FLOOR, 0, ELEV_FLOOR)

    for r in [CHAMBER, GATE, UPPER, LOWER, VEST] + CORRIDORS + ALCOVES:
        carve(r)
    for p in MOUTHS:
        g[p] = (FLOOR, 0, ELEV_FLOOR)
    for p in BUTTONS:
        g[p] = (BUTTON, 0, ELEV_FLOOR)
    for p in DOORS:
        g[p] = (DOOR, 0, ELEV_FLOOR)

    # give the solid rock a face wherever it meets the floor, so the room does
    # not read as one flat slab
    faces = {}
    for (x, y), (m, c, e) in g.items():
        if not c:
            continue
        for dx, dy in ((0, 1), (0, -1), (-1, 0), (1, 0)):
            n = g.get((x + dx, y + dy))
            if n and not n[1]:
                faces[(x, y)] = (WALL if dy else WALL_V, 1, ELEV_WALL)
                break
    g.update(faces)
    return g


def main(repo='.'):
    g = build_blocks()
    out = os.path.join(repo, 'data/layouts/OrangeIslands_Navel_Gym')
    os.makedirs(out, exist_ok=True)
    data = b''
    for y in range(H):
        for x in range(W):
            m, c, e = g[(x, y)]
            data += struct.pack('<H', m | (c << 10) | (e << 12))
    open(os.path.join(out, 'map.bin'), 'wb').write(data)
    open(os.path.join(out, 'border.bin'), 'wb').write(
        struct.pack('<4H', *([FILL | (1 << 10)] * 4)))

    # register the layout
    p = os.path.join(repo, 'data/layouts/layouts.json')
    L = json.load(open(p))
    entry = {
        'id': 'LAYOUT_ORANGE_ISLANDS_NAVEL_GYM',
        'name': 'OrangeIslands_Navel_Gym_Layout',
        'width': W, 'height': H, 'border_width': 2, 'border_height': 2,
        'primary_tileset': 'gTileset_General', 'secondary_tileset': 'gTileset_Cave',
        'border_filepath': 'data/layouts/OrangeIslands_Navel_Gym/border.bin',
        'blockdata_filepath': 'data/layouts/OrangeIslands_Navel_Gym/map.bin',
    }
    L['layouts'] = [l for l in L['layouts'] if l['id'] != entry['id']] + [entry]
    json.dump(L, open(p, 'w'), indent=2)
    open(p, 'a').write('\n')
    print('navel gym layout: %dx%d, %d floor tiles'
          % (W, H, sum(1 for v in g.values() if not v[1])))


if __name__ == '__main__':
    import sys
    main(sys.argv[1] if len(sys.argv) > 1 else '.')


# --------------------------------------------------------------------------
# events, scripts, flags and the var that keeps the plates boulder-only
# --------------------------------------------------------------------------
LOCALIDS = ['DANNY', 'KAI', 'SORA', 'YUKI', 'GUIDE',
            'BOULDER_LEFT', 'BOULDER_CENTRE', 'BOULDER_RIGHT']
PEOPLE = [('OBJ_EVENT_GFX_DANNY', 9, 3, 'DOWN', 'Danny'),
          ('OBJ_EVENT_GFX_HIKER', 4, 4, 'DOWN', 'Kai'),
          ('OBJ_EVENT_GFX_BLACKBELT', 14, 4, 'DOWN', 'Sora'),
          ('OBJ_EVENT_GFX_COOLTRAINER_F', 11, 5, 'DOWN', 'Yuki'),
          ('OBJ_EVENT_GFX_GYM_GUY', 7, 20, 'DOWN', 'GymGuy')]
SIGNS = [(6, 22), (12, 22)]
PLATE_NAMES = ['Left', 'Centre', 'Right']


def obj(gfx, x, y, face, script, ident='0'):
    return {'type': 'object', 'graphics_id': gfx, 'x': x, 'y': y, 'elevation': 3,
            'movement_type': 'MOVEMENT_TYPE_FACE_' + face,
            'movement_range_x': 1, 'movement_range_y': 1,
            'trainer_type': 'TRAINER_TYPE_NONE',
            'trainer_sight_or_berry_tree_id': ident,
            'script': script, 'flag': '0'}


def write_events(repo='.'):
    M = 'OrangeIslands_Navel_Gym'
    p = os.path.join(repo, 'data/maps/%s/map.json' % M)
    j = json.load(open(p))
    j['layout'] = 'LAYOUT_ORANGE_ISLANDS_NAVEL_GYM'

    objs = [obj(g, x, y, f, '%s_EventScript_%s' % (M, s)) for g, x, y, f, s in PEOPLE]
    for (x, y) in BOULDERS:
        objs.append(obj('OBJ_EVENT_GFX_PUSHABLE_BOULDER', x, y, 'DOWN',
                        'EventScript_StrengthBoulder'))
    for (x, y) in ROCKS:
        objs.append(obj('OBJ_EVENT_GFX_ROCK_SMASH_ROCK', x, y, 'DOWN',
                        'EventScript_RockSmash'))
    j['object_events'] = objs

    j['warp_events'] = [{'x': x, 'y': y, 'elevation': 3,
                         'dest_map': 'MAP_ORANGE_ISLANDS_NAVEL', 'dest_warp_id': '2'}
                        for (x, y) in DOORS]
    j['bg_events'] = [{'type': 'sign', 'x': x, 'y': y, 'elevation': 0,
                       'player_facing_dir': 'BG_EVENT_PLAYER_FACING_ANY',
                       'script': '%s_EventScript_GymStatue' % M} for (x, y) in SIGNS]
    # var_value 99 is never true, so walking over a plate does nothing. Only
    # HandleBoulderActivateVictoryRoadSwitch, which matches on position alone,
    # can fire these.
    j['coord_events'] = [{'type': 'trigger', 'x': x, 'y': y, 'elevation': 3,
                          'var': 'VAR_MAP_SCENE_NAVEL_GYM', 'var_value': '99',
                          'script': '%s_EventScript_Plate%s' % (M, PLATE_NAMES[i])}
                         for i, (x, y) in enumerate(BUTTONS)]
    json.dump(j, open(p, 'w'), indent=2)
    open(p, 'a').write('\n')
    print('navel gym events: %d objects, %d warps, %d plates'
          % (len(objs), len(DOORS), len(BUTTONS)))
