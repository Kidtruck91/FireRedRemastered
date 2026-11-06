"""Fill Mandarin South Beach: villas, kiosks, a fishing derby, and a crowd.

The beach was rebuilt by hand as a bare sand shelf behind a cliff wall, with the
doorway from Mandarin South at rows 32-34 on its west edge. This puts a resort
on it.

Everything stamped here is a block the author already drew on Kumquat Beach,
which runs on the same two tilesets (OrangeGeneral + OrangeSevii45), so nothing
is invented and nothing can land on a metatile this tileset pair does not have:

    villa      5x4   the house at Kumquat x45-49 y22-25, door 06F9 at (1,3)
    palm       2x3   x10-11 y12-14
    parasol    2x3   x34-35 y24-26
    boulder    3x3   x18-20 y24-26
    sign       1x1   0402, collision 1, as at x21 y41
    jetty            31BD/31BE/31BF over 31C5/31C6/31C7 over 11CD/11CE

The jetty only starts where the water does. Its bottom row is elevation 1 --
the pier's underside, drawn over the sea -- so laying it across sand would
carve a hole in the beach.

What goes on the beach:

  * four villas along the north shore, each with an interior on one of the
    shared eleven-by-nine house layouts
  * three parasol kiosks on the south shore, where the player arrives: a
    lemonade stand undercutting the marts, a beachcomber selling whatever the
    tide brought in, and a bait shack that will sell you an OLD ROD if you
    turned up without one
  * the Mandarin Fishing Derby, judged at the foot of the jetty
  * ten people to talk to and six trainers

The derby is the Bug Catching Contest's idea without its machinery. You enter,
you fish, you come back and show the judge one POKeMON; it only counts if it
was caught in Mandarin South waters, which the met location records, so you
cannot win it with something from your box. The score is its level plus a
twentieth of its base stat total, which ranks a GYARADOS over a MAGIKARP
without anyone having to write a species table. One small native does the
reading (src/fishing_derby.c); everything else is script.

    python3 docs/orange-islands/build_mandarin_beach.py
"""
import json
import os
import struct

REPO = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
LAYOUT = 'LAYOUT_ORANGE_ISLANDS_MANDARIN_SOUTH_BEACH'
NAME = 'OrangeIslands_Mandarin_South_Beach'
MAPDIR = os.path.join(REPO, 'data/maps', NAME)
PREFIX = 'OrangeIslands_Mandarin_South_Beach'

# ---------------------------------------------------------------------------
# blocks, all copied from Kumquat Beach
# ---------------------------------------------------------------------------
VILLA = [[0x0756, 0x0757, 0x0757, 0x0757, 0x0758],
         [0x0769, 0x076A, 0x076A, 0x076A, 0x076B],
         [0x076C, 0x076D, 0x076E, 0x076F, 0x0770],
         [0x0766, 0x06F9, 0x0767, 0x0768, 0x0765]]
VILLA_DOOR = (1, 3)
PALM = [[0x0682, 0x0683], [0x068A, 0x068B], [0x0692, 0x0693]]
PARASOL = [[0x36BE, 0x36BF], [0x36C6, 0x36C7], [0x32CE, 0x32CF]]
BOULDER = [[0x0498, 0x0499, 0x049A], [0x04A0, 0x0471, 0x04A2], [0x04A8, 0x047C, 0x04AA]]
SIGN = 0x0402
JETTY = {'top': (0x31BD, 0x31BE, 0x31BF), 'mid': (0x31C5, 0x31C6, 0x31C7),
         'under': (0x11CD, 0x11CE, 0x11CE)}
OCEAN = {0x12B, 0x12A, 0x12C, 0x203}

VILLAS = [(12, 7), (20, 7), (28, 7), (36, 7)]
PALMS = [(10, 11), (18, 11), (26, 11), (34, 11), (44, 9), (44, 16), (50, 12),
         (19, 50), (19, 58), (19, 66), (28, 68), (45, 42), (48, 64), (30, 35)]
PARASOLS = [(24, 41), (31, 41), (38, 41),            # the three kiosks
            (16, 18), (40, 20), (26, 55), (36, 62), (44, 52)]
BOULDERS = [(52, 6), (20, 22), (50, 30), (22, 70), (40, 70)]
JETTY_ROWS = (49, 50, 51, 52)      # top, mid, mid, underside
JETTY_X = (54, 68)

# bg sign tiles: (x, y, script suffix)
SIGNS = [(23, 44, 'LemonadeSign'), (30, 44, 'CurioSign'), (37, 44, 'BaitSign'),
         (52, 48, 'DerbySign'), (14, 12, 'VillaSign'),
         # the beach's own sign, which the tidy pass had parked on the cliff by
         # the doorway; it keeps its old script name so nothing else breaks
         (4, 31, 'Town')]

# ---------------------------------------------------------------------------
# people
# ---------------------------------------------------------------------------
# (x, y, elevation, graphics, movement, script suffix, trainer or None)
PEOPLE = [
    (24, 44, 3, 'OBJ_EVENT_GFX_LASS',           'MOVEMENT_TYPE_FACE_DOWN',  'Lemonade', None),
    (31, 44, 3, 'OBJ_EVENT_GFX_OLD_WOMAN',      'MOVEMENT_TYPE_FACE_DOWN',  'Curio', None),
    (38, 44, 3, 'OBJ_EVENT_GFX_LITTLE_BOY',     'MOVEMENT_TYPE_FACE_DOWN',  'Bait', None),
    (52, 50, 3, 'OBJ_EVENT_GFX_GENTLEMAN',      'MOVEMENT_TYPE_FACE_RIGHT', 'Judge', None),
    (14, 11, 3, 'OBJ_EVENT_GFX_WOMAN_3',        'MOVEMENT_TYPE_FACE_DOWN',  'Landlady', None),
    (18, 16, 3, 'OBJ_EVENT_GFX_BEAUTY',         'MOVEMENT_TYPE_FACE_LEFT',  'Sunbather', None),
    (65, 51, 3, 'OBJ_EVENT_GFX_OLD_MAN_1',      'MOVEMENT_TYPE_FACE_RIGHT', 'OldSalt', None),
    (33, 20, 3, 'OBJ_EVENT_GFX_LITTLE_GIRL',    'MOVEMENT_TYPE_WANDER_AROUND', 'Sandcastle', None),
    (43, 30, 3, 'OBJ_EVENT_GFX_FAT_MAN',        'MOVEMENT_TYPE_FACE_DOWN',  'Tourist', None),
    (45, 60, 3, 'OBJ_EVENT_GFX_SAILOR',         'MOVEMENT_TYPE_FACE_RIGHT', 'Lifeguard', None),
    # trainers
    (64, 14, 1, 'OBJ_EVENT_GFX_SWIMMER_F_WATER', 'MOVEMENT_TYPE_FACE_LEFT', 'OrangeNerissa',
     ('TRAINER_ORANGE_SWIMMER_F_NERISSA', 'SWIMMER_F', 'SwimmerF_Intermediate', 4)),
    (60, 55, 1, 'OBJ_EVENT_GFX_SWIMMER_M_WATER', 'MOVEMENT_TYPE_FACE_LEFT', 'OrangeCoy',
     ('TRAINER_ORANGE_SWIMMER_M_COY', 'SWIMMER_M', 'SwimmerM_Intermediate', 4)),
    (60, 50, 3, 'OBJ_EVENT_GFX_FISHERMAN',      'MOVEMENT_TYPE_FACE_LEFT',  'OrangeMarlin',
     ('TRAINER_ORANGE_FISHERMAN_MARLIN', 'FISHERMAN', 'Fisherman_Advanced', 4)),
    (27, 30, 3, 'OBJ_EVENT_GFX_BEAUTY',         'MOVEMENT_TYPE_FACE_DOWN',  'OrangeCoral',
     ('TRAINER_ORANGE_BEAUTY_CORAL', 'BEAUTY', 'Beauty_Intermediate', 3)),
    (36, 15, 3, 'OBJ_EVENT_GFX_PICNICKER',      'MOVEMENT_TYPE_FACE_DOWN',  'OrangePia',
     ('TRAINER_ORANGE_PICNICKER_PIA', 'PICNICKER', 'Picnicker_Intermediate', 3)),
    (22, 65, 3, 'OBJ_EVENT_GFX_SAILOR',         'MOVEMENT_TYPE_FACE_UP',    'OrangeBosun',
     ('TRAINER_ORANGE_SAILOR_BOSUN', 'SAILOR', 'Sailor_Advanced', 4)),
]

DERBY_LEVEL = 36          # the derby crowd is about where the fourth badge is

# ---------------------------------------------------------------------------
# villa interiors
# ---------------------------------------------------------------------------
INTERIORS = [
    ('OrangeIslands_Mandarin_South_Beach_Villa1', 'LAYOUT_HOUSE1', (3, 4, 5),
     [('OBJ_EVENT_GFX_WOMAN_2', 3, 3, 'MOVEMENT_TYPE_FACE_DOWN', 'Owner',
       ['"We let the villa out by the week.\\n"',
        '"Sea on three sides and not a single\\l"', '"car. Bliss.$"']),
      ('OBJ_EVENT_GFX_LITTLE_BOY', 7, 4, 'MOVEMENT_TYPE_WANDER_AROUND', 'Boy',
       ['"Dad says if I win the DERBY he\'ll\\n"', '"buy me a GOOD ROD.$"'])]),
    ('OrangeIslands_Mandarin_South_Beach_Villa2', 'LAYOUT_HOUSE5', (3, 4, 5),
     [('OBJ_EVENT_GFX_SCIENTIST', 4, 3, 'MOVEMENT_TYPE_FACE_DOWN', 'Scientist',
       ['"The shelf off the jetty drops to\\n"', '"forty strides. That is where the\\l"',
        '"big ones sit.$"']),
      ('OBJ_EVENT_GFX_FAT_MAN', 8, 5, 'MOVEMENT_TYPE_FACE_LEFT', 'Man',
       ['"I come for a week every year and\\n"', '"I have never once been in the sea.$"'])]),
    ('OrangeIslands_Mandarin_South_Beach_Villa3', 'LAYOUT_HOUSE3', (4,),
     [('OBJ_EVENT_GFX_OLD_WOMAN', 4, 3, 'MOVEMENT_TYPE_FACE_DOWN', 'OldWoman',
       ['"The DERBY judge has stood at the\\n"', '"foot of that jetty for forty years.\\l"',
        '"He has never entered it once.$"'])]),
    ('OrangeIslands_Mandarin_South_Beach_Villa4', 'LAYOUT_HOUSE4', (3,),
     [('OBJ_EVENT_GFX_YOUNGSTER', 4, 4, 'MOVEMENT_TYPE_FACE_DOWN', 'Youngster',
       ['"A POKeMON caught somewhere else\\n"', '"will not do for the DERBY. They\\l"',
        '"check where it was hooked.$"'])]),
]

LEMONADE_PRICE = 200      # the marts want 350
OLD_ROD_PRICE = 300

CURIO = [('ITEM_PEARL', 400, 'PEARL'), ('ITEM_BIG_PEARL', 1200, 'BIG PEARL'),
         ('ITEM_STARDUST', 600, 'STARDUST'), ('ITEM_WATER_STONE', 2000, 'WATER STONE'),
         ('ITEM_HEART_SCALE', 900, 'HEART SCALE'), ('ITEM_MYSTIC_WATER', 1500, 'MYSTIC WATER')]


def layout():
    doc = json.load(open(os.path.join(REPO, 'data/layouts/layouts.json')))
    return next(l for l in doc['layouts'] if l['id'] == LAYOUT)


def paint():
    l = layout()
    w, h = l['width'], l['height']
    path = os.path.join(REPO, l['blockdata_filepath'])
    data = bytearray(open(path, 'rb').read())
    at = lambda x, y: struct.unpack_from('<H', data, 2 * (y * w + x))[0]
    put = lambda x, y, v: struct.pack_into('<H', data, 2 * (y * w + x), v)

    def stamp(block, x0, y0):
        for dy, row in enumerate(block):
            for dx, v in enumerate(row):
                put(x0 + dx, y0 + dy, v)

    for p in VILLAS:
        stamp(VILLA, *p)
    for p in PALMS:
        stamp(PALM, *p)
    for p in PARASOLS:
        stamp(PARASOL, *p)
    for p in BOULDERS:
        stamp(BOULDER, *p)
    for (x, y, _s) in SIGNS:
        put(x, y, SIGN)

    x0, x1 = JETTY_X
    ytop, ya, yb, yu = JETTY_ROWS
    for x in range(x0, x1 + 1):
        pick = 0 if x == x0 else (2 if x == x1 else 1)
        put(x, ytop, JETTY['top'][pick])
        put(x, ya, JETTY['mid'][pick])
        put(x, yb, JETTY['mid'][pick])
        # the underside is elevation 1: only ever over water, never over sand
        if (at(x, yu) & 0x3FF) in OCEAN:
            put(x, yu, JETTY['under'][pick])
    open(path, 'wb').write(bytes(data))
    print('  painted %d villas, %d palms, %d parasols, %d boulders, a %d-tile jetty'
          % (len(VILLAS), len(PALMS), len(PARASOLS), len(BOULDERS), x1 - x0 + 1))


def map_json():
    p = os.path.join(MAPDIR, 'map.json')
    mj = json.load(open(p))
    mj['warp_events'] = [
        {"x": vx + VILLA_DOOR[0], "y": vy + VILLA_DOOR[1], "elevation": 0,
         "dest_map": 'MAP_' + INTERIORS[i][0].upper().replace('ORANGEISLANDS',
                                                              'ORANGE_ISLANDS'),
         "dest_warp_id": "0"}
        for i, (vx, vy) in enumerate(VILLAS)]

    mj['object_events'] = []
    for (x, y, el, gfx, move, suffix, trainer) in PEOPLE:
        mj['object_events'].append({
            "type": "object", "graphics_id": gfx, "x": x, "y": y, "elevation": el,
            "movement_type": move, "movement_range_x": 1, "movement_range_y": 1,
            "trainer_type": "TRAINER_TYPE_NORMAL" if trainer else "TRAINER_TYPE_NONE",
            "trainer_sight_or_berry_tree_id": "4" if trainer else "0",
            "script": "%s_EventScript_%s" % (PREFIX, suffix), "flag": "0"})

    keep = []
    for (x, y, suffix) in SIGNS:
        keep.append({"type": "sign", "x": x, "y": y, "elevation": 0,
                     "player_facing_dir": "BG_EVENT_PLAYER_FACING_ANY",
                     "script": "%s_EventScript_%s" % (PREFIX, suffix)})
    mj['bg_events'] = keep
    with open(p, 'w', newline='\n') as f:
        json.dump(mj, f, indent=2)
        f.write('\n')
    print('  map.json: %d villa doors, %d people, %d signs'
          % (len(mj['warp_events']), len(mj['object_events']), len(mj['bg_events'])))


# ---------------------------------------------------------------------------
# what the people say
# ---------------------------------------------------------------------------
TALK = {
    'Landlady': ['"Villa four is free next week if\\n"', '"you fancy it.$"'],
    'Sunbather': ['"Half of MANDARIN SOUTH comes out\\n"', '"here the moment the sun is up.$"'],
    'OldSalt': ['"Sixty years I have fished off this\\n"', '"jetty. The sea still surprises me.$"'],
    'Sandcastle': ['"My sandcastle has a moat AND a\\n"', '"drawbridge. Do not step on it!$"'],
    'Tourist': ['"There is nothing out here but sand\\n"', '"and water, and that is the point.$"'],
    'Lifeguard': ['"Past the jetty the current runs\\n"', '"hard. Surf it if you must, but\\l"',
                  '"keep your POKeMON close.$"'],
    'LemonadeSign': ['"SHELLY\'S LEMONADE\\n"', '"Cheaper than town. Always has been.$"'],
    'CurioSign': ['"THE TIDE LINE\\n"', '"Whatever the sea left this morning.$"'],
    'BaitSign': ['"BAIT SHACK\\n"', '"Rods, bait, and bad advice.$"'],
    'DerbySign': ['"THE MANDARIN FISHING DERBY\\n"',
                  '"Entries judged at the jetty.\\l"',
                  '"Hooked in MANDARIN waters only.$"'],
    'VillaSign': ['"BEACH VILLAS 1 TO 4\\n"', '"Quiet, please. People are asleep.$"'],
    'Town': ['"MANDARIN SOUTH BEACH\\n"',
             '"Through the gap and you are back\\l"', '"in the city.$"'],
}

TRAINER_LINES = {
    'OrangeNerissa': (['"The water is warmer on this side\\n"', '"of the island. Come in!$"'],
                      ['"Brrr! Out, out!$"'],
                      ['"My POKeMON swim out past the\\n"', '"jetty every morning.$"']),
    'OrangeCoy': (['"You are a long way from the\\n"', '"shore, friend.$"'],
                  ['"Out of my depth.$"'],
                  ['"Watch the current past the jetty.\\n"', '"It will carry you off.$"']),
    'OrangeMarlin': (['"Shh. No -- too late, you have\\n"', '"scared them. Battle me instead.$"'],
                     ['"Reeled in.$"'],
                     ['"I have won the DERBY twice. Never\\n"', '"with anything I expected.$"']),
    'OrangeCoral': (['"Do not kick sand on my towel.$"'],
                    ['"My hair! My hair!$"'],
                    ['"I only came out here to read.$"']),
    'OrangePia': (['"We are having a picnic. You can\\n"', '"have a battle.$"'],
                  ['"No pudding for me, then.$"'],
                  ['"The lemonade stand is the cheapest\\n"', '"on the islands. Go on.$"']),
    'OrangeBosun': (['"Off the boat an hour and already\\n"', '"spoiling for a scrap.$"'],
                    ['"Back to the galley.$"'],
                    ['"We put in here for water. I stayed\\n"', '"for the lemonade.$"']),
}


def scripts():
    L = []
    T = []
    w = L.append

    def text(name, lines):
        T.append('%s_Text_%s::' % (PREFIX, name))
        for ln in lines:
            T.append('\t.string %s' % ln)
        T.append('')

    w('%s_MapScripts::' % PREFIX)
    w('\t.byte 0')
    w('')

    # ---- Shelly's lemonade stand -----------------------------------------
    w('@ The marts want 350 for a LEMONADE. Shelly wants %d, and says so.' % LEMONADE_PRICE)
    w('%s_EventScript_Lemonade::' % PREFIX)
    w('\tlock')
    w('\tfaceplayer')
    w('\tmsgbox %s_Text_LemonadeAsk, MSGBOX_YESNO' % PREFIX)
    w('\tgoto_if_eq VAR_RESULT, NO, %s_EventScript_LemonadeNo' % PREFIX)
    w('\tcheckmoney %d' % LEMONADE_PRICE)
    w('\tgoto_if_eq VAR_RESULT, FALSE, %s_EventScript_LemonadeBroke' % PREFIX)
    w('\tshowmoneybox 0, 0')
    w('\tcheckitemspace ITEM_LEMONADE')
    w('\tgoto_if_eq VAR_RESULT, FALSE, %s_EventScript_LemonadeFull' % PREFIX)
    w('\tremovemoney %d' % LEMONADE_PRICE)
    w('\tupdatemoneybox')
    w('\tplayse SE_SHOP')
    w('\thidemoneybox')
    w('\tgiveitem ITEM_LEMONADE')
    w('\tmsgbox %s_Text_LemonadeThanks, MSGBOX_DEFAULT' % PREFIX)
    w('\trelease')
    w('\tend')
    for tag, body in (('No', 'LemonadeNo'), ('Broke', 'LemonadeBroke'), ('Full', 'LemonadeFull')):
        w('%s_EventScript_%s::' % (PREFIX, body))
        w('\tmsgbox %s_Text_%s, MSGBOX_DEFAULT' % (PREFIX, body))
        w('\trelease')
        w('\tend')
    w('')
    text('LemonadeAsk', ['"LEMONADE, ice cold, %d.\\n"' % LEMONADE_PRICE,
                         '"Town charges 350. Want one?$"'])
    text('LemonadeThanks', ['"Shelly: Drink it before it goes\\n"', '"warm, mind.$"'])
    text('LemonadeNo', ['"Shelly: Suit yourself. It is only\\n"', '"going to get hotter.$"'])
    text('LemonadeBroke', ['"Shelly: Come back when you have\\n"', '"the coins, love.$"'])
    text('LemonadeFull', ['"Shelly: You have nowhere to put it!$"'])

    # ---- the tide line: one item, whatever washed up ----------------------
    w('@ A different thing every time you ask, because that is what the tide left.')
    w('@ checkmoney and removemoney take a literal, never a variable, so the price')
    w('@ has to be branched on the same way the CELADON vending machine does it.')
    w('%s_EventScript_Curio::' % PREFIX)
    w('\tlock')
    w('\tfaceplayer')
    w('\tmsgbox %s_Text_CurioIntro, MSGBOX_DEFAULT' % PREFIX)
    w('\trandom %d' % len(CURIO))
    w('\tcopyvar VAR_TEMP_1, VAR_RESULT')
    for i in range(len(CURIO)):
        w('\tgoto_if_eq VAR_TEMP_1, %d, %s_EventScript_CurioStock%d' % (i, PREFIX, i))
    w('\tgoto %s_EventScript_CurioStock0' % PREFIX)
    w('\tend')
    w('')
    for i, (item, price, _label) in enumerate(CURIO):
        w('%s_EventScript_CurioStock%d::' % (PREFIX, i))
        w('\tsetvar VAR_TEMP_0, %s' % item)
        w('\tsetvar VAR_TEMP_2, %d' % price)
        w('\tgoto %s_EventScript_CurioOffer' % PREFIX)
        w('\tend')
    w('')
    w('%s_EventScript_CurioOffer::' % PREFIX)
    w('\tbufferitemname STR_VAR_1, VAR_TEMP_0')
    w('\tbuffernumberstring STR_VAR_2, VAR_TEMP_2')
    w('\tmsgbox %s_Text_CurioOffer, MSGBOX_YESNO' % PREFIX)
    w('\tgoto_if_eq VAR_RESULT, NO, %s_EventScript_CurioNo' % PREFIX)
    for i in range(len(CURIO)):
        w('\tcall_if_eq VAR_TEMP_1, %d, %s_EventScript_CurioCheck%d' % (i, PREFIX, i))
    w('\tgoto_if_eq VAR_RESULT, FALSE, %s_EventScript_CurioBroke' % PREFIX)
    w('\tcheckitemspace VAR_TEMP_0')
    w('\tgoto_if_eq VAR_RESULT, FALSE, %s_EventScript_CurioFull' % PREFIX)
    w('\tshowmoneybox 0, 0')
    for i in range(len(CURIO)):
        w('\tcall_if_eq VAR_TEMP_1, %d, %s_EventScript_CurioPay%d' % (i, PREFIX, i))
    w('\tupdatemoneybox')
    w('\tplayse SE_SHOP')
    w('\thidemoneybox')
    w('\tgiveitem VAR_TEMP_0')
    w('\tmsgbox %s_Text_CurioThanks, MSGBOX_DEFAULT' % PREFIX)
    w('\trelease')
    w('\tend')
    w('')
    for i, (item, price, _label) in enumerate(CURIO):
        w('%s_EventScript_CurioCheck%d::' % (PREFIX, i))
        w('\tcheckmoney %d' % price)
        w('\treturn')
        w('%s_EventScript_CurioPay%d::' % (PREFIX, i))
        w('\tremovemoney %d' % price)
        w('\treturn')
    w('')
    for body in ('CurioNo', 'CurioBroke', 'CurioFull'):
        w('%s_EventScript_%s::' % (PREFIX, body))
        w('\tmsgbox %s_Text_%s, MSGBOX_DEFAULT' % (PREFIX, body))
        w('\trelease')
        w('\tend')
    w('')
    text('CurioIntro', ['"I only sell what the tide brings\\n"', '"in. Let me see what I have...$"'])
    text('CurioOffer', ['"A {STR_VAR_1}! That is a fine one.\\p"',
                        '"{STR_VAR_2} and it is yours.$"'])
    text('CurioThanks', ['"Mind the sharp edge. Come back at\\n"', '"low tide.$"'])
    text('CurioNo', ['"It will be gone by this evening,\\n"', '"you know.$"'])
    text('CurioBroke', ['"No coins, no curio. Them is the\\n"', '"rules of the beach.$"'])
    text('CurioFull', ['"You are carrying too much already!$"'])

    # ---- bait shack: sells the rod the derby needs ------------------------
    w('@ The derby is unenterable without a rod, so the shack keeps a spare.')
    w('%s_EventScript_Bait::' % PREFIX)
    w('\tlock')
    w('\tfaceplayer')
    w('\tcheckitem ITEM_OLD_ROD')
    w('\tgoto_if_eq VAR_RESULT, TRUE, %s_EventScript_BaitHasRod' % PREFIX)
    w('\tcheckitem ITEM_GOOD_ROD')
    w('\tgoto_if_eq VAR_RESULT, TRUE, %s_EventScript_BaitHasRod' % PREFIX)
    w('\tcheckitem ITEM_SUPER_ROD')
    w('\tgoto_if_eq VAR_RESULT, TRUE, %s_EventScript_BaitHasRod' % PREFIX)
    w('\tmsgbox %s_Text_BaitAsk, MSGBOX_YESNO' % PREFIX)
    w('\tgoto_if_eq VAR_RESULT, NO, %s_EventScript_BaitNo' % PREFIX)
    w('\tcheckmoney %d' % OLD_ROD_PRICE)
    w('\tgoto_if_eq VAR_RESULT, FALSE, %s_EventScript_BaitBroke' % PREFIX)
    w('\tshowmoneybox 0, 0')
    w('\tremovemoney %d' % OLD_ROD_PRICE)
    w('\tupdatemoneybox')
    w('\tplayse SE_SHOP')
    w('\thidemoneybox')
    w('\tgiveitem ITEM_OLD_ROD')
    w('\tmsgbox %s_Text_BaitThanks, MSGBOX_DEFAULT' % PREFIX)
    w('\trelease')
    w('\tend')
    for body in ('BaitHasRod', 'BaitNo', 'BaitBroke'):
        w('%s_EventScript_%s::' % (PREFIX, body))
        w('\tmsgbox %s_Text_%s, MSGBOX_DEFAULT' % (PREFIX, body))
        w('\trelease')
        w('\tend')
    w('')
    text('BaitAsk', ['"No rod? You cannot enter the DERBY\\n"',
                     '"without one.\\p"', '"OLD ROD, %d. Go on.$"' % OLD_ROD_PRICE])
    text('BaitThanks', ['"It has caught a winner before. Not\\n"', '"for me, mind.$"'])
    text('BaitHasRod', ['"Already tooled up, I see. The DERBY\\n"',
                        '"judge is down by the jetty.$"'])
    text('BaitNo', ['"Suit yourself. Watching is free.$"'])
    text('BaitBroke', ['"Not even %d? Try the DERBY\'s\\n"' % OLD_ROD_PRICE,
                       '"consolation prize, that pays.$"'])

    # ---- the derby --------------------------------------------------------
    w('@ The Bug Catching Contest without the machinery. You fish in your own')
    w('@ time and bring the judge one POKeMON; FishingDerby_Score reads its met')
    w('@ location, so only something hooked in MANDARIN SOUTH waters counts, and')
    w('@ ranks it by level plus a twentieth of its base stat total.')
    w('@   VAR_RESULT      0 rejected, 1-5 the prize band')
    w('@   VAR_0x8005      the score itself')
    w('@   VAR_0x8006      0 fine, 1 no POKeMON there, 2 an egg, 3 caught elsewhere,')
    w('@                   4 not a WATER type')
    w('@   STR_VAR_1       its nickname')
    w('%s_EventScript_Judge::' % PREFIX)
    w('\tlock')
    w('\tfaceplayer')
    w('\tgoto_if_set FLAG_ORANGE_DERBY_RULES, %s_EventScript_JudgeAsk' % PREFIX)
    w('\tmsgbox %s_Text_DerbyRules, MSGBOX_DEFAULT' % PREFIX)
    w('\tsetflag FLAG_ORANGE_DERBY_RULES')
    w('%s_EventScript_JudgeAsk::' % PREFIX)
    w('\tgoto_if_eq VAR_ORANGE_DERBY_BEST, 0, %s_EventScript_JudgeAskNow' % PREFIX)
    w('\tbuffernumberstring STR_VAR_3, VAR_ORANGE_DERBY_BEST')
    w('\tmsgbox %s_Text_DerbyBest, MSGBOX_DEFAULT' % PREFIX)
    w('%s_EventScript_JudgeAskNow::' % PREFIX)
    w('\tmsgbox %s_Text_DerbyAsk, MSGBOX_YESNO' % PREFIX)
    w('\tgoto_if_eq VAR_RESULT, NO, %s_EventScript_DerbyLater' % PREFIX)
    w('\tspecial ChoosePartyMon')
    w('\twaitstate')
    w('\tgoto_if_ge VAR_0x8004, PARTY_SIZE, %s_EventScript_DerbyLater' % PREFIX)
    w('\tcallnative FishingDerby_Score')
    w('\tgoto_if_eq VAR_RESULT, 0, %s_EventScript_DerbyRejected' % PREFIX)
    w('\tbuffernumberstring STR_VAR_2, VAR_0x8005')
    w('\tmsgbox %s_Text_DerbyWeighIn, MSGBOX_DEFAULT' % PREFIX)
    w('\tcall %s_EventScript_DerbyRecordBest' % PREFIX)
    w('\tgoto_if_eq VAR_RESULT, 5, %s_EventScript_DerbyGrand' % PREFIX)
    w('\tgoto_if_eq VAR_RESULT, 4, %s_EventScript_DerbyFirst' % PREFIX)
    w('\tgoto_if_eq VAR_RESULT, 3, %s_EventScript_DerbySecond' % PREFIX)
    w('\tgoto_if_eq VAR_RESULT, 2, %s_EventScript_DerbyThird' % PREFIX)
    w('\tgoto %s_EventScript_DerbyConsolation' % PREFIX)
    w('\tend')
    w('')
    w('@ The personal best survives between visits so the judge can hold you to it.')
    w('%s_EventScript_DerbyRecordBest::' % PREFIX)
    w('\tgoto_if_le VAR_0x8005, VAR_ORANGE_DERBY_BEST, %s_EventScript_DerbyKeepBest' % PREFIX)
    w('\tcopyvar VAR_ORANGE_DERBY_BEST, VAR_0x8005')
    w('%s_EventScript_DerbyKeepBest::' % PREFIX)
    w('\treturn')
    w('')
    w('@ The grand prize is handed out once; after that the top band pays as first.')
    w('%s_EventScript_DerbyGrand::' % PREFIX)
    w('\tgoto_if_set FLAG_ORANGE_DERBY_GRAND, %s_EventScript_DerbyFirst' % PREFIX)
    w('\tsetflag FLAG_ORANGE_DERBY_GRAND')
    w('\tmsgbox %s_Text_DerbyGrand, MSGBOX_DEFAULT' % PREFIX)
    w('\tgiveitem ITEM_MYSTIC_WATER')
    w('\trelease')
    w('\tend')
    for band, (label, item, say) in enumerate((
            ('DerbyFirst', 'ITEM_BIG_PEARL', 'DerbyFirstText'),
            ('DerbySecond', 'ITEM_PEARL', 'DerbySecondText'),
            ('DerbyThird', 'ITEM_STARDUST', 'DerbyThirdText'),
            ('DerbyConsolation', 'ITEM_LEMONADE', 'DerbyConsolationText'))):
        w('%s_EventScript_%s::' % (PREFIX, label))
        w('\tmsgbox %s_Text_%s, MSGBOX_DEFAULT' % (PREFIX, say))
        w('\tgiveitem %s' % item)
        w('\trelease')
        w('\tend')
    w('%s_EventScript_DerbyRejected::' % PREFIX)
    w('\tgoto_if_eq VAR_0x8006, 2, %s_EventScript_DerbyEgg' % PREFIX)
    w('\tgoto_if_eq VAR_0x8006, 3, %s_EventScript_DerbyElsewhere' % PREFIX)
    w('\tgoto_if_eq VAR_0x8006, 4, %s_EventScript_DerbyDryLand' % PREFIX)
    w('\tmsgbox %s_Text_DerbyNothing, MSGBOX_DEFAULT' % PREFIX)
    w('\trelease')
    w('\tend')
    for label, say in (('DerbyEgg', 'DerbyEggText'),
                       ('DerbyElsewhere', 'DerbyElsewhereText'),
                       ('DerbyDryLand', 'DerbyDryLandText'),
                       ('DerbyLater', 'DerbyLaterText')):
        w('%s_EventScript_%s::' % (PREFIX, label))
        w('\tmsgbox %s_Text_%s, MSGBOX_DEFAULT' % (PREFIX, say))
        w('\trelease')
        w('\tend')
    w('')
    text('DerbyRules', ['"The MANDARIN FISHING DERBY, young\\n"', '"one. The rules are short.\\p"',
                        '"Fish these waters. Bring me one\\n"', '"POKeMON. I will judge it.\\p"',
                        '"It must have been hooked here, in\\n"',
                        '"MANDARIN waters. I can tell.$"'])
    text('DerbyBest', ['"Your best so far scored {STR_VAR_3}.$"'])
    text('DerbyAsk', ['"Do you have something to show me?$"'])
    text('DerbyWeighIn', ['"Let me see... {STR_VAR_1}.\\p"',
                          '"Hmm. I make that {STR_VAR_2} points.$"'])
    text('DerbyGrand', ['"{STR_VAR_2}! In forty years I have\\n"', '"written that number twice.\\p"',
                        '"The DERBY has a standing prize for\\n"',
                        '"a catch like that. Take it.$"'])
    text('DerbyFirstText', ['"First class. The jetty regulars\\n"', '"will be sour about this.$"'])
    text('DerbySecondText', ['"A respectable catch. Not a winner,\\n"', '"but respectable.$"'])
    text('DerbyThirdText', ['"Modest. The sea was not generous\\n"', '"to you today.$"'])
    text('DerbyConsolationText', ['"Everyone goes home with something.\\n"',
                                  '"Even you.$"'])
    text('DerbyNothing', ['"There is nothing in that slot.$"'])
    text('DerbyEggText', ['"An EGG. I admire the nerve.$"'])
    text('DerbyElsewhereText', ['"This was never hooked here. I have\\n"',
                                '"judged this DERBY for forty years;\\l"',
                                '"do not try it on with me.$"'])
    text('DerbyDryLandText', ['"That has never been in the water in\\n"',
                              '"its life. A WATER type, please.$"'])
    text('DerbyLaterText', ['"The jetty is free all day. Go and\\n"', '"catch something.$"'])

    # ---- everyone else ----------------------------------------------------
    w('@ ---- the rest of the beach ----')
    for name in sorted(TALK):
        w('%s_EventScript_%s::' % (PREFIX, name))
        w('\tmsgbox %s_Text_%s, %s' % (PREFIX, name,
                                       'MSGBOX_SIGN' if name.endswith('Sign') else 'MSGBOX_NPC'))
        w('\tend')
        w('')
        text(name, TALK[name])

    w('@ ---- trainers ----')
    for (_x, _y, _el, _g, _m, suffix, trainer) in PEOPLE:
        if not trainer:
            continue
        w('%s_EventScript_%s::' % (PREFIX, suffix))
        w('\ttrainerbattle_single %s, %s_Text_%sIntro, %s_Text_%sDefeat'
          % (trainer[0], PREFIX, suffix, PREFIX, suffix))
        w('\tmsgbox %s_Text_%sPostBattle, MSGBOX_AUTOCLOSE' % (PREFIX, suffix))
        w('\tend')
        w('')
        intro, defeat, post = TRAINER_LINES[suffix]
        text(suffix + 'Intro', intro)
        text(suffix + 'Defeat', defeat)
        text(suffix + 'PostBattle', post)

    open(os.path.join(MAPDIR, 'scripts.inc'), 'w', encoding='utf-8',
         newline='\n').write('\n'.join(L) + '\n')
    # Both files are written whole: everything the beach says is generated here,
    # the old town sign included, so nothing is carried over and nothing doubles up.
    open(os.path.join(MAPDIR, 'text.inc'), 'w', encoding='utf-8',
         newline='\n').write('\n'.join(T) + '\n')
    print('  scripts.inc: 3 kiosks, the derby, %d talkers, %d signs, %d trainers'
          % (len(TALK) - len(SIGNS), len(SIGNS), sum(1 for p in PEOPLE if p[6])))


def interiors():
    groups = os.path.join(REPO, 'data/maps/map_groups.json')
    gj = json.load(open(groups))
    # Interiors belong in the island's interior group, not the exterior one;
    # the map checker takes the exterior group as its list of maps to walk.
    key = 'gMapGroup_Orange_Mandarin_South_Interior'
    outside = next(k for k, v in gj.items() if isinstance(v, list) and NAME in v)
    for name, _l, _w, _p in INTERIORS:
        if name in gj[outside]:
            gj[outside].remove(name)
    esp = os.path.join(REPO, 'data/event_scripts.s')
    es = open(esp, encoding='utf-8').read()

    for i, (name, lay, warps, people) in enumerate(INTERIORS):
        mid = 'MAP_' + name.upper().replace('ORANGEISLANDS', 'ORANGE_ISLANDS')
        d = os.path.join(REPO, 'data/maps', name)
        os.makedirs(d, exist_ok=True)
        mj = {"id": mid, "name": name, "layout": lay, "music": "MUS_FUCHSIA",
              "region_map_section": "MAPSEC_MANDARIN_SOUTH", "requires_flash": False,
              "weather": "WEATHER_NONE", "map_type": "MAP_TYPE_INDOOR",
              "allow_cycling": False, "allow_escaping": False, "allow_running": False,
              "show_map_name": False, "floor_number": 0,
              "battle_scene": "MAP_BATTLE_SCENE_NORMAL", "connections": 0,
              "object_events": [
                  {"type": "object", "graphics_id": g, "x": x, "y": y, "elevation": 3,
                   "movement_type": m, "movement_range_x": 1, "movement_range_y": 1,
                   "trainer_type": "TRAINER_TYPE_NONE",
                   "trainer_sight_or_berry_tree_id": "0",
                   "script": "%s_EventScript_%s" % (name, s), "flag": "0"}
                  for (g, x, y, m, s, _t) in people],
              "warp_events": [
                  {"x": x, "y": 7, "elevation": 3,
                   "dest_map": "MAP_ORANGE_ISLANDS_MANDARIN_SOUTH_BEACH",
                   "dest_warp_id": str(i)} for x in warps],
              "coord_events": [], "bg_events": []}
        with open(os.path.join(d, 'map.json'), 'w', newline='\n') as f:
            json.dump(mj, f, indent=2)
            f.write('\n')
        with open(os.path.join(d, 'scripts.inc'), 'w', newline='\n') as f:
            f.write('%s_MapScripts::\n\t.byte 0\n' % name)
            for (_g, _x, _y, _m, s, _t) in people:
                f.write('\n%s_EventScript_%s::\n\tmsgbox %s_Text_%s, MSGBOX_NPC\n\tend\n'
                        % (name, s, name, s))
        with open(os.path.join(d, 'text.inc'), 'w', newline='\n') as f:
            for (_g, _x, _y, _m, s, lines) in people:
                f.write('%s_Text_%s::\n' % (name, s))
                for ln in lines:
                    f.write('\t.string %s\n' % ln)
                f.write('\n')
        if name not in gj[key]:
            gj[key].append(name)
        inc = ('\t.include "data/maps/%s/scripts.inc"\n'
               '\t.include "data/maps/%s/text.inc"\n' % (name, name))
        if inc not in es:
            es += inc

    with open(groups, 'w', newline='\n') as f:
        json.dump(gj, f, indent=2)
        f.write('\n')
    open(esp, 'w', encoding='utf-8', newline='\n').write(es)
    print('  %d villa interiors written and registered' % len(INTERIORS))


def trainers():
    """Append the six beach trainers to the constants and the trainer table."""
    op = os.path.join(REPO, 'include/constants/opponents.h')
    s = open(op, encoding='utf-8').read()
    import re
    num = int(re.search(r'#define NUM_TRAINERS\s+(\d+)', s).group(1))
    tp = os.path.join(REPO, 'src/data/trainers.h')
    table = open(tp, encoding='utf-8').read()
    # The constant and the table entry are added independently, so decide each
    # on its own; a half-finished run must be able to finish on the next one.
    added = [t for t in (p[6] for p in PEOPLE if p[6]) if t[0] not in table]
    block = ''
    nxt = num
    for (const, cls, pool, size) in (t for t in (p[6] for p in PEOPLE if p[6])
                                     if t[0] not in s):
        block += '#define %-40s %d\n' % (const, nxt)
        nxt += 1
    if block:
        anchor = '\n#define NUM_TRAINERS'
        s = s.replace(anchor, '\n@MARK' + anchor, 1)
        s = s.replace('@MARK\n#define NUM_TRAINERS',
                      '\n@ Mandarin South Beach (build_mandarin_beach.py)\n' + block
                      + '\n#define NUM_TRAINERS', 1)
        s = re.sub(r'#define NUM_TRAINERS\s+\d+',
                   '#define NUM_TRAINERS                             %d' % nxt, s)
        open(op, 'w', encoding='utf-8', newline='\n').write(s)

    t = table
    entries = ''
    for (const, cls, pool, size) in added:
        gender = 'FEMALE' if cls.endswith('_F') or cls in ('BEAUTY', 'PICNICKER') else 'MALE'
        music = {'SWIMMER_F': 'FEMALE', 'SWIMMER_M': 'MALE', 'BEAUTY': 'FEMALE',
                 'PICNICKER': 'FEMALE', 'FISHERMAN': 'MALE', 'SAILOR': 'MALE'}[cls]
        entries += (
            '    [%s] = {\n'
            '        .trainerClass = TRAINER_CLASS_%s,\n'
            '        .encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_%s,\n'
            '        .trainerPic = TRAINER_PIC_%s,\n'
            '        .trainerName = _("%s"),\n'
            '        .items = {},\n'
            '        .doubleBattle = FALSE,\n'
            '        .aiFlags = AI_LEVEL_NORMAL,\n'
            '        .party = POOL_PARTY(sPool_%s, %d, %d, %d, 2),\n'
            '    },\n'
            % (const, cls, music, cls, const.rsplit('_', 1)[-1], pool,
               max(2, size - 1), size, DERBY_LEVEL))
    if entries:
        # The table ends with "};" and a trailing comment block about the pools,
        # so splice in before that brace rather than at the end of the file.
        close = t.rindex('\n};')
        t = t[:close + 1] + entries + t[close + 1:]
        open(tp, 'w', encoding='utf-8', newline='\n').write(t)
    print('  trainers: %d added (NUM_TRAINERS now %d)' % (len(added), nxt))


NATIVE = r'''// The Mandarin Fishing Derby.
//
// A contest in the spirit of the Bug Catching Contest, but without a contest
// mode: the player fishes in their own time and brings the judge at the foot
// of the Mandarin South Beach jetty one POKeMON to be weighed.
//
// What makes it a fishing contest rather than "show me your best mon" is the
// met location. A POKeMON records the MAPSEC it was caught in, and Mandarin
// South and its beach share MAPSEC_MANDARIN_SOUTH, so an entry only counts if
// it was hooked (or surfed up) in these waters. Nothing from the box, nothing
// traded, nothing carried over from Kanto.
//
// The score is the level plus a twentieth of the base stat total. That ranks a
// GYARADOS (540 -> +27) well above the MAGIKARP it came from (200 -> +10)
// without anyone maintaining a table of species, and it keeps levelling a
// modest catch worthwhile.
//
// Script use:
//   special ChoosePartyMon   /  waitstate      @ VAR_0x8004 = slot, 255 cancelled
//   callnative FishingDerby_Score
//     VAR_RESULT  0 rejected, else the prize band 1 (consolation) to 5 (grand)
//     VAR_0x8005  the score
//     VAR_0x8006  why it was rejected: 1 empty slot, 2 an egg, 3 caught
//                 elsewhere, 4 not a WATER type
//     STR_VAR_1   its nickname

#include "global.h"
#include "event_data.h"
#include "pokemon.h"
#include "string_util.h"
#include "constants/region_map_sections.h"
#include "constants/pokemon.h"

#define DERBY_MAPSEC        MAPSEC_MANDARIN_SOUTH
#define DERBY_BAND_GRAND    95
#define DERBY_BAND_FIRST    70
#define DERBY_BAND_SECOND   50
#define DERBY_BAND_THIRD    30

enum {
    DERBY_OK,
    DERBY_NO_MON,
    DERBY_EGG,
    DERBY_WRONG_PLACE,
    DERBY_NOT_WATER,
};

static u16 BaseStatTotal(u16 species)
{
    const struct SpeciesInfo * info = &gSpeciesInfo[species];

    return info->baseHP + info->baseAttack + info->baseDefense
         + info->baseSpeed + info->baseSpAttack + info->baseSpDefense;
}

void FishingDerby_Score(void)
{
    struct Pokemon * mon;
    u16 species;
    u32 score;
    u8 nickname[POKEMON_NAME_LENGTH + 1];

    gSpecialVar_Result = 0;
    gSpecialVar_0x8005 = 0;
    gSpecialVar_0x8006 = DERBY_NO_MON;

    if (gSpecialVar_0x8004 >= CalculatePlayerPartyCount())
        return;

    mon = &gPlayerParty[gSpecialVar_0x8004];
    GetMonData(mon, MON_DATA_NICKNAME, nickname);
    StringCopy_Nickname(gStringVar1, nickname);

    if (GetMonData(mon, MON_DATA_IS_EGG))
    {
        gSpecialVar_0x8006 = DERBY_EGG;
        return;
    }
    if (GetMonData(mon, MON_DATA_MET_LOCATION) != DERBY_MAPSEC)
    {
        gSpecialVar_0x8006 = DERBY_WRONG_PLACE;
        return;
    }

    species = GetMonData(mon, MON_DATA_SPECIES);
    if (gSpeciesInfo[species].types[0] != TYPE_WATER
     && gSpeciesInfo[species].types[1] != TYPE_WATER)
    {
        gSpecialVar_0x8006 = DERBY_NOT_WATER;
        return;
    }

    score = GetMonData(mon, MON_DATA_LEVEL) + BaseStatTotal(species) / 20;
    if (score > 999)
        score = 999;

    gSpecialVar_0x8006 = DERBY_OK;
    gSpecialVar_0x8005 = score;
    if (score >= DERBY_BAND_GRAND)
        gSpecialVar_Result = 5;
    else if (score >= DERBY_BAND_FIRST)
        gSpecialVar_Result = 4;
    else if (score >= DERBY_BAND_SECOND)
        gSpecialVar_Result = 3;
    else if (score >= DERBY_BAND_THIRD)
        gSpecialVar_Result = 2;
    else
        gSpecialVar_Result = 1;
}
'''


def native():
    p = os.path.join(REPO, 'src/fishing_derby.c')
    open(p, 'w', encoding='utf-8', newline='\n').write(NATIVE)

    vp = os.path.join(REPO, 'include/constants/vars.h')
    v = open(vp, encoding='utf-8').read()
    if 'VAR_ORANGE_DERBY_BEST' not in v:
        anchor = '#define VAR_ORANGE_STORY'
        line = ('#define VAR_ORANGE_DERBY_BEST      0x402C '
                '// best Mandarin Fishing Derby score so far\n')
        v = v.replace(anchor, line + anchor, 1)
        open(vp, 'w', encoding='utf-8', newline='\n').write(v)

    fp = os.path.join(REPO, 'include/constants/flags.h')
    f = open(fp, encoding='utf-8').read()
    add = [('FLAG_ORANGE_DERBY_RULES', '0x4A7', 'the derby judge has explained himself'),
           ('FLAG_ORANGE_DERBY_GRAND', '0x4A8', 'the derby grand prize has been claimed')]
    out = ''
    for name, val, why in add:
        if name not in f:
            out += '#define %-24s %s // %s\n' % (name, val, why)
    if out:
        anchor = '#define FLAG_UNUSED_0x4A9'
        f = f.replace(anchor, out + anchor, 1)
        open(fp, 'w', encoding='utf-8', newline='\n').write(f)
    print('  src/fishing_derby.c written; VAR_ORANGE_DERBY_BEST and two flags defined')


def landmarks():
    """Keep the beach's reachability fixture pointing at open sand.

    (20,60) was fine on the bare shelf and is now the trunk of a palm, so the
    goal moves a few tiles east. The rest still stand: the doorway from
    Mandarin South, the far corners, and the north shore.
    """
    p = os.path.join(REPO, 'docs/orange-islands/island_landmarks.json')
    doc = json.load(open(p))
    fresh = [[45, 45], [[0, 33], [30, 4], [60, 19], [35, 73], [25, 60]], False]
    if doc.get('Mandarin_South_Beach') != fresh:
        doc['Mandarin_South_Beach'] = fresh
        with open(p, 'w', newline='\n') as f:
            json.dump(doc, f, indent=1)
            f.write('\n')
        print('  landmark fixture moved off the new palm')


def main():
    paint()
    map_json()
    scripts()
    interiors()
    trainers()
    native()
    landmarks()


if __name__ == '__main__':
    main()
