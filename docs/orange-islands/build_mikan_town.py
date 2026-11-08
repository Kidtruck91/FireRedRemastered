"""Open Mikan's doors and put people on the island.

Mikan was drawn as a town and wired as a corridor. Six doors, one of which went
anywhere -- the POKeMON CENTER -- and not a single person outdoors on either
map. The gym is on the beach, below the town, at the water.

    (37,46)  the MART
    (18,41)  the harbourmaster's house, north-west
    (41,41)  a wave-rider's house, north-east
    (19,55)  a retired Orange Crew sailor's house, south-west
    (40,56)  a family house, south-east

Each interior reuses a layout the game already ships, so no new tilesets and no
new graphics.

Cissy is out swimming, so the gym is shut until you find her. She is on the
sand at the inlet, and what she says is the thing a player most needs to hear
on the first island of the Orange League: the ORANGE CREW is four leaders and
a champion, each with their own flavour, and they do not all play by mainland
rules -- so talk to the gym guide before challenging any of them. That turns
the gym guide from scenery into the thing that tells you how to win, which on
Mikan matters: her gym is a surf maze and she fights in rain, where her THUNDER
never misses.

Everyone else is talking about the same island from a different angle. The pool
keeper in the north-east house is the one who explains the maze before you are
standing in it; the old sailor in the south-west tells you the NAVEL crossing
exists and which water to take.

Every coordinate is checked against the map before anything is written, so an
NPC can never end up standing in a wall.

    python3 docs/orange-islands/build_mikan_town.py
"""
import json
import os
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from repair_landmark_fixtures import behaviours, layouts_and_maps  # noqa: E402

REPO = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
TOWN = 'OrangeIslands_Mikan'
TOWN_MAP = 'MAP_ORANGE_ISLANDS_MIKAN'
BEACH = 'OrangeIslands_Mikan_Beach'
GROUP = 'gMapGroup_Orange_Mikan_Interior'
P = 'OrangeIslands_Mikan_'

MART_ITEMS = ['ITEM_POKE_BALL', 'ITEM_GREAT_BALL', 'ITEM_POTION', 'ITEM_SUPER_POTION',
              'ITEM_ANTIDOTE', 'ITEM_PARALYZE_HEAL', 'ITEM_AWAKENING', 'ITEM_BURN_HEAL',
              'ITEM_ESCAPE_ROPE', 'ITEM_REPEL']


def mid(name):
    return 'MAP_' + name.upper().replace('ORANGEISLANDS', 'ORANGE_ISLANDS')


# ---------------------------------------------------------------------------
# (graphics, x, y, movement, script suffix, lines or None)
# ---------------------------------------------------------------------------
MART = [
    ('OBJ_EVENT_GFX_CLERK', 2, 3, 'MOVEMENT_TYPE_FACE_DOWN', 'Clerk', None),
    ('OBJ_EVENT_GFX_YOUNGSTER', 8, 4, 'MOVEMENT_TYPE_FACE_LEFT', 'Youngster',
     ['"I keep taking FIRE types in to\\n"', '"CISSY. In her rain they barely\\l"',
      '"singe her.\\p"', '"I keep buying POTIONs about it.$"']),
    ('OBJ_EVENT_GFX_WOMAN_1', 5, 6, 'MOVEMENT_TYPE_WANDER_AROUND', 'Woman',
     ['"Stock up here. The next island is\\n"', '"a long way across open water and\\l"',
      '"there are no shops on the way.$"']),
]

HARBOUR = [
    ('OBJ_EVENT_GFX_SAILOR', 3, 3, 'MOVEMENT_TYPE_FACE_DOWN', 'Harbourmaster',
     ['"Harbourmaster, MIKAN. I log every\\n"', '"hull that ties up here.\\p"',
      '"Four gyms in the ORANGE LEAGUE,\\n"', '"and MIKAN is the one everyone\\l"',
      '"reaches first. The badges are not\\l"', '"in any order. The sea is.$"']),
    ('OBJ_EVENT_GFX_LITTLE_GIRL', 7, 5, 'MOVEMENT_TYPE_WANDER_AROUND', 'Daughter',
     ['"Dad knows the name of every boat.\\n"', '"He does not know my friends\'.$"']),
]

POOLKEEPER = [
    ('OBJ_EVENT_GFX_SWIMMER_M_LAND', 4, 3, 'MOVEMENT_TYPE_FACE_DOWN', 'PoolKeeper',
     ['"I keep CISSY\'s pool. You will not\\n"', '"walk to her -- you SURF, and the\\l"',
      '"water does the steering.\\p"',
      '"Three rocks between the door and\\n"', '"her LEADER\'s stand, each one\\l"',
      '"behind its own channel.\\p"',
      '"Pick the wrong column and the\\n"', '"outflow puts you back at the\\l"',
      '"bottom. It is not cruel. It is\\l"', '"just how water works.$"']),
    ('OBJ_EVENT_GFX_BEAUTY', 8, 6, 'MOVEMENT_TYPE_FACE_LEFT', 'Partner',
     ['"He rode that maze all summer and\\n"', '"she still beat him in the rain.$"']),
]

VETERAN = [
    ('OBJ_EVENT_GFX_OLD_MAN_2', 3, 4, 'MOVEMENT_TYPE_FACE_DOWN', 'Veteran',
     ['"ORANGE CREW, thirty years. I kept\\n"', '"the light on NAVEL.\\p"',
      '"Due east of MIKAN and a long swim\\n"', '"short of comfortable. Take the\\l"',
      '"southern water, not the strait.$"']),
    ('OBJ_EVENT_GFX_FISHER', 7, 3, 'MOVEMENT_TYPE_FACE_LEFT', 'Fisher',
     ['"He has told me about NAVEL every\\n"', '"evening for eleven years.\\p"',
      '"He is right every time.$"']),
]

FAMILY = [
    ('OBJ_EVENT_GFX_WOMAN_3', 4, 3, 'MOVEMENT_TYPE_FACE_DOWN', 'Mother',
     ['"Our boy is out at the gym again.\\n"', '"He cannot beat CISSY and he will\\l"',
      '"not stop trying.$"']),
    ('OBJ_EVENT_GFX_LITTLE_BOY', 8, 5, 'MOVEMENT_TYPE_WANDER_AROUND', 'Brother',
     ['"I have watched CISSY all summer.\\p"', '"Nobody ever seems to hit her.\\n"',
      '"They just keep swinging and\\l"', '"missing until it is over.$"']),
]

# (door, map suffix, layout, exit cells, people, extra script kind)
PLAN = [
    ((37, 46), 'Mart', 'LAYOUT_MART', [(3, 7), (4, 7), (5, 7)], MART, 'mart'),
    ((18, 41), 'House1', 'LAYOUT_HOUSE1', [(3, 7), (4, 7), (5, 7)], HARBOUR, None),
    ((41, 41), 'House2', 'LAYOUT_HOUSE5', [(3, 7), (4, 7), (5, 7)], POOLKEEPER, None),
    ((19, 55), 'House3', 'LAYOUT_HOUSE3', [(4, 7)], VETERAN, None),
    ((40, 56), 'House4', 'LAYOUT_HOUSE4', [(3, 7)], FAMILY, None),
]

# The island itself, which had nobody on it at all.
TOWN_PEOPLE = [
    ('OBJ_EVENT_GFX_LASS', 26, 48, 'MOVEMENT_TYPE_FACE_UP', 'Girl',
     ['"The POKeMON CENTER, the MART and\\n"', '"four houses. That is MIKAN.\\p"',
      '"Everything else is the gym, and\\n"', '"the gym is down on the sand.$"']),
    ('OBJ_EVENT_GFX_FAT_MAN', 32, 44, 'MOVEMENT_TYPE_FACE_LEFT', 'Man',
     ['"Trainers come off the boat, lose\\n"', '"to CISSY, and buy a lot of\\l"',
      '"POTIONs they do not need.$"']),
    ('OBJ_EVENT_GFX_YOUNGSTER', 30, 36, 'MOVEMENT_TYPE_WANDER_AROUND', 'Youngster',
     ['"North of here is all water. The\\n"', '"ORANGE ISLANDS are mostly water,\\l"',
      '"when you look at a chart.$"']),
    ('OBJ_EVENT_GFX_OLD_WOMAN', 14, 48, 'MOVEMENT_TYPE_FACE_RIGHT', 'OldWoman',
     ['"The orchards to the south grow\\n"', '"the fruit the island is named\\l"',
      '"for. Mind the trainers in them.$"']),
    ('OBJ_EVENT_GFX_LITTLE_BOY', 33, 57, 'MOVEMENT_TYPE_WANDER_AROUND', 'Boy',
     ['"I am going to be in the ORANGE\\n"', '"CREW. You have to beat all four\\l"',
      '"gyms first. I have beaten none.$"']),
]

# Cissy is out swimming, so the gym is shut until the player finds her. She
# stands on the sand at the inlet, facing the water she has just come out of.
CISSY = ('OBJ_EVENT_GFX_CISSY', 44, 24, 'MOVEMENT_TYPE_FACE_DOWN', 'Cissy', None)

BEACH_PEOPLE = [
    CISSY,
    # beside the door's approach rather than in it, so he reads as a bystander
    ('OBJ_EVENT_GFX_GENTLEMAN', 49, 19, 'MOVEMENT_TYPE_FACE_LEFT', 'Spectator',
     ['"CISSY tells every challenger the\\n"', '"same thing. Talk to the GYM GUIDE\\l"',
      '"before you go up.\\p"',
      '"They hear it, walk straight past\\n"', '"him, and lose in the rain.$"']),
    ('OBJ_EVENT_GFX_SWIMMER_F_LAND', 34, 22, 'MOVEMENT_TYPE_WANDER_AROUND', 'Swimmer',
     ['"She swims the inlet most mornings\\n"', '"and leaves the gym shut.\\p"',
      '"Nobody minds. She has never lost\\l"', '"a challenge she turned up for.$"']),
    ('OBJ_EVENT_GFX_LITTLE_GIRL', 30, 11, 'MOVEMENT_TYPE_WANDER_AROUND', 'Child',
     ['"The gym is full of water. All the\\n"', '"way to the top!\\p"',
      '"Mum says I can go in when I can\\n"', '"swim it.$"']),
    ('OBJ_EVENT_GFX_FISHER', 52, 24, 'MOVEMENT_TYPE_FACE_LEFT', 'Fisher',
     ['"Everyone watches the gym. Nobody\\n"', '"watches the water.\\p"',
      '"There is plenty in it.$"']),
]

# The tile the player must cross to reach the gym door at (46,17).
GYM_STEP = (46, 18)

CISSY_FLAG = 'FLAG_ORANGE_CISSY_MET'

# Script suffixes this file used to write and no longer does.
RETIRED = ['GymGuide', 'Rider']

CISSY_LINES = {
    'CissyMeet': [
        '"Hey! Were you looking for the\\n"', '"gym? It\'s shut. That\'s my fault.\\p"',
        '"I\'m CISSY, MIKAN\'s LEADER. The\\n"', '"water was too good to waste on\\l"',
        '"sitting indoors.\\p"',
        '"Before you challenge any of us,\\n"', '"here\'s what you should know.\\p"',
        '"The ORANGE CREW is four LEADERS\\n"', '"and our champ, DRAKE. We\'ve each\\l"',
        '"got our own flavour, and we don\'t\\l"', '"all play by the same rules the\\l"',
        '"mainland LEADERS do.\\p"',
        '"So talk to our GYM GUIDES. Every\\n"', '"one of us has one, and they\'ll\\l"',
        '"give you the scoop on how that\\l"', '"gym rolls.\\p"',
        '"Mine\'s just inside the door. Go\\n"', '"on -- I\'ll dry off and meet you\\l"',
        '"at the top of the pool.$"'],
    'CissyWaiting': [
        '"Still down here? The pool\'s\\n"', '"waiting, and so am I.\\p"',
        '"Talk to my GYM GUIDE on the way\\n"', '"in. That\'s what he\'s for.$"'],
    'CissyBeaten': [
        '"One down. Three LEADERS and DRAKE\\n"', '"to go.\\p"',
        '"Try NAVEL next. DANNY runs his\\n"', '"gym nothing like I run mine --\\l"',
        '"ask his GYM GUIDE before you find\\l"', '"that out the hard way.$"'],
    'GymShut': [
        '"The door is shut.\\p"',
        '"A card is wedged in the handle:\\n"', '"GONE SWIMMING. -- C.$"'],
}


# ---------------------------------------------------------------------------


def walkable(name):
    """Where a person can actually stand, read off the map."""
    L, M = layouts_and_maps()
    l = L[M[name]['layout']]
    raw = open(os.path.join(REPO, l['blockdata_filepath']), 'rb').read()
    b = struct.unpack('<' + 'H' * (len(raw) // 2), raw)
    beh = behaviours(l)
    w, h = l['width'], l['height']
    water = {0x10, 0x11, 0x12, 0x13, 0x15, 0x17, 0x50, 0x51, 0x52, 0x53}

    def ok(x, y):
        if not (0 <= x < w and 0 <= y < h):
            return False
        v = b[y * w + x]
        return not (v & 0xC00) and beh[v & 0x3FF] not in water
    return ok


def interior_json(suffix, lay, exits, people, door_index):
    name = P + suffix
    return {
        "id": mid(name), "name": name, "layout": lay, "music": "MUS_FUCHSIA",
        "region_map_section": "MAPSEC_MIKAN_ISLAND", "requires_flash": False,
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


FENCE_TOP = '@ --- built by docs/orange-islands/build_mikan_town.py ---'
FENCE_END = '@ --- end build_mikan_town.py ---'


def splice(path, body):
    """Replace this script's own block in a file, or append it.

    Everything between the two fences belongs to this script, so an edited line
    or a renamed person takes effect on the next run instead of being appended
    beside the version it replaced.
    """
    s = open(path, encoding='utf-8').read() if os.path.exists(path) else ''
    block = FENCE_TOP + '\n' + '\n'.join(body).rstrip('\n') + '\n' + FENCE_END + '\n'
    if FENCE_TOP in s and FENCE_END in s:
        head = s[:s.index(FENCE_TOP)]
        tail = s[s.index(FENCE_END) + len(FENCE_END):].lstrip('\n')
        s = head + block + tail
    else:
        s = s.rstrip('\n') + ('\n\n' if s.strip() else '') + block
    open(path, 'w', encoding='utf-8', newline='\n').write(s)


def beach_scripts(name, out, text):
    """Cissy on the sand, and the gym door that stays shut until you find her."""
    out += [
        '@ The gym is shut while CISSY is in the water. Finding her is what',
        '@ opens it, and what she says is the point of the detour.',
        '%s_EventScript_Cissy::' % name,
        '\tlock', '\tfaceplayer',
        '\tgoto_if_set FLAG_BADGE_CORAL_EYE, %s_EventScript_CissyBeaten' % name,
        '\tgoto_if_set %s, %s_EventScript_CissyWaiting' % (CISSY_FLAG, name),
        '\tmsgbox %s_Text_CissyMeet, MSGBOX_DEFAULT' % name,
        '\tsetflag %s' % CISSY_FLAG,
        '\trelease', '\tend', '',
        '%s_EventScript_CissyWaiting::' % name,
        '\tmsgbox %s_Text_CissyWaiting, MSGBOX_DEFAULT' % name,
        '\trelease', '\tend', '',
        '%s_EventScript_CissyBeaten::' % name,
        '\tmsgbox %s_Text_CissyBeaten, MSGBOX_DEFAULT' % name,
        '\trelease', '\tend', '',
        '@ One step in front of the door. The flag check comes before the lock,',
        '@ so once the gym is open the player walks through without a hitch.',
        '%s_EventScript_GymShut::' % name,
        '\tgoto_if_set %s, %s_EventScript_GymOpen' % (CISSY_FLAG, name),
        '\tlockall',
        '\tmsgbox %s_Text_GymShut, MSGBOX_DEFAULT' % name,
        '\tapplymovement OBJ_EVENT_ID_PLAYER, %s_Movement_StepBack' % name,
        '\twaitmovement 0',
        '\treleaseall', '\tend', '',
        '%s_EventScript_GymOpen::' % name,
        '\tend', '',
        '%s_Movement_StepBack::' % name,
        '\twalk_down',
        '\tstep_end', '']
    for tag, lines in CISSY_LINES.items():
        text.append('%s_Text_%s::' % (name, tag))
        text += ['\t.string %s' % ln for ln in lines]
        text.append('')


def add_outdoor(name, people, ok, report):
    """Put the listed people on an existing map, after checking each cell."""
    path = os.path.join(REPO, 'data/maps', name, 'map.json')
    mj = json.load(open(path))
    # drop the people this script owns and lay them down again, so a re-run
    # picks up an edited line or a moved coordinate instead of doing nothing
    mine = {'%s_EventScript_%s' % (name, s) for (_g, _x, _y, _m, s, _l) in people}
    # People this script used to create and has since renamed or dropped. The
    # object outlives the script it points at, so without this the map keeps a
    # dangling reference and the link fails.
    mine |= {'%s_EventScript_%s' % (name, s) for s in RETIRED}
    mj['object_events'] = [o for o in mj['object_events'] if o.get('script') not in mine]
    taken = {(o['x'], o['y']) for o in mj['object_events']}
    added = 0
    for (g, x, y, m, s, lines) in people:
        label = '%s_EventScript_%s' % (name, s)
        if not ok(x, y):
            report.append('  %s: (%d,%d) is not walkable, %s not placed' % (name, x, y, s))
            continue
        if (x, y) in taken:
            report.append('  %s: (%d,%d) already occupied, %s not placed' % (name, x, y, s))
            continue
        taken.add((x, y))
        mj['object_events'].append({
            "type": "object", "graphics_id": g, "x": x, "y": y, "elevation": 3,
            "movement_type": m, "movement_range_x": 1 if 'WANDER' not in m else 2,
            "movement_range_y": 1 if 'WANDER' not in m else 2,
            "trainer_type": "TRAINER_TYPE_NONE", "trainer_sight_or_berry_tree_id": "0",
            "script": label, "flag": "0"})
        added += 1

    if name == BEACH:
        label = '%s_EventScript_GymShut' % name
        mj['coord_events'] = [e for e in mj['coord_events']
                              if e.get('script') != label]
        mj['coord_events'].append({
            "type": "trigger", "x": GYM_STEP[0], "y": GYM_STEP[1], "elevation": 0,
            "var": "VAR_TEMP_0", "var_value": "0", "script": label})

    with open(path, 'w', newline='\n') as f:
        json.dump(mj, f, indent=2)
        f.write('\n')

    d = os.path.join(REPO, 'data/maps', name)
    out, text = [], []
    talkers(name, people, out, text)
    if name == BEACH:
        beach_scripts(name, out, text)
    splice(os.path.join(d, 'scripts.inc'), out)
    splice(os.path.join(d, 'text.inc'), text)
    return added


def main():
    report = []
    town_path = os.path.join(REPO, 'data/maps', TOWN, 'map.json')
    town = json.load(open(town_path))
    # The POKeMON CENTER stays warp 0 so its own exits keep working; the doors
    # this script owns are dropped and rebuilt, so running it twice is the same
    # as running it once.
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

    for (door, suffix, lay, exits, people, kind) in PLAN:
        name = P + suffix
        mj = interior_json(suffix, lay, exits, people, door_index[suffix])
        d = os.path.join(REPO, 'data/maps', name)
        os.makedirs(d, exist_ok=True)
        with open(os.path.join(d, 'map.json'), 'w', newline='\n') as f:
            json.dump(mj, f, indent=2)
            f.write('\n')

        out = ['%s_MapScripts::' % name, '\t.byte 0', '']
        text = []
        talkers(name, people, out, text)
        if kind == 'mart':
            mart_clerk(name, out)
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

    a = add_outdoor(TOWN, TOWN_PEOPLE, walkable(TOWN), report)
    b = add_outdoor(BEACH, BEACH_PEOPLE, walkable(BEACH), report)
    print('  %d interiors written; MIKAN now has %d doors that open'
          % (len(PLAN), len(warps)))
    print('  %d people outdoors on MIKAN, %d on its beach' % (a, b))
    for line in report:
        print(line)


if __name__ == '__main__':
    main()
