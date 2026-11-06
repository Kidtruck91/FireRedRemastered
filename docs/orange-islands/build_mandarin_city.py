"""Open every door in Mandarin South.

The city was drawn with fifteen doors and exactly one of them went anywhere --
the POKeMON CENTER. This wires the other thirteen (the fifteenth, at (37,15),
is rooftop decoration and was sealed by the tidy pass).

Every interior reuses a layout the game already ships, so no new tilesets, no
new graphics, and the rooms look like rooms:

    (17,22) + (21,22)  MANDARIN DEPT. STORE 1F / 2F / 3F   Celadon's store
    (39,42)            the GAME CORNER                     Celadon's floor
    (53,43)            its PRIZE CORNER next door          Celadon's prize room
    (37,22)            the SEAVIEW HOTEL                   Celadon's hotel
    (68,22)            a restaurant                        Celadon's restaurant
    (76,44)            the MART                            the shared mart
    (72,22) (76,22) (16,56) (21,56) (55,56) (37,59)        six houses

The department store is three floors, not Celadon's five. Its 1F layout has a
lift shaft drawn at (6,1), and a working lift would need a new
MULTICHOICE table and a new floor list in C for a three-floor building, so
instead a clerk stands at (6,2) on each floor with an out-of-order sign. That
is also why the lift door is never a dead end the player can walk into.

The GAME CORNER is the real thing: Celadon's slot machine coordinates come with
its layout, so the fourteen machines sit exactly where the floor was drawn for
them, and `playslotmachine` drives them. Coins are sold at the counter and
spent next door. Roulette and VOLTORB FLIP are not here -- FireRed has no
roulette code at all (data/specials.inc still reads "NullFieldSpecial @ Berry
Blender/Roulette specials") and VOLTORB FLIP is a DS game -- so both are jobs
of their own.

    python3 docs/orange-islands/build_mandarin_city.py
"""
import json
import os

REPO = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
CITY = 'OrangeIslands_Mandarin_South'
CITY_MAP = 'MAP_ORANGE_ISLANDS_MANDARIN_SOUTH'
GROUP = 'gMapGroup_Orange_Mandarin_South_Interior'

P = 'OrangeIslands_Mandarin_South_'     # name prefix for every new map


def mid(name):
    return 'MAP_' + name.upper().replace('ORANGEISLANDS', 'ORANGE_ISLANDS')


# ---------------------------------------------------------------------------
# Who stands where. (graphics, x, y, movement, script suffix, lines or None)
# A script suffix with no lines is wired by hand further down.
# ---------------------------------------------------------------------------
LIFT_CLERK = ('OBJ_EVENT_GFX_WORKER_F', 6, 2, 'MOVEMENT_TYPE_FACE_DOWN', 'LiftClerk',
              ['"The lift has been out since the\\n"', '"spring. Please use the stairs.$"'])

DEPT_1F = [LIFT_CLERK,
           ('OBJ_EVENT_GFX_WORKER_F', 6, 10, 'MOVEMENT_TYPE_FACE_DOWN', 'Guide',
            ['"MANDARIN DEPT. STORE.\\p"',
             '"2F, everyday goods. 3F, TECHNICAL\\n"', '"MACHINES and medicine.$"']),
           ('OBJ_EVENT_GFX_FAT_MAN', 11, 10, 'MOVEMENT_TYPE_FACE_LEFT', 'Shopper',
            ['"Three floors and not one of them\\n"', '"sells a decent cup of tea.$"'])]
DEPT_2F = [LIFT_CLERK,
           ('OBJ_EVENT_GFX_CLERK', 1, 6, 'MOVEMENT_TYPE_FACE_DOWN', 'Clerk2F', None),
           ('OBJ_EVENT_GFX_LASS', 5, 10, 'MOVEMENT_TYPE_WANDER_AROUND', 'Girl2F',
            ['"REPELs, REPELs, REPELs. That is\\n"', '"all anyone buys here.$"'])]
DEPT_3F = [LIFT_CLERK,
           ('OBJ_EVENT_GFX_CLERK', 10, 9, 'MOVEMENT_TYPE_FACE_DOWN', 'Clerk3F', None),
           ('OBJ_EVENT_GFX_SCIENTIST', 4, 9, 'MOVEMENT_TYPE_FACE_LEFT', 'Scientist3F',
            ['"A TECHNICAL MACHINE is cheaper\\n"', '"than the hours of training it\\l"',
             '"saves you. Think about it.$"'])]

MART_2F_ITEMS = ['ITEM_POKE_BALL', 'ITEM_GREAT_BALL', 'ITEM_POTION', 'ITEM_SUPER_POTION',
                 'ITEM_ANTIDOTE', 'ITEM_PARALYZE_HEAL', 'ITEM_AWAKENING', 'ITEM_BURN_HEAL',
                 'ITEM_ICE_HEAL', 'ITEM_REPEL', 'ITEM_SUPER_REPEL', 'ITEM_ESCAPE_ROPE']
MART_3F_ITEMS = ['ITEM_TM13', 'ITEM_TM24', 'ITEM_TM45', 'ITEM_HYPER_POTION',
                 'ITEM_FULL_HEAL', 'ITEM_REVIVE', 'ITEM_PROTEIN', 'ITEM_IRON',
                 'ITEM_CARBOS', 'ITEM_CALCIUM', 'ITEM_HP_UP']
MART_ITEMS = ['ITEM_POKE_BALL', 'ITEM_GREAT_BALL', 'ITEM_POTION', 'ITEM_SUPER_POTION',
              'ITEM_ANTIDOTE', 'ITEM_PARALYZE_HEAL', 'ITEM_AWAKENING', 'ITEM_ESCAPE_ROPE',
              'ITEM_REPEL']

# The slot machines come with Celadon's game corner layout, so they sit exactly
# where the floor was drawn for them.
SLOTS = [(0, 7), (0, 9), (0, 10), (5, 6), (5, 9), (5, 10), (6, 6), (6, 7),
         (6, 9), (11, 6), (11, 7), (11, 8), (11, 10), (12, 7)]
BROKEN_SLOT = (5, 8)

GAME_CORNER_PEOPLE = [
    ('OBJ_EVENT_GFX_WORKER_F', 4, 2, 'MOVEMENT_TYPE_FACE_DOWN', 'InfoClerk',
     ['"Welcome to the MANDARIN GAME\\n"', '"CORNER.\\p"',
      '"Win coins at the machines, spend\\n"', '"them next door at the PRIZE\\l"',
      '"CORNER.$"']),
    ('OBJ_EVENT_GFX_WORKER_M', 6, 2, 'MOVEMENT_TYPE_FACE_DOWN', 'CoinsClerk', None),
    # The two ROULETTE tables, ported from Emerald. Mauville put them on the
    # floor tiles themselves; this room was drawn for slot machines, so the
    # tables are the croupiers who run them. Minimums are Emerald's: 1 and 3.
    ('OBJ_EVENT_GFX_WORKER_M', 10, 4, 'MOVEMENT_TYPE_FACE_DOWN', 'CroupierLeft', None),
    ('OBJ_EVENT_GFX_WORKER_F', 13, 4, 'MOVEMENT_TYPE_FACE_DOWN', 'CroupierRight', None),
    ('OBJ_EVENT_GFX_GENTLEMAN', 16, 10, 'MOVEMENT_TYPE_FACE_LEFT', 'Gentleman',
     ['"I have a system. It has cost me\\n"', '"a great deal of money.$"']),
    ('OBJ_EVENT_GFX_OLD_MAN_2', 10, 9, 'MOVEMENT_TYPE_FACE_DOWN', 'OldMan',
     ['"Machines by the wall pay out. Do\\n"', '"not ask me how I know.$"']),
    ('OBJ_EVENT_GFX_SAILOR', 1, 6, 'MOVEMENT_TYPE_FACE_RIGHT', 'Sailor',
     ['"Came ashore with a month of pay.\\n"', '"Leaving with a LEMONADE.$"']),
    ('OBJ_EVENT_GFX_WOMAN_1', 7, 8, 'MOVEMENT_TYPE_FACE_DOWN', 'Woman',
     ['"This one has not paid out all\\n"', '"evening. It is due. It must be.$"']),
]

# (coins, kind, what, label) -- kind is 'item' or 'mon'
PRIZES_ITEM = [(1000, 'ITEM_TM13', 'TM13'), (3500, 'ITEM_TM24', 'TM24'),
               (2000, 'ITEM_TM45', 'TM45')]
PRIZES_MON = [(1200, 'SPECIES_HORSEA', 18, 'HORSEA'),
              (2800, 'SPECIES_STARYU', 20, 'STARYU'),
              (6500, 'SPECIES_DRATINI', 22, 'DRATINI')]

PRIZE_PEOPLE = [
    ('OBJ_EVENT_GFX_WORKER_M', 2, 2, 'MOVEMENT_TYPE_FACE_DOWN', 'PrizeItems', None),
    ('OBJ_EVENT_GFX_WORKER_F', 4, 2, 'MOVEMENT_TYPE_FACE_DOWN', 'PrizeMons', None),
    ('OBJ_EVENT_GFX_BALDING_MAN', 2, 6, 'MOVEMENT_TYPE_FACE_RIGHT', 'PrizeBalding',
     ['"A DRATINI for six and a half\\n"', '"thousand coins. I have been at\\l"',
      '"those machines since Tuesday.$"']),
]

HOTEL_PEOPLE = [
    ('OBJ_EVENT_GFX_WOMAN_2', 5, 4, 'MOVEMENT_TYPE_FACE_DOWN', 'Receptionist',
     ['"The SEAVIEW HOTEL. Every room\\n"', '"faces the sea, which is more than\\l"',
      '"the name promises elsewhere.$"']),
    ('OBJ_EVENT_GFX_GENTLEMAN', 9, 7, 'MOVEMENT_TYPE_FACE_LEFT', 'Guest',
     ['"I booked a week for the DERBY.\\n"', '"I have not caught a thing.$"']),
    ('OBJ_EVENT_GFX_LITTLE_GIRL', 2, 6, 'MOVEMENT_TYPE_WANDER_AROUND', 'Child',
     ['"There is a lift in the DEPT.\\n"', '"STORE but it does not work!$"']),
]

RESTAURANT_PEOPLE = [
    ('OBJ_EVENT_GFX_CHEF', 12, 7, 'MOVEMENT_TYPE_FACE_DOWN', 'Waiter',
     ['"Today it is grilled MAGIKARP.\\p"', '"It is always grilled MAGIKARP.$"']),
    ('OBJ_EVENT_GFX_FAT_MAN', 9, 4, 'MOVEMENT_TYPE_FACE_RIGHT', 'Diner',
     ['"The DERBY judge eats here every\\n"', '"evening. Same table. Same soup.$"']),
    ('OBJ_EVENT_GFX_BEAUTY', 11, 4, 'MOVEMENT_TYPE_FACE_LEFT', 'Lady',
     ['"One does not come to MANDARIN\\n"', '"SOUTH for the cooking.$"']),
]

HOUSES = [
    ((72, 22), 'House1', 'LAYOUT_HOUSE1', (3, 4, 5),
     [('OBJ_EVENT_GFX_OLD_MAN_2', 3, 3, 'MOVEMENT_TYPE_FACE_DOWN', 'OldMan',
       ['"The lights of the GAME CORNER\\n"', '"come through my curtains all\\l"',
        '"night. I have complained twice.$"']),
      ('OBJ_EVENT_GFX_LITTLE_BOY', 7, 5, 'MOVEMENT_TYPE_WANDER_AROUND', 'Boy',
       ['"When I am older I am going to\\n"', '"win the DERBY.$"'])]),
    ((76, 22), 'House2', 'LAYOUT_HOUSE5', (3, 4, 5),
     [('OBJ_EVENT_GFX_WOMAN_3', 4, 3, 'MOVEMENT_TYPE_FACE_DOWN', 'Woman',
       ['"My husband went to buy milk and\\n"', '"came back with forty coins and\\l"',
        '"no milk.$"'])]),
    ((16, 56), 'House3', 'LAYOUT_HOUSE3', (4,),
     [('OBJ_EVENT_GFX_FISHER', 4, 3, 'MOVEMENT_TYPE_FACE_DOWN', 'Fisher',
       ['"There is a gap in the cliff on\\n"', '"the east side. Goes right\\l"',
        '"through to the beach.$"'])]),
    ((21, 56), 'House4', 'LAYOUT_HOUSE4', (3,),
     [('OBJ_EVENT_GFX_SCIENTIST', 4, 4, 'MOVEMENT_TYPE_FACE_DOWN', 'Scientist',
       ['"I am mapping the currents between\\n"', '"here and NAVEL. They do not\\l"',
        '"behave as the charts say.$"'])]),
    ((55, 56), 'House5', 'LAYOUT_HOUSE1', (3, 4, 5),
     [('OBJ_EVENT_GFX_OLD_WOMAN', 3, 3, 'MOVEMENT_TYPE_FACE_DOWN', 'OldWoman',
       ['"I remember when the east side was\\n"', '"open water. Then they cut the\\l"',
        '"cliff and there was a beach.$"']),
      ('OBJ_EVENT_GFX_YOUNGSTER', 8, 5, 'MOVEMENT_TYPE_FACE_LEFT', 'Youngster',
       ['"Gran tells that one to everybody.$"'])]),
    ((37, 59), 'House6', 'LAYOUT_HOUSE5', (3, 4, 5),
     [('OBJ_EVENT_GFX_BEAUTY', 4, 3, 'MOVEMENT_TYPE_FACE_DOWN', 'Beauty',
       ['"MANDARIN SOUTH has a DEPT. STORE,\\n"', '"a GAME CORNER and a hotel.\\p"',
        '"It is practically CELADON.$"'])]),
]

COIN_PRICE = 1000       # for fifty coins, the going rate
COIN_BATCH = 50


# ---------------------------------------------------------------------------
# Every interior, in the order their doors are wired into the city.
#   (door, map suffix, layout, exit warp cells, people, extra script writer)
# ---------------------------------------------------------------------------
def plan():
    return [
        ((17, 22), 'DepartmentStore_1F', 'LAYOUT_CELADON_CITY_DEPARTMENT_STORE_1F',
         [(1, 15), (2, 14), (3, 15)], DEPT_1F, 'dept1f'),
        (None, 'DepartmentStore_2F', 'LAYOUT_CELADON_CITY_DEPARTMENT_STORE_2F',
         [], DEPT_2F, 'dept2f'),
        (None, 'DepartmentStore_3F', 'LAYOUT_CELADON_CITY_DEPARTMENT_STORE_3F',
         [], DEPT_3F, 'dept3f'),
        ((39, 42), 'GameCorner', 'LAYOUT_CELADON_CITY_GAME_CORNER',
         [(9, 13), (10, 13), (11, 13)], GAME_CORNER_PEOPLE, 'gamecorner'),
        ((53, 43), 'GameCorner_PrizeRoom', 'LAYOUT_CELADON_CITY_GAME_CORNER_PRIZE_ROOM',
         [(3, 9), (4, 8), (5, 9)], PRIZE_PEOPLE, 'prizeroom'),
        ((37, 22), 'Hotel', 'LAYOUT_CELADON_CITY_HOTEL',
         [(3, 10), (4, 9), (5, 10)], HOTEL_PEOPLE, None),
        ((68, 22), 'Restaurant', 'LAYOUT_CELADON_CITY_RESTAURANT',
         [(5, 9), (6, 9), (7, 9)], RESTAURANT_PEOPLE, None),
        ((76, 44), 'Mart', 'LAYOUT_MART',
         [(3, 7), (4, 7), (5, 7)],
         [('OBJ_EVENT_GFX_CLERK', 2, 3, 'MOVEMENT_TYPE_FACE_DOWN', 'Clerk', None),
          ('OBJ_EVENT_GFX_SAILOR', 9, 5, 'MOVEMENT_TYPE_FACE_LEFT', 'Sailor',
           ['"Stock up before you surf for\\n"', '"NAVEL. There is nothing out\\l"',
            '"there but water.$"'])], 'mart'),
    ] + [(door, suffix, lay, [(x, 7) for x in warps], people, None)
         for (door, suffix, lay, warps, people) in HOUSES]


def interior_json(suffix, lay, exits, people, door_index):
    name = P + suffix
    music = 'MUS_GAME_CORNER' if 'GameCorner' in suffix else 'MUS_FUCHSIA'
    return {
        "id": mid(name), "name": name, "layout": lay, "music": music,
        "region_map_section": "MAPSEC_MANDARIN_SOUTH", "requires_flash": False,
        "weather": "WEATHER_NONE", "map_type": "MAP_TYPE_INDOOR",
        "allow_cycling": False, "allow_escaping": False, "allow_running": False,
        "show_map_name": False, "floor_number": 0,
        "battle_scene": "MAP_BATTLE_SCENE_NORMAL", "connections": 0,
        "object_events": [
            {"type": "object", "graphics_id": g, "x": x, "y": y, "elevation": 3,
             "movement_type": m, "movement_range_x": 1, "movement_range_y": 1,
             "trainer_type": "TRAINER_TYPE_NONE", "trainer_sight_or_berry_tree_id": "0",
             "script": "%s_EventScript_%s" % (name, s), "flag": "0"}
            for (g, x, y, m, s, _lines) in people],
        "warp_events": [{"x": x, "y": y, "elevation": 0, "dest_map": CITY_MAP,
                         "dest_warp_id": str(door_index)} for (x, y) in exits],
        "coord_events": [], "bg_events": []}


def talkers(name, people, out, text):
    """The plain msgbox people; anyone with no lines is wired by hand."""
    for (_g, _x, _y, _m, s, lines) in people:
        if lines is None:
            continue
        out.append('%s_EventScript_%s::' % (name, s))
        out.append('\tmsgbox %s_Text_%s, MSGBOX_NPC' % (name, s))
        out.append('\tend')
        out.append('')
        text.append('%s_Text_%s::' % (name, s))
        for ln in lines:
            text.append('\t.string %s' % ln)
        text.append('')


def mart_clerk(name, label, items, out, text):
    out += ['%s_EventScript_%s::' % (name, label),
            '\tgoto_if_questlog EventScript_ReleaseEnd',
            '\tlock', '\tfaceplayer',
            '\tmessage Text_MayIHelpYou', '\twaitmessage',
            '\tpokemart %s_Items%s' % (name, label),
            '\tmsgbox Text_PleaseComeAgain', '\trelease', '\tend', '',
            '\t.align 2',
            '%s_Items%s::' % (name, label)]
    for it in items:
        out.append('\t.2byte %s' % it)
    out += ['\t.2byte ITEM_NONE', '\t.align 2', '']


def extra_scripts(kind, name, out, text):
    """The handful of people who do more than talk."""
    if kind in ('dept2f', 'dept3f'):
        mart_clerk(name, 'Clerk2F' if kind == 'dept2f' else 'Clerk3F',
                   MART_2F_ITEMS if kind == 'dept2f' else MART_3F_ITEMS, out, text)
    elif kind == 'mart':
        mart_clerk(name, 'Clerk', MART_ITEMS, out, text)
    elif kind == 'gamecorner':
        out += ['@ Celadon sells fifty coins for a thousand; so does MANDARIN.',
                '%s_EventScript_CoinsClerk::' % name,
                '\tlock', '\tfaceplayer',
                '\tgoto_if_unset FLAG_GOT_COIN_CASE, %s_EventScript_NoCase' % name,
                '\tmsgbox %s_Text_CoinsAsk, MSGBOX_YESNO' % name,
                '\tgoto_if_eq VAR_RESULT, NO, %s_EventScript_CoinsNo' % name,
                '\tcheckmoney %d' % COIN_PRICE,
                '\tgoto_if_eq VAR_RESULT, FALSE, %s_EventScript_CoinsBroke' % name,
                '\tcheckcoins VAR_TEMP_0',
                '\tgoto_if_gt VAR_TEMP_0, 9949, %s_EventScript_CoinsFull' % name,
                '\tshowmoneybox 0, 0', '\tshowcoinsbox 0, 5',
                '\tremovemoney %d' % COIN_PRICE,
                '\taddcoins %d' % COIN_BATCH,
                '\tupdatemoneybox', '\tupdatecoinsbox 0, 5', '\tplayse SE_SHOP',
                '\tmsgbox %s_Text_CoinsThanks, MSGBOX_DEFAULT' % name,
                '\thidemoneybox', '\thidecoinsbox 0, 5',
                '\trelease', '\tend', '']
        for tag in ('NoCase', 'CoinsNo', 'CoinsBroke', 'CoinsFull'):
            out += ['%s_EventScript_%s::' % (name, tag),
                    '\tmsgbox %s_Text_%s, MSGBOX_DEFAULT' % (name, tag),
                    '\trelease', '\tend', '']
        for tag, lines in (
                ('CoinsAsk', ['"Fifty coins for %d. Shall I?$"' % COIN_PRICE]),
                ('CoinsThanks', ['"Fifty coins. Good luck in there.$"']),
                ('NoCase', ['"You will want a COIN CASE before\\n"',
                            '"you sit down at a machine.$"']),
                ('CoinsNo', ['"Watching is free, of course.$"']),
                ('CoinsBroke', ['"Come back when you have the\\n"', '"money, friend.$"']),
                ('CoinsFull', ['"Your COIN CASE will not hold\\n"', '"another one.$"'])):
            text.append('%s_Text_%s::' % (name, tag))
            for ln in lines:
                text.append('\t.string %s' % ln)
            text.append('')

        out += ['@ ROULETTE. Table 1 takes a one-coin minimum, table 2 takes three;',
                '@ both numbers are Emerald own, from sTableMinBets.',
                '%s_EventScript_CroupierLeft::' % name,
                '\tlock', '\tfaceplayer',
                '\tmsgbox %s_Text_CroupierLeft, MSGBOX_DEFAULT' % name,
                '\tgoto Roulette_EventScript_Table1',
                '\tend', '',
                '%s_EventScript_CroupierRight::' % name,
                '\tlock', '\tfaceplayer',
                '\tmsgbox %s_Text_CroupierRight, MSGBOX_DEFAULT' % name,
                '\tgoto Roulette_EventScript_Table2',
                '\tend', '']
        for tag, lines in (
                ('CroupierLeft', ['"ROULETTE, the near table. One coin\\n"',
                                  '"the spin, and the wheel is kind.$"']),
                ('CroupierRight', ['"ROULETTE, the far table. Three\\n"',
                                   '"coins the spin.\\p"',
                                   '"It pays better. It also takes\\n"', '"rather more.$"'])):
            text.append('%s_Text_%s::' % (name, tag))
            for ln in lines:
                text.append('\t.string %s' % ln)
            text.append('')

        out += ['@ The machines sit where Celadon drew them; the layout is the same one.',
                '%s_EventScript_SlotMachine::' % name,
                '\tlockall',
                '\tgoto_if_unset FLAG_GOT_COIN_CASE, %s_EventScript_SlotNoCase' % name,
                '\tmsgbox %s_Text_SlotPlay, MSGBOX_YESNO' % name,
                '\tgoto_if_eq VAR_RESULT, NO, %s_EventScript_SlotWalkAway' % name,
                '\tspecialvar VAR_RESULT, GetRandomSlotMachineId',
                '\tplayslotmachine VAR_RESULT',
                '\treleaseall', '\tend', '',
                '%s_EventScript_SlotNoCase::' % name,
                '\tmsgbox %s_Text_SlotNoCase, MSGBOX_DEFAULT' % name,
                '\treleaseall', '\tend', '',
                '%s_EventScript_SlotWalkAway::' % name,
                '\treleaseall', '\tend', '',
                '%s_EventScript_BrokenSlot::' % name,
                '\tlockall',
                '\tmsgbox %s_Text_BrokenSlot, MSGBOX_DEFAULT' % name,
                '\treleaseall', '\tend', '']
        for tag, lines in (
                ('SlotPlay', ['"A slot machine. Play it?$"']),
                ('SlotNoCase', ['"It is a slot machine. Without a\\n"',
                                '"COIN CASE there is nothing to\\l"', '"play it with.$"']),
                ('BrokenSlot', ['"OUT OF ORDER.\\p"',
                                '"Somebody has written UNLUCKY on\\n"',
                                '"the sign in marker pen.$"'])):
            text.append('%s_Text_%s::' % (name, tag))
            for ln in lines:
                text.append('\t.string %s' % ln)
            text.append('')

    elif kind == 'prizeroom':
        for label, prizes in (('PrizeItems', PRIZES_ITEM), ('PrizeMons', PRIZES_MON)):
            out += ['%s_EventScript_%s::' % (name, label),
                    '\tlock', '\tfaceplayer',
                    '\tgoto_if_unset FLAG_GOT_COIN_CASE, %s_EventScript_PrizeNoCase' % name,
                    '\tshowcoinsbox 0, 0',
                    '\tmsgbox %s_Text_%sIntro, MSGBOX_DEFAULT' % (name, label)]
            for i, pr in enumerate(prizes):
                out.append('\tcall %s_EventScript_%s%d' % (name, label, i))
            out += ['\thidecoinsbox 0, 0', '\trelease', '\tend', '']
            for i, pr in enumerate(prizes):
                coins = pr[0]
                out += ['%s_EventScript_%s%d::' % (name, label, i),
                        '\tbuffernumberstring STR_VAR_2, %d' % coins]
                if label == 'PrizeItems':
                    out.append('\tbufferitemname STR_VAR_1, %s' % pr[1])
                else:
                    out.append('\tbufferspeciesname STR_VAR_1, %s' % pr[1])
                out += ['\tmsgbox %s_Text_PrizeOffer, MSGBOX_YESNO' % name,
                        '\tgoto_if_eq VAR_RESULT, NO, %s_EventScript_PrizeSkip' % name,
                        '\tcheckcoins VAR_TEMP_0',
                        '\tgoto_if_lt VAR_TEMP_0, %d, %s_EventScript_PrizeShort'
                        % (coins, name)]
                if label == 'PrizeItems':
                    out += ['\tcheckitemspace %s' % pr[1],
                            '\tgoto_if_eq VAR_RESULT, FALSE, %s_EventScript_PrizeFull' % name,
                            '\tremovecoins %d' % coins,
                            '\tupdatecoinsbox 0, 0',
                            '\tgiveitem %s' % pr[1]]
                else:
                    out += ['\tgetpartysize',
                            '\tgoto_if_eq VAR_RESULT, PARTY_SIZE, %s_EventScript_PrizeNoRoom'
                            % name,
                            '\tremovecoins %d' % coins,
                            '\tupdatecoinsbox 0, 0',
                            '\tgivemon %s, %d' % (pr[1], pr[2])]
                out += ['\tmsgbox %s_Text_PrizeThanks, MSGBOX_DEFAULT' % name,
                        '\treturn', '']
            out += ['']
        out += ['%s_EventScript_PrizeSkip::' % name, '\treturn', '']
        for tag in ('PrizeNoCase', 'PrizeShort', 'PrizeFull', 'PrizeNoRoom'):
            out += ['%s_EventScript_%s::' % (name, tag),
                    '\tmsgbox %s_Text_%s, MSGBOX_DEFAULT' % (name, tag),
                    '\treturn', '']
        for tag, lines in (
                ('PrizeItemsIntro', ['"TECHNICAL MACHINES, for coins.\\n"',
                                     '"Let me run through them.$"']),
                ('PrizeMonsIntro', ['"POKeMON, for coins. Rather a lot\\n"',
                                    '"of coins, in one case.$"']),
                ('PrizeOffer', ['"{STR_VAR_1}, {STR_VAR_2} coins.\\n"', '"Would you like it?$"']),
                ('PrizeThanks', ['"There you are. Do come again.$"']),
                ('PrizeNoCase', ['"Coins only here, and you have\\n"', '"nowhere to keep them.$"']),
                ('PrizeShort', ['"You have not the coins for that\\n"', '"one, I am afraid.$"']),
                ('PrizeFull', ['"You are carrying too much.$"']),
                ('PrizeNoRoom', ['"Your party is full. I cannot\\n"', '"hand it over.$"'])):
            text.append('%s_Text_%s::' % (name, tag))
            for ln in lines:
                text.append('\t.string %s' % ln)
            text.append('')


def main():
    city_path = os.path.join(REPO, 'data/maps', CITY, 'map.json')
    city = json.load(open(city_path))
    # Keep the POKeMON CENTER as warp 0; everything else is rebuilt from the plan.
    keep = [w for w in city['warp_events'] if 'POKEMON_CENTER' in w['dest_map']]
    warps = list(keep)

    groups = os.path.join(REPO, 'data/maps/map_groups.json')
    gj = json.load(open(groups))
    gj.setdefault(GROUP, [])
    order = gj.get('group_order')
    if order is not None and GROUP not in order:
        order.append(GROUP)

    esp = os.path.join(REPO, 'data/event_scripts.s')
    es = open(esp, encoding='utf-8').read()

    entries = plan()
    # Work out each interior's warp id in the city before writing anything, so
    # the two department store doors can both land on 1F.
    door_index = {}
    for (door, suffix, _l, _e, _p, _k) in entries:
        if door is None:
            continue
        door_index[suffix] = len(warps)
        warps.append({"x": door[0], "y": door[1], "elevation": 0,
                      "dest_map": mid(P + suffix), "dest_warp_id": "0"})
    # the store's second entrance, landing on the right-hand doors of 1F
    second = len(warps)
    warps.append({"x": 21, "y": 22, "elevation": 0,
                  "dest_map": mid(P + 'DepartmentStore_1F'), "dest_warp_id": "3"})

    made = 0
    for (door, suffix, lay, exits, people, kind) in entries:
        name = P + suffix
        idx = door_index.get(suffix, 0)
        mj = interior_json(suffix, lay, exits, people, idx)

        if suffix == 'DepartmentStore_1F':
            # right-hand doors out, then the stairs up
            mj['warp_events'] += [
                {"x": x, "y": y, "elevation": 0, "dest_map": CITY_MAP,
                 "dest_warp_id": str(second)} for (x, y) in ((9, 15), (10, 14), (11, 15))]
            mj['warp_events'].append(
                {"x": 4, "y": 2, "elevation": 0,
                 "dest_map": mid(P + 'DepartmentStore_2F'), "dest_warp_id": "0"})
        elif suffix == 'DepartmentStore_2F':
            mj['warp_events'] = [
                {"x": 3, "y": 2, "elevation": 0,
                 "dest_map": mid(P + 'DepartmentStore_1F'), "dest_warp_id": "6"},
                {"x": 9, "y": 2, "elevation": 0,
                 "dest_map": mid(P + 'DepartmentStore_3F'), "dest_warp_id": "0"}]
        elif suffix == 'DepartmentStore_3F':
            mj['warp_events'] = [
                {"x": 9, "y": 2, "elevation": 0,
                 "dest_map": mid(P + 'DepartmentStore_2F'), "dest_warp_id": "1"}]

        if kind == 'gamecorner':
            for (x, y) in SLOTS:
                mj['bg_events'].append({
                    "type": "sign", "x": x, "y": y, "elevation": 0,
                    "player_facing_dir": "BG_EVENT_PLAYER_FACING_ANY",
                    "script": "%s_EventScript_SlotMachine" % name})
            mj['bg_events'].append({
                "type": "sign", "x": BROKEN_SLOT[0], "y": BROKEN_SLOT[1], "elevation": 0,
                "player_facing_dir": "BG_EVENT_PLAYER_FACING_ANY",
                "script": "%s_EventScript_BrokenSlot" % name})

        d = os.path.join(REPO, 'data/maps', name)
        os.makedirs(d, exist_ok=True)
        with open(os.path.join(d, 'map.json'), 'w', newline='\n') as f:
            json.dump(mj, f, indent=2)
            f.write('\n')

        out = ['%s_MapScripts::' % name, '\t.byte 0', '']
        text = []
        talkers(name, people, out, text)
        if kind:
            extra_scripts(kind, name, out, text)
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
        made += 1

    city['warp_events'] = warps
    with open(city_path, 'w', newline='\n') as f:
        json.dump(city, f, indent=2)
        f.write('\n')
    with open(groups, 'w', newline='\n') as f:
        json.dump(gj, f, indent=2)
        f.write('\n')
    open(esp, 'w', encoding='utf-8', newline='\n').write(es)
    print('  %d interiors written; Mandarin South now has %d doors that open'
          % (made, len(warps)))


if __name__ == '__main__':
    main()
