"""Fill SUNBURST CAVE's two lower floors and put a STEELIX at the bottom.

The cave was three floors of good level design with almost nothing in it. 2F
is a forty-six by thirty-five rock maze in the Rock Tunnel mould and had not a
single object on it. 3F is forty-four by sixty-three and had one item ball.
Both floors were walked in silence.

The shape of the place already says how it should be filled. Flood-filling 2F
from the stairs the player arrives at shows the far 1F entrance is fifty-seven
steps away while the stairs down to 3F are twenty-five, so the maze has a
short way on and a long way round, and the long way round is where the reward
goes. On 3F the deepest cell from the entrance is in the bottom-left chamber,
eighty-one steps in, and that is where the STEELIX stands.

    2F   four trainers along the three main galleries, a HARD STONE at the end
         of the long way round, and an ESCAPE ROPE in the pocket off the top
    3F   two trainers on the descent, a REVIVE in the east pocket, the STAR
         PIECE that was already there, and the STEELIX in the last room

The STEELIX is an overworld encounter, not a wild one: the follower sprite set
has a steelix, and object event graphics ids are u16 precisely so that follower
ids fit, so it can simply stand there. It is level twenty-five with SHEER FORCE
-- its hidden ability -- and a moveset chosen around it; src/orange_static_mons.c
explains why those four moves and not others. It runs away if you beat it or
flee, like any static encounter, and the flag remembers.

Every coordinate is checked against the map before anything is written.

    python3 docs/orange-islands/build_sunburst_cave.py
"""
import json
import os
import re
import struct
import sys
from collections import deque

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from repair_landmark_fixtures import layouts_and_maps  # noqa: E402

REPO = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))

F2 = 'OrangeIslands_Sunburst_Cave_2F'
F3 = 'OrangeIsland_Sunburst_Cave_3F'

STEELIX_FLAG = 'FLAG_ORANGE_SUNBURST_STEELIX'
STEELIX_LEVEL = 25

FENCE_TOP = '@ --- built by docs/orange-islands/build_sunburst_cave.py ---'
FENCE_END = '@ --- end build_sunburst_cave.py ---'

# (script suffix, constant, class, pic, music, pool, level, name, x, y, lines)
TRAINERS = {
    F2: [
        ('Dolan', 'TRAINER_ORANGE_HIKER_DOLAN', 'HIKER', 'HIKER', 'HIKER',
         'sPool_Hiker_Intermediate', 21, 'DOLAN', 14, 14,
         ['"This gallery runs the whole width\\n"', '"of the hill. You are not getting\\l"',
          '"down it round me.$"'],
         ['"Fair. Go on down.$"'],
         ['"Two ways up from the first floor\\n"', '"and only one of them is short.\\p"',
          '"You came the short way.$"']),
        ('Maeve', 'TRAINER_ORANGE_PICNICKER_MAEVE', 'PICNICKER', 'PICNICKER', 'FEMALE',
         'sPool_Picnicker_Basic', 20, 'MAEVE', 26, 21,
         ['"I have eaten lunch in this cave\\n"', '"every day this month.\\p"',
          '"You are the first person to walk\\n"', '"past.$"'],
         ['"Oh, well played.$"'],
         ['"There is a dead end at the top of\\n"', '"the maze. People think it is\\l"',
          '"empty. It is not.$"']),
        ('Ross', 'TRAINER_ORANGE_CAMPER_ROSS', 'CAMPER', 'CAMPER', 'MALE',
         'sPool_Camper_Basic', 20, 'ROSS', 20, 27,
         ['"Down here the rock is warm. Put\\n"', '"your hand on it.\\p"',
          '"Something underneath is doing\\n"', '"that.$"'],
         ['"Still warm.$"'],
         ['"Nobody has ever got to the bottom\\n"', '"of the third floor and come back\\l"',
          '"up saying nothing was there.$"']),
        ('Tam', 'TRAINER_ORANGE_YOUNGSTER_TAM', 'YOUNGSTER', 'YOUNGSTER', 'MALE',
         'sPool_Youngster_Intermediate', 20, 'TAM', 7, 19,
         ['"I came the long way round. Took\\n"', '"me an hour.\\p"',
          '"I am not doing it again for\\n"', '"nothing!$"'],
         ['"An hour! For this!$"'],
         ['"Keep going up this side. There is\\n"', '"something at the top worth the\\l"',
          '"walk.$"']),
    ],
    F3: [
        ('Grant', 'TRAINER_ORANGE_HIKER_GRANT', 'HIKER', 'HIKER', 'HIKER',
         'sPool_Hiker_Intermediate', 22, 'GRANT', 30, 17,
         ['"Third floor. Past here the\\n"', '"passages only go down.\\p"',
          '"Last chance to think about it.$"'],
         ['"Then you are going down. Mind how\\n"', '"you go.$"'],
         ['"There is a thing living in the\\n"', '"bottom room. I have heard it\\l"',
          '"move. I have never gone in.$"']),
        ('Odar', 'TRAINER_ORANGE_BLACK_BELT_ODAR', 'BLACK_BELT', 'BLACK_BELT', 'MALE',
         'sPool_BlackBelt_Basic', 22, 'ODAR', 18, 38,
         ['"I train against the rock itself.\\n"', '"It does not tire and it does not\\l"',
          '"flinch.\\p"', '"Neither do I.$"'],
         ['"The rock has never done that.$"'],
         ['"Whatever is below us is bigger\\n"', '"than the room it is in. You can\\l"',
          '"hear that much.$"']),
    ],
}

# (map, script suffix, item, x, y, flag)
ITEMS = [
    (F2, 'ItemHardStone', 'ITEM_HARD_STONE', 8, 3, 'FLAG_HIDE_SUNBURST_CAVE_2F_HARD_STONE'),
    (F2, 'ItemEscapeRope', 'ITEM_ESCAPE_ROPE', 24, 10, 'FLAG_HIDE_SUNBURST_CAVE_2F_ESCAPE_ROPE'),
    (F3, 'ItemRevive', 'ITEM_REVIVE', 34, 41, 'FLAG_HIDE_SUNBURST_CAVE_3F_REVIVE'),
]

STEELIX_AT = (7, 55)


def walkable(name):
    L, M = layouts_and_maps()
    l = L[M[name]['layout']]
    raw = open(os.path.join(REPO, l['blockdata_filepath']), 'rb').read()
    b = struct.unpack('<' + 'H' * (len(raw) // 2), raw)
    w, h = l['width'], l['height']
    return (lambda x, y: 0 <= x < w and 0 <= y < h and not (b[y * w + x] & 0xC00)), w, h


def reachable_from(name, start):
    ok, w, h = walkable(name)
    seen, q = {start}, deque([start])
    while q:
        c = q.popleft()
        for n in ((c[0] - 1, c[1]), (c[0] + 1, c[1]), (c[0], c[1] - 1), (c[0], c[1] + 1)):
            if n not in seen and ok(*n):
                seen.add(n)
                q.append(n)
    return seen


def splice(path, body):
    s = open(path, encoding='utf-8').read() if os.path.exists(path) else ''
    block = FENCE_TOP + '\n' + '\n'.join(body).rstrip('\n') + '\n' + FENCE_END + '\n'
    if FENCE_TOP in s and FENCE_END in s:
        s = s[:s.index(FENCE_TOP)] + block + s[s.index(FENCE_END) + len(FENCE_END):].lstrip('\n')
    else:
        s = s.rstrip('\n') + ('\n\n' if s.strip() else '') + block
    open(path, 'w', encoding='utf-8', newline='\n').write(s)


def add_flags(names):
    p = os.path.join(REPO, 'include/constants/flags.h')
    s = open(p, encoding='utf-8').read()
    added = 0
    for name, comment in names:
        if '#define %s ' % name in s or '#define %s\t' % name in s:
            continue
        free = re.search(r'#define (FLAG_UNUSED_0x([0-9A-F]{3}))\s+0x\2 // Unused Flag\n', s)
        if not free:
            raise SystemExit('no unused flag left for ' + name)
        s = s.replace(free.group(0),
                      free.group(0) + '#define %-24s 0x%s // %s\n' % (name, free.group(2), comment))
        added += 1
    if added:
        open(p, 'w', encoding='utf-8', newline='\n').write(s)
    return added


def add_trainers():
    """Constants and table entries, each decided on its own so a half-finished
    run can finish on the next one."""
    op = os.path.join(REPO, 'include/constants/opponents.h')
    s = open(op, encoding='utf-8').read()
    num = int(re.search(r'#define NUM_TRAINERS\s+(\d+)', s).group(1))
    tp = os.path.join(REPO, 'src/data/trainers.h')
    table = open(tp, encoding='utf-8').read()

    flat = [t for group in TRAINERS.values() for t in group]
    block, nxt = '', num
    for t in flat:
        if t[1] in s:
            continue
        block += '#define %-40s %d\n' % (t[1], nxt)
        nxt += 1
    if block:
        s = s.replace('\n#define NUM_TRAINERS',
                      '\n// Sunburst Cave 2F and 3F (build_sunburst_cave.py)\n'
                      + block + '\n#define NUM_TRAINERS', 1)
        s = re.sub(r'#define NUM_TRAINERS\s+\d+',
                   '#define NUM_TRAINERS                             %d' % nxt, s)
        open(op, 'w', encoding='utf-8', newline='\n').write(s)

    entries = ''
    made = 0
    for (_sfx, const, cls, pic, music, pool, level, name, _x, _y, _i, _d, _p) in flat:
        if '[%s] = {' % const in table:
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
            '        .party = POOL_PARTY(%s, 2, 3, %d, 2),\n'
            '    },\n' % (const, cls, music, pic, name, pool, level))
        made += 1
    if entries:
        close = table.rindex('\n};')
        table = table[:close + 1] + entries + table[close + 1:]
        open(tp, 'w', encoding='utf-8', newline='\n').write(table)
    return made, nxt


def build_floor(name, report):
    path = os.path.join(REPO, 'data/maps', name, 'map.json')
    mj = json.load(open(path))
    ok, _w, _h = walkable(name)

    mine = {'%s_EventScript_%s' % (name, t[0]) for t in TRAINERS.get(name, [])}
    mine |= {'%s_EventScript_%s' % (name, i[1]) for i in ITEMS if i[0] == name}
    mine.add('%s_EventScript_Steelix' % name)
    mj['object_events'] = [o for o in mj['object_events'] if o.get('script') not in mine]
    taken = {(o['x'], o['y']) for o in mj['object_events']}
    taken |= {(w['x'], w['y']) for w in mj['warp_events']}

    def place(gfx, x, y, movement, script, trainer=None, flag='0'):
        if not ok(x, y):
            report.append('  %s: (%d,%d) is not walkable, %s not placed'
                          % (name, x, y, script))
            return False
        if (x, y) in taken:
            report.append('  %s: (%d,%d) occupied, %s not placed' % (name, x, y, script))
            return False
        taken.add((x, y))
        mj['object_events'].append({
            "type": "object", "graphics_id": gfx, "x": x, "y": y, "elevation": 3,
            "movement_type": movement, "movement_range_x": 1, "movement_range_y": 1,
            "trainer_type": "TRAINER_TYPE_NORMAL" if trainer else "TRAINER_TYPE_NONE",
            "trainer_sight_or_berry_tree_id": str(trainer or 0),
            "script": '%s_EventScript_%s' % (name, script), "flag": flag})
        return True

    gfx = {'HIKER': 'OBJ_EVENT_GFX_HIKER', 'PICNICKER': 'OBJ_EVENT_GFX_PICNICKER',
           'CAMPER': 'OBJ_EVENT_GFX_CAMPER', 'YOUNGSTER': 'OBJ_EVENT_GFX_YOUNGSTER',
           'BLACK_BELT': 'OBJ_EVENT_GFX_BLACKBELT'}
    n = 0
    for t in TRAINERS.get(name, []):
        if place(gfx[t[2]], t[8], t[9], 'MOVEMENT_TYPE_FACE_DOWN', t[0], trainer=4):
            n += 1
    items = 0
    for (m, sfx, _it, x, y, flag) in ITEMS:
        if m == name and place('OBJ_EVENT_GFX_ITEM_BALL', x, y,
                               'MOVEMENT_TYPE_NONE', sfx, flag=flag):
            items += 1
    steelix = 0
    if name == F3 and place('OBJ_EVENT_GFX_FOLLOWER(SPECIES_STEELIX)',
                            STEELIX_AT[0], STEELIX_AT[1],
                            'MOVEMENT_TYPE_FACE_UP', 'Steelix', flag=STEELIX_FLAG):
        steelix = 1

    with open(path, 'w', newline='\n') as f:
        json.dump(mj, f, indent=2)
        f.write('\n')

    out, text = [], []
    for (sfx, const, _c, _pic, _mu, _pool, _lv, _nm, _x, _y, intro, defeat, post) in \
            TRAINERS.get(name, []):
        out += ['%s_EventScript_%s::' % (name, sfx),
                '\ttrainerbattle_single %s, %s_Text_%sIntro, %s_Text_%sDefeat'
                % (const, name, sfx, name, sfx),
                '\tmsgbox %s_Text_%sPost, MSGBOX_AUTOCLOSE' % (name, sfx),
                '\tend', '']
        for tag, lines in (('Intro', intro), ('Defeat', defeat), ('Post', post)):
            text.append('%s_Text_%s%s::' % (name, sfx, tag))
            text += ['\t.string %s' % ln for ln in lines]
            text.append('')
    for (m, sfx, item, _x, _y, flag) in ITEMS:
        if m != name:
            continue
        # finditem is callstd STD_FIND_ITEM, which removes the ball and sets
        # the object's own flag, so the script is the one line.
        out += ['%s_EventScript_%s::' % (name, sfx),
                '\tfinditem %s' % item,
                '\tend', '']
    if name == F3:
        localid = 1 + next(i for i, o in enumerate(mj['object_events'])
                           if o['script'].endswith('_EventScript_Steelix'))
        out += steelix_script(name, localid)
        text += steelix_text(name)
    splice(os.path.join(REPO, 'data/maps', name, 'scripts.inc'), out)
    splice(os.path.join(REPO, 'data/maps', name, 'text.inc'), text)
    return n, items, steelix


def steelix_script(name, localid):
    return [
        '@ The STEELIX in the bottom room. setwildbattle builds a plain wild',
        '@ mon; the callnative gives it SHEER FORCE -- its hidden ability --',
        '@ and the moveset that goes with it. See src/orange_static_mons.c.',
        '%s_EventScript_Steelix::' % name,
        '\tlock', '\tfaceplayer',
        '\twaitse',
        '\tplaymoncry SPECIES_STEELIX, CRY_MODE_ENCOUNTER',
        '\tmessage %s_Text_Steelix' % name,
        '\twaitmessage',
        '\twaitmoncry',
        '\tdelay 20',
        '\tplaybgm MUS_ENCOUNTER_GYM_LEADER, 0',
        '\twaitbuttonpress',
        '\tsetwildbattle SPECIES_STEELIX, %d' % STEELIX_LEVEL,
        '\tcallnative OrangeStaticMon_SunburstSteelix',
        '\tsetflag FLAG_SYS_SPECIAL_WILD_BATTLE',
        '\tspecial StartLegendaryBattle',
        '\twaitstate',
        '\tclearflag FLAG_SYS_SPECIAL_WILD_BATTLE',
        '\tspecialvar VAR_RESULT, GetBattleOutcome',
        '\tgoto_if_eq VAR_RESULT, B_OUTCOME_WON, %s_EventScript_SteelixGone' % name,
        '\tgoto_if_eq VAR_RESULT, B_OUTCOME_RAN, %s_EventScript_SteelixGone' % name,
        '\tgoto_if_eq VAR_RESULT, B_OUTCOME_PLAYER_TELEPORTED, '
        '%s_EventScript_SteelixGone' % name,
        '\trelease', '\tend', '',
        '@ Caught, beaten or fled from, it does not stay in the room. The',
        '@ local id is its place in the map object list, worked out when this',
        '@ was written rather than guessed.',
        '%s_EventScript_SteelixGone::' % name,
        '\tsetflag %s' % STEELIX_FLAG,
        '\tremoveobject %d' % localid,
        '\trelease', '\tend', '']


def steelix_text(name):
    return ['%s_Text_Steelix::' % name,
            '\t.string "The whole far wall of the chamber\\n"',
            '\t.string "moves.\\p"',
            '\t.string "It is not a wall.$"', '']


def main():
    report = []
    flags = add_flags(
        [(STEELIX_FLAG, 'the Sunburst Cave STEELIX has been fought')]
        + [(i[5], 'Sunburst Cave item: %s' % i[2].replace('ITEM_', ''))
           for i in ITEMS])
    made, nxt = add_trainers()

    # Say where each floor's far points are, so the placements can be judged
    # against the shape of the map rather than taken on trust.
    for name, entry in ((F2, (28, 5)), (F3, (31, 9))):
        seen = reachable_from(name, entry)
        print('  %-32s %d cells reachable from its entrance %s'
              % (name.replace('OrangeIsland', '').replace('s_Sunburst_', ''),
                 len(seen), entry))

    t2, i2, _ = build_floor(F2, report)
    t3, i3, st = build_floor(F3, report)
    print('  %d flags, %d trainers added (NUM_TRAINERS now %d)' % (flags, made, nxt))
    print('  2F: %d trainers, %d items' % (t2, i2))
    print('  3F: %d trainers, %d items, %d STEELIX' % (t3, i3, st))
    for line in report:
        print(line)


if __name__ == '__main__':
    main()
