"""Moor a battle boat at the end of KINNOW's pier and fit it out.

KINNOW's pier ran twenty rows south out of town and stopped in open water. The
boat is what it was always pointing at: a chartered liner out of HOENN that
takes paying challengers, three decks of them, with a hired arena in the hold.

    Deck     the open boarding deck. The pier runs onto it through a gap in
             the rail -- vanilla's SS ANNE deck has that gap drawn and never
             opens it, so the only change to the copied blocks is clearing the
             collision on that one tile.
    Saloon   the long first-floor corridor. Three cabins off it are let to
             trainers; the other four doors are crew quarters and say so.
    Hold     the bottom corridor. Two more trainers, and the arena door.
    Arena    the challenge desk. See build_battle_boat_arena below.

The six layouts are copies of the SS ANNE's, not references to them: a map can
share another map's layout, but then repainting the boat would repaint the
SS ANNE, so each one gets its own blockdata under data/layouts/.

Every coordinate is checked against the map before anything is written, and
the script is safe to re-run: it rewrites its own fenced block in each
scripts.inc and skips constants and table rows it has already added.

    python3 docs/orange-islands/build_battle_boat.py
"""
import json
import os
import re
import shutil
import struct
import sys

REPO = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))

GROUP = 'gMapGroup_Orange_Kinnow_Interior'
P = 'OrangeIslands_Kinnow_BattleBoat_'
TOWN = 'OrangeIslands_Kinnow'
TOWN_MAP = 'MAP_ORANGE_ISLANDS_KINNOW'
MAPSEC = 'MAPSEC_KINNOW_ISLAND'
MUSIC = 'MUS_SS_ANNE'

FENCE_TOP = '@ --- built by docs/orange-islands/build_battle_boat.py ---'
FENCE_END = '@ --- end build_battle_boat.py ---'

# The pier's last stone, which becomes the gangway, and the plain walkable
# pier block already used all the way up the jetty.
GANGWAY = (33, 48)
PIER_BLOCK = 449

# Where the sign goes: the pier's right-hand rail, which is solid, so the
# player reads it standing on the walkway beside it.
PIER_SIGN = (34, 44)

# Nothing is drawn in the water. There are no ship-hull metatiles in the
# VERMILION tileset -- what looks like a moored hull on that map is the brown
# rock islet, blocks 104-130 -- and vanilla does not draw one either: the
# SS ANNE is a dock map warping to a deck map, with the hull never in frame.
# The boat reads the same way here. The Deck layout has the pier drawn running
# onto it, so stepping off the end of KINNOW's jetty and arriving on that pier
# is continuous.


def mid(name):
    """MAP_ constant for a map directory name."""
    return 'MAP_' + re.sub(r'(?<=[a-z0-9])(?=[A-Z])', '_', name).upper()


def layouts():
    path = os.path.join(REPO, 'data/layouts/layouts.json')
    return path, json.load(open(path))


def layout_by_id(lj, lid):
    for l in lj['layouts']:
        if l and l['id'] == lid:
            return l
    raise KeyError(lid)


def blocks(layout):
    raw = open(os.path.join(REPO, layout['blockdata_filepath']), 'rb').read()
    return list(struct.unpack('<%dH' % (len(raw) // 2), raw))


def write_blocks(layout, data):
    with open(os.path.join(REPO, layout['blockdata_filepath']), 'wb') as f:
        f.write(struct.pack('<%dH' % len(data), *data))


# (suffix, source layout, [(x, y, block, collision) to overwrite])
#
# The Deck is the one layout that changes. The SS ANNE's deck has a gap drawn
# in its rail at x=14, which is where its pier lands -- but the two tiles
# below x=14 are stacked cargo, so a gap there opens onto crates. One tile
# left, at x=13, the deck planking is clear all the way in. So the gap block
# (942) and the plain rail beside it (946) swap places, and the gap is the one
# tile on the whole rail without collision.
LAYOUTS = [
    ('Deck',   'LAYOUT_SSANNE_DECK',            [(13, 4, 942, 0),
                                                 (14, 4, 946, 1)]),
    ('Saloon', 'LAYOUT_SSANNE_1F_CORRIDOR',     []),
    ('Hold',   'LAYOUT_SSANNE_B1F_CORRIDOR',    []),
    ('Cabin',  'LAYOUT_SSANNE_ROOM1',           []),
    ('Galley', 'LAYOUT_SSANNE_KITCHEN',         []),
    ('Arena',  'LAYOUT_SSANNE_CAPTAINS_OFFICE', []),
]


def copy_layouts(report):
    path, lj = layouts()
    made = 0
    for (suffix, src_id, opens) in LAYOUTS:
        new_id = 'LAYOUT_ORANGE_ISLANDS_KINNOW_BATTLE_BOAT_' + suffix.upper()
        if any(l and l['id'] == new_id for l in lj['layouts']):
            continue
        src = layout_by_id(lj, src_id)
        folder = P + suffix
        d = os.path.join(REPO, 'data/layouts', folder)
        os.makedirs(d, exist_ok=True)
        shutil.copyfile(os.path.join(REPO, src['blockdata_filepath']),
                        os.path.join(d, 'map.bin'))
        shutil.copyfile(os.path.join(REPO, src['border_filepath']),
                        os.path.join(d, 'border.bin'))
        new = dict(src)
        new['id'] = new_id
        new['name'] = folder + '_Layout'
        new['border_filepath'] = 'data/layouts/%s/border.bin' % folder
        new['blockdata_filepath'] = 'data/layouts/%s/map.bin' % folder
        lj['layouts'].append(new)

        made += 1
    if made:
        with open(path, 'w', newline='\n') as f:
            json.dump(lj, f, indent=2)
            f.write('\n')
    return made


def apply_layout_edits():
    """Re-asserted on every run, not only when the copy is made, so an edit
    added to LAYOUTS later still reaches a layout that already exists."""
    _path, lj = layouts()
    changed = 0
    for (suffix, _src_id, edits) in LAYOUTS:
        if not edits:
            continue
        lay = layout_by_id(
            lj, 'LAYOUT_ORANGE_ISLANDS_KINNOW_BATTLE_BOAT_' + suffix.upper())
        data = blocks(lay)
        w = lay['width']
        before = list(data)
        for (x, y, block, collision) in edits:
            i = y * w + x
            data[i] = (data[i] & 0xf000) | block | (collision << 10)
        if data != before:
            write_blocks(lay, data)
            changed += 1
    return changed


def open_the_gangway(report):
    """Make the pier's last stone walkable so the warp has somewhere to sit."""
    _path, lj = layouts()
    town = layout_by_id(lj, 'LAYOUT_ORANGE_ISLANDS_KINNOW')
    w = town['width']
    data = blocks(town)
    gx, gy = GANGWAY
    i = gy * w + gx
    if data[i] == PIER_BLOCK:
        return 0
    if data[(gy - 1) * w + gx] & 0xc00:
        report.append('  the jetty does not reach (%d,%d)' % (gx, gy - 1))
        return 0
    data[i] = PIER_BLOCK                                # walkable, collision 0
    write_blocks(town, data)
    return 1


# ---------------------------------------------------------------------------
# The maps.
#
# Warp coordinates are the SS ANNE's own, since the blocks are its blocks: the
# stairs and doors are where that map put them. Only the destinations change.
# A door with no cabin behind it gets a sign rather than a warp -- the four
# spare doors in the Saloon and the four in the Hold are crew quarters, and
# saying so is cheaper and truer than inventing eight more rooms.

CABINS = 3

MAPS = {
    # suffix: (layout suffix, [(x, y, dest suffix or None for the town,
    #                           dest warp index)])
    'Deck': ('Deck', [
        (13, 0, None, None),            # 0  back down the jetty
        (16, 8, 'Saloon', 0),           # 1  companionway
        (16, 9, 'Saloon', 0),           # 2
    ]),
    'Saloon': ('Saloon', [
        (19, 1, 'Deck', 1),             # 0  up to the deck
        (20, 0, 'Deck', 1),             # 1
        (3, 8, 'Hold', 0),              # 2  down to the hold
        (28, 17, 'Hold', 0),            # 3
        (2, 18, 'Galley', 0),           # 4  down to the galley
        (3, 20, 'Galley', 0),           # 5
        (5, 10, 'Cabin1', 0),           # 6
        (8, 10, 'Cabin2', 0),           # 7
        (11, 10, 'Cabin3', 0),          # 8
    ]),
    'Hold': ('Hold', [
        (19, 3, 'Saloon', 2),           # 0  back up
        (2, 2, 'Arena', 0),             # 1  the arena
    ]),
    'Cabin1': ('Cabin', [(2, 1, 'Saloon', 6)]),
    'Cabin2': ('Cabin', [(2, 1, 'Saloon', 7)]),
    'Cabin3': ('Cabin', [(2, 1, 'Saloon', 8)]),
    'Galley': ('Galley', [(7, 1, 'Saloon', 4)]),
    'Arena': ('Arena', [(3, 7, 'Hold', 1)]),
}

# Doors with nothing behind them.
CREW_DOORS = {
    'Saloon': [(14, 11), (17, 11), (20, 11), (23, 11)],
    'Hold': [(6, 2), (10, 2), (14, 2), (18, 2)],
}

# The rules board beside the desk, read from the tile below it.
ARENA_BOARD = (3, 4)


def map_json(suffix, lay_suffix, warps, town_warp):
    name = P + suffix
    out = []
    for (x, y, dest, idx) in warps:
        if dest is None:
            out.append({"x": x, "y": y, "elevation": 0, "dest_map": TOWN_MAP,
                        "dest_warp_id": str(town_warp)})
        else:
            out.append({"x": x, "y": y, "elevation": 0,
                        "dest_map": mid(P + dest), "dest_warp_id": str(idx)})
    return {
        "id": mid(name), "name": name,
        "layout": 'LAYOUT_ORANGE_ISLANDS_KINNOW_BATTLE_BOAT_' + lay_suffix.upper(),
        "music": MUSIC, "region_map_section": MAPSEC, "requires_flash": False,
        "weather": "WEATHER_NONE", "map_type": "MAP_TYPE_INDOOR",
        "allow_cycling": False, "allow_escaping": False, "allow_running": True,
        "show_map_name": False, "floor_number": 0,
        "battle_scene": "MAP_BATTLE_SCENE_NORMAL", "connections": None,
        "object_events": [], "warp_events": out,
        "coord_events": [], "bg_events": []}


# ---------------------------------------------------------------------------
# Who is aboard.
#
# The decks are a shade above the island: KINNOW's own trainers are level
# twenty-two and these are twenty-four to thirty, so a party that cleared the
# island has somewhere to go next. The arena in the hold is a different thing
# entirely at level fifty, and the purser says so at the top of the gangway
# rather than letting someone walk down there and find out.

# (script suffix, graphics, x, y, movement, lines) -- lines None means the
# NPC is a trainer and its dialogue comes from TRAINERS instead.
PEOPLE = {
    'Deck': [
        ('Purser', 'OBJ_EVENT_GFX_GENTLEMAN', 12, 6, 'MOVEMENT_TYPE_FACE_UP',
         ['"Welcome aboard the MERIDIAN.\\p"',
          '"We are chartered out of HOENN, and\\n"',
          '"everyone aboard is a paying\\l"',
          '"challenger. The decks are friendly\\l"',
          '"enough.\\p"',
          '"The arena in the hold is not. That\\n"',
          '"is a level fifty room and the\\l"',
          '"house means it.$"']),
        ('Lookout', 'OBJ_EVENT_GFX_SAILOR', 6, 9, 'MOVEMENT_TYPE_FACE_RIGHT',
         ['"We never leave the pier. The boat\\n"',
          '"is the venue, not the voyage.\\p"',
          '"Companionway is amidships. Arena\\n"',
          '"is all the way down.$"']),
    ],
    'Saloon': [
        ('Steward', 'OBJ_EVENT_GFX_SAILOR', 25, 9, 'MOVEMENT_TYPE_FACE_DOWN',
         ['"Three cabins are let to guests\\n"',
          '"this week. The rest of this deck\\l"',
          '"is crew.\\p"',
          '"Galley is forward and down. The\\n"',
          '"arena is in the hold.$"']),
    ],
    'Galley': [
        ('Cook', 'OBJ_EVENT_GFX_CHEF', 5, 3, 'MOVEMENT_TYPE_FACE_DOWN',
         ['"Hungry? Everything here is for\\n"',
          '"the crew, I am afraid.\\p"',
          '"Though if you are going down to\\n"',
          '"the arena -- eat first. Those\\l"',
          '"runs go longer than people think.$"']),
        ('Galley_Hand', 'OBJ_EVENT_GFX_LASS', 12, 10, 'MOVEMENT_TYPE_FACE_LEFT',
         ['"The rental POKEMON downstairs are\\n"',
          '"ours. We feed them, we train them,\\l"',
          '"and we have no idea which six you\\l"',
          '"will be shown.\\p"',
          '"Neither does the house. That is\\n"',
          '"the point of it.$"']),
    ],
    'Hold': [],
    'Cabin1': [], 'Cabin2': [], 'Cabin3': [],
    'Arena': [
        # The desk's script is written by arena_scripts, not by talkers.
        ('Desk', 'OBJ_EVENT_GFX_BEAUTY', 4, 3, 'MOVEMENT_TYPE_FACE_DOWN', None),
    ],
}

# (map, script suffix, trainer constant, class, pic, music, graphics,
#  pool, level, NAME, x, y, movement, intro, defeat, after)
TRAINERS = [
    ('Deck', 'Brac', 'TRAINER_ORANGE_SAILOR_BRAC', 'SAILOR', 'SAILOR', 'MALE',
     'OBJ_EVENT_GFX_SAILOR', 'sPool_HoennCrew_Basic', 24, 'BRAC',
     10, 8, 'MOVEMENT_TYPE_FACE_DOWN',
     ['"Crossed from SLATEPORT on this\\n"', '"boat and never got off it.$"'],
     ['"Worth the fare, that.$"'],
     ['"Half the team aboard came out of\\n"', '"HOENN with us. You will not have\\l"',
      '"seen some of them before.$"']),
    ('Cabin1', 'Ardith', 'TRAINER_ORANGE_COOLTRAINER_ARDITH', 'COOLTRAINER',
     'COOLTRAINER_F', 'FEMALE', 'OBJ_EVENT_GFX_COOLTRAINER_F',
     'sPool_HoennCrew_Basic', 26, 'ARDITH', 3, 4, 'MOVEMENT_TYPE_FACE_DOWN',
     ['"I took this cabin for the season.\\n"', '"The door stays open.$"'],
     ['"Then take the one down the hall\\n"', '"as well.$"'],
     ['"Everyone on this deck is running\\n"', '"a HOENN team. Good practice for\\l"',
      '"the arena.$"']),
    ('Cabin2', 'Fenn', 'TRAINER_ORANGE_COOLTRAINER_FENN', 'COOLTRAINER',
     'COOLTRAINER_M', 'MALE', 'OBJ_EVENT_GFX_COOLTRAINER_M',
     'sPool_HoennCrew_Basic', 27, 'FENN', 3, 4, 'MOVEMENT_TYPE_FACE_DOWN',
     ['"Cabin battles. No room to run, no\\n"', '"reason to.$"'],
     ['"No room to hide either.$"'],
     ['"The arena below hands you six you\\n"', '"have never used. Up here at least\\l"',
      '"the team is mine.$"']),
    ('Cabin3', 'Lorcan', 'TRAINER_ORANGE_GENTLEMAN_LORCAN', 'GENTLEMAN',
     'GENTLEMAN', 'RICH', 'OBJ_EVENT_GFX_GENTLEMAN',
     'sPool_HoennCrew_Intermediate', 28, 'LORCAN', 3, 4, 'MOVEMENT_TYPE_FACE_DOWN',
     ['"I have lost a great deal of money\\n"', '"in the hold this week.\\p"',
      '"Well out of my depth down there.\\n"', '"Let me win some of it back up\\l"',
      '"here.$"'],
     ['"Ah. Well.$"'],
     ['"The house pays by how far you get.\\n"', '"I have never got past the fifth.$"']),
    ('Hold', 'Mabon', 'TRAINER_ORANGE_BLACK_BELT_MABON', 'BLACK_BELT',
     'BLACK_BELT', 'MALE', 'OBJ_EVENT_GFX_BLACKBELT',
     'sPool_HoennCrew_Intermediate', 30, 'MABON', 7, 4, 'MOVEMENT_TYPE_FACE_RIGHT',
     ['"Queue starts behind me.$"'],
     ['"Or in front of you. Fine.$"'],
     ['"They draw your six at the desk.\\n"', '"After each win you may take one\\l"',
      '"from whoever you just beat.$"']),
    ('Hold', 'Iseult', 'TRAINER_ORANGE_BEAUTY_ISEULT', 'BEAUTY', 'BEAUTY',
     'FEMALE', 'OBJ_EVENT_GFX_BEAUTY',
     'sPool_HoennCrew_Intermediate', 30, 'ISEULT', 14, 4, 'MOVEMENT_TYPE_FACE_LEFT',
     ['"Eight rounds. I have done seven.\\n"', '"Twice.$"'],
     ['"Seven again, then.$"'],
     ['"Rounds four and eight are not one\\n"', '"of us. The house draws a GYM\\l"',
      '"LEADER of HOENN for those two, and\\l"', '"they bring a full six.$"']),
]

CREW_SIGN = ['"Crew quarters.$"']


def door_signs(name, doors, out, text):
    for n, (x, y) in enumerate(doors, 1):
        label = '%s_EventScript_CrewDoor%d' % (name, n)
        out += [label + '::', '\tmsgbox %s_Text_CrewDoor, MSGBOX_SIGN' % name,
                '\tend', '']
    text.append('%s_Text_CrewDoor::' % name)
    text += ['\t.string %s' % ln for ln in CREW_SIGN]
    text.append('')


def talkers(name, people, out, text):
    for (s, _g, _x, _y, _m, lines) in people:
        if lines is None:
            continue
        out += ['%s_EventScript_%s::' % (name, s),
                '\tmsgbox %s_Text_%s, MSGBOX_NPC' % (name, s), '\tend', '']
        text.append('%s_Text_%s::' % (name, s))
        text += ['\t.string %s' % ln for ln in lines]
        text.append('')


def fighters(suffix, name, out, text):
    for t in [t for t in TRAINERS if t[0] == suffix]:
        (_m, s, const, _c, _p, _mu, _g, _pool, _lv, _n, _x, _y, _mv,
         intro, defeat, after) = t
        out += ['%s_EventScript_%s::' % (name, s),
                '\ttrainerbattle_single %s, %s_Text_%sIntro, %s_Text_%sDefeat'
                % (const, name, s, name, s),
                '\tmsgbox %s_Text_%sAfter, MSGBOX_AUTOCLOSE' % (name, s),
                '\tend', '']
        for (tag, lines) in (('Intro', intro), ('Defeat', defeat), ('After', after)):
            text.append('%s_Text_%s%s::' % (name, s, tag))
            text += ['\t.string %s' % ln for ln in lines]
            text.append('')


# ---------------------------------------------------------------------------
# The two pools the boat's trainers draw from. Everything in them is from the
# HOENN dex, which is the boat's whole conceit, and the _Advanced one obeys
# scripts/check_trainer_pools.py: fully evolved, four moves, an item and a
# chosen ability, or `make` refuses to build.

POOLS = '''
// --- build_battle_boat.py pools ---
// ---------------------------------------------------------------------------
// The KINNOW battle boat. Chartered out of HOENN, crewed and patronised by
// HOENN trainers, so both pools are HOENN-dex only (build_battle_boat.py).
//
// These are the people lounging on the decks, not the arena. KINNOW's own
// trainers are level twenty-two and these run twenty-four to thirty, so the
// pools stay at first and middle stages -- a fully evolved team would read as
// the arena's level rather than the deck's.

static const struct PoolMon sPool_HoennCrew_Basic[] =
{
    { .species = SPECIES_ZIGZAGOON, .iv = 60 },
    { .species = SPECIES_POOCHYENA, .iv = 60 },
    { .species = SPECIES_WINGULL,   .iv = 60 },
    { .species = SPECIES_TAILLOW,   .iv = 60 },
    { .species = SPECIES_SHROOMISH, .iv = 60 },
    { .species = SPECIES_WHISMUR,   .iv = 60 },
    { .species = SPECIES_ARON,      .iv = 60 },
    { .species = SPECIES_NUMEL,     .iv = 60 },
    { .species = SPECIES_CARVANHA,  .iv = 60 },
    { .species = SPECIES_ELECTRIKE, .iv = 60 },
    { .species = SPECIES_MAKUHITA,  .iv = 60 },
    { .species = SPECIES_SWABLU,    .iv = 60 },
    { .species = SPECIES_SPOINK,    .iv = 60 },
    { .species = SPECIES_BARBOACH,  .iv = 60 },
    POOL_END
};

static const struct PoolMon sPool_HoennCrew_Intermediate[] =
{
    { .species = SPECIES_LINOONE,   .iv = 100 },
    { .species = SPECIES_MIGHTYENA, .iv = 100 },
    { .species = SPECIES_PELIPPER,  .iv = 100 },
    { .species = SPECIES_SWELLOW,   .iv = 100 },
    { .species = SPECIES_LOMBRE,    .iv = 100 },
    { .species = SPECIES_MARSHTOMP, .iv = 100 },
    { .species = SPECIES_COMBUSKEN, .iv = 100 },
    { .species = SPECIES_GROVYLE,   .iv = 100 },
    { .species = SPECIES_LAIRON,    .iv = 100, .heldItem = ITEM_HARD_STONE },
    { .species = SPECIES_VIBRAVA,   .iv = 100 },
    { .species = SPECIES_WAILMER,   .iv = 100 },
    { .species = SPECIES_KIRLIA,    .iv = 100 },
    { .species = SPECIES_NINCADA,   .iv = 100 },
    { .species = SPECIES_SANDSHREW, .iv = 100 },
    POOL_END
};
// --- end build_battle_boat.py pools ---
'''


POOL_FENCE_TOP = '// --- build_battle_boat.py pools ---'
POOL_FENCE_END = '// --- end build_battle_boat.py pools ---'


def add_pools():
    """Fenced, so changing a pool here rewrites the one in the file rather
    than appending a second definition of the same name."""
    p = os.path.join(REPO, 'src/data/trainer_pools.h')
    s = open(p, encoding='utf-8').read()
    if POOL_FENCE_TOP in s and POOL_FENCE_END in s:
        head = s[:s.index(POOL_FENCE_TOP)]
        tail = s[s.index(POOL_FENCE_END) + len(POOL_FENCE_END):].lstrip('\n')
        new = head.rstrip('\n') + '\n' + POOLS.lstrip('\n') + tail
    else:
        new = s.rstrip('\n') + '\n' + POOLS
    if new == s:
        return 0
    open(p, 'w', encoding='utf-8', newline='\n').write(new)
    return 2


def add_trainers():
    """Constants and table rows, each decided on its own so a half-finished
    run can finish on the next one."""
    op = os.path.join(REPO, 'include/constants/opponents.h')
    s = open(op, encoding='utf-8').read()
    num = int(re.search(r'#define NUM_TRAINERS\s+(\d+)', s).group(1))

    block, nxt = '', num
    for t in TRAINERS:
        if t[2] in s:
            continue
        block += '#define %-40s %d\n' % (t[2], nxt)
        nxt += 1
    if block:
        s = s.replace('\n#define NUM_TRAINERS',
                      '\n// The KINNOW battle boat (build_battle_boat.py)\n'
                      + block + '\n#define NUM_TRAINERS', 1)
        s = re.sub(r'#define NUM_TRAINERS\s+\d+',
                   '#define NUM_TRAINERS                             %d' % nxt, s)
        open(op, 'w', encoding='utf-8', newline='\n').write(s)

    tp = os.path.join(REPO, 'src/data/trainers.h')
    table = open(tp, encoding='utf-8').read()
    entries, made = '', 0
    for t in TRAINERS:
        (_m, _s, const, cls, pic, music, _g, pool, level, name) = t[:10]
        if '[%s] = {' % const in table:
            # Already there: just re-assert the party, which is the part that
            # gets retuned.
            table = re.sub(
                r'(\[%s\] = \{.*?\.party = )POOL_PARTY\([^)]*\)' % const,
                lambda m: m.group(1) + 'POOL_PARTY(%s, 3, 4, %d, 2)' % (pool, level),
                table, count=1, flags=re.S)
            continue
        entries += (
            '    [%s] = {\n'
            '        .trainerClass = TRAINER_CLASS_%s,\n'
            '        .encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_%s,\n'
            '        .trainerPic = TRAINER_PIC_%s,\n'
            '        .trainerName = _("%s"),\n'
            '        .items = {},\n'
            '        .doubleBattle = FALSE,\n'
            '        .aiFlags = AI_LEVEL_NORMAL,\n'
            '        .party = POOL_PARTY(%s, 3, 4, %d, 2),\n'
            '    },\n' % (const, cls, music, pic, name, pool, level))
        made += 1
    if entries:
        close = table.rindex('\n};')
        table = table[:close + 1] + entries + table[close + 1:]
    open(tp, 'w', encoding='utf-8', newline='\n').write(table)
    return made, nxt


# ---------------------------------------------------------------------------
# Writing it all out.

def walkable(layout):
    """Cells you can stand on: collision clear and not water."""
    data = blocks(layout)
    w, h = layout['width'], layout['height']

    def ok(x, y):
        if not (0 <= x < w and 0 <= y < h):
            return False
        return not data[y * w + x] & 0xc00

    return ok


def splice(path, body):
    s = open(path, encoding='utf-8').read() if os.path.exists(path) else ''
    if not [ln for ln in body if ln.strip()]:
        block = ''
    else:
        block = FENCE_TOP + '\n' + '\n'.join(body).rstrip('\n') + '\n' + FENCE_END + '\n'
    if FENCE_TOP in s and FENCE_END in s:
        s = s[:s.index(FENCE_TOP)] + block + s[s.index(FENCE_END) + len(FENCE_END):].lstrip('\n')
    else:
        s = s.rstrip('\n') + ('\n\n' if s.strip() else '') + block
    open(path, 'w', encoding='utf-8', newline='\n').write(s)


def write_maps(report):
    _path, lj = layouts()

    # The town end of the gangway, added once and then left alone.
    town_path = os.path.join(REPO, 'data/maps', TOWN, 'map.json')
    town = json.load(open(town_path))
    deck_map = mid(P + 'Deck')
    town_warp = next((i for i, w in enumerate(town['warp_events'])
                      if w['dest_map'] == deck_map), None)
    if town_warp is None:
        town_warp = len(town['warp_events'])
        town['warp_events'].append({"x": GANGWAY[0], "y": GANGWAY[1], "elevation": 0,
                                    "dest_map": deck_map, "dest_warp_id": "0"})
        with open(town_path, 'w', newline='\n') as f:
            json.dump(town, f, indent=2)
            f.write('\n')

    groups_path = os.path.join(REPO, 'data/maps/map_groups.json')
    gj = json.load(open(groups_path))
    esp = os.path.join(REPO, 'data/event_scripts.s')
    es = open(esp, encoding='utf-8').read()

    for (suffix, (lay_suffix, warps)) in MAPS.items():
        name = P + suffix
        d = os.path.join(REPO, 'data/maps', name)
        os.makedirs(d, exist_ok=True)
        mj = map_json(suffix, lay_suffix, warps, town_warp)
        ok = walkable(layout_by_id(
            lj, 'LAYOUT_ORANGE_ISLANDS_KINNOW_BATTLE_BOAT_' + lay_suffix.upper()))

        taken = {(w['x'], w['y']) for w in mj['warp_events']}
        for (s, gfx, x, y, move, _lines) in PEOPLE[suffix]:
            if not ok(x, y):
                report.append('  %s: (%d,%d) is not walkable, %s not placed'
                              % (suffix, x, y, s))
                continue
            if (x, y) in taken:
                report.append('  %s: (%d,%d) is taken, %s not placed'
                              % (suffix, x, y, s))
                continue
            taken.add((x, y))
            mj['object_events'].append({
                "type": "object", "graphics_id": gfx, "x": x, "y": y,
                "elevation": 3, "movement_type": move,
                "movement_range_x": 1, "movement_range_y": 1,
                "trainer_type": "TRAINER_TYPE_NONE",
                "trainer_sight_or_berry_tree_id": "0",
                "script": '%s_EventScript_%s' % (name, s), "flag": "0"})

        for t in [t for t in TRAINERS if t[0] == suffix]:
            (_m, s, _c, _cl, _p, _mu, gfx, _pool, _lv, _n, x, y, move) = t[:13]
            if not ok(x, y):
                report.append('  %s: (%d,%d) is not walkable, %s not placed'
                              % (suffix, x, y, s))
                continue
            if (x, y) in taken:
                report.append('  %s: (%d,%d) is taken, %s not placed'
                              % (suffix, x, y, s))
                continue
            taken.add((x, y))
            mj['object_events'].append({
                "type": "object", "graphics_id": gfx, "x": x, "y": y,
                "elevation": 3, "movement_type": move,
                "movement_range_x": 1, "movement_range_y": 1,
                "trainer_type": "TRAINER_TYPE_NORMAL",
                "trainer_sight_or_berry_tree_id": "3",
                "script": '%s_EventScript_%s' % (name, s), "flag": "0"})

        if suffix == 'Arena':
            mj['bg_events'].append({
                "type": "sign", "x": ARENA_BOARD[0], "y": ARENA_BOARD[1],
                "elevation": 0,
                "player_facing_dir": "BG_EVENT_PLAYER_FACING_ANY",
                "script": '%s_EventScript_Rules' % name})

        for n, (x, y) in enumerate(CREW_DOORS.get(suffix, []), 1):
            mj['bg_events'].append({
                "type": "sign", "x": x, "y": y, "elevation": 0,
                "player_facing_dir": "BG_EVENT_PLAYER_FACING_ANY",
                "script": '%s_EventScript_CrewDoor%d' % (name, n)})

        with open(os.path.join(d, 'map.json'), 'w', newline='\n') as f:
            json.dump(mj, f, indent=2)
            f.write('\n')

        out, text = [], []
        talkers(name, PEOPLE[suffix], out, text)
        fighters(suffix, name, out, text)
        if suffix in CREW_DOORS:
            door_signs(name, CREW_DOORS[suffix], out, text)
        if suffix == 'Arena':
            arena_scripts(name, out, text)

        sp = os.path.join(d, 'scripts.inc')
        if not os.path.exists(sp):
            open(sp, 'w', encoding='utf-8', newline='\n').write(
                '%s_MapScripts::\n\t.byte 0\n' % name)
        splice(sp, out)
        splice(os.path.join(d, 'text.inc'), text)

        if name not in gj[GROUP]:
            gj[GROUP].append(name)
        inc = ('\t.include "data/maps/%s/scripts.inc"\n'
               '\t.include "data/maps/%s/text.inc"\n' % (name, name))
        if inc not in es:
            es += inc

    with open(groups_path, 'w', newline='\n') as f:
        json.dump(gj, f, indent=2)
        f.write('\n')
    open(esp, 'w', encoding='utf-8', newline='\n').write(es)
    return len(MAPS)


# ---------------------------------------------------------------------------
# The arena desk: the whole challenge, driven from one script.
#
# Seven rounds. Six against rented frontier trainers, the seventh against a
# HOENN GYM LEADER with a written team at level fifty. Between rounds you may
# trade one of your three for one you have just beaten, which is the FACTORY's
# own rule and the reason the draft matters. Retire whenever you like and the
# house pays for the rounds you cleared.
#
# The round loop is one label the script jumps back to. Nothing is stored in a
# script variable: the round number, the streak and the purse all live in
# gSaveBlock2Ptr->factory, so a run survives being saved mid-challenge.

ARENA_TEXT = {
    'Offer': ['"Thirty-two rounds, and we lend you\\n"',
              '"the POKEMON.\\p"',
              '"Six are brought up, you keep\\n"',
              '"three. Beat a team and you may\\l"',
              '"take one of them, up to six; once\\l"',
              '"you are full it becomes a trade.\\p"',
              '"Every fourth round is a GYM\\n"',
              '"LEADER of HOENN. Six of their own,\\l"',
              '"all one type. {STR_VAR_2} is first.\\p"',
              '"We pay by depth, and you may stop\\n"',
              '"between rounds. Sit down?$"'],
    'Decline': ['"The boat is moored here all\\n"', '"season.$"'],
    'Draft': ['"Six are brought up. Keep three.$"'],
    'Resume': ['"Your run is still open at round\\n"', '"{STR_VAR_3}. Carry on?$"'],
    'NextRound': ['"Round {STR_VAR_3} of 32. Ready?$"'],
    'SwapOffer': ['"Your six are full. Trade one of\\n"',
                  '"yours for one of theirs?$"'],
    'Swapped': ['"Done. The trade stands for the\\n"', '"rest of the run.$"'],
    'SpoilOffer': ['"You beat their three. Take one of\\n"',
                   '"them onto your side?$"'],
    'SpoilPick': ['"Which one?$"'],
    'SpoilTaken': ['"{STR_VAR_2} is yours for the rest of\\n"',
                   '"the run.$"'],
    'LeaderRound': ['"Round {STR_VAR_3}. {STR_VAR_2} brings six of\\n"',
                    '"their own and will not hold back.\\p"',
                    '"Nothing of theirs is ours, so\\n"',
                    '"there is nothing to take after.\\p"',
                    '"Ready?$"'],
    'StopMenu': ['"You are {STR_VAR_3} rounds in. Taking\\n"',
                 '"the purse now pays {STR_VAR_1}, three\\l"',
                 '"quarters; being stopped pays half.\\p"',
                 '"Or we hold the run for you.$"'],
    'Saved': ['"Held, and the ledger is written.\\p"',
              '"Your own POKEMON are yours again.\\n"',
              '"The run is here when you are.$"'],
    'Cleared': ['"All thirty-two, and all eight\\n"',
                '"leaders.\\p"',
                '"{STR_VAR_1}, in full. We shuffle them\\n"',
                '"again for your next one.$"'],
    'Lost': ['"{STR_VAR_3} rounds. Half of that is\\n"',
             '"{STR_VAR_1}.\\p"',
             '"The run is closed. Come back when\\n"',
             '"you want another.$"'],
    'Retired': ['"Stopped at {STR_VAR_3}. {STR_VAR_1}, and\\n"',
                '"the run is closed.$"'],
    'Nothing': ['"No rounds, no purse. Come back\\n"', '"when you want them.$"'],
    'Rules': ['"THE HOUSE PAYS BY DEPTH.\\p"',
              '"Rented POKEMON. Three of six, and\\n"',
              '"one off every team you beat until\\l"',
              '"you have six of your own.\\p"',
              '"Thirty-two rounds. Every fourth is\\n"',
              '"a GYM LEADER of HOENN, all eight of\\l"',
              '"them in an order drawn on the\\l"',
              '"night: six of their own type\\l"',
              '"at level fifty, and nothing to\\l"',
              '"take off them.\\p"',
              '"Stop between rounds for three\\n"',
              '"quarters. Be stopped, and it is\\l"',
              '"half.$"'],
}



def arena_scripts(name, out, text):
    e = '%s_EventScript_' % name
    t = '%s_Text_' % name
    out += [
        e + 'Desk::',
        '\tlock',
        '\tfaceplayer',
        '\tgoto_if_set FLAG_ORANGE_IN_FACTORY, ' + e + 'Resume',
        '\tspecial OrangeBoat_DrawLeaders',
        '\tspecial OrangeBoat_GetLeaderName',
        '\tmsgbox ' + t + 'Offer, MSGBOX_YESNO',
        '\tgoto_if_eq VAR_RESULT, NO, ' + e + 'Decline',
        '\tsetflag FLAG_ORANGE_IN_FACTORY',
        '\tspecial SavePlayerParty',
        '\tsetvar VAR_0x8004, BATTLE_FACTORY_FUNC_INIT',
        '\tspecial CallBattleFactoryFunction',
        '\tsetvar VAR_0x8004, BATTLE_FACTORY_FUNC_GENERATE_RENTAL_MONS',
        '\tspecial CallBattleFactoryFunction',
        '\tsetvar VAR_0x8004, BATTLE_FACTORY_FUNC_GENERATE_OPPONENT_MONS',
        '\tspecial CallBattleFactoryFunction',
        '\tmsgbox ' + t + 'Draft, MSGBOX_DEFAULT',
        '\tclosemessage',
        '\tfadescreen FADE_TO_BLACK',
        '\tsetvar VAR_0x8004, BATTLE_FACTORY_FUNC_SET_SWAPPED',
        '\tspecial CallBattleFactoryFunction',
        '\tsetvar VAR_0x8004, BATTLE_FACTORY_FUNC_SELECT_RENT_MONS',
        '\tspecial CallBattleFactoryFunction',
        '\twaitstate',
        '\tgoto ' + e + 'Round',
        '',
        e + 'Resume::',
        # Fills in the round number the message asks for.
        '\tspecial OrangeBoat_PrepareRound',
        '\tmsgbox ' + t + 'Resume, MSGBOX_YESNO',
        '\tgoto_if_eq VAR_RESULT, NO, ' + e + 'AskStop',
        # Re-stash: the team may have changed while the run was paused, and
        # this is what gets handed back at the end of it.
        '\tspecial SavePlayerParty',
        '\tsetvar VAR_0x8004, BATTLE_FACTORY_FUNC_SET_PARTIES',
        '\tsetvar VAR_0x8005, 0',
        '\tspecial CallBattleFactoryFunction',
        '\tspecial OrangeBoat_RestoreSpoils',
        '\tgoto ' + e + 'Round',
        '',
        # One round. Jumped back to for each of the seven.
        e + 'Round::',
        '\tspecial OrangeBoat_PrepareRound',
        # The announce is a yes/no, so between rounds is where you can stop.
        # Stash the leader flag first: the msgbox inside the call overwrites
        # VAR_RESULT, and NO and FALSE are the same number.
        '\tcopyvar VAR_TEMP_1, VAR_RESULT',
        '\tcall_if_eq VAR_TEMP_1, TRUE, ' + e + 'AnnounceLeader',
        '\tcall_if_eq VAR_TEMP_1, FALSE, ' + e + 'AnnounceRound',
        '\tgoto_if_eq VAR_RESULT, NO, ' + e + 'AskStop',
        '\tclosemessage',
        '\tplayfanfare MUS_HEAL',
        '\twaitfanfare',
        '\tspecial HealPlayerParty',
        '\tspecial OrangeBoat_StartBattle',
        '\twaitstate',
        '\tgoto_if_eq VAR_RESULT, B_OUTCOME_WON, ' + e + 'RoundWon',
        '\tgoto ' + e + 'Finish',
        '',
        e + 'AnnounceLeader::',
        '\tmsgbox ' + t + 'LeaderRound, MSGBOX_YESNO',
        '\treturn',
        '',
        e + 'AnnounceRound::',
        '\tmsgbox ' + t + 'NextRound, MSGBOX_YESNO',
        '\treturn',
        '',
        e + 'RoundWon::',
        '\tspecial OrangeBoat_RoundWon',
        '\tgoto_if_eq VAR_RESULT, TRUE, ' + e + 'Cleared',
        '\tcall_if_eq VAR_0x8006, FALSE, ' + e + 'TakeOpponentMons',
        '\tsetvar VAR_0x8004, BATTLE_FACTORY_FUNC_RESET_HELD_ITEMS',
        '\tspecial CallBattleFactoryFunction',
        '\tsetvar VAR_0x8004, BATTLE_FACTORY_FUNC_GENERATE_OPPONENT_MONS',
        '\tspecial CallBattleFactoryFunction',
        '\tgoto_if_eq VAR_0x8006, TRUE, ' + e + 'Round',
        # Under six, you take one of the beaten three; at six, the swap.
        '\tspecial OrangeBoat_OfferSpoils',
        '\tgoto_if_eq VAR_RESULT, FALSE, ' + e + 'AskSwap',
        '\tmsgbox ' + t + 'SpoilOffer, MSGBOX_YESNO',
        '\tgoto_if_eq VAR_RESULT, NO, ' + e + 'Round',
        '\tmessage ' + t + 'SpoilPick',
        '\twaitmessage',
        '\tmultichoice 18, 6, MULTICHOICE_BOAT_SPOILS, FALSE',
        '\tgoto_if_eq VAR_RESULT, SCR_MENU_UNSET, ' + e + 'Round',
        '\tspecial OrangeBoat_TakeSpoil',
        '\tmsgbox ' + t + 'SpoilTaken, MSGBOX_DEFAULT',
        '\tgoto ' + e + 'Round',
        '',
        e + 'AskSwap::',
        '\tmsgbox ' + t + 'SwapOffer, MSGBOX_YESNO',
        '\tgoto_if_eq VAR_RESULT, NO, ' + e + 'Round',
        '\tclosemessage',
        '\tfadescreen FADE_TO_BLACK',
        '\tsetvar VAR_0x8004, BATTLE_FACTORY_FUNC_SWAP_RENT_MONS',
        '\tspecial CallBattleFactoryFunction',
        '\twaitstate',
        '\tgoto_if_eq VAR_RESULT, TRUE, ' + e + 'Round',
        '\tsetvar VAR_0x8004, BATTLE_FACTORY_FUNC_SET_SWAPPED',
        '\tspecial CallBattleFactoryFunction',
        '\tmsgbox ' + t + 'Swapped, MSGBOX_DEFAULT',
        '\tgoto ' + e + 'Round',
        '',
        e + 'TakeOpponentMons::',
        '\tsetvar VAR_0x8004, BATTLE_FACTORY_FUNC_SET_OPPONENT_MONS',
        '\tspecial CallBattleFactoryFunction',
        '\treturn',
        '',
        e + 'Cleared::',
        '\tsetvar VAR_0x8005, FALSE',
        '\tspecial OrangeBoat_SettleUp',
        '\tmsgbox ' + t + 'Cleared, MSGBOX_DEFAULT',
        '\tgoto ' + e + 'Close',
        '',
        e + 'Finish::',
        '\tsetvar VAR_0x8005, FALSE',
        '\tspecial OrangeBoat_SettleUp',
        '\tgoto_if_eq VAR_RESULT, FALSE, ' + e + 'Nothing',
        '\tmsgbox ' + t + 'Lost, MSGBOX_DEFAULT',
        '\tgoto ' + e + 'Close',
        '',
        e + 'AskStop::',
        '\tspecial OrangeBoat_QuoteRetirement',
        '\tmessage ' + t + 'StopMenu',
        '\twaitmessage',
        '\tmultichoice 16, 6, MULTICHOICE_BOAT_PAUSE, FALSE',
        '\tgoto_if_eq VAR_RESULT, 0, ' + e + 'SaveAndStop',
        '\tgoto_if_eq VAR_RESULT, 1, ' + e + 'Retire',
        '\tgoto ' + e + 'Round',
        '',
        # Saving the run keeps the flag set and hands the real team back, so
        # the desk picks it up where it left off next time.
        e + 'SaveAndStop::',
        '\tclosemessage',
        '\tsetvar VAR_0x8004, BATTLE_FACTORY_FUNC_SAVE',
        '\tsetvar VAR_0x8005, FACTORY_CHALLENGE_PAUSED',
        '\tspecial CallBattleFactoryFunction',
        '\tplayse SE_SAVE',
        '\twaitse',
        '\tmsgbox ' + t + 'Saved, MSGBOX_DEFAULT',
        '\tspecial LoadPlayerParty',
        '\tspecial HealPlayerParty',
        '\tspecial CalculatePlayerPartyCount',
        '\trelease',
        '\tend',
        '',
        e + 'Retire::',
        '\tsetvar VAR_0x8005, TRUE',
        '\tspecial OrangeBoat_SettleUp',
        '\tgoto_if_eq VAR_RESULT, FALSE, ' + e + 'Nothing',
        '\tmsgbox ' + t + 'Retired, MSGBOX_DEFAULT',
        '\tgoto ' + e + 'Close',
        '',
        e + 'Nothing::',
        '\tmsgbox ' + t + 'Nothing, MSGBOX_DEFAULT',
        '\tgoto ' + e + 'Close',
        '',
        e + 'Decline::',
        '\tmsgbox ' + t + 'Decline, MSGBOX_DEFAULT',
        '\tgoto ' + e + 'Close',
        '',
        e + 'Close::',
        '\tclearflag FLAG_ORANGE_IN_FACTORY',
        '\tspecial LoadPlayerParty',
        '\tspecial HealPlayerParty',
        '\tspecial CalculatePlayerPartyCount',
        '\trelease',
        '\tend',
        '',
        e + 'Rules::',
        '\tmsgbox ' + t + 'Rules, MSGBOX_SIGN',
        '\tend',
        '',
    ]
    for (tag, lines) in ARENA_TEXT.items():
        text.append(t + tag + '::')
        text += ['\t.string %s' % ln for ln in lines]
        text.append('')


def main():
    report = []
    n = copy_layouts(report)
    apply_layout_edits()
    g = open_the_gangway(report)
    p = add_pools()
    made, total = add_trainers()
    m = write_maps(report)
    print('layouts copied:   %d' % n)
    print('gangway opened:   %d' % g)
    print('pools added:      %d' % p)
    print('trainers added:   %d  (NUM_TRAINERS now %d)' % (made, total))
    print('maps written:     %d' % m)
    for line in report:
        print(line)


if __name__ == '__main__':
    main()
