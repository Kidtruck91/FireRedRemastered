#!/usr/bin/env python3
# Cissy's MIKAN GYM: a current puzzle.
#
# Four one-way channels run north out of the entrance bay. Each feeds a cross-corridor
# that shunts you sideways, and only the third channel lines up with the chimney that
# leads to the top pool and Cissy's landing. The other three carry you into a side pocket
# where one of her swimmers is waiting; win or lose, the current then drags you back to
# the dock to pick again.
#
# Currents are four metatiles appended to the Cerulean gym tileset, reusing that tileset's
# own pool-water graphics so they look like ordinary water -- as currents do everywhere
# else in the game. Appending leaves every existing metatile id alone, so Cerulean City
# Gym, which shares the tileset, is untouched.
#
# Run from the repo root.
import json, os, struct

TS = 'data/tilesets/secondary/cerulean_gym/'
WATER_MT = 0x291
BEHAVIORS = [0x52, 0x53, 0x51, 0x50]          # north, south, west, east


def add_current_metatiles():
    """Append N/S/W/E currents that look exactly like WATER_MT. Idempotent."""
    mt = bytearray(open(TS + 'metatiles.bin', 'rb').read())
    at = bytearray(open(TS + 'metatile_attributes.bin', 'rb').read())
    assert len(mt) // 16 == len(at) // 4, 'tileset metatile/attribute count mismatch'
    gfx = bytes(mt[(WATER_MT - 640) * 16:(WATER_MT - 640) * 16 + 16])
    base = struct.unpack_from('<I', at, (WATER_MT - 640) * 4)[0]
    want = [(base & ~0xff) | b for b in BEHAVIORS]

    have = {}
    for i in range(len(mt) // 16):
        if bytes(mt[i * 16:i * 16 + 16]) == gfx:
            have[struct.unpack_from('<I', at, i * 4)[0]] = 640 + i
    if all(a in have for a in want):
        return [have[a] for a in want]

    ids = []
    for a in want:
        ids.append(640 + len(mt) // 16)
        mt += gfx
        at += struct.pack('<I', a)
    open(TS + 'metatiles.bin', 'wb').write(mt)
    open(TS + 'metatile_attributes.bin', 'wb').write(at)
    return ids


CUR_N, CUR_S, CUR_W, CUR_E = add_current_metatiles()

W, H = 24, 20

# ---- the plan, in characters. x 1..22 and y 3..18 are the room interior. ----
#   '.' floor   'w' still water   '^v<>' currents   'C' Cissy   'G' guide
#   's' sign    'D' door/warp     '1','2' swimmers
PLAN = {}


def put(y, spans):
    row = ['.'] * W
    for x0, x1, c in spans:
        for x in range(x0, x1 + 1):
            row[x] = c
    PLAN[y] = row


for y in range(3, 19):
    put(y, [])
put(6,  [(10, 22, 'w')])                                    # top pool
put(7,  [(10, 22, 'w')])
put(8,  [(13, 14, 'w'), (21, 22, 'v')])                     # chimney up, chute back down
put(9,  [(1, 2, 'w'), (13, 14, '^'), (21, 22, 'w')])        # pockets + the way up
put(10, [(1, 2, 'w'), (3, 12, '<'), (13, 14, '^'),          # sorting corridor
         (15, 20, '>'), (21, 22, 'w')])
put(11, [(5, 6, '^'), (9, 10, '^'), (13, 14, '^'), (17, 18, '^')])    # channels
put(12, [(5, 6, '^'), (9, 10, '^'), (13, 14, '^'), (17, 18, '^')])
for y in (13, 14, 15, 16):
    put(y, [(1, 22, 'w')])                                  # entrance bay
put(18, [(11, 13, 'D')])

WATER = set('w^v<>')
land = lambda x, y: not (0 <= x < W and 3 <= y <= 18 and PLAN[y][x] in WATER)

# ---- metatiles, lifted from Cerulean City Gym ----
FLOOR, OPEN = 0x281, 0x291
EDGE = {0: OPEN, 1: 0x289, 2: 0x290, 4: 0x292, 8: 0x299,
        3: 0x288, 5: 0x28A, 10: 0x298, 12: 0x29A}            # N=1 W=2 E=4 S=8
CUR = {'^': CUR_N, 'v': CUR_S, '<': CUR_W, '>': CUR_E}
blk = lambda mt, coll, elev: (elev << 12) | (coll << 10) | mt

grid = [[0] * W for _ in range(H)]
for y in range(H):
    for x in range(W):
        if y == 0:
            mt = 0x2A6 if x == 0 else 0x2A7 if x == W - 1 else 0x285 if x == 1 \
                else 0x287 if x == W - 2 else 0x286
            grid[y][x] = blk(mt, 1, 0)
        elif y == 1:
            mt = 0x2AE if x == 0 else 0x2AF if x == W - 1 else 0x28D if x == 1 \
                else 0x28F if x == W - 2 else 0x28E
            grid[y][x] = blk(mt, 1, 0)
        elif y == 2:
            mt = 0x2AE if x == 0 else 0x2AF if x == W - 1 else 0x295 if x == 1 \
                else 0x297 if x == W - 2 else 0x296
            grid[y][x] = blk(mt, 1, 0)
        elif y == H - 1:
            mt = {11: 0x174, 12: 0x175, 13: 0x176}.get(x, 0x008)
            grid[y][x] = blk(mt, 1, 0)
        elif x == 0 or x == W - 1:
            grid[y][x] = blk(0x2AE if x == 0 else 0x2AF, 1, 0)
        elif y == 3:
            mt = 0x29D if x == 1 else 0x29F if x == W - 2 else 0x29E
            grid[y][x] = blk(mt, 0, 3)
        else:
            c = PLAN[y][x]
            if c in WATER:
                m = (1 if land(x, y - 1) else 0) | (2 if land(x - 1, y) else 0) \
                    | (4 if land(x + 1, y) else 0) | (8 if land(x, y + 1) else 0)
                base = EDGE.get(m, OPEN)
                # a current keeps its behaviour; it is only drawn as open water
                grid[y][x] = blk(CUR[c], 0, 1) if c in CUR else blk(base, 0, 1)
            else:
                grid[y][x] = blk(FLOOR, 0, 3)

# dividers and banks must not be surfed onto, or the maze can be walked around
for y in range(4, 18):
    for x in range(1, W - 1):
        if PLAN[y][x] == '.' and any(not land(x + dx, y + dy)
                                     for dx, dy in ((1, 0), (-1, 0), (0, 1), (0, -1))):
            if not (y >= 17 or (y == 5 and 10 <= x <= 17)):   # dock and Cissy's bank stay walkable
                grid[y][x] = blk(FLOOR, 1, 0)

for x in (11, 12, 13):
    grid[18][x] = blk(0x2C9, 0, 3)                            # MB_SOUTH_ARROW_WARP

OUT = 'data/layouts/OrangeIslands_Mikan_Gym/'
os.makedirs(OUT, exist_ok=True)
open(OUT + 'map.bin', 'wb').write(b''.join(struct.pack('<H', v) for r in grid for v in r))
open(OUT + 'border.bin', 'wb').write(struct.pack('<4H', *([blk(0x008, 1, 0)] * 4)))

L = json.load(open('data/layouts/layouts.json'))
ID = 'LAYOUT_ORANGE_ISLANDS_MIKAN_GYM'
L['layouts'] = [e for e in L['layouts'] if e['id'] != ID] + [{
    'id': ID, 'name': 'OrangeIslands_Mikan_Gym_Layout',
    'width': W, 'height': H, 'border_width': 2, 'border_height': 2,
    'primary_tileset': 'gTileset_Building', 'secondary_tileset': 'gTileset_CeruleanGym',
    'border_filepath': OUT + 'border.bin', 'blockdata_filepath': OUT + 'map.bin'}]
json.dump(L, open('data/layouts/layouts.json', 'w'), indent=2)
open('data/layouts/layouts.json', 'a').write('\n')

print('mikan gym layout ok: %dx%d, currents N/S/W/E = %s'
      % (W, H, ' '.join(hex(c) for c in (CUR_N, CUR_S, CUR_W, CUR_E))))

# ---------------------------------------------------------------- events
def obj(gfx, x, y, face, script, trainer=0, sight=0):
    return {'type': 'object', 'graphics_id': gfx, 'x': x, 'y': y, 'elevation': 0,
            'movement_type': 'MOVEMENT_TYPE_FACE_' + face,
            'movement_range_x': 1, 'movement_range_y': 1,
            'trainer_type': 'TRAINER_TYPE_NORMAL' if trainer else 'TRAINER_TYPE_NONE',
            'trainer_sight_or_berry_tree_id': str(sight), 'script': script, 'flag': '0'}


P = 'OrangeIslands_Mikan_Gym_EventScript_'
M = 'data/maps/OrangeIslands_Mikan_Gym/map.json'
j = json.load(open(M))
j['layout'] = ID
j['object_events'] = [
    obj('OBJ_EVENT_GFX_CISSY', 12, 4, 'DOWN', P + 'Cissy'),
    obj('OBJ_EVENT_GFX_SWIMMER_M_WATER', 1, 10, 'RIGHT', P + 'Marco'),
    obj('OBJ_EVENT_GFX_SWIMMER_F_WATER', 22, 10, 'LEFT', P + 'Coral'),
    obj('OBJ_EVENT_GFX_GYM_GUY', 7, 18, 'DOWN', P + 'GymGuy'),
]
j['warp_events'] = [{'x': x, 'y': 18, 'elevation': 0,
                     'dest_map': 'MAP_ORANGE_ISLANDS_MIKAN_BEACH', 'dest_warp_id': '0'}
                    for x in (11, 12, 13)]
j['coord_events'] = [{'type': 'trigger', 'x': x, 'y': y, 'elevation': 0,
                      'var': 'VAR_TEMP_1', 'var_value': '0', 'script': P + s}
                     for xs, s in (((1, 2), 'PocketLeft'), ((21, 22), 'PocketRight'))
                     for x in xs for y in (9, 10)]
j['bg_events'] = [{'type': 'sign', 'x': x, 'y': 18, 'elevation': 0,
                   'player_facing_dir': 'BG_EVENT_PLAYER_FACING_ANY',
                   'script': P + 'GymStatue'} for x in (9, 15)]
json.dump(j, open(M, 'w'), indent=2)
open(M, 'a').write('\n')
print('mikan gym events ok')
