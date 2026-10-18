# Run from the repo root, after orange_drake_art.py.
# Drake, head of the Orange Crew, in Pummelo Stadium.
#  - No gimmick: after four gyms with rules (rain, no items, type picks, doubles) Drake's
#    battle is a plain 6-on-6 single battle. His anime team, with straight attacking sets
#    (no sleep, no setup); Lv 46-50 under the Lv 50 Orange cap.
#  - Needs all four Orange badges. Win: FLAG_ORANGE_LEAGUE_CHAMPION (0x21A) and the
#    Winner's Trophy message; level caps and the team swap end (existing Drake checks).
#  - After the Kanto Champion (FLAG_DEFEATED_CHAMP): repeatable rematch,
#    TRAINER_CHAMPION_DRAKE_REMATCH, Lv 70-75.
#  - Drake stands on the east trainer platform (13,6), facing the field.
import json, re
def rd(p): return open(p, encoding='utf-8').read()
def wr(p, s): open(p, 'w', newline='\n', encoding='utf-8').write(s)
def sub1(s, old, new, p):
    assert old in s, (p, old[:70]); return s.replace(old, new, 1)

p = 'include/constants/flags.h'; s = rd(p)
if 'FLAG_ORANGE_LEAGUE_CHAMPION' not in s:
    s = re.sub(r'#define FLAG_0x21A\s+0x21A', '#define FLAG_ORANGE_LEAGUE_CHAMPION   0x21A // beat Drake in Pummelo Stadium', s)
    assert 'FLAG_ORANGE_LEAGUE_CHAMPION' in s; wr(p, s)

p = 'include/constants/opponents.h'; s = rd(p)
if 'TRAINER_CHAMPION_DRAKE_REMATCH' not in s:
    n = int(re.search(r'#define NUM_TRAINERS\s+(\d+)', s).group(1))
    s = sub1(s, '#define TRAINER_ORANGE_SWIMMER_M_EMMET           1194\n',
             '#define TRAINER_ORANGE_SWIMMER_M_EMMET           1194\n\n#define TRAINER_CHAMPION_DRAKE_REMATCH           %d\n' % n, p)
    s = re.sub(r'#define NUM_TRAINERS\s+\d+', '#define NUM_TRAINERS                             %d' % (n + 1), s)
    wr(p, s)

def mon(lvl, sp, item, moves, iv):
    return ('    {\n        .iv = %d,\n        .lvl = %d,\n        .species = SPECIES_%s,\n        .heldItem = ITEM_%s,\n'
            '        .moves = {%s},\n    },\n') % (iv, lvl, sp, item, ', '.join('MOVE_' + m for m in moves))
TEAM = [
    (46, 70, 'DITTO',      'QUICK_CLAW',   ['TRANSFORM', 'NONE', 'NONE', 'NONE'],                     ['TRANSFORM', 'NONE', 'NONE', 'NONE']),
    (47, 72, 'STEELIX',    'LEFTOVERS',    ['IRON_TAIL', 'EARTHQUAKE', 'ROCK_SLIDE', 'CRUNCH'],       ['IRON_TAIL', 'EARTHQUAKE', 'ROCK_SLIDE', 'CRUNCH']),
    (48, 72, 'GENGAR',     'SPELL_TAG',    ['SHADOW_BALL', 'THUNDERBOLT', 'PSYCHIC', 'SLUDGE_BOMB'],  ['SHADOW_BALL', 'THUNDERBOLT', 'PSYCHIC', 'SLUDGE_BOMB']),
    (48, 72, 'ELECTABUZZ', 'MAGNET',       ['THUNDERBOLT', 'BRICK_BREAK', 'ICE_PUNCH', 'QUICK_ATTACK'], ['THUNDERBOLT', 'CROSS_CHOP', 'ICE_PUNCH', 'FIRE_PUNCH']),
    (49, 73, 'VENUSAUR',   'MIRACLE_SEED', ['GIGA_DRAIN', 'SLUDGE_BOMB', 'EARTHQUAKE', 'BODY_SLAM'],  ['GIGA_DRAIN', 'SLUDGE_BOMB', 'EARTHQUAKE', 'BODY_SLAM']),
    (50, 75, 'DRAGONITE',  'LEFTOVERS',    ['DRAGON_CLAW', 'EARTHQUAKE', 'FLAMETHROWER', 'EXTREME_SPEED'], ['DRAGON_CLAW', 'EARTHQUAKE', 'FIRE_BLAST', 'EXTREME_SPEED']),
]
p = 'src/data/trainer_parties.h'; s = rd(p)
i = s.index('static const struct TrainerMonItemCustomMoves sParty_ChampionDrake[] = {'); j = s.index('};\n', i) + 3
new = ('static const struct TrainerMonItemCustomMoves sParty_ChampionDrake[] = {\n' + ''.join(mon(a, sp, it, m1, 250) for a, b, sp, it, m1, m2 in TEAM) + '};\n')
if 'sParty_ChampionDrakeRematch' not in s:
    new += ('\nstatic const struct TrainerMonItemCustomMoves sParty_ChampionDrakeRematch[] = {\n' + ''.join(mon(b, sp, it, m2, 255) for a, b, sp, it, m1, m2 in TEAM) + '};\n')
else:
    k = s.index('};\n', s.index('sParty_ChampionDrakeRematch[] = {')) + 3
    new += s[j:k]; j = k
s = s[:i] + new + s[j:]; wr(p, s)

p = 'src/data/trainers.h'; s = rd(p)
if '[TRAINER_CHAMPION_DRAKE_REMATCH]' not in s:
    i = s.index('    [TRAINER_ORANGE_SWIMMER_M_EMMET] = {'); j = s.index('    },\n', i) + len('    },\n')
    s = s[:j] + '''    [TRAINER_CHAMPION_DRAKE_REMATCH] = {
        .trainerClass = TRAINER_CLASS_CHAMPION,
        .encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_MALE,
        .trainerPic = TRAINER_PIC_ORANGE_DRAKE,
        .trainerName = _("DRAKE"),
        .items = {ITEM_FULL_RESTORE, ITEM_FULL_RESTORE},
        .doubleBattle = FALSE,
        .mugshotEnabled = TRUE,
        .mugshotColor = MUGSHOT_COLOR_DARK_RED,
        .aiFlags = AI_LEVEL_ADVANCED,
        .party = ITEM_CUSTOM_MOVES(sParty_ChampionDrakeRematch),
    },
''' + s[j:]
    wr(p, s)

# "DRAKE" in battle text, not the rival's name (CHAMPION class)
p = 'src/battle_message.c'; s = rd(p)
if 'TRAINER_CHAMPION_DRAKE_REMATCH' not in s:
    s = sub1(s, '&& gTrainerBattleOpponent_A != TRAINER_CHAMPION_DRAKE))',
             '&& gTrainerBattleOpponent_A != TRAINER_CHAMPION_DRAKE\n                      && gTrainerBattleOpponent_A != TRAINER_CHAMPION_DRAKE_REMATCH))', p)
    wr(p, s)

M = 'OrangeIslands_Pummelo_Stadium'
p = 'data/maps/%s/map.json' % M; mj = json.loads(rd(p))
if not any(o.get('script') == M + '_EventScript_Drake' for o in mj['object_events']):
    mj['object_events'].append({'type': 'object', 'graphics_id': 'OBJ_EVENT_GFX_ORANGE_DRAKE', 'x': 13, 'y': 6, 'elevation': 3,
        'movement_type': 'MOVEMENT_TYPE_FACE_LEFT', 'movement_range_x': 0, 'movement_range_y': 0,
        'trainer_type': 'TRAINER_TYPE_NONE', 'trainer_sight_or_berry_tree_id': '0',
        'script': M + '_EventScript_Drake', 'flag': '0'})
    wr(p, json.dumps(mj, indent=2, ensure_ascii=False) + '\n')

SCRIPT = r'''
@ ---- Drake (orange_drake_battle.py) ----
@ No rules: a plain 6-on-6 single battle. Needs the four Orange badges.
@ After the Kanto Champion (FLAG_DEFEATED_CHAMP) he takes rematches any number of times.
M_EventScript_Drake::
	lock
	faceplayer
	goto_if_defeated TRAINER_CHAMPION_DRAKE, M_EventScript_DrakeAfter
	goto_if_unset FLAG_BADGE_CORAL_EYE, M_EventScript_DrakeNoBadges
	goto_if_unset FLAG_BADGE_SPIKE_SHELL, M_EventScript_DrakeNoBadges
	goto_if_unset FLAG_BADGE_SEA_RUBY, M_EventScript_DrakeNoBadges
	goto_if_unset FLAG_BADGE_JADE_STAR, M_EventScript_DrakeNoBadges
	msgbox M_Text_DrakeIntro
	msgbox M_Text_DrakeReady, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, M_EventScript_DrakeNotReady
	msgbox M_Text_DrakeGo
	trainerbattle_no_intro TRAINER_CHAMPION_DRAKE, M_Text_DrakeDefeat
	msgbox M_Text_DrakeWon
	setflag FLAG_ORANGE_LEAGUE_CHAMPION
	message M_Text_ReceivedTrophy
	waitmessage
	playfanfare MUS_OBTAIN_BADGE
	waitfanfare
	msgbox M_Text_DrakeAfterTrophy
	release
	end

M_EventScript_DrakeNoBadges::
	msgbox M_Text_DrakeNoBadges
	release
	end

M_EventScript_DrakeNotReady::
	msgbox M_Text_DrakeNotReady
	release
	end

M_EventScript_DrakeAfter::
	setflag FLAG_ORANGE_LEAGUE_CHAMPION
	goto_if_unset FLAG_DEFEATED_CHAMP, M_EventScript_DrakePostBattle
	msgbox M_Text_DrakeRematchAsk, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, M_EventScript_DrakeNoRematch
	msgbox M_Text_DrakeRematchGo
	cleartrainerflag TRAINER_CHAMPION_DRAKE_REMATCH
	trainerbattle_no_intro TRAINER_CHAMPION_DRAKE_REMATCH, M_Text_DrakeRematchDefeat
	msgbox M_Text_DrakeRematchWon
	release
	end

M_EventScript_DrakePostBattle::
	msgbox M_Text_DrakePostBattle
	release
	end

M_EventScript_DrakeNoRematch::
	msgbox M_Text_DrakeNoRematch
	release
	end

M_Text_DrakeNoBadges::
	.string "This is the ORANGE LEAGUE.\n"
	.string "Four badges gets you a battle.\p"
	.string "Come back when you've got\n"
	.string "all four.$"

M_Text_DrakeIntro::
	.string "So you're the one who beat\n"
	.string "the whole ORANGE CREW.\p"
	.string "I'm DRAKE. I run this league.\p"
	.string "CISSY had her rain. DANNY took\n"
	.string "your items. RUDY picked your\l"
	.string "team. LUANA made you pair up.\p"
	.string "Not here.\p"
	.string "No rules. No tricks.\n"
	.string "Six on six. Last one standing\l"
	.string "wins.$"

M_Text_DrakeReady::
	.string "Ready?$"

M_Text_DrakeNotReady::
	.string "Then get ready.\n"
	.string "I'm not going anywhere.$"

M_Text_DrakeGo::
	.string "Good. Let's fight.$"

M_Text_DrakeDefeat::
	.string "...That's it.\n"
	.string "You beat me straight up.$"

M_Text_DrakeWon::
	.string "No excuses. That was the best\n"
	.string "fight I've had in years.\p"
	.string "You're the ORANGE LEAGUE\n"
	.string "CHAMPION. Take this.$"

M_Text_ReceivedTrophy::
	.string "{PLAYER} received the\n"
	.string "WINNER'S TROPHY!$"

M_Text_DrakeAfterTrophy::
	.string "Your name goes up on the wall\n"
	.string "with the others.\p"
	.string "The islands are yours now.\n"
	.string "No level limits, and you can\l"
	.string "bring any team you like.\p"
	.string "Any PC will let you switch\n"
	.string "between your KANTO and ORANGE\l"
	.string "boxes.$"

M_Text_DrakePostBattle::
	.string "You won. That's all there is\n"
	.string "to it.\p"
	.string "Go win the INDIGO PLATEAU too.\n"
	.string "Then come back and we'll go\l"
	.string "again. No holding back.$"

M_Text_DrakeRematchAsk::
	.string "The INDIGO CHAMPION, huh?\n"
	.string "Then I'm done holding back.\p"
	.string "Another round?$"

M_Text_DrakeNoRematch::
	.string "Suit yourself.\n"
	.string "I'll be here.$"

M_Text_DrakeRematchGo::
	.string "Full strength this time.\n"
	.string "Let's fight.$"

M_Text_DrakeRematchDefeat::
	.string "Ha! Still the champ.$"

M_Text_DrakeRematchWon::
	.string "Good fight. Come back any time.\n"
	.string "I'll be stronger.$"
'''.replace('M_', M + '_')
p = 'data/maps/%s/scripts.inc' % M; s = rd(p)
if M + '_EventScript_Drake::' not in s:
    wr(p, s.rstrip('\n') + '\n' + SCRIPT)
print('drake battle ok')
