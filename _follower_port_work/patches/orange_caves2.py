#!/usr/bin/env python3
"""The rest of the Orange Islands cave mouths.

PUMMELO's two remaining mouths are not on the main island at all: (39,55) sits
in a 16-tile cove and (26,65) in a 42-tile one, both cut off from everywhere you
can walk. You reach them by Surf. They are now the two ends of a three-floor
system that also opens onto PUMMELO BEACH -- a genuine back network under the
champion's island, which is what the mouths were clearly drawn for.

The island's existing cave is left as it is: (21,24) up to (12,9) already climbs
from the Centre plateau to the stadium, so the route to DRAKE was already there.

KUMQUAT's mouth and KUMQUAT BEACH's are both on their maps' main walkable area,
so those become a through-cave: a second way between the island and its beach.

Also removes OrangeIsland_Pummelo_Cave_2F, the duplicate blank that was filed
under the Sunburst group by mistake. NOTE: it is not last in that group, so this
shifts the map numbers of the Sunburst Pokemon Center 1F/2F. Flags and heal
tables are resolved at build time and are fine; save files made before this are
not.
"""
import json, os, shutil, sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from cavegen import build
from orange_caves import write_layout, map_header, warp, layout_id, TS

DEAD = 'OrangeIsland_Pummelo_Cave_2F'

GENERATED = {
    'OrangeIslands_Pummelo_Secret_Cave':    (42, 30, 20251110, 8,
        [('cove_north', (33, 6)), ('cove_south', (9, 24)), ('beach', (6, 6)), ('down', (22, 20))]),
    'OrangeIslands_Pummelo_Secret_Cave_B1F': (40, 34, 20251111, 9,
        [('up', (8, 7)), ('down', (31, 27)), ('stash', (30, 7))]),
    'OrangeIslands_Pummelo_Secret_Cave_B2F': (34, 26, 20251112, 6,
        [('up', (6, 6)), ('vault', (26, 20))]),
    'OrangeIslands_Kumquat_Cave':          (44, 24, 20251113, 8,
        [('island', (7, 12)), ('beach', (36, 12)), ('nook', (22, 5))]),
}
NEW_MAPS = {
    'OrangeIslands_Pummelo_Secret_Cave': 'OrangeIslands_Pummelo',
    'OrangeIslands_Pummelo_Secret_Cave_B1F': 'OrangeIslands_Pummelo',
    'OrangeIslands_Pummelo_Secret_Cave_B2F': 'OrangeIslands_Pummelo',
    'OrangeIslands_Kumquat_Cave': 'OrangeIslands_Kumquat',
}
GROUPS = {'OrangeIslands_Pummelo_Secret_Cave': 'gMapGroup_Orange_Pummelo_Interior',
          'OrangeIslands_Pummelo_Secret_Cave_B1F': 'gMapGroup_Orange_Pummelo_Interior',
          'OrangeIslands_Pummelo_Secret_Cave_B2F': 'gMapGroup_Orange_Pummelo_Interior',
          'OrangeIslands_Kumquat_Cave': 'gMapGroup_Orange_Kumquat_Interior'}
TREASURE = [('OrangeIslands_Pummelo_Secret_Cave_B1F', 'stash', 'ITEM_RARE_CANDY',
             'FLAG_HIDE_PUMMELO_SECRET_CAVE_B1F_RARE_CANDY', 0x229),
            ('OrangeIslands_Pummelo_Secret_Cave_B2F', 'vault', 'ITEM_PP_MAX',
             'FLAG_HIDE_PUMMELO_SECRET_CAVE_B2F_PP_MAX', 0x22A),
            ('OrangeIslands_Kumquat_Cave', 'nook', 'ITEM_BIG_PEARL',
             'FLAG_HIDE_KUMQUAT_CAVE_BIG_PEARL', 0x22B)]
LADDER_FEATURES = {'cove_north', 'cove_south', 'beach', 'down', 'up', 'island'}


def remove_dead_map(repo):
    """Drop the duplicate blank. Only safe to do wholesale because nothing
    points at it; it still shifts later maps in its group, which is why the
    module docstring says so."""
    g = os.path.join(repo, 'data/maps/map_groups.json')
    j = json.load(open(g))
    for k, v in list(j.items()):
        if k != 'group_order' and DEAD in v:
            v.remove(DEAD)
    json.dump(j, open(g, 'w'), indent=2)
    open(g, 'a').write('\n')

    p = os.path.join(repo, 'data/layouts/layouts.json')
    L = json.load(open(p))
    L['layouts'] = [l for l in L['layouts'] if l['name'] != DEAD + '_Layout']
    json.dump(L, open(p, 'w'), indent=2)
    open(p, 'a').write('\n')

    e = os.path.join(repo, 'data/event_scripts.s')
    s = open(e).read()
    s = '\n'.join(l for l in s.split('\n') if DEAD not in l)
    open(e, 'w').write(s)

    for d in ('data/maps/%s' % DEAD, 'data/layouts/%s' % DEAD):
        full = os.path.join(repo, d)
        if os.path.isdir(full):
            shutil.rmtree(full)
    print('  removed the duplicate blank %s' % DEAD)


def main(repo='.'):
    remove_dead_map(repo)
    placed = {}
    for name, (w, h, seed, rooms, feats) in GENERATED.items():
        f, pos = build(w, h, seed, rooms, feats)
        ladders = [v for k, v in pos.items() if k in LADDER_FEATURES]
        write_layout(repo, name, w, h, f.blocks(ladders))
        placed[name] = pos
        print('  %-40s %2dx%-2d %4d open  %s'
              % (name, w, h, len(f.open),
                 ' '.join('%s@%d,%d' % (k, v[0], v[1]) for k, v in pos.items())))

    for name, parent in NEW_MAPS.items():
        mapsec = json.load(open(os.path.join(
            repo, 'data/maps/%s/map.json' % parent)))['region_map_section']
        d = os.path.join(repo, 'data/maps', name)
        os.makedirs(d, exist_ok=True)
        if not os.path.exists(os.path.join(d, 'map.json')):
            json.dump(map_header(name, mapsec, 'MUS_SEVII_CAVE', layout_id(repo, name)),
                      open(os.path.join(d, 'map.json'), 'w'), indent=2)
            open(os.path.join(d, 'map.json'), 'a').write('\n')
        for fn, body in (('scripts.inc', '%s_MapScripts::\n\t.byte 0\n' % name), ('text.inc', '')):
            if not os.path.exists(os.path.join(d, fn)):
                open(os.path.join(d, fn), 'w').write(body)
    G = os.path.join(repo, 'data/maps/map_groups.json')
    g = json.load(open(G))
    for name, grp in GROUPS.items():
        if name not in g[grp]:
            g[grp].append(name)
    json.dump(g, open(G, 'w'), indent=2)
    open(G, 'a').write('\n')
    E = os.path.join(repo, 'data/event_scripts.s')
    s = open(E).read()
    extra = []
    for name in NEW_MAPS:
        if 'data/maps/%s/scripts.inc' % name not in s:
            extra += ['\t.include "data/maps/%s/scripts.inc"' % name,
                      '\t.include "data/maps/%s/text.inc"' % name, '']
    if extra:
        open(E, 'w').write(s.rstrip('\n') + '\n\n' + '\n'.join(extra).rstrip('\n') + '\n')

    def setw(m, warps):
        p = os.path.join(repo, 'data/maps/%s/map.json' % m)
        j = json.load(open(p))
        j['warp_events'] = warps
        json.dump(j, open(p, 'w'), indent=2)
        open(p, 'a').write('\n')

    def cur(m):
        return json.load(open(os.path.join(repo, 'data/maps/%s/map.json' % m)))['warp_events']

    S1 = placed['OrangeIslands_Pummelo_Secret_Cave']
    SB1 = placed['OrangeIslands_Pummelo_Secret_Cave_B1F']
    SB2 = placed['OrangeIslands_Pummelo_Secret_Cave_B2F']
    KC = placed['OrangeIslands_Kumquat_Cave']
    # the MAP_ constant is derived from the directory name, so read it back
    # rather than spelling it out -- writing SECRET_CAVE while the name was
    # SecretCave produced an undefined symbol the warp checker did not catch.
    SC = json.load(open(os.path.join(
        repo, 'data/maps/OrangeIslands_Pummelo_Secret_Cave/map.json')))['id']

    # Pummelo keeps its four existing warps; the two coves are appended as 4 and 5
    setw('OrangeIslands_Pummelo', cur('OrangeIslands_Pummelo') + [
        warp(39, 55, SC, 0, elev=0), warp(26, 65, SC, 1, elev=0)])
    setw('OrangeIslands_Pummelo_Beach', cur('OrangeIslands_Pummelo_Beach') + [
        warp(66, 11, SC, 2, elev=0)])
    setw('OrangeIslands_Pummelo_Secret_Cave', [
        warp(*S1['cove_north'], 'MAP_ORANGE_ISLANDS_PUMMELO', 4),
        warp(*S1['cove_south'], 'MAP_ORANGE_ISLANDS_PUMMELO', 5),
        warp(*S1['beach'], 'MAP_ORANGE_ISLANDS_PUMMELO_BEACH',
             len(cur('OrangeIslands_Pummelo_Beach')) - 1),
        warp(*S1['down'], SC + '_B1F', 0)])
    setw('OrangeIslands_Pummelo_Secret_Cave_B1F', [
        warp(*SB1['up'], SC, 3),
        warp(*SB1['down'], SC + '_B2F', 0)])
    setw('OrangeIslands_Pummelo_Secret_Cave_B2F', [warp(*SB2['up'], SC + '_B1F', 1)])

    setw('OrangeIslands_Kumquat', cur('OrangeIslands_Kumquat') + [
        warp(17, 45, 'MAP_ORANGE_ISLANDS_KUMQUAT_CAVE', 0, elev=0)])
    setw('OrangeIslands_Kumquat_Beach', cur('OrangeIslands_Kumquat_Beach') + [
        warp(76, 23, 'MAP_ORANGE_ISLANDS_KUMQUAT_CAVE', 1, elev=0)])
    setw('OrangeIslands_Kumquat_Cave', [
        warp(*KC['island'], 'MAP_ORANGE_ISLANDS_KUMQUAT',
             len(cur('OrangeIslands_Kumquat')) - 1),
        warp(*KC['beach'], 'MAP_ORANGE_ISLANDS_KUMQUAT_BEACH',
             len(cur('OrangeIslands_Kumquat_Beach')) - 1)])

    # treasure
    fp = os.path.join(repo, 'include/constants/flags.h')
    fs = open(fp).read()
    for _m, _f, _i, flag, num in TREASURE:
        if flag not in fs:
            fs = fs.replace('#define FLAG_0x%03X               0x%03X' % (num, num),
                            '#define %-44s 0x%03X' % (flag, num))
    open(fp, 'w').write(fs)
    for mapname, feature, item, flag, _n in TREASURE:
        x, y = placed[mapname][feature]
        script = '%s_EventScript_Item%s' % (
            mapname, ''.join(w.capitalize() for w in item[5:].split('_')))
        p = os.path.join(repo, 'data/maps/%s/map.json' % mapname)
        jj = json.load(open(p))
        jj['object_events'] = [o for o in jj['object_events'] if o['script'] != script] + [{
            'type': 'object', 'graphics_id': 'OBJ_EVENT_GFX_ITEM_BALL', 'x': x, 'y': y,
            'elevation': 3, 'movement_type': 'MOVEMENT_TYPE_FACE_DOWN',
            'movement_range_x': 1, 'movement_range_y': 1, 'trainer_type': 'TRAINER_TYPE_NONE',
            'trainer_sight_or_berry_tree_id': '0', 'script': script, 'flag': flag}]
        json.dump(jj, open(p, 'w'), indent=2)
        open(p, 'a').write('\n')
        sp = os.path.join(repo, 'data/maps/%s/scripts.inc' % mapname)
        body = open(sp).read()
        if script not in body:
            open(sp, 'w').write(body.rstrip('\n') + '\n\n%s::\n\tfinditem %s\n\tend\n' % (script, item))
        print('  %-40s %s at (%d,%d)' % (mapname, item, x, y))
    return placed


if __name__ == '__main__':
    main(sys.argv[1] if len(sys.argv) > 1 else '.')
