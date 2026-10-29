#!/usr/bin/env python3
"""Rudy's Trovita Gym: a Blaine-style quiz, but the questions are type matchups.

Six hedge gates stand between the door and Rudy. Each has a panel in the hedge
beside it that asks one question; answer it and the hedge parts, miss it and the
student posted in that room comes over for a battle. Once a student is beaten a
wrong answer just costs you the walk back to the panel, which is how Blaine's
gym behaves once its trainers are gone.

Every answer is checked against gTypeEffectiveness itself rather than written
from memory, so the quiz cannot disagree with the battle engine. See
docs/orange-islands/trovita-quiz.md for the worked multipliers.

Run from the repo root.
"""
import json, os, re, struct, sys

W, H = 15, 27

FLOOR, WALL, HEDGE = 0x2A5, 0x2A0, 0x28D
PANEL_L, PANEL_R = 0x2B8, 0x2B9      # the panel is a signpost pair in the hedge
FLOWER_L, FLOWER_R = 0x2B6, 0x2B7    # planted under each gate, so the opening
                                     # is visible before the hedge parts
MAT = 0x2B1                           # south-arrow exit mat
ELEV = 3

CHAMBER = (2, 12, 2, 6)               # Rudy
ENTRANCE = (2, 12, 23, 25)
# gate rows from the door upward, and where each gate and its panel sit
GATES = [
    # hedge row, gate x (2 wide), panel x (2 wide), the room it opens into
    (22,  3,  6, (2, 12, 20, 21)),
    (19, 10,  7, (2, 12, 17, 18)),
    (16,  3,  6, (2, 12, 14, 15)),
    (13, 10,  7, (2, 12, 11, 12)),
    (10,  3,  6, (2, 12,  8,  9)),
    ( 7,  7,  4, None),               # last gate opens into Rudy's chamber
]
DOORS = [(6, 26), (7, 26), (8, 26)]


def build_blocks():
    g = {(x, y): (WALL, 1, 0) for x in range(W) for y in range(H)}

    def carve(r):
        x0, x1, y0, y1 = r
        for x in range(x0, x1 + 1):
            for y in range(y0, y1 + 1):
                g[(x, y)] = (FLOOR, 0, ELEV)

    carve(CHAMBER)
    carve(ENTRANCE)
    for _, _, _, room in GATES:
        if room:
            carve(room)
    # hedge rows, with the gate left solid until the quiz opens it
    for row, gx, px, _ in GATES:
        for x in range(2, 13):
            g[(x, row)] = (HEDGE, 1, 0)
        g[(px, row)] = (PANEL_L, 1, 0)
        g[(px + 1, row)] = (PANEL_R, 1, 0)
    # the panel can sit several tiles from its gate, so mark the gate
    for row, gx, _px, _r in GATES:
        g[(gx, row + 1)] = (FLOWER_L, 0, ELEV)
        g[(gx + 1, row + 1)] = (FLOWER_R, 0, ELEV)
    for p in DOORS:
        g[p] = (MAT, 0, ELEV)
    return g


def gate_tiles(i):
    row, gx, _, _ = GATES[i]
    return [(gx, row), (gx + 1, row)]


def main(repo='.'):
    g = build_blocks()
    out = os.path.join(repo, 'data/layouts/OrangeIslands_Trovita_Gym')
    os.makedirs(out, exist_ok=True)
    data = b''.join(struct.pack('<H', m | (c << 10) | (e << 12))
                    for y in range(H) for x in range(W)
                    for (m, c, e) in [g[(x, y)]])
    open(os.path.join(out, 'map.bin'), 'wb').write(data)
    open(os.path.join(out, 'border.bin'), 'wb').write(
        struct.pack('<4H', *([WALL | (1 << 10)] * 4)))

    p = os.path.join(repo, 'data/layouts/layouts.json')
    L = json.load(open(p))
    entry = {'id': 'LAYOUT_ORANGE_ISLANDS_TROVITA_GYM',
             'name': 'OrangeIslands_Trovita_Gym_Layout',
             'width': W, 'height': H, 'border_width': 2, 'border_height': 2,
             'primary_tileset': 'gTileset_Building',
             'secondary_tileset': 'gTileset_CeladonGym',
             'border_filepath': 'data/layouts/OrangeIslands_Trovita_Gym/border.bin',
             'blockdata_filepath': 'data/layouts/OrangeIslands_Trovita_Gym/map.bin'}
    L['layouts'] = [l for l in L['layouts'] if l['id'] != entry['id']] + [entry]
    json.dump(L, open(p, 'w'), indent=2)
    open(p, 'a').write('\n')
    print('trovita gym layout: %dx%d, %d gates' % (W, H, len(GATES)))


if __name__ == '__main__':
    main(sys.argv[1] if len(sys.argv) > 1 else '.')


# ---------------------------------------------------------------------------
# the quiz
# ---------------------------------------------------------------------------
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import quizdata as qz

M = 'OrangeIslands_Trovita_Gym'
POOL = qz.GATES
NEW_TRAINERS = [
    ('TRAINER_COOLTRAINER_TROVITA_YUZU', 'YUZU', 'F',
     [('GYARADOS', 34), ('MAGNETON', 34), ('VICTREEBEL', 35)]),
    ('TRAINER_COOLTRAINER_TROVITA_CEDRO', 'CEDRO', 'M',
     [('GRAVELER', 34), ('HAUNTER', 35), ('SCYTHER', 34)]),
]
PEOPLE = [
    ('OBJ_EVENT_GFX_RUDY', 7, 3, 'DOWN', 'Rudy'),
    ('OBJ_EVENT_GFX_COOLTRAINER_F', 4, 24, 'UP', 'Lime'),
    ('OBJ_EVENT_GFX_COOLTRAINER_M', 10, 21, 'UP', 'Rowan'),
    ('OBJ_EVENT_GFX_COOLTRAINER_F', 4, 18, 'UP', 'Yuzu'),
    ('OBJ_EVENT_GFX_COOLTRAINER_M', 10, 15, 'UP', 'Cedro'),
    ('OBJ_EVENT_GFX_GYM_GUY', 10, 24, 'DOWN', 'GymGuy'),
]
GATE_TRAINER = ['Lime', 'Rowan', 'Yuzu', 'Cedro', None, None]
TRAINER_CONST = {'Lime': 'TRAINER_COOLTRAINER_TROVITA_LIME',
                 'Rowan': 'TRAINER_COOLTRAINER_TROVITA_ROWAN',
                 'Yuzu': 'TRAINER_COOLTRAINER_TROVITA_YUZU',
                 'Cedro': 'TRAINER_COOLTRAINER_TROVITA_CEDRO'}
FLAGS = ['FLAG_TROVITA_GYM_GATE_%d' % (i + 1) for i in range(6)]
TAGS = [(g + 1, 'AB'[p]) for g in range(len(POOL)) for p in range(len(POOL[g]))]

STUDENT_LINES = {
 'Lime': ("LIME: Wrong! And a wrong answer in\\nthis gym gets you me.$",
          "Fine. You've earned the gate.$", None, None),
 'Rowan': ("ROWAN: Not even close. Let's see if\\nyou battle better than you think.$",
           "All right, all right. Go through.$", None, None),
 'Yuzu': ("YUZU: Hah! RUDY said someone would\\nguess that one.$",
          "You're better in a battle than at\\nthe panel. Go on.$",
          "YUZU: Every POKeMON I carry has two\\ntypes. Work out why that matters.$",
          "Two types, twice the holes.$"),
 'Cedro': ("CEDRO: That's the answer people give\\nwhen they only read half of it.$",
           "Beaten fair. The hedge is yours.$",
           "CEDRO: Read the whole thing. Type,\\nability, item. That's the lesson.$",
           "I read mine wrong that time.$"),
}


def emit_flags(repo):
    p = os.path.join(repo, 'include/constants/flags.h')
    s = open(p).read()
    if FLAGS[0] not in s:
        for i, f in enumerate(FLAGS):
            s = s.replace('#define FLAG_0x%03X               0x%03X' % (0x220 + i, 0x220 + i),
                          "#define %-29s 0x%03X // Rudy's gym: panel %d answered"
                          % (f, 0x220 + i, i + 1))
        open(p, 'w').write(s)
    p = os.path.join(repo, 'include/constants/metatile_labels.h')
    s = open(p).read()
    if 'METATILE_CeladonGym_Floor' not in s:
        s = s.replace('#define METATILE_Cave_Floor              0x281',
                      '#define METATILE_CeladonGym_Floor        0x2A5\n'
                      '#define METATILE_Cave_Floor              0x281')
        open(p, 'w').write(s)


def emit_menu(repo):
    p = os.path.join(repo, 'include/constants/menu.h')
    s = open(p).read()
    s = re.sub(r'#define MULTICHOICE_TROVITA_QUIZ_\w+\s+\d+\n', '', s)
    nums = [int(n) for n in re.findall(r'#define MULTICHOICE_\w+\s+(\d+)', s) if int(n) < 200]
    nxt = max(nums) + 1
    lines = ''.join('#define MULTICHOICE_TROVITA_QUIZ_%d%s%s%d\n'
                    % (g, t, ' ' * 32, nxt + i) for i, (g, t) in enumerate(TAGS))
    s = s.replace('#define MULTICHOICE_NONE', lines + '\n#define MULTICHOICE_NONE')
    open(p, 'w').write(s)


def emit_lists(repo):
    p = os.path.join(repo, 'src/script_menu.c')
    s = open(p).read()
    s = re.sub(r'static const struct MenuAction sMultichoiceList_TrovitaQuiz\w+\[\] = \{\n'
               r'(?:.*\n)*?\};\n\n', '', s)
    s = re.sub(r'    \[MULTICHOICE_TROVITA_QUIZ_\w+\]\s+= MULTICHOICE\(\w+\),\n', '', s)
    blocks, rows = [], []
    for (g, t) in TAGS:
        q = POOL[g - 1]['AB'.index(t)]
        sym = qz.SYMBOL[qz.option_kind(q)]
        name = 'sMultichoiceList_TrovitaQuiz%d%s' % (g, t)
        blocks.append('static const struct MenuAction %s[] = {\n%s};\n'
                      % (name, ''.join('    { %s },\n' % sym(o) for o in q['opts'])))
        rows.append('    [MULTICHOICE_TROVITA_QUIZ_%d%s]%s= MULTICHOICE(%s),\n'
                    % (g, t, ' ' * 29, name))
    s = s.replace('static const struct MultichoiceListStruct sMultichoiceLists[] = {',
                  '\n'.join(blocks) + '\nstatic const struct MultichoiceListStruct sMultichoiceLists[] = {')
    s = s.replace('    [MULTICHOICE_PC_BOX_SET]                                 = MULTICHOICE(sMultichoiceList_PCBoxSet),\n',
                  '    [MULTICHOICE_PC_BOX_SET]                                 = MULTICHOICE(sMultichoiceList_PCBoxSet),\n'
                  + ''.join(rows))
    # the option text comes straight from the ROM's own name tables
    for inc in ('battle_main.h', 'data.h', 'item.h', 'constants/items.h',
                'constants/moves.h', 'constants/abilities.h', 'constants/pokemon.h'):
        if '#include "%s"' % inc not in s:
            s = s.replace('#include "constants/menu.h"',
                          '#include "%s"\n#include "constants/menu.h"' % inc, 1)
    open(p, 'w').write(s)


def emit_trainers(repo):
    p = os.path.join(repo, 'include/constants/opponents.h')
    s = open(p).read()
    if NEW_TRAINERS[0][0] not in s:
        n = int(re.search(r'#define NUM_TRAINERS\s+(\d+)', s).group(1))
        add = ''.join('#define %-40s %d\n' % (t[0], n + i)
                      for i, t in enumerate(NEW_TRAINERS))
        s = s.replace('#define NUM_TRAINERS', '\n' + add + '\n#define NUM_TRAINERS')
        s = re.sub(r'(#define NUM_TRAINERS\s+)\d+', r'\g<1>%d' % (n + len(NEW_TRAINERS)), s)
        open(p, 'w').write(s)
    p = os.path.join(repo, 'src/data/trainer_parties.h')
    s = open(p).read()
    if 'sParty_CooltrainerTrovitaYuzu' not in s:
        out = ''
        for const, name, _g, mons in NEW_TRAINERS:
            out += ('static const struct TrainerMonNoItemDefaultMoves '
                    'sParty_CooltrainerTrovita%s[] = {\n' % name.title()
                    + ''.join('    {\n        .iv = 100,\n        .lvl = %d,\n'
                              '        .species = SPECIES_%s,\n    },\n' % (lv, sp)
                              for sp, lv in mons) + '};\n\n')
        anchor = 'static const struct TrainerMonNoItemDefaultMoves sParty_TwinsKumquatAnaBel[]'
        open(p, 'w').write(s.replace(anchor, out + anchor, 1))
    p = os.path.join(repo, 'src/data/trainers.h')
    s = open(p).read()
    if NEW_TRAINERS[0][0] not in s:
        out = ''
        for const, name, g, _m in NEW_TRAINERS:
            out += ('    [%s] = {\n'
                    '        .trainerClass = TRAINER_CLASS_COOLTRAINER,\n'
                    '        .encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_%s%s,\n'
                    '        .trainerPic = TRAINER_PIC_COOLTRAINER_%s,\n'
                    '        .trainerName = _("%s"),\n'
                    '        .items = {},\n'
                    '        .doubleBattle = FALSE,\n'
                    '        .aiFlags = AI_LEVEL_ADVANCED,\n'
                    '        .party = NO_ITEM_DEFAULT_MOVES(sParty_CooltrainerTrovita%s),\n'
                    '    },\n'
                    % (const, 'FEMALE' if g == 'F' else 'MALE',
                       ' | F_TRAINER_FEMALE' if g == 'F' else '', g, name, name.title()))
        anchor = '    [TRAINER_TWINS_KUMQUAT_ANA_BEL] = {'
        open(p, 'w').write(s.replace(anchor, out + anchor, 1))


def emit_map(repo):
    p = os.path.join(repo, 'data/maps/%s/map.json' % M)
    j = json.load(open(p))
    j['layout'] = 'LAYOUT_ORANGE_ISLANDS_TROVITA_GYM'
    j['object_events'] = [
        {'type': 'object', 'graphics_id': g, 'x': x, 'y': y, 'elevation': 3,
         'movement_type': 'MOVEMENT_TYPE_FACE_' + f,
         'movement_range_x': 1, 'movement_range_y': 1,
         'trainer_type': 'TRAINER_TYPE_NONE', 'trainer_sight_or_berry_tree_id': '0',
         'script': '%s_EventScript_%s' % (M, s), 'flag': '0'}
        for g, x, y, f, s in PEOPLE]
    j['warp_events'] = [{'x': x, 'y': y, 'elevation': 3,
                         'dest_map': 'MAP_ORANGE_ISLANDS_TROVITA', 'dest_warp_id': '0'}
                        for (x, y) in DOORS]
    bg = []
    for i, (row, _gx, px, _r) in enumerate(GATES):
        for dx in (0, 1):
            bg.append({'type': 'sign', 'x': px + dx, 'y': row, 'elevation': 0,
                       'player_facing_dir': 'BG_EVENT_PLAYER_FACING_ANY',
                       'script': '%s_EventScript_Panel%d' % (M, i + 1)})
    bg += [{'type': 'sign', 'x': x, 'y': 26, 'elevation': 0,
            'player_facing_dir': 'BG_EVENT_PLAYER_FACING_ANY',
            'script': '%s_EventScript_GymStatue' % M} for x in (4, 10)]
    j['bg_events'] = bg
    j['coord_events'] = []
    json.dump(j, open(p, 'w'), indent=2)
    open(p, 'a').write('\n')


def emit_scripts():
    P = M + '_'
    out = []
    a = out.append
    a('.equ LOCALID_RUDY, 1\n')
    a('%sMapScripts::' % P)
    a('\tmap_script MAP_SCRIPT_ON_LOAD, %sOnLoad' % P)
    a('\t.byte 0\n')
    a('@ The hedges are shut in the layout; ON_LOAD parts the ones already answered.')
    a('%sOnLoad::' % P)
    for i in range(len(POOL)):
        a('\tcall_if_set %s, %sOnLoadOpenGate%d' % (FLAGS[i], P, i + 1))
    a('\tend\n')
    for i in range(len(POOL)):
        a('%sOnLoadOpenGate%d::' % (P, i + 1))
        a('\tcall %sEventScript_OpenGate%d' % (P, i + 1))
        a('\treturn\n')
    for i in range(len(POOL)):
        a('%sEventScript_OpenGate%d::' % (P, i + 1))
        for (x, y) in gate_tiles(i):
            a('\tsetmetatile %2d, %2d, METATILE_CeladonGym_Floor, 0' % (x, y))
        a('\treturn\n')
    a('@ Each panel keeps two questions and picks one, so a second run through')
    a('@ the gym is not the same six answers. Every index below is what the ROM')
    a('@ data gives -- see docs/orange-islands/trovita-quiz.md.')
    for gi, pool in enumerate(POOL):
        n = gi + 1
        a('%sEventScript_Panel%d::' % (P, n))
        a('\tlockall')
        a('\tgoto_if_set %s, %sEventScript_GateAlreadyOpen' % (FLAGS[gi], P))
        a('\trandom %d' % len(pool))
        for pi in range(len(pool) - 1):
            a('\tgoto_if_eq VAR_RESULT, %d, %sEventScript_Ask%d%s'
              % (pi, P, n, 'AB'[pi]))
        a('\tgoto %sEventScript_Ask%d%s' % (P, n, 'AB'[len(pool) - 1]))
        a('')
        for pi, q in enumerate(pool):
            t = 'AB'[pi]
            ans, _s = qz.solve(q, 'gate %d%s' % (n, t))
            a('%sEventScript_Ask%d%s::' % (P, n, t))
            a('\tmessage %sText_Q%d%s' % (P, n, t))
            a('\twaitmessage')
            a('\tmultichoice 0, 0, MULTICHOICE_TROVITA_QUIZ_%d%s, FALSE' % (n, t))
            a('\tgoto_if_eq VAR_RESULT, SCR_MENU_CANCEL, %sEventScript_QuizCancelled' % P)
            a('\tgoto_if_eq VAR_RESULT, %d, %sEventScript_Gate%dCorrect' % (ans, P, n))
            a('\tgoto %sEventScript_Gate%dWrong' % (P, n))
            a('')
        a('%sEventScript_Gate%dCorrect::' % (P, n))
        a('\tcall %sEventScript_CorrectAnswer' % P)
        a('\tcall %sEventScript_Gate%dOpens' % (P, n))
        a('\treleaseall')
        a('\tend\n')
        a('%sEventScript_Gate%dOpens::' % (P, n))
        a('\tplayse SE_UNLOCK')
        a('\twaitse')
        a('\tcall %sEventScript_OpenGate%d' % (P, n))
        a('\tspecial DrawWholeMapView')
        a('\tsetflag %s' % FLAGS[gi])
        a('\treturn\n')
        a('%sEventScript_Gate%dWrong::' % (P, n))
        a('\tcall %sEventScript_WrongAnswer' % P)
        who = GATE_TRAINER[gi]
        if who:
            a('\tgoto_if_not_defeated %s, %sEventScript_Battle%s'
              % (TRAINER_CONST[who], P, who))
        a('\treleaseall')
        a('\tend\n')
        if who:
            a('@ Beat the student and the hedge opens anyway -- know it or earn it.')
            a('%sEventScript_Battle%s::' % (P, who))
            a('\tmsgbox %sText_%sStepsIn' % (P, who))
            a('\ttrainerbattle_no_intro %s, %sText_%sDefeat' % (TRAINER_CONST[who], P, who))
            a('\tgoto_if_set %s, %sEventScript_GateAlreadyOpen' % (FLAGS[gi], P))
            a('\tcall %sEventScript_Gate%dOpens' % (P, n))
            a('\treleaseall')
            a('\tend\n')
    a('%sEventScript_CorrectAnswer::' % P)
    a('\tplayfanfare MUS_LEVEL_UP')
    a('\twaitfanfare')
    a('\tmsgbox %sText_Correct' % P)
    a('\treturn\n')
    a('%sEventScript_WrongAnswer::' % P)
    a('\tplayse SE_BOO')
    a('\twaitse')
    a('\tmsgbox %sText_Wrong' % P)
    a('\tclosemessage')
    a('\treturn\n')
    a('%sEventScript_GateAlreadyOpen::' % P)
    a('\treleaseall')
    a('\tend\n')
    a('%sEventScript_QuizCancelled::' % P)
    a('\tclosemessage')
    a('\treleaseall')
    a('\tend\n')
    for who in ('Yuzu', 'Cedro'):
        a('%sEventScript_%s::' % (P, who))
        a('\ttrainerbattle_single %s, %sText_%sIntro, %sText_%sDefeat'
          % (TRAINER_CONST[who], P, who, P, who))
        a('\tmsgbox %sText_%sPostBattle, MSGBOX_AUTOCLOSE' % (P, who))
        a('\tend\n')
    return '\n'.join(out)


def emit_texts():
    P = M + '_'
    o = []
    a = o.append
    for (g, t) in TAGS:
        q = POOL[g - 1]['AB'.index(t)]
        a('%sText_Q%d%s::' % (P, g, t))
        a('\t.string "RUDY\'S PANEL:\\p"')
        a('\t.string "%s$"\n' % q['text'].replace('POKeMON', 'POKéMON'))
    a('%sText_Correct::' % P)
    a('\t.string "Right! The hedge is opening!$"\n')
    a('%sText_Wrong::' % P)
    a('\t.string "Wrong. Read the whole POKéMON,\\nnot just the first thing you saw.$"\n')
    for who, (steps, defeat, intro, post) in STUDENT_LINES.items():
        a('%sText_%sStepsIn::' % (P, who))
        a('\t.string "%s"\n' % steps.replace('POKeMON', 'POKéMON'))
        if intro:
            for tag, line in (('Intro', intro), ('Defeat', defeat), ('PostBattle', post)):
                a('%sText_%s%s::' % (P, who, tag))
                a('\t.string "%s"\n' % line.replace('POKeMON', 'POKéMON'))
    return '\n'.join(o)


def write_scripts(repo, base):
    p = os.path.join(repo, 'data/maps/%s/scripts.inc' % M)
    s = open(base).read()
    s = emit_scripts() + '\n' + s[s.index('@ The player brings three'):]
    s = s.rstrip('\n') + '\n\n@ ---- quiz ----\n' + emit_texts() + '\n'
    open(p, 'w').write(s)


def build_all(repo='.', base=None):
    qz.check_all()
    main(repo)
    emit_flags(repo)
    emit_menu(repo)
    emit_lists(repo)
    emit_trainers(repo)
    emit_map(repo)
    write_scripts(repo, base or os.path.join(repo, 'data/maps/%s/scripts.inc' % M))
    print('trovita gym: %d panels, %d questions across %d kinds'
          % (len(POOL), len(TAGS),
             len({q['kind'] for pool in POOL for q in pool})))
