"""Open KINNOW's doors and put people on the island, and open a massage parlour.

KINNOW had one door that went anywhere, the POKeMON CENTER, and seven people on
a seventy-two by sixty-six island. Five of its six closed doors are buildings
and they open here:

    (30,13)  the MART, which carries a MART sign
    (39,12)  the massage parlour, under the green awning
    (22,24)  a citrus grower's house
    (29,25)  the harbour office, facing the road down to the pier
    (47,25)  a house

The sixth, (33,48), is not a building at all. It sits at the end of a long pier
running south into the water, which is a gangway, and it is where ROUTE 9 now
arrives. That one waits for the boat it belongs to; it stays sealed until there
is something on the other side of it.

The massage parlour is the island's own trade. FireRed already has the
machinery -- DaisyMassageServices applies FRIENDSHIP_EVENT_MASSAGE to the mon
the player picks -- but that event is worth a flat three friendship at every
level, so Daisy's once-per-five-hundred-steps massage is a rounding error. This
one charges for it and is worth the money, by calling the same special several
times in a row: no new code at all, just a script that pays for what it asks
for.

    a rub-down   P500    the special twice, so six
    the course   P2000   the special ten times, so thirty

No cooldown, because the price is the limit. Thirty friendship a visit makes
the parlour a real route to a friendship evolution and a real money sink, which
an island with a harbour and no gym can carry.

Every coordinate is checked against the map before anything is written.

    python3 docs/orange-islands/build_kinnow_town.py
"""
import json
import os
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from repair_landmark_fixtures import behaviours, layouts_and_maps, WATER  # noqa: E402

REPO = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))

TOWN = 'OrangeIslands_Kinnow'
TOWN_MAP = 'MAP_ORANGE_ISLANDS_KINNOW'
BEACH = 'OrangeIslands_Kinnow_Beach'
GROUP = 'gMapGroup_Orange_Kinnow_Interior'
P = 'OrangeIslands_Kinnow_'

FENCE_TOP = '@ --- built by docs/orange-islands/build_kinnow_town.py ---'
FENCE_END = '@ --- end build_kinnow_town.py ---'

MART_ITEMS = ['ITEM_POKE_BALL', 'ITEM_GREAT_BALL', 'ITEM_POTION', 'ITEM_SUPER_POTION',
              'ITEM_ANTIDOTE', 'ITEM_PARALYZE_HEAL', 'ITEM_AWAKENING',
              'ITEM_ESCAPE_ROPE', 'ITEM_REPEL', 'ITEM_SUPER_REPEL', 'ITEM_REVIVE']

# Script suffixes this file used to write and no longer does.
RETIRED = []


def mid(name):
    return 'MAP_' + name.upper().replace('ORANGEISLANDS', 'ORANGE_ISLANDS')



MASSAGE_PRICE_SHORT = 500
MASSAGE_PRICE_LONG = 2000

MART = [
    ('OBJ_EVENT_GFX_CLERK', 2, 3, 'MOVEMENT_TYPE_FACE_DOWN', 'Clerk', None),
    ('OBJ_EVENT_GFX_SAILOR', 8, 4, 'MOVEMENT_TYPE_FACE_LEFT', 'Sailor',
     ['"I stock up here before every\\n"', '"crossing. There is nothing to buy\\l"',
      '"out on the water.$"']),
    ('OBJ_EVENT_GFX_WOMAN_1', 5, 6, 'MOVEMENT_TYPE_WANDER_AROUND', 'Woman',
     ['"We get more boats than visitors.\\n"', '"Most of them tie up, trade and\\l"',
      '"go.$"']),
]

# The parlour. The masseuse's script is written by masseuse() rather than
# talkers(), because it has to take money and call the special.
PARLOUR = [
    ('OBJ_EVENT_GFX_BEAUTY', 5, 4, 'MOVEMENT_TYPE_FACE_DOWN', 'Masseuse', None),
    ('OBJ_EVENT_GFX_FAT_MAN', 8, 7, 'MOVEMENT_TYPE_FACE_LEFT', 'Customer',
     ['"My GOLBAT has been one good mood\\n"', '"away from something else for\\l"',
      '"weeks.\\p"', '"I have spent a fortune in here.\\n"',
      '"I regret none of it.$"']),
    ('OBJ_EVENT_GFX_LASS', 2, 7, 'MOVEMENT_TYPE_WANDER_AROUND', 'Girl',
     ['"A POKeMON that likes you fights\\n"', '"harder. Everyone knows that.\\p"',
      '"Not everyone knows some of them\\n"', '"change shape over it.$"']),
]

GROWER = [
    ('OBJ_EVENT_GFX_OLD_MAN_1', 3, 4, 'MOVEMENT_TYPE_FACE_DOWN', 'Grower',
     ['"The island is named for the fruit\\n"', '"and the fruit is named for\\l"',
      '"nothing. It was just here.\\p"',
      '"Two groves left. The rest went\\n"', '"over to the harbour.$"']),
    ('OBJ_EVENT_GFX_LITTLE_BOY', 7, 6, 'MOVEMENT_TYPE_WANDER_AROUND', 'Son',
     ['"Grandad says the boat ruined the\\n"', '"island.\\p"',
      '"I think the boat is the best\\n"', '"thing that ever happened here.$"']),
]

HARBOUR = [
    ('OBJ_EVENT_GFX_GENTLEMAN', 4, 3, 'MOVEMENT_TYPE_FACE_DOWN', 'Harbourmaster',
     ['"KINNOW HARBOUR OFFICE. I book the\\n"', '"pier and I settle the arguments\\l"',
      '"about it.\\p"',
      '"The long jetty at the south end\\n"', '"is not ours. That berth is\\l"',
      '"spoken for.$"']),
    ('OBJ_EVENT_GFX_WOMAN_3', 8, 5, 'MOVEMENT_TYPE_FACE_LEFT', 'Clerk2',
     ['"Trainers come off ROUTE 9 and walk\\n"', '"straight back down the pier.\\p"',
      '"Whatever is moored there, they\\n"', '"want it more than they want us.$"']),
]

HOUSE = [
    ('OBJ_EVENT_GFX_OLD_WOMAN', 4, 4, 'MOVEMENT_TYPE_FACE_DOWN', 'OldWoman',
     ['"You can hear the cheering from\\n"', '"here on a still night.\\p"',
      '"All the way up from the water.$"']),
    ('OBJ_EVENT_GFX_YOUNGSTER', 8, 6, 'MOVEMENT_TYPE_WANDER_AROUND', 'Youngster',
     ['"One day I will be old enough to\\n"', '"go down there.\\p"',
      '"Mum says the same thing every\\n"', '"year.$"']),
]

# (door, map suffix, layout, exit cells, people, extra script kind)
PLAN = [
    ((30, 13), 'Mart', 'LAYOUT_MART', [(3, 7), (4, 7), (5, 7)], MART, 'mart'),
    ((39, 12), 'Massage', 'LAYOUT_CERULEAN_CITY_BIKE_SHOP',
     [(4, 9), (5, 8), (6, 9)], PARLOUR, 'massage'),
    ((22, 24), 'House1', 'LAYOUT_HOUSE1', [(3, 7), (4, 7), (5, 7)], GROWER, None),
    ((29, 25), 'HarbourOffice', 'LAYOUT_HOUSE5', [(3, 7), (4, 7), (5, 7)], HARBOUR, None),
    ((47, 25), 'House2', 'LAYOUT_HOUSE3', [(4, 7)], HOUSE, None),
]

TOWN_PEOPLE = [
    ('OBJ_EVENT_GFX_FISHER', 20, 17, 'MOVEMENT_TYPE_FACE_DOWN', 'Fisher',
     ['"KINNOW grows fruit and KINNOW\\n"', '"mends boats. That is the whole\\l"',
      '"island.\\p"', '"Well. That was the whole island.$"']),
    ('OBJ_EVENT_GFX_LASS', 43, 18, 'MOVEMENT_TYPE_WANDER_AROUND', 'Girl',
     ['"The parlour under the green\\n"', '"awning will groom your POKeMON\\l"',
      '"properly. It is not cheap.\\p"',
      '"It works, though. Mine has never\\n"', '"been happier.$"']),
    ('OBJ_EVENT_GFX_FAT_MAN', 26, 29, 'MOVEMENT_TYPE_FACE_RIGHT', 'Man',
     ['"That pier goes a long way out for\\n"', '"an island this size.\\p"',
      '"It was built for something\\n"', '"bigger than us.$"']),
    ('OBJ_EVENT_GFX_SAILOR', 33, 31, 'MOVEMENT_TYPE_FACE_DOWN', 'Sailor',
     ['"Down the pier, is it.\\p"', '"Everyone goes down the pier.$"']),
    ('OBJ_EVENT_GFX_LITTLE_GIRL', 50, 28, 'MOVEMENT_TYPE_WANDER_AROUND', 'Child',
     ['"North of here is all water until\\n"', '"you hit ROUTE 10.\\p"',
      '"South is all water too. We are\\n"', '"very surrounded.$"']),
]

BEACH_PEOPLE = []

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
    """Replace this script's own block in a file, or append it.

    A body with nothing in it writes no block and removes any it left before,
    so a map this script has nothing to say about keeps a clean file rather
    than collecting an empty pair of fences.
    """
    s = open(path, encoding='utf-8').read() if os.path.exists(path) else ''
    if not [b for b in body if b.strip()]:
        if FENCE_TOP in s and FENCE_END in s:
            s = (s[:s.index(FENCE_TOP)]
                 + s[s.index(FENCE_END) + len(FENCE_END):].lstrip('\n'))
            open(path, 'w', encoding='utf-8', newline='\n').write(s)
        return
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
        "region_map_section": "MAPSEC_KINNOW_ISLAND", "requires_flash": False,
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


def masseuse(name, out, text):
    """Pay, pick a mon, and get the massage the price bought.

    DaisyMassageServices applies FRIENDSHIP_EVENT_MASSAGE once to the mon in
    VAR_0x8004, and that event is a flat three at every friendship level. So
    the course is simply the special called ten times over, which needs no new
    code and cannot drift from however the game values a massage later.

    checkmoney and removemoney take a literal and never a variable, so each
    price is its own branch rather than one parameterised path.
    """
    L = out.append

    def tier(tag, price, times):
        L('%s_EventScript_%s::' % (name, tag))
        L('\tcheckmoney %d' % price)
        L('\tgoto_if_eq VAR_RESULT, FALSE, %s_EventScript_NoMoney' % name)
        L('\tmsgbox %s_Text_WhichMon' % name)
        L('\tspecial ChoosePartyMon')
        L('\twaitstate')
        L('\tlock')
        L('\tfaceplayer')
        L('\tgoto_if_ge VAR_0x8004, PARTY_SIZE, %s_EventScript_Declined' % name)
        L('\tspecialvar VAR_RESULT, GetPartyMonSpecies')
        L('\tgoto_if_eq VAR_RESULT, SPECIES_EGG, %s_EventScript_NotAnEgg' % name)
        L('\tremovemoney %d' % price)
        L('\tupdatemoneybox')
        L('\tmsgbox %s_Text_LieBack' % name)
        L('\tclosemessage')
        L('\tfadescreen FADE_TO_BLACK')
        L('\tplayfanfare MUS_HEAL')
        L('\twaitfanfare')
        for _ in range(times):
            L('\tspecial DaisyMassageServices')
        L('\tfadescreen FADE_FROM_BLACK')
        L('\thidemoneybox')
        L('\tspecial BufferMonNickname')
        L('\tmsgbox %s_Text_AllDone' % name)
        L('\trelease')
        L('\tend')
        L('')

    L("@ The parlour. The price buys repetitions of the game's own massage,")
    L('@ which is worth a flat three friendship however high the mon already is.')
    L('%s_EventScript_Masseuse::' % name)
    L('\tgoto_if_questlog EventScript_ReleaseEnd')
    L('\tlock')
    L('\tfaceplayer')
    L('\tspecial QuestLog_CutRecording')
    L('\tshowmoneybox 0, 0')
    L('\tmsgbox %s_Text_Menu, MSGBOX_DEFAULT' % name)
    L('\tmultichoice 0, 0, MULTICHOICE_KINNOW_MASSAGE, FALSE')
    L('\tgoto_if_eq VAR_RESULT, 0, %s_EventScript_Short' % name)
    L('\tgoto_if_eq VAR_RESULT, 1, %s_EventScript_Long' % name)
    L('\tgoto %s_EventScript_Declined' % name)
    L('')
    tier('Short', MASSAGE_PRICE_SHORT, 2)
    tier('Long', MASSAGE_PRICE_LONG, 10)
    for tag in ('NoMoney', 'Declined', 'NotAnEgg'):
        L('%s_EventScript_%s::' % (name, tag))
        L('\thidemoneybox')
        L('\tmsgbox %s_Text_%s' % (name, tag))
        L('\trelease')
        L('\tend')
        L('')

    for tag, lines in (
        ('Menu', ['"Welcome to the KINNOW ROOMS.\\p"',
                  '"We groom POKeMON, and we are told\\n"', '"we are very good at it.$"']),
        ('WhichMon', ['"Which one shall we see to?$"']),
        ('LieBack', ['"Lovely. This will take a little\\n"', '"while.$"']),
        ('AllDone', ['"There. {STR_VAR_1} looks\\n"', '"wonderful.\\p"',
                     '"And rather fond of you, if I may\\n"', '"say so.$"']),
        ('NoMoney', ['"Oh dear. That is more than you\\n"', '"have on you.$"']),
        ('Declined', ['"Another time, then.$"']),
        ('NotAnEgg', ['"I cannot groom an EGG!\\p"',
                      '"There is nothing to groom yet.$"'])):
        text.append('%s_Text_%s::' % (name, tag))
        text += ['\t.string %s' % ln for ln in lines]
        text.append('')


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
        if kind == 'massage':
            masseuse(name, out, text)
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
    print('  %d interiors written; KINNOW now has %d doors that open'
          % (len(PLAN), len(warps)))
    print('  %d people outdoors on KINNOW, %d on its beach' % (a, b))
    for line in report:
        print(line)


if __name__ == '__main__':
    main()
