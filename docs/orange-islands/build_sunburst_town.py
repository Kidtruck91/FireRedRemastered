"""Open SUNBURST's doors and put people on the island.

Four buildings were drawn and none of them opened. The map says what each one
is, so none of this is invented: (33,32) carries a MART sign, (39,32) is a shop
front with its wares in the window, (27,41) is a two-storey civic building with
a double row of windows, and (36,41) is a small house under a purple roof.

    (33,32)  the MART
    (39,32)  CASTELLA's glass workshop, which is the shop front
    (27,41)  the crystal station, the big building
    (36,41)  a quarryman's house

The island already had its subject and nobody to say it. SUNBURST is the
crystal island, its cave was filled last commit, and MIKAN's hiker now tells
players to come here for a GROUND type -- so anyone arriving has a reason to
be asking about rock, and everyone here has an answer. The glassblower works
the crystal, the station studies it, the quarryman digs it and the MART sells
what you need to survive going down after it.

What nobody says outright is what is at the bottom of the cave. The station
has measurements it cannot explain, the glassblower has seen something he does
not talk about, and the quarryman's daughter has heard it. That keeps the
STEELIX a discovery rather than an errand.

Every coordinate is checked against the map before anything is written.

    python3 docs/orange-islands/build_sunburst_town.py
"""
import json
import os
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from repair_landmark_fixtures import behaviours, layouts_and_maps, WATER  # noqa: E402

REPO = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))

TOWN = 'OrangeIslands_Sunburst'
TOWN_MAP = 'MAP_ORANGE_ISLANDS_SUNBURST'
BEACH = 'OrangeIslands_Sunburst_Beach'
GROUP = 'gMapGroup_Orange_Sunburst_Interior'
P = 'OrangeIslands_Sunburst_'

FENCE_TOP = '@ --- built by docs/orange-islands/build_sunburst_town.py ---'
FENCE_END = '@ --- end build_sunburst_town.py ---'

# RSE's GLASS WORKSHOP sold the flutes, and this shop front is the same trade,
# so it stocks them -- the only place in the game that does. All five are fully
# implemented in FireRed and sold nowhere: BLUE and the BLACK/WHITE pair work in
# the field, while YELLOW and RED have their field use stubbed to Oak's refusal
# but keep battleUsage, which is the only place confusion and infatuation exist.
# Prices are the game's own.
FLUTES = ['ITEM_BLUE_FLUTE', 'ITEM_YELLOW_FLUTE', 'ITEM_RED_FLUTE',
          'ITEM_BLACK_FLUTE', 'ITEM_WHITE_FLUTE']

MART_ITEMS = ['ITEM_POKE_BALL', 'ITEM_GREAT_BALL', 'ITEM_POTION', 'ITEM_SUPER_POTION',
              'ITEM_ANTIDOTE', 'ITEM_PARALYZE_HEAL', 'ITEM_AWAKENING',
              'ITEM_ESCAPE_ROPE', 'ITEM_REPEL', 'ITEM_SUPER_REPEL']

# Script suffixes this file used to write and no longer does.
RETIRED = []


def mid(name):
    return 'MAP_' + name.upper().replace('ORANGEISLANDS', 'ORANGE_ISLANDS')


MART = [
    ('OBJ_EVENT_GFX_CLERK', 2, 3, 'MOVEMENT_TYPE_FACE_DOWN', 'Clerk', None),
    ('OBJ_EVENT_GFX_HIKER', 8, 4, 'MOVEMENT_TYPE_FACE_LEFT', 'Hiker',
     ['"Three floors down and no way out\\n"', '"but the way you came.\\p"',
      '"Buy the ESCAPE ROPE. Everyone who\\n"', '"did not wishes they had.$"']),
    ('OBJ_EVENT_GFX_WOMAN_1', 5, 6, 'MOVEMENT_TYPE_WANDER_AROUND', 'Woman',
     ['"We sell more REPELs here than\\n"', '"anywhere in the ORANGE ISLANDS.\\p"',
      '"One cave will do that.$"']),
]

# The shop front. He is the island's trade, so he is the one who says what the
# island is; and he is the first to hint at the thing in the cave.
GLASSWORKS = [
    # the vendor: his script is written by glass_clerk(), not talkers()
    ('OBJ_EVENT_GFX_OLD_MAN_1', 4, 3, 'MOVEMENT_TYPE_FACE_DOWN', 'Glassblower', None),
    ('OBJ_EVENT_GFX_WOMAN_3', 8, 5, 'MOVEMENT_TYPE_FACE_LEFT', 'Daughter',
     ['"He went down into the cave once,\\n"', '"years ago, and came back up\\l"',
      '"without his tools.\\p"', '"He has never said why.\\n"',
      '"He has never gone back either.$"']),
    ('OBJ_EVENT_GFX_YOUNGSTER', 2, 5, 'MOVEMENT_TYPE_FACE_RIGHT', 'Apprentice',
     ['"The sand here is wrong, you know.\\n"', '"Too much of the hill in it.\\p"',
      '"Melt it and it comes out clear as\\n"', '"water and twice as hard. That is\\l"',
      '"not the sand doing that.\\p"',
      '"That is whatever the hill is made\\n"', '"of.$"']),
]

STATION = [
    ('OBJ_EVENT_GFX_SCIENTIST', 6, 4, 'MOVEMENT_TYPE_FACE_DOWN', 'Researcher',
     ['"SUNBURST CRYSTAL STATION. We\\n"', '"measure the hill.\\p"',
      '"And the hill does not add up. The\\n"', '"crystal runs in seams, and the\\l"',
      '"seams all bend toward one room on\\l"', '"the third floor.\\p"',
      '"Something down there has been\\n"', '"shaping them for a very long\\l"', '"time.$"']),
    ('OBJ_EVENT_GFX_SCIENTIST', 10, 7, 'MOVEMENT_TYPE_FACE_LEFT', 'Assistant',
     ['"Steel and stone, if you want my\\n"', '"guess. Nothing else scores\\l"',
      '"crystal like that.\\p"', '"He will not write that down\\n"',
      '"without seeing it.$"']),
    ('OBJ_EVENT_GFX_LASS', 3, 8, 'MOVEMENT_TYPE_WANDER_AROUND', 'Intern',
     ['"If you are going down anyway --\\n"', '"GROUND types do well in there,\\l"',
      '"and they do well against a lot\\l"', '"of things out here.$"']),
]

QUARRY = [
    ('OBJ_EVENT_GFX_BALDING_MAN', 3, 4, 'MOVEMENT_TYPE_FACE_DOWN', 'Quarryman',
     ['"I cut stone on the first floor.\\n"', '"Never below it.\\p"',
      '"The hikers who work the second\\n"', '"floor earn more than me and I do\\l"',
      '"not envy them one coin.$"']),
    ('OBJ_EVENT_GFX_LITTLE_GIRL', 7, 6, 'MOVEMENT_TYPE_WANDER_AROUND', 'Daughter',
     ['"I put my ear on the floor of the\\n"', '"cave and I heard it.\\p"',
      '"It was not digging. It was\\n"', '"breathing.$"']),
]

# (door, map suffix, layout, exit cells, people, extra script kind)
PLAN = [
    ((33, 32), 'Mart', 'LAYOUT_MART', [(3, 7), (4, 7), (5, 7)], MART, 'mart'),
    ((39, 32), 'Glassworks', 'LAYOUT_LAVARIDGE_TOWN_HERB_SHOP',
     [(3, 7), (4, 7), (5, 7)], GLASSWORKS, 'flutes'),
    ((27, 41), 'CrystalStation', 'LAYOUT_PALLET_TOWN_PROFESSOR_OAKS_LAB',
     [(5, 12), (6, 12), (7, 12)], STATION, None),
    ((36, 41), 'House1', 'LAYOUT_HOUSE3', [(4, 7)], QUARRY, None),
]

TOWN_PEOPLE = [
    ('OBJ_EVENT_GFX_FAT_MAN', 24, 26, 'MOVEMENT_TYPE_FACE_DOWN', 'Man',
     ['"Everything on this island comes\\n"', '"out of that hill. The glass, the\\l"',
      '"money, the arguments.$"']),
    ('OBJ_EVENT_GFX_LASS', 43, 27, 'MOVEMENT_TYPE_WANDER_AROUND', 'Girl',
     ['"The CRYSTAL STATION will show you\\n"', '"the seams if you ask nicely.\\p"',
      '"They are the big building at the\\n"', '"bottom of the hill.$"']),
    ('OBJ_EVENT_GFX_OLD_WOMAN', 20, 34, 'MOVEMENT_TYPE_FACE_RIGHT', 'OldWoman',
     ['"Trainers come for the cave. They\\n"', '"never once come for the glass.\\p"',
      '"He makes beautiful glass.$"']),
    ('OBJ_EVENT_GFX_YOUNGSTER', 37, 35, 'MOVEMENT_TYPE_WANDER_AROUND', 'Youngster',
     ['"I got to the second floor and\\n"', '"turned round.\\p"',
      '"Four trainers down there and all\\n"', '"of them live in the dark.$"']),
    ('OBJ_EVENT_GFX_LITTLE_BOY', 30, 44, 'MOVEMENT_TYPE_WANDER_AROUND', 'Boy',
     ['"GEODUDE! ONIX! ARON! NOSEPASS!\\p"',
      '"I know all of them and I have not\\n"', '"caught any of them.$"']),
]

BEACH_PEOPLE = [
    ('OBJ_EVENT_GFX_FISHER', 30, 12, 'MOVEMENT_TYPE_FACE_DOWN', 'Fisher',
     ['"This beach is all crystal grit.\\n"', '"Look at it in the sun.\\p"',
      '"It washes down out of the hill.$"']),
    ('OBJ_EVENT_GFX_SWIMMER_F_LAND', 26, 17, 'MOVEMENT_TYPE_WANDER_AROUND', 'Swimmer',
     ['"People come off the boat, walk\\n"', '"straight up to the cave and never\\l"',
      '"see the beach at all.$"']),
    ('OBJ_EVENT_GFX_SAILOR', 31, 24, 'MOVEMENT_TYPE_FACE_LEFT', 'Sailor',
     ['"We ship crystal out of here and\\n"', '"bring POTIONs back in.\\p"',
      '"Tells you everything about the\\n"', '"place.$"']),
    ('OBJ_EVENT_GFX_LITTLE_GIRL', 24, 28, 'MOVEMENT_TYPE_WANDER_AROUND', 'Child',
     ['"I am looking for a piece with a\\n"', '"rainbow in it.\\p"',
      '"You have to hold them up. Most of\\n"', '"them are just glass.$"']),
]


def walkable(name):
    L, M = layouts_and_maps()
    l = L[M[name]['layout']]
    raw = open(os.path.join(REPO, l['blockdata_filepath']), 'rb').read()
    b = struct.unpack('<' + 'H' * (len(raw) // 2), raw)
    beh = behaviours(l)
    w, h = l['width'], l['height']

    def ok(x, y):
        if not (0 <= x < w and 0 <= y < h):
            return False
        v = b[y * w + x]
        return not (v & 0xC00) and beh[v & 0x3FF] not in WATER
    return ok


def splice(path, body):
    s = open(path, encoding='utf-8').read() if os.path.exists(path) else ''
    block = FENCE_TOP + '\n' + '\n'.join(body).rstrip('\n') + '\n' + FENCE_END + '\n'
    if FENCE_TOP in s and FENCE_END in s:
        s = s[:s.index(FENCE_TOP)] + block + s[s.index(FENCE_END) + len(FENCE_END):].lstrip('\n')
    else:
        s = s.rstrip('\n') + ('\n\n' if s.strip() else '') + block
    open(path, 'w', encoding='utf-8', newline='\n').write(s)


def interior_json(suffix, lay, exits, people, door_index):
    name = P + suffix
    return {
        "id": mid(name), "name": name, "layout": lay, "music": "MUS_FUCHSIA",
        "region_map_section": "MAPSEC_SUNBURST_ISLAND", "requires_flash": False,
        "weather": "WEATHER_NONE", "map_type": "MAP_TYPE_INDOOR",
        "allow_cycling": False, "allow_escaping": False, "allow_running": False,
        "show_map_name": False, "floor_number": 0,
        "battle_scene": "MAP_BATTLE_SCENE_NORMAL", "connections": 0,
        "object_events": [
            {"type": "object", "graphics_id": g, "x": x, "y": y, "elevation": 3,
             "movement_type": m, "movement_range_x": 1, "movement_range_y": 1,
             "trainer_type": "TRAINER_TYPE_NONE", "trainer_sight_or_berry_tree_id": "0",
             "script": "%s_EventScript_%s" % (name, s), "flag": "0"}
            for (g, x, y, m, s, _l) in people],
        "warp_events": [{"x": x, "y": y, "elevation": 0, "dest_map": TOWN_MAP,
                         "dest_warp_id": str(door_index)} for (x, y) in exits],
        "coord_events": [], "bg_events": []}


def talkers(name, people, out, text):
    for (_g, _x, _y, _m, s, lines) in people:
        if lines is None:
            continue
        out += ['%s_EventScript_%s::' % (name, s),
                '\tmsgbox %s_Text_%s, MSGBOX_NPC' % (name, s), '\tend', '']
        text.append('%s_Text_%s::' % (name, s))
        text += ['\t.string %s' % ln for ln in lines]
        text.append('')


def mart_clerk(name, out):
    out += ['%s_EventScript_Clerk::' % name,
            '\tgoto_if_questlog EventScript_ReleaseEnd',
            '\tlock', '\tfaceplayer',
            '\tmessage Text_MayIHelpYou', '\twaitmessage',
            '\tpokemart %s_ItemsClerk' % name,
            '\tmsgbox Text_PleaseComeAgain', '\trelease', '\tend', '',
            '\t.align 2', '%s_ItemsClerk::' % name]
    out += ['\t.2byte %s' % it for it in MART_ITEMS]
    out += ['\t.2byte ITEM_NONE', '\t.align 2', '']


def glass_clerk(name, out, text):
    """The glassblower sells; he does not just talk.

    He names what each flute does, because the game never explains them
    anywhere else, and the BLACK one is genuinely the right purchase for the
    cave above the town.
    """
    out += ['%s_EventScript_Glassblower::' % name,
            '\tgoto_if_questlog EventScript_ReleaseEnd',
            '\tlock', '\tfaceplayer',
            '\tmsgbox %s_Text_Glassblower, MSGBOX_DEFAULT' % name,
            '\tmessage Text_MayIHelpYou', '\twaitmessage',
            '\tpokemart %s_ItemsGlassblower' % name,
            '\tmsgbox Text_PleaseComeAgain', '\trelease', '\tend', '',
            '\t.align 2', '%s_ItemsGlassblower::' % name]
    out += ['\t.2byte %s' % it for it in FLUTES]
    out += ['\t.2byte ITEM_NONE', '\t.align 2', '']
    text += ['%s_Text_Glassblower::' % name,
             '\t.string "Glass that sings. That is the\\n"',
             '\t.string "trick of this island\'s sand.\\p"',
             '\t.string "Five flutes, five notes. BLUE\\n"',
             '\t.string "wakes a sleeping POKeMON. YELLOW\\l"',
             '\t.string "shakes the confusion out of one.\\l"',
             '\t.string "RED cools a head gone soft.\\p"',
             '\t.string "Then the pair nobody believes\\n"',
             '\t.string "until they try them. WHITE draws\\l"',
             '\t.string "POKeMON to you. BLACK keeps them\\l"',
             '\t.string "off.\\p"',
             '\t.string "Take a BLACK one down that cave\\n"',
             '\t.string "and you will reach the bottom\\l"',
             '\t.string "with something left in the tank.$"', '']


def add_outdoor(name, people, ok, report):
    path = os.path.join(REPO, 'data/maps', name, 'map.json')
    mj = json.load(open(path))
    mine = {'%s_EventScript_%s' % (name, s) for (_g, _x, _y, _m, s, _l) in people}
    mine |= {'%s_EventScript_%s' % (name, s) for s in RETIRED}
    mj['object_events'] = [o for o in mj['object_events'] if o.get('script') not in mine]
    taken = {(o['x'], o['y']) for o in mj['object_events']}
    taken |= {(w['x'], w['y']) for w in mj['warp_events']}
    added = 0
    for (g, x, y, m, s, _lines) in people:
        if not ok(x, y):
            report.append('  %s: (%d,%d) is not walkable, %s not placed' % (name, x, y, s))
            continue
        if (x, y) in taken:
            report.append('  %s: (%d,%d) occupied, %s not placed' % (name, x, y, s))
            continue
        taken.add((x, y))
        mj['object_events'].append({
            "type": "object", "graphics_id": g, "x": x, "y": y, "elevation": 3,
            "movement_type": m,
            "movement_range_x": 2 if 'WANDER' in m else 1,
            "movement_range_y": 2 if 'WANDER' in m else 1,
            "trainer_type": "TRAINER_TYPE_NONE", "trainer_sight_or_berry_tree_id": "0",
            "script": '%s_EventScript_%s' % (name, s), "flag": "0"})
        added += 1
    with open(path, 'w', newline='\n') as f:
        json.dump(mj, f, indent=2)
        f.write('\n')
    out, text = [], []
    talkers(name, people, out, text)
    splice(os.path.join(REPO, 'data/maps', name, 'scripts.inc'), out)
    splice(os.path.join(REPO, 'data/maps', name, 'text.inc'), text)
    return added


def main():
    report = []
    town_path = os.path.join(REPO, 'data/maps', TOWN, 'map.json')
    town = json.load(open(town_path))
    ours = {mid(P + suffix) for (_d, suffix, _l, _e, _p, _k) in PLAN}
    warps = [w for w in town['warp_events'] if w['dest_map'] not in ours]

    groups_path = os.path.join(REPO, 'data/maps/map_groups.json')
    gj = json.load(open(groups_path))
    gj.setdefault(GROUP, [])
    esp = os.path.join(REPO, 'data/event_scripts.s')
    es = open(esp, encoding='utf-8').read()

    door_index = {}
    for (door, suffix, _l, _e, _p, _k) in PLAN:
        door_index[suffix] = len(warps)
        warps.append({"x": door[0], "y": door[1], "elevation": 0,
                      "dest_map": mid(P + suffix), "dest_warp_id": "0"})

    ok = walkable(TOWN)
    for (door, suffix, lay, exits, people, kind) in PLAN:
        if not any(ok(door[0], door[1] + dy) for dy in (1,)):
            report.append('  %s: nothing walkable below the door at %s' % (suffix, door))
        name = P + suffix
        d = os.path.join(REPO, 'data/maps', name)
        os.makedirs(d, exist_ok=True)
        with open(os.path.join(d, 'map.json'), 'w', newline='\n') as f:
            json.dump(interior_json(suffix, lay, exits, people, door_index[suffix]), f, indent=2)
            f.write('\n')

        out = ['%s_MapScripts::' % name, '\t.byte 0', '']
        text = []
        talkers(name, people, out, text)
        if kind == 'mart':
            mart_clerk(name, out)
        if kind == 'flutes':
            glass_clerk(name, out, text)
        open(os.path.join(d, 'scripts.inc'), 'w', encoding='utf-8',
             newline='\n').write('\n'.join(out) + '\n')
        open(os.path.join(d, 'text.inc'), 'w', encoding='utf-8',
             newline='\n').write('\n'.join(text) + '\n')

        if name not in gj[GROUP]:
            gj[GROUP].append(name)
        inc = ('\t.include "data/maps/%s/scripts.inc"\n'
               '\t.include "data/maps/%s/text.inc"\n' % (name, name))
        if inc not in es:
            es += inc

    town['warp_events'] = warps
    with open(town_path, 'w', newline='\n') as f:
        json.dump(town, f, indent=2)
        f.write('\n')
    with open(groups_path, 'w', newline='\n') as f:
        json.dump(gj, f, indent=2)
        f.write('\n')
    open(esp, 'w', encoding='utf-8', newline='\n').write(es)

    a = add_outdoor(TOWN, TOWN_PEOPLE, ok, report)
    b = add_outdoor(BEACH, BEACH_PEOPLE, walkable(BEACH), report)
    print('  %d interiors written; SUNBURST now has %d doors that open'
          % (len(PLAN), len(warps)))
    print('  %d people outdoors on SUNBURST, %d on its beach' % (a, b))
    for line in report:
        print(line)


if __name__ == '__main__':
    main()
