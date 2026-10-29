#!/usr/bin/env python3
"""Build out the Orange Islands cave systems.

Before: SUNBURST CAVE and PUMMELO CAVE each had one drawn ground floor whose
ladder tiles led nowhere; Sunburst's "2F" was the ground floor copied into a
bigger canvas, Sunburst 3F and both Pummelo 2Fs were blank rectangles, and the
cave mouths on Route 4 (two of them) and Route 9 opened onto nothing.

After: Sunburst is three connected floors, Pummelo is two, Route 4's two mouths
are a through-tunnel that actually saves you the walk, and Route 9's is a short
dead end worth visiting once.

The two drawn ground floors are the user's work and their blockdata is never
touched -- only warps are added onto the ladder tiles already drawn on them.
Every other floor is generated and checked for connectivity before it is
written. See docs/orange-islands/caves.md.

Run from the repo root.
"""
import json, os, struct, sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from cavegen import build

TS = ('gTileset_General', 'gTileset_Cave')

# name -> (w, h, seed, rooms, [(feature, preferred spot)])
GENERATED = {
    'OrangeIslands_Sunburst_Cave_2F': (46, 35, 20251029, 9,
        [('to1F_west', (8, 8)), ('to1F_east', (26, 5)), ('to3F', (32, 26))]),
    'OrangeIsland_Sunburst_Cave_3F': (44, 63, 20251030, 14,
        [('to2F', (30, 8)), ('chamber', (20, 52))]),
    'OrangeIslands_Pummelo_Cave2': (40, 32, 20251031, 8,
        [('to1F', (26, 5)), ('deep', (10, 25))]),
    'OrangeIslands_Route_4_Cave': (38, 26, 20251101, 7,
        [('west', (6, 13)), ('east', (31, 13))]),
    'OrangeIslands_Route_9_Cave': (26, 22, 20251102, 5,
        [('mouth', (13, 18)), ('deep', (13, 4))]),
}

NEW_MAPS = {
    'OrangeIslands_Route_4_Cave': ('MAPSEC_ORANGE_ROUTE_4', 'MUS_MT_MOON'),
    'OrangeIslands_Route_9_Cave': ('MAPSEC_ORANGE_ROUTE_9', 'MUS_SEVII_CAVE'),
}
NEW_GROUP = 'gMapGroup_Orange_Route_Interior'

# every warp this pass creates: (map, position, destination map, destination warp)
LINKS = []


def layout_id(repo, name):
    """Reuse the id the map already declares; only invent one for a new map.
    Deriving it was a mistake -- it turned _2F into _2_F and quietly added a
    second layout with the same symbol name, which the assembler rejected."""
    p = os.path.join(repo, 'data/maps/%s/map.json' % name)
    if os.path.exists(p):
        return json.load(open(p))['layout']
    out = ''.join(('_' + c if c.isupper() and i and not name[i - 1].isupper()
                   and not name[i - 1].isdigit() else c)
                  for i, c in enumerate(name))
    return 'LAYOUT_' + out.upper().replace('__', '_')


def write_layout(repo, name, w, h, data):
    d = os.path.join(repo, 'data/layouts', name)
    os.makedirs(d, exist_ok=True)
    open(os.path.join(d, 'map.bin'), 'wb').write(data)
    open(os.path.join(d, 'border.bin'), 'wb').write(
        struct.pack('<4H', *([FILL_BLOCK] * 4)))
    p = os.path.join(repo, 'data/layouts/layouts.json')
    L = json.load(open(p))
    lid = layout_id(repo, name)
    entry = {'id': lid, 'name': name + '_Layout', 'width': w, 'height': h,
             'border_width': 2, 'border_height': 2,
             'primary_tileset': TS[0], 'secondary_tileset': TS[1],
             'border_filepath': 'data/layouts/%s/border.bin' % name,
             'blockdata_filepath': 'data/layouts/%s/map.bin' % name}
    existing = next((l for l in L['layouts'] if l['id'] == lid), None)
    if existing:
        existing.update(entry)
    else:
        L['layouts'].append(entry)
    json.dump(L, open(p, 'w'), indent=2)
    open(p, 'a').write('\n')
    return lid


FILL_BLOCK = 0x291 | (1 << 10)


def map_header(name, mapsec, music, layout):
    return {
        'id': 'MAP_' + name.upper().replace('ORANGEISLANDS_', 'ORANGE_ISLANDS_'),
        'name': name, 'layout': layout, 'music': music,
        'region_map_section': mapsec, 'requires_flash': False,
        'weather': 'WEATHER_NONE', 'map_type': 'MAP_TYPE_UNDERGROUND',
        'allow_cycling': False, 'allow_escaping': False, 'allow_running': False,
        'show_map_name': True, 'floor_number': 0,
        'battle_scene': 'MAP_BATTLE_SCENE_NORMAL', 'connections': 0,
        'object_events': [], 'warp_events': [], 'coord_events': [], 'bg_events': [],
    }


def warp(x, y, dest, wid, elev=3):
    # the route-side cave mouths sit on solid door tiles; every cave door that
    # already works in this ROM (Pummelo's two, Mt Ember's Ruby Path) is on
    # elevation 0, so match that rather than the ladder elevation.
    return {'x': x, 'y': y, 'elevation': elev, 'dest_map': dest, 'dest_warp_id': str(wid)}


def main(repo='.'):
    placed = {}
    # ---- generate every floor that is blank or a copy -----------------------
    for name, (w, h, seed, rooms, feats) in GENERATED.items():
        f, pos = build(w, h, seed, rooms, feats)
        # only the features that are actually warps get a ladder tile; the
        # others are just somewhere worth walking to, and get an item ball
        ladders = [v for k, v in pos.items() if k not in ('chamber', 'deep')]
        write_layout(repo, name, w, h, f.blocks(ladders))
        placed[name] = pos
        print('  %-34s %2dx%-2d  %4d open tiles  %s'
              % (name, w, h, len(f.open),
                 ' '.join('%s@%d,%d' % (k, v[0], v[1]) for k, v in pos.items())))

    # ---- the two brand new route caves need map.json and a group ------------
    for name, (mapsec, music) in NEW_MAPS.items():
        parent = name.replace('_Cave', '')
        mapsec = json.load(open(os.path.join(
            repo, 'data/maps/%s/map.json' % parent)))['region_map_section']
        p = os.path.join(repo, 'data/maps', name)
        os.makedirs(p, exist_ok=True)
        if not os.path.exists(os.path.join(p, 'map.json')):
            json.dump(map_header(name, mapsec, music, layout_id(repo, name)),
                      open(os.path.join(p, 'map.json'), 'w'), indent=2)
            open(os.path.join(p, 'map.json'), 'a').write('\n')
        for fn, body in (('scripts.inc', '%s_MapScripts::\n\t.byte 0\n' % name),
                         ('text.inc', '')):
            if not os.path.exists(os.path.join(p, fn)):
                open(os.path.join(p, fn), 'w').write(body)

    G = os.path.join(repo, 'data/maps/map_groups.json')
    g = json.load(open(G))
    if NEW_GROUP not in g:
        g[NEW_GROUP] = []
        g['group_order'].append(NEW_GROUP)
    for name in NEW_MAPS:
        if name not in g[NEW_GROUP]:
            g[NEW_GROUP].append(name)
    json.dump(g, open(G, 'w'), indent=2)
    open(G, 'a').write('\n')

    E = os.path.join(repo, 'data/event_scripts.s')
    s = open(E).read()
    add = [l for name in NEW_MAPS
           for l in ('\t.include "data/maps/%s/scripts.inc"' % name,
                     '\t.include "data/maps/%s/text.inc"' % name, '')
           if 'data/maps/%s/scripts.inc' % name not in s or '/text.inc' in l]
    add = [l for l in add if l == '' or l.split('"')[1] not in s]
    if add:
        open(E, 'w').write(s.rstrip('\n') + '\n\n' + '\n'.join(add).rstrip('\n') + '\n')

    # ---- wire it all together ----------------------------------------------
    def setw(mapname, warps):
        p = os.path.join(repo, 'data/maps/%s/map.json' % mapname)
        j = json.load(open(p))
        j['warp_events'] = warps
        json.dump(j, open(p, 'w'), indent=2)
        open(p, 'a').write('\n')

    def existing(mapname):
        return json.load(open(os.path.join(
            repo, 'data/maps/%s/map.json' % mapname)))['warp_events']

    SUN2, SUN3 = placed['OrangeIslands_Sunburst_Cave_2F'], placed['OrangeIsland_Sunburst_Cave_3F']
    PUM2 = placed['OrangeIslands_Pummelo_Cave2']
    R4, R9 = placed['OrangeIslands_Route_4_Cave'], placed['OrangeIslands_Route_9_Cave']

    # Sunburst: ground floor keeps its island exit, its two drawn ladders become warps
    setw('OrangeIslands_Sunburst_Cave', existing('OrangeIslands_Sunburst_Cave')[:1] + [
        warp(6, 8, 'MAP_ORANGE_ISLANDS_SUNBURST_CAVE_2F', 0),
        warp(25, 3, 'MAP_ORANGE_ISLANDS_SUNBURST_CAVE_2F', 1)])
    setw('OrangeIslands_Sunburst_Cave_2F', [
        warp(*SUN2['to1F_west'], 'MAP_ORANGE_ISLANDS_SUNBURST_CAVE', 1),
        warp(*SUN2['to1F_east'], 'MAP_ORANGE_ISLANDS_SUNBURST_CAVE', 2),
        warp(*SUN2['to3F'], 'MAP_ORANGE_ISLAND_SUNBURST_CAVE_3F', 0)])
    setw('OrangeIsland_Sunburst_Cave_3F', [
        warp(*SUN3['to2F'], 'MAP_ORANGE_ISLANDS_SUNBURST_CAVE_2F', 2)])

    # Pummelo: the through-cave keeps both island mouths, its ladder goes up
    setw('OrangeIslands_Pummelo_Cave', existing('OrangeIslands_Pummelo_Cave')[:2] + [
        warp(25, 3, 'MAP_ORANGE_ISLANDS_PUMMELO_CAVE2', 0)])
    setw('OrangeIslands_Pummelo_Cave2', [
        warp(*PUM2['to1F'], 'MAP_ORANGE_ISLANDS_PUMMELO_CAVE', 2)])

    # Route 4's two mouths become a tunnel; Route 9's is a dead end
    setw('OrangeIslands_Route_4', [
        warp(18, 12, 'MAP_ORANGE_ISLANDS_ROUTE_4_CAVE', 0, elev=0),
        warp(57, 33, 'MAP_ORANGE_ISLANDS_ROUTE_4_CAVE', 1, elev=0)])
    setw('OrangeIslands_Route_4_Cave', [
        warp(*R4['west'], 'MAP_ORANGE_ISLANDS_ROUTE_4', 0),
        warp(*R4['east'], 'MAP_ORANGE_ISLANDS_ROUTE_4', 1)])
    setw('OrangeIslands_Route_9', [
        warp(9, 27, 'MAP_ORANGE_ISLANDS_ROUTE_9_CAVE', 0, elev=0)])
    setw('OrangeIslands_Route_9_Cave', [
        warp(*R9['mouth'], 'MAP_ORANGE_ISLANDS_ROUTE_9', 0)])
    print('  wired: Sunburst 3 floors, Pummelo 2 floors, 2 new route caves')
    return placed


if __name__ == '__main__':
    main(sys.argv[1] if len(sys.argv) > 1 else '.')


# a reward for walking to the end of each dead end
TREASURE = {
    'OrangeIsland_Sunburst_Cave_3F': ('chamber', 'ITEM_STAR_PIECE',
                                      'FLAG_HIDE_SUNBURST_CAVE_3F_STAR_PIECE'),
    'OrangeIslands_Pummelo_Cave2': ('deep', 'ITEM_MAX_REVIVE',
                                    'FLAG_HIDE_PUMMELO_CAVE_2F_MAX_REVIVE'),
    'OrangeIslands_Route_9_Cave': ('deep', 'ITEM_NUGGET',
                                   'FLAG_HIDE_ROUTE_9_CAVE_NUGGET'),
}
TREASURE_FLAGS = [(0x226, 'FLAG_HIDE_SUNBURST_CAVE_3F_STAR_PIECE'),
                  (0x227, 'FLAG_HIDE_PUMMELO_CAVE_2F_MAX_REVIVE'),
                  (0x228, 'FLAG_HIDE_ROUTE_9_CAVE_NUGGET')]


def add_treasure(repo, placed):
    p = os.path.join(repo, 'include/constants/flags.h')
    s = open(p).read()
    for num, name in TREASURE_FLAGS:
        if name not in s:
            s = s.replace('#define FLAG_0x%03X               0x%03X' % (num, num),
                          '#define %-37s 0x%03X' % (name, num))
    open(p, 'w').write(s)

    for mapname, (feature, item, flag) in TREASURE.items():
        x, y = placed[mapname][feature]
        script = '%s_EventScript_Item%s' % (
            mapname, ''.join(w.capitalize() for w in item[5:].split('_')))
        p = os.path.join(repo, 'data/maps/%s/map.json' % mapname)
        j = json.load(open(p))
        j['object_events'] = [o for o in j['object_events'] if o['script'] != script] + [{
            'type': 'object', 'graphics_id': 'OBJ_EVENT_GFX_ITEM_BALL',
            'x': x, 'y': y, 'elevation': 3,
            'movement_type': 'MOVEMENT_TYPE_FACE_DOWN',
            'movement_range_x': 1, 'movement_range_y': 1,
            'trainer_type': 'TRAINER_TYPE_NONE',
            'trainer_sight_or_berry_tree_id': '0', 'script': script, 'flag': flag}]
        json.dump(j, open(p, 'w'), indent=2)
        open(p, 'a').write('\n')
        sp = os.path.join(repo, 'data/maps/%s/scripts.inc' % mapname)
        body = open(sp).read()
        if script not in body:
            open(sp, 'w').write(body.rstrip('\n') +
                                '\n\n%s::\n\tfinditem %s\n\tend\n' % (script, item))
        print('  %-34s %s at (%d,%d)' % (mapname, item, x, y))
