#!/usr/bin/env python3
# C06: wire up the Orange Islands Pokemon Centers.
#
# The Center buildings are already drawn on the islands -- most use the Kanto building
# (door metatile 0x062), Kumquat and Pummelo use the Hoenn one (0x061) -- but none of
# the doors had a warp behind them, there were no interiors, and the region had no
# entry in either heal-location table. Whiting out anywhere in the Orange Islands had
# nowhere to send the player.
#
# This adds a 1F and 2F for each island on the standard shared Pokemon Center layouts,
# so there is no interior drawing involved: the same two layouts every other Center in
# the game uses. Each 1F sets its island's respawn; each 2F carries the usual cable-club
# attendants, which is also how the Training Room is reached.
#
# Moro and Pinkan deliberately have no Center and get none here.
#
# Run from the repo root. Re-running is safe: it rewrites the same entries.
import json
import os
import re

# island -> (door x, door y). The tile below each door is the spawn point.
CENTERS = [
    ('Valencia',       51, 44),
    ('Mikan',          26, 46),
    ('Navel',          28, 40),
    ('Trovita',        43, 37),
    ('Sunburst',       26, 32),
    ('Kinnow',         24, 13),
    ('Mandarin_South', 52, 22),
    ('Kumquat',        47, 21),   # Hoenn-style building
    ('Pummelo',        10, 36),   # Hoenn-style building
]

SPAWN = {n: 'SPAWN_ORANGE_' + n.upper() for n, _, _ in CENTERS}


def const(name):
    return 'MAP_ORANGE_ISLANDS_%s_POKEMON_CENTER_%s'


def header(name, floor, mapsec):
    return {
        'id': 'MAP_ORANGE_ISLANDS_%s_POKEMON_CENTER_%s' % (name.upper(), floor),
        'name': 'OrangeIslands_%s_PokemonCenter_%s' % (name, floor),
        'layout': 'LAYOUT_POKEMON_CENTER_%s' % floor,
        'music': 'MUS_POKE_CENTER', 'region_map_section': mapsec,
        'requires_flash': False, 'weather': 'WEATHER_NONE', 'map_type': 'MAP_TYPE_INDOOR',
        'allow_cycling': False, 'allow_escaping': False, 'allow_running': False,
        'show_map_name': False, 'floor_number': 0,
        'battle_scene': 'MAP_BATTLE_SCENE_NORMAL', 'connections': 0,
        'object_events': [], 'warp_events': [], 'coord_events': [], 'bg_events': [],
    }


def obj(gfx, x, y, face, script):
    return {'type': 'object', 'graphics_id': gfx, 'x': x, 'y': y, 'elevation': 3,
            'movement_type': 'MOVEMENT_TYPE_FACE_' + face,
            'movement_range_x': 1, 'movement_range_y': 1,
            'trainer_type': 'TRAINER_TYPE_NONE', 'trainer_sight_or_berry_tree_id': '0',
            'script': script, 'flag': '0'}


def warp(x, y, dest, wid, elev=0):
    return {'x': x, 'y': y, 'elevation': elev, 'dest_map': dest, 'dest_warp_id': str(wid)}


def write_json(path, data):
    os.makedirs(os.path.dirname(path), exist_ok=True)
    json.dump(data, open(path, 'w'), indent=2)
    open(path, 'a').write('\n')


maps = {}
for name, dx, dy in CENTERS:
    island = json.load(open('data/maps/OrangeIslands_%s/map.json' % name))
    mapsec = island['region_map_section']
    island_id = island['id']
    pc1 = 'MAP_ORANGE_ISLANDS_%s_POKEMON_CENTER_1F' % name.upper()
    pc2 = 'MAP_ORANGE_ISLANDS_%s_POKEMON_CENTER_2F' % name.upper()

    # the island's door gets a warp into the new 1F
    warps = island.get('warp_events') or []
    warps = [w for w in warps if not (w['x'] == dx and w['y'] == dy)]
    door_index = len(warps)
    warps.append(warp(dx, dy, pc1, 0))
    island['warp_events'] = warps
    write_json('data/maps/OrangeIslands_%s/map.json' % name, island)

    one = header(name, '1F', mapsec)
    one['object_events'] = [obj('OBJ_EVENT_GFX_NURSE', 7, 2, 'DOWN',
                                'OrangeIslands_%s_PokemonCenter_1F_EventScript_Nurse' % name)]
    # elevations copied from a stock Center: 3 on the exits, 4 on the stairwell
    one['warp_events'] = [warp(x, 8, island_id, door_index, elev=3) for x in (6, 7, 8)] + \
                         [warp(1, 6, pc2, 0, elev=4)]
    write_json('data/maps/OrangeIslands_%s_PokemonCenter_1F/map.json' % name, one)

    two = header(name, '2F', mapsec)
    two['object_events'] = [
        obj('OBJ_EVENT_GFX_CABLE_CLUB_RECEPTIONIST', 6, 2, 'DOWN', 'Common_EventScript_UnionRoomAttendant'),
        obj('OBJ_EVENT_GFX_CABLE_CLUB_RECEPTIONIST', 2, 2, 'DOWN', 'Common_EventScript_WirelessClubAttendant'),
        obj('OBJ_EVENT_GFX_CABLE_CLUB_RECEPTIONIST', 10, 2, 'DOWN', 'Common_EventScript_DirectCornerAttendant'),
    ]
    two['warp_events'] = [warp(1, 6, pc1, 3, elev=4),
                          warp(5, 1, 'MAP_UNION_ROOM', 0),
                          warp(9, 1, 'MAP_TRADE_CENTER', 0)]
    write_json('data/maps/OrangeIslands_%s_PokemonCenter_2F/map.json' % name, two)

    open('data/maps/OrangeIslands_%s_PokemonCenter_1F/scripts.inc' % name, 'w').write(
        'OrangeIslands_{n}_PokemonCenter_1F_MapScripts::\n'
        '\tmap_script MAP_SCRIPT_ON_TRANSITION, OrangeIslands_{n}_PokemonCenter_1F_OnTransition\n'
        '\tmap_script MAP_SCRIPT_ON_RESUME, CableClub_OnResume\n'
        '\t.byte 0\n\n'
        'OrangeIslands_{n}_PokemonCenter_1F_OnTransition::\n'
        '\tsetrespawn {s}\n'
        '\tend\n\n'
        'OrangeIslands_{n}_PokemonCenter_1F_EventScript_Nurse::\n'
        '\tlock\n\tfaceplayer\n\tcall EventScript_PkmnCenterNurse\n\trelease\n\tend\n'
        .format(n=name, s=SPAWN[name]))
    open('data/maps/OrangeIslands_%s_PokemonCenter_2F/scripts.inc' % name, 'w').write(
        'OrangeIslands_{n}_PokemonCenter_2F_MapScripts::\n'
        '\tmap_script MAP_SCRIPT_ON_FRAME_TABLE, CableClub_OnFrame\n'
        '\tmap_script MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE, CableClub_OnWarp\n'
        '\tmap_script MAP_SCRIPT_ON_LOAD, CableClub_OnLoad\n'
        '\tmap_script MAP_SCRIPT_ON_TRANSITION, CableClub_OnTransition\n'
        '\t.byte 0\n'.format(n=name))
    for f in ('1F', '2F'):
        open('data/maps/OrangeIslands_%s_PokemonCenter_%s/text.inc' % (name, f), 'w').write('')
    maps[name] = (dx, dy + 1)

# ---- map groups: each Center joins its island's interior group ----
G = 'data/maps/map_groups.json'
g = json.load(open(G))
for name, _, _ in CENTERS:
    key = 'gMapGroup_Orange_%s_Interior' % name
    if key not in g:
        g[key] = []
        g['group_order'].append(key)
    for f in ('1F', '2F'):
        m = 'OrangeIslands_%s_PokemonCenter_%s' % (name, f)
        if m not in g[key]:
            g[key].append(m)
json.dump(g, open(G, 'w'), indent=2)
open(G, 'a').write('\n')

# ---- SPAWN_ constants ----
C = 'include/constants/heal_locations.h'
s = open(C).read()
nums = [int(m) for m in re.findall(r'#define SPAWN_\w+ (\d+)', s)]
nxt = max(nums) + 1
lines = []
for name, _, _ in CENTERS:
    if SPAWN[name] in s:
        continue
    lines.append('#define %s %d' % (SPAWN[name], nxt))
    nxt += 1
if lines:
    s = s.replace('\n#endif', '\n' + '\n'.join(lines) + '\n\n#endif')
    open(C, 'w').write(s)

# ---- both heal-location tables ----
H = 'src/data/heal_locations.h'
s = open(H).read()
spawn_rows, center_rows, nurse_rows = [], [], []
for name, _, _ in CENTERS:
    if SPAWN[name] + ' - 1' in s:
        continue
    x, y = maps[name]
    spawn_rows.append('    [%s - 1] = {MAP_GROUP(ORANGE_ISLANDS_%s), MAP_NUM(ORANGE_ISLANDS_%s), %d, %d},'
                      % (SPAWN[name], name.upper(), name.upper(), x, y))
    center_rows.append('    [%s - 1] = {MAP_GROUP(ORANGE_ISLANDS_%s_POKEMON_CENTER_1F), MAP_NUM(ORANGE_ISLANDS_%s_POKEMON_CENTER_1F)},'
                       % (SPAWN[name], name.upper(), name.upper()))
    # the nurse is the first object event on each generated 1F
    nurse_rows.append('    [%s - 1] = 1,' % SPAWN[name])
if spawn_rows:
    # three parallel tables, each keyed by SPAWN id: where you reappear, which Center
    # interior you wake in, and which object there is the nurse.
    s = s.replace('};\n\nstatic const u16 sWhiteoutRespawnHealCenterMapIdxs',
                  '\n'.join(spawn_rows) + '\n};\n\nstatic const u16 sWhiteoutRespawnHealCenterMapIdxs', 1)
    s = s.replace('};\n\nstatic const u8 sWhiteoutRespawnHealerNpcIds',
                  '\n'.join(center_rows) + '\n};\n\nstatic const u8 sWhiteoutRespawnHealerNpcIds', 1)
    s = s.rstrip('\n')
    assert s.endswith('};'), 'unexpected tail in heal_locations.h'
    s = s[:-2] + '\n'.join(nurse_rows) + '\n};\n'
    open(H, 'w').write(s)

# ---- script includes ----
E = 'data/event_scripts.s'
s = open(E).read()
lines = []
for name, _, _ in CENTERS:
    for f in ('1F', '2F'):
        m = 'OrangeIslands_%s_PokemonCenter_%s' % (name, f)
        if 'data/maps/%s/scripts.inc' % m in s:
            continue
        lines.append('\t.include "data/maps/%s/scripts.inc"' % m)
        lines.append('\t.include "data/maps/%s/text.inc"' % m)
        lines.append('')
if lines:
    s = s.rstrip('\n') + '\n\n' + '\n'.join(lines).rstrip('\n') + '\n'
    open(E, 'w').write(s)

print('orange pokecenters ok: %d centers (1F+2F), doors wired, spawns and whiteout entries added'
      % len(CENTERS))
