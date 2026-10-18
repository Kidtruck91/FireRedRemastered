# Run from the repo root, after orange_pc_merge.py.
#  - Navel Gym (Pewter layout): Danny's gauntlet. Talking to Danny starts three crew battles in a
#    row, then Danny, with no healing in between and no items (FLAG_NO_BATTLE_ITEMS). Losing
#    resets the gauntlet.
#  - Trovita Gym (Celadon layout, Cut trees removed): Rudy's type triad, bring one FIRE, one WATER
#    and one GRASS POKeMON (OrangeGym_ChooseTypeTriad).
#  - Kumquat Gym (Olivine layout): double battles, Luana included.
#  - Badges: Spike Shell (Danny), Sea Ruby (Rudy), Jade Star (Luana) + TM07 / TM10 / TM33.
#  - After beating Drake, OrangeChallenge_Begin no longer swaps teams (the trip is a plain visit).
import json, os, re

def rd(p): return open(p, encoding='utf-8').read()
def wr(p, s): open(p, 'w', newline='\n', encoding='utf-8').write(s)
def sub1(t, old, new, label):
    assert old in t, label
    return t.replace(old, new, 1)

# ---------------------------------------------------------------- flags
p = 'include/constants/flags.h'; t = rd(p)
for old, new in [
    ('#define FLAG_0x20C               0x20C', '#define FLAG_BADGE_SPIKE_SHELL       0x20C // Orange League badge 2 (Danny, Navel)'),
    ('#define FLAG_0x20D               0x20D', '#define FLAG_BADGE_SEA_RUBY          0x20D // Orange League badge 3 (Rudy, Trovita)'),
    ('#define FLAG_0x20E               0x20E', '#define FLAG_BADGE_JADE_STAR         0x20E // Orange League badge 4 (Luana, Kumquat)'),
    ('#define FLAG_0x20F               0x20F', '#define FLAG_GOT_TM07_FROM_DANNY     0x20F'),
    ('#define FLAG_0x210               0x210', '#define FLAG_GOT_TM10_FROM_RUDY      0x210'),
    ('#define FLAG_0x211               0x211', '#define FLAG_GOT_TM33_FROM_LUANA     0x211'),
]:
    if new.split()[1] not in t:
        t = sub1(t, old, new, old)
wr(p, t)

# ---------------------------------------------------------------- trainers
NEW = [
 # const, class, pic, name, music, double, party (lvl, species)
 ('TRAINER_HIKER_NAVEL_KAI', 'TRAINER_CLASS_HIKER', 'TRAINER_PIC_HIKER', 'KAI', 'TRAINER_ENCOUNTER_MUSIC_HIKER', False,
  [(24, 'GEODUDE'), (25, 'MACHOP')]),
 ('TRAINER_BLACK_BELT_NAVEL_SORA', 'TRAINER_CLASS_BLACK_BELT', 'TRAINER_PIC_BLACK_BELT', 'SORA', 'TRAINER_ENCOUNTER_MUSIC_HIKER', False,
  [(25, 'MANKEY'), (25, 'NIDORINO')]),
 ('TRAINER_COOLTRAINER_NAVEL_YUKI', 'TRAINER_CLASS_COOLTRAINER', 'TRAINER_PIC_COOLTRAINER_F', 'YUKI', 'TRAINER_ENCOUNTER_MUSIC_FEMALE | F_TRAINER_FEMALE', False,
  [(26, 'SEEL'), (26, 'SHELLDER'), (26, 'SWINUB')]),
 ('TRAINER_COOLTRAINER_TROVITA_LIME', 'TRAINER_CLASS_COOLTRAINER', 'TRAINER_PIC_COOLTRAINER_F', 'LIME', 'TRAINER_ENCOUNTER_MUSIC_FEMALE | F_TRAINER_FEMALE', False,
  [(34, 'GROWLITHE'), (34, 'POLIWHIRL'), (34, 'GLOOM')]),
 ('TRAINER_COOLTRAINER_TROVITA_ROWAN', 'TRAINER_CLASS_COOLTRAINER', 'TRAINER_PIC_COOLTRAINER_M', 'ROWAN', 'TRAINER_ENCOUNTER_MUSIC_MALE', False,
  [(34, 'PONYTA'), (35, 'SEADRA'), (34, 'WEEPINBELL')]),
 ('TRAINER_TWINS_KUMQUAT_ANA_BEL', 'TRAINER_CLASS_TWINS', 'TRAINER_PIC_TWINS', 'ANA & BEL', 'TRAINER_ENCOUNTER_MUSIC_TWINS | F_TRAINER_FEMALE', True,
  [(41, 'NIDORINA'), (41, 'NIDORINO')]),
 ('TRAINER_COOL_COUPLE_KUMQUAT_LEX_NYA', 'TRAINER_CLASS_COOL_COUPLE', 'TRAINER_PIC_COOL_COUPLE', 'LEX & NYA', 'TRAINER_ENCOUNTER_MUSIC_TWINS', True,
  [(42, 'DODRIO'), (42, 'RAPIDASH'), (42, 'GOLDUCK'), (42, 'PERSIAN')]),
]
def sym(c): return 'sParty_' + ''.join(w.capitalize() for w in c[len('TRAINER_'):].split('_'))

p = 'include/constants/opponents.h'; t = rd(p)
if NEW[0][0] not in t:
    n = int(re.search(r'#define NUM_TRAINERS\s+(\d+)', t).group(1))
    old = re.search(r'#define TRAINER_SWIMMER_F_CORAL\s+\d+\n', t).group(0)
    t = t.replace(old, old + ''.join('#define %-40s %d\n' % (c[0], n + i) for i, c in enumerate(NEW)), 1)
    t = re.sub(r'#define NUM_TRAINERS\s+\d+', '#define NUM_TRAINERS                             %d' % (n + len(NEW)), t)
    wr(p, t)
p = 'src/data/trainer_parties.h'; t = rd(p)
if sym(NEW[0][0]) not in t:
    for c, cls, pic, name, mus, dbl, party in NEW:
        t = t.rstrip('\n') + '\n\nstatic const struct TrainerMonNoItemDefaultMoves %s[] = {\n' % sym(c)
        for lvl, sp in party:
            t += '    {\n        .iv = 100,\n        .lvl = %d,\n        .species = SPECIES_%s,\n    },\n' % (lvl, sp)
        t += '};\n'
    wr(p, t)
p = 'src/data/trainers.h'; t = rd(p)
if '[%s]' % NEW[0][0] not in t:
    ent = ''
    for c, cls, pic, name, mus, dbl, party in NEW:
        ent += ('    [%s] = {\n        .trainerClass = %s,\n        .encounterMusic_gender = %s,\n        .trainerPic = %s,\n'
                '        .trainerName = _("%s"),\n        .items = {},\n        .doubleBattle = %s,\n'
                '        .aiFlags = AI_LEVEL_ADVANCED,\n        .party = NO_ITEM_DEFAULT_MOVES(%s),\n    },\n') % (
                c, cls, mus, pic, name, 'TRUE' if dbl else 'FALSE', sym(c))
    i = t.index('    [TRAINER_SWIMMER_F_CORAL] = {'); j = t.index('    },\n', i) + len('    },\n')
    t = t[:j] + ent + t[j:]
    wr(p, t)

# ---------------------------------------------------------------- after Drake: the trip is a plain visit
p = 'src/orange_islands.c'; t = rd(p)
if 'plain visit' not in t:
    t = sub1(t, '''    gSpecialVar_Result = FALSE;
    if (FlagGet(FLAG_ORANGE_CHALLENGE))
        return;
    if (!ShowBoxSet(FALSE)) // the Kanto boxes may be switched out (after Drake)''', '''    gSpecialVar_Result = FALSE;
    if (FlagGet(FLAG_ORANGE_CHALLENGE))
        return;
    if (FlagGet(TRAINER_FLAGS_START + TRAINER_CHAMPION_DRAKE))
    {
        gSpecialVar_Result = TRUE; // after Drake the trip is a plain visit: no team swap, no caps
        return;
    }
    if (!ShowBoxSet(FALSE)) // the Kanto boxes may be switched out (after Drake)''', 'begin')
    if '#include "constants/opponents.h"' not in t:
        t = sub1(t, '#include "constants/flags.h"\n', '#include "constants/flags.h"\n#include "constants/opponents.h"\n', 'inc')
    wr(p, t)

# ---------------------------------------------------------------- maps
def objev(gfx, x, y, script, mt='MOVEMENT_TYPE_FACE_DOWN', tt='TRAINER_TYPE_NONE', sight='0'):
    return {"type": "object", "graphics_id": gfx, "x": x, "y": y, "elevation": 3, "movement_type": mt,
            "movement_range_x": 1, "movement_range_y": 1, "trainer_type": tt, "trainer_sight_or_berry_tree_id": sight,
            "script": script, "flag": "0"}
def sign(x, y, script):
    return {"type": "sign", "x": x, "y": y, "elevation": 0, "player_facing_dir": "BG_EVENT_PLAYER_FACING_ANY", "script": script}

GYMS = [
 # town, gym name, layout, door (x,y), gym warps [(x,y)], warp the door lands on, objects, signs
 ('OrangeIslands_Navel', 'OrangeIslands_Navel_Gym', 'LAYOUT_PEWTER_CITY_GYM', (37, 13), [(5, 14), (6, 14), (7, 14)], 1,
  [objev('OBJ_EVENT_GFX_BLACKBELT', 6, 5, 'OrangeIslands_Navel_Gym_EventScript_Danny'),
   objev('OBJ_EVENT_GFX_HIKER', 3, 6, 'OrangeIslands_Navel_Gym_EventScript_Kai', 'MOVEMENT_TYPE_FACE_RIGHT'),
   objev('OBJ_EVENT_GFX_BLACKBELT', 9, 6, 'OrangeIslands_Navel_Gym_EventScript_Sora', 'MOVEMENT_TYPE_FACE_LEFT'),
   objev('OBJ_EVENT_GFX_COOLTRAINER_F', 6, 3, 'OrangeIslands_Navel_Gym_EventScript_Yuki'),
   objev('OBJ_EVENT_GFX_GYM_GUY', 7, 12, 'OrangeIslands_Navel_Gym_EventScript_GymGuy')],
  [sign(4, 12, 'OrangeIslands_Navel_Gym_EventScript_GymStatue'), sign(8, 12, 'OrangeIslands_Navel_Gym_EventScript_GymStatue')]),
 ('OrangeIslands_Trovita', 'OrangeIslands_Trovita_Gym', 'LAYOUT_CELADON_CITY_GYM', (26, 34), [(5, 18), (6, 18), (7, 18)], 1,
  [objev('OBJ_EVENT_GFX_COOLTRAINER_M', 6, 4, 'OrangeIslands_Trovita_Gym_EventScript_Rudy'),
   objev('OBJ_EVENT_GFX_COOLTRAINER_F', 3, 11, 'OrangeIslands_Trovita_Gym_EventScript_Lime', 'MOVEMENT_TYPE_FACE_RIGHT', 'TRAINER_TYPE_NORMAL', '3'),
   objev('OBJ_EVENT_GFX_COOLTRAINER_M', 9, 10, 'OrangeIslands_Trovita_Gym_EventScript_Rowan', 'MOVEMENT_TYPE_FACE_LEFT', 'TRAINER_TYPE_NORMAL', '3'),
   objev('OBJ_EVENT_GFX_GYM_GUY', 7, 16, 'OrangeIslands_Trovita_Gym_EventScript_GymGuy')],
  [sign(4, 16, 'OrangeIslands_Trovita_Gym_EventScript_GymStatue'), sign(8, 16, 'OrangeIslands_Trovita_Gym_EventScript_GymStatue')]),
 ('OrangeIslands_Kumquat', 'OrangeIslands_Kumquat_Gym', 'LAYOUT_OLIVINE_GYM', (25, 20), [(11, 20)], 0,
  [objev('OBJ_EVENT_GFX_BEAUTY', 11, 9, 'OrangeIslands_Kumquat_Gym_EventScript_Luana'),
   objev('OBJ_EVENT_GFX_LITTLE_GIRL', 10, 16, 'OrangeIslands_Kumquat_Gym_EventScript_Ana', 'MOVEMENT_TYPE_FACE_RIGHT', 'TRAINER_TYPE_NORMAL', '2'),
   objev('OBJ_EVENT_GFX_LITTLE_GIRL', 10, 17, 'OrangeIslands_Kumquat_Gym_EventScript_Bel', 'MOVEMENT_TYPE_FACE_RIGHT', 'TRAINER_TYPE_NORMAL', '2'),
   objev('OBJ_EVENT_GFX_COOLTRAINER_M', 13, 12, 'OrangeIslands_Kumquat_Gym_EventScript_Lex', 'MOVEMENT_TYPE_FACE_LEFT', 'TRAINER_TYPE_NORMAL', '2'),
   objev('OBJ_EVENT_GFX_COOLTRAINER_F', 13, 13, 'OrangeIslands_Kumquat_Gym_EventScript_Nya', 'MOVEMENT_TYPE_FACE_LEFT', 'TRAINER_TYPE_NORMAL', '2'),
   objev('OBJ_EVENT_GFX_GYM_GUY', 13, 19, 'OrangeIslands_Kumquat_Gym_EventScript_GymGuy')],
  []),
]
G = 'data/maps/map_groups.json'; g = json.loads(rd(G))
for town, name, layout, door, warps, land, objs, signs in GYMS:
    if name not in g['gMapGroup_OrangeIslands']:
        g['gMapGroup_OrangeIslands'].append(name)
wr(G, json.dumps(g, indent=2) + '\n')

for town, name, layout, door, warps, land, objs, signs in GYMS:
    tp = 'data/maps/%s/map.json' % town; tm = json.loads(rd(tp))
    mapid = 'MAP_' + re.sub(r'(?<!^)(?=[A-Z])', '_', name.replace('OrangeIslands_', 'OrangeIslands')).upper()
    mapid = 'MAP_ORANGE_ISLANDS_' + name[len('OrangeIslands_'):].upper()
    townid = 'MAP_ORANGE_ISLANDS_' + town[len('OrangeIslands_'):].upper()
    idx = [i for i, w in enumerate(tm['warp_events']) if w['dest_map'] == mapid]
    if not idx:
        tm['warp_events'].append({"x": door[0], "y": door[1], "elevation": 3, "dest_map": mapid, "dest_warp_id": str(land)})
        idx = [len(tm['warp_events']) - 1]
        wr(tp, json.dumps(tm, indent=2) + '\n')
    m = {"id": mapid, "name": name, "layout": layout, "music": "MUS_GYM",
         "region_map_section": tm["region_map_section"], "requires_flash": False, "weather": "WEATHER_NONE",
         "map_type": "MAP_TYPE_INDOOR", "allow_cycling": False, "allow_escaping": False, "allow_running": False,
         "show_map_name": False, "floor_number": 0, "battle_scene": "MAP_BATTLE_SCENE_GYM", "connections": None,
         "object_events": objs,
         "warp_events": [{"x": x, "y": y, "elevation": 3, "dest_map": townid, "dest_warp_id": str(idx[0])} for x, y in warps],
         "coord_events": [], "bg_events": signs}
    os.makedirs('data/maps/' + name, exist_ok=True)
    wr('data/maps/%s/map.json' % name, json.dumps(m, indent=2) + '\n')

# ---------------------------------------------------------------- scripts
NAVEL = r'''.equ LOCALID_DANNY, 1
.equ LOCALID_KAI,   2
.equ LOCALID_SORA,  3
.equ LOCALID_YUKI,  4

OrangeIslands_Navel_Gym_MapScripts::
	.byte 0

@ Danny's gauntlet: his crew, then Danny, back to back. No healing, no items.
OrangeIslands_Navel_Gym_EventScript_Danny::
	lock
	faceplayer
	goto_if_defeated TRAINER_LEADER_DANNY, OrangeIslands_Navel_Gym_EventScript_DannyAfter
	msgbox OrangeIslands_Navel_Gym_Text_DannyChallenge, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, OrangeIslands_Navel_Gym_EventScript_DannyDeclined
	cleartrainerflag TRAINER_HIKER_NAVEL_KAI
	cleartrainerflag TRAINER_BLACK_BELT_NAVEL_SORA
	cleartrainerflag TRAINER_COOLTRAINER_NAVEL_YUKI
	setflag FLAG_NO_BATTLE_ITEMS
	msgbox OrangeIslands_Navel_Gym_Text_DannyGo
	closemessage
	applymovement LOCALID_KAI, Common_Movement_FacePlayer
	waitmovement 0
	msgbox OrangeIslands_Navel_Gym_Text_KaiIntro
	trainerbattle_no_intro TRAINER_HIKER_NAVEL_KAI, OrangeIslands_Navel_Gym_Text_KaiDefeat
	applymovement LOCALID_SORA, Common_Movement_FacePlayer
	waitmovement 0
	msgbox OrangeIslands_Navel_Gym_Text_SoraIntro
	trainerbattle_no_intro TRAINER_BLACK_BELT_NAVEL_SORA, OrangeIslands_Navel_Gym_Text_SoraDefeat
	applymovement LOCALID_YUKI, Common_Movement_FacePlayer
	waitmovement 0
	msgbox OrangeIslands_Navel_Gym_Text_YukiIntro
	trainerbattle_no_intro TRAINER_COOLTRAINER_NAVEL_YUKI, OrangeIslands_Navel_Gym_Text_YukiDefeat
	applymovement LOCALID_DANNY, Common_Movement_FacePlayer
	waitmovement 0
	msgbox OrangeIslands_Navel_Gym_Text_DannyIntro
	trainerbattle_no_intro TRAINER_LEADER_DANNY, OrangeIslands_Navel_Gym_Text_DannyDefeat
	clearflag FLAG_NO_BATTLE_ITEMS
	setflag FLAG_BADGE_SPIKE_SHELL
	message OrangeIslands_Navel_Gym_Text_ReceivedBadge
	waitmessage
	playfanfare MUS_OBTAIN_BADGE
	waitfanfare
	msgbox OrangeIslands_Navel_Gym_Text_ExplainBadge
	goto OrangeIslands_Navel_Gym_EventScript_GiveTM07
	end

OrangeIslands_Navel_Gym_EventScript_DannyDeclined::
	msgbox OrangeIslands_Navel_Gym_Text_DannyDeclined
	release
	end

OrangeIslands_Navel_Gym_EventScript_DannyAfter::
	goto_if_unset FLAG_GOT_TM07_FROM_DANNY, OrangeIslands_Navel_Gym_EventScript_GiveTM07
	msgbox OrangeIslands_Navel_Gym_Text_DannyPostBattle
	release
	end

OrangeIslands_Navel_Gym_EventScript_GiveTM07::
	checkitemspace ITEM_TM07
	goto_if_eq VAR_RESULT, FALSE, OrangeIslands_Navel_Gym_EventScript_NoRoom
	giveitem_msg OrangeIslands_Navel_Gym_Text_ReceivedTM07, ITEM_TM07
	setflag FLAG_GOT_TM07_FROM_DANNY
	msgbox OrangeIslands_Navel_Gym_Text_ExplainTM07
	release
	end

OrangeIslands_Navel_Gym_EventScript_NoRoom::
	msgbox OrangeIslands_Navel_Gym_Text_NoRoom
	release
	end

OrangeIslands_Navel_Gym_EventScript_Kai::
	msgbox OrangeIslands_Navel_Gym_Text_KaiTalk, MSGBOX_NPC
	end

OrangeIslands_Navel_Gym_EventScript_Sora::
	msgbox OrangeIslands_Navel_Gym_Text_SoraTalk, MSGBOX_NPC
	end

OrangeIslands_Navel_Gym_EventScript_Yuki::
	msgbox OrangeIslands_Navel_Gym_Text_YukiTalk, MSGBOX_NPC
	end

OrangeIslands_Navel_Gym_EventScript_GymGuy::
	lock
	faceplayer
	goto_if_set FLAG_BADGE_SPIKE_SHELL, OrangeIslands_Navel_Gym_EventScript_GymGuyAfter
	msgbox OrangeIslands_Navel_Gym_Text_GymGuyAdvice
	release
	end

OrangeIslands_Navel_Gym_EventScript_GymGuyAfter::
	msgbox OrangeIslands_Navel_Gym_Text_GymGuyAfter
	release
	end

OrangeIslands_Navel_Gym_EventScript_GymStatue::
	lockall
	goto_if_set FLAG_BADGE_SPIKE_SHELL, OrangeIslands_Navel_Gym_EventScript_GymStatueAfter
	msgbox OrangeIslands_Navel_Gym_Text_GymStatue
	releaseall
	end

OrangeIslands_Navel_Gym_EventScript_GymStatueAfter::
	msgbox OrangeIslands_Navel_Gym_Text_GymStatueAfter
	releaseall
	end

OrangeIslands_Navel_Gym_Text_DannyChallenge::
	.string "I'm DANNY, NAVEL's LEADER.\p"
	.string "My challenge is simple. Beat my\n"
	.string "crew, all three, then me. One\l"
	.string "after another, no breaks.\p"
	.string "No healing in between, and no\n"
	.string "items in battle. Lose once and\l"
	.string "you start over.\p"
	.string "Ready to climb?$"

OrangeIslands_Navel_Gym_Text_DannyDeclined::
	.string "Heal up and come back when you're\n"
	.string "ready. The summit isn't going\l"
	.string "anywhere.$"

OrangeIslands_Navel_Gym_Text_DannyGo::
	.string "Crew! We've got a challenger!$"

OrangeIslands_Navel_Gym_Text_KaiIntro::
	.string "KAI: I'm first! Let's see if you\n"
	.string "can make it past the foothills!$"

OrangeIslands_Navel_Gym_Text_KaiDefeat::
	.string "You're tougher than this\n"
	.string "mountain!$"

OrangeIslands_Navel_Gym_Text_SoraIntro::
	.string "SORA: Don't catch your breath!\n"
	.string "I'm next!$"

OrangeIslands_Navel_Gym_Text_SoraDefeat::
	.string "Hyah! You win!$"

OrangeIslands_Navel_Gym_Text_YukiIntro::
	.string "YUKI: The summit is cold.\n"
	.string "Can your team take the chill?$"

OrangeIslands_Navel_Gym_Text_YukiDefeat::
	.string "Brr... You melted right\n"
	.string "through us.$"

OrangeIslands_Navel_Gym_Text_DannyIntro::
	.string "You beat my whole crew without\n"
	.string "a rest. Now it's my turn!$"

OrangeIslands_Navel_Gym_Text_DannyDefeat::
	.string "You made it to the top!$"

OrangeIslands_Navel_Gym_Text_ReceivedBadge::
	.string "{PLAYER} received the\n"
	.string "SPIKE SHELL BADGE from DANNY!$"

OrangeIslands_Navel_Gym_Text_ExplainBadge::
	.string "That's the SPIKE SHELL BADGE.\n"
	.string "The ORANGE CREW will raise your\l"
	.string "level cap now.\p"
	.string "And take this. It's a souvenir\n"
	.string "from the top of NAVEL.$"

OrangeIslands_Navel_Gym_Text_ReceivedTM07::
	.string "{PLAYER} received TM07\n"
	.string "from DANNY.$"

OrangeIslands_Navel_Gym_Text_ExplainTM07::
	.string "TM07 is HAIL. It hurts every\n"
	.string "POKeMON that isn't ICE type\l"
	.string "for five turns.$"

OrangeIslands_Navel_Gym_Text_NoRoom::
	.string "Your BAG is full. Make room and\n"
	.string "come back for this.$"

OrangeIslands_Navel_Gym_Text_DannyPostBattle::
	.string "TROVITA ISLAND is next, along\n"
	.string "ROUTE 11 from NAVEL BEACH.$"

OrangeIslands_Navel_Gym_Text_KaiTalk::
	.string "Talk to DANNY when you're ready.\n"
	.string "Then we come at you one by one!$"

OrangeIslands_Navel_Gym_Text_SoraTalk::
	.string "Bring a full team. You won't get\n"
	.string "to heal between us.$"

OrangeIslands_Navel_Gym_Text_YukiTalk::
	.string "I'm last before DANNY. My team\n"
	.string "loves the cold.$"

OrangeIslands_Navel_Gym_Text_GymGuyAdvice::
	.string "Yo! DANNY's gym is a marathon!\p"
	.string "You fight his crew and then him,\n"
	.string "back to back, with no items.\p"
	.string "Bring six POKeMON at full health,\n"
	.string "and watch out for ROCK, FIGHTING\l"
	.string "and ICE types!$"

OrangeIslands_Navel_Gym_Text_GymGuyAfter::
	.string "You made it through the whole\n"
	.string "gauntlet! Incredible!$"

OrangeIslands_Navel_Gym_Text_GymStatue::
	.string "NAVEL ISLAND POKeMON GYM\n"
	.string "LEADER: DANNY\p"
	.string "WINNING TRAINERS:\n"
	.string "{RIVAL}$"

OrangeIslands_Navel_Gym_Text_GymStatueAfter::
	.string "NAVEL ISLAND POKeMON GYM\n"
	.string "LEADER: DANNY\p"
	.string "WINNING TRAINERS:\n"
	.string "{RIVAL}, {PLAYER}$"
'''

TROVITA = r'''.equ LOCALID_RUDY, 1

OrangeIslands_Trovita_Gym_MapScripts::
	.byte 0

@ Type triad: the player battles with one FIRE, one WATER and one GRASS POKeMON.
OrangeIslands_Trovita_Gym_EventScript_Rudy::
	lock
	faceplayer
	goto_if_defeated TRAINER_LEADER_RUDY, OrangeIslands_Trovita_Gym_EventScript_RudyAfter
	msgbox OrangeIslands_Trovita_Gym_Text_RudyChallenge, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, OrangeIslands_Trovita_Gym_EventScript_RudyDeclined
	msgbox OrangeIslands_Trovita_Gym_Text_ChooseThree
	closemessage
	setvar VAR_0x8005, TYPE_FIRE
	setvar VAR_0x8006, TYPE_WATER
	setvar VAR_0x8007, TYPE_GRASS
	callnative OrangeGym_ChooseTypeTriad
	waitstate
	goto_if_eq VAR_RESULT, FALSE, OrangeIslands_Trovita_Gym_EventScript_RudyDeclined
	msgbox OrangeIslands_Trovita_Gym_Text_RudyIntro
	trainerbattle_no_intro TRAINER_LEADER_RUDY, OrangeIslands_Trovita_Gym_Text_RudyDefeat
	setflag FLAG_BADGE_SEA_RUBY
	message OrangeIslands_Trovita_Gym_Text_ReceivedBadge
	waitmessage
	playfanfare MUS_OBTAIN_BADGE
	waitfanfare
	msgbox OrangeIslands_Trovita_Gym_Text_ExplainBadge
	goto OrangeIslands_Trovita_Gym_EventScript_GiveTM10
	end

OrangeIslands_Trovita_Gym_EventScript_RudyDeclined::
	msgbox OrangeIslands_Trovita_Gym_Text_RudyDeclined
	release
	end

OrangeIslands_Trovita_Gym_EventScript_RudyAfter::
	goto_if_unset FLAG_GOT_TM10_FROM_RUDY, OrangeIslands_Trovita_Gym_EventScript_GiveTM10
	msgbox OrangeIslands_Trovita_Gym_Text_RudyPostBattle
	release
	end

OrangeIslands_Trovita_Gym_EventScript_GiveTM10::
	checkitemspace ITEM_TM10
	goto_if_eq VAR_RESULT, FALSE, OrangeIslands_Trovita_Gym_EventScript_NoRoom
	giveitem_msg OrangeIslands_Trovita_Gym_Text_ReceivedTM10, ITEM_TM10
	setflag FLAG_GOT_TM10_FROM_RUDY
	msgbox OrangeIslands_Trovita_Gym_Text_ExplainTM10
	release
	end

OrangeIslands_Trovita_Gym_EventScript_NoRoom::
	msgbox OrangeIslands_Trovita_Gym_Text_NoRoom
	release
	end

OrangeIslands_Trovita_Gym_EventScript_Lime::
	trainerbattle_single TRAINER_COOLTRAINER_TROVITA_LIME, OrangeIslands_Trovita_Gym_Text_LimeIntro, OrangeIslands_Trovita_Gym_Text_LimeDefeat
	msgbox OrangeIslands_Trovita_Gym_Text_LimePostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Trovita_Gym_EventScript_Rowan::
	trainerbattle_single TRAINER_COOLTRAINER_TROVITA_ROWAN, OrangeIslands_Trovita_Gym_Text_RowanIntro, OrangeIslands_Trovita_Gym_Text_RowanDefeat
	msgbox OrangeIslands_Trovita_Gym_Text_RowanPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Trovita_Gym_EventScript_GymGuy::
	lock
	faceplayer
	goto_if_set FLAG_BADGE_SEA_RUBY, OrangeIslands_Trovita_Gym_EventScript_GymGuyAfter
	msgbox OrangeIslands_Trovita_Gym_Text_GymGuyAdvice
	release
	end

OrangeIslands_Trovita_Gym_EventScript_GymGuyAfter::
	msgbox OrangeIslands_Trovita_Gym_Text_GymGuyAfter
	release
	end

OrangeIslands_Trovita_Gym_EventScript_GymStatue::
	lockall
	goto_if_set FLAG_BADGE_SEA_RUBY, OrangeIslands_Trovita_Gym_EventScript_GymStatueAfter
	msgbox OrangeIslands_Trovita_Gym_Text_GymStatue
	releaseall
	end

OrangeIslands_Trovita_Gym_EventScript_GymStatueAfter::
	msgbox OrangeIslands_Trovita_Gym_Text_GymStatueAfter
	releaseall
	end

OrangeIslands_Trovita_Gym_Text_RudyChallenge::
	.string "Welcome to TROVITA. I'm RUDY.\p"
	.string "A good trainer balances FIRE,\n"
	.string "WATER and GRASS, each covering\l"
	.string "the others.\p"
	.string "So here, you battle me with one\n"
	.string "of each. Just three POKeMON.\p"
	.string "Shall we?$"

OrangeIslands_Trovita_Gym_Text_ChooseThree::
	.string "Pick one FIRE, one WATER and one\n"
	.string "GRASS POKeMON.$"

OrangeIslands_Trovita_Gym_Text_RudyDeclined::
	.string "Come back once you have a FIRE,\n"
	.string "a WATER and a GRASS POKeMON.$"

OrangeIslands_Trovita_Gym_Text_RudyIntro::
	.string "A fine triad. Let's see how\n"
	.string "well it holds together!$"

OrangeIslands_Trovita_Gym_Text_RudyDefeat::
	.string "Perfectly balanced. You've\n"
	.string "earned this.$"

OrangeIslands_Trovita_Gym_Text_ReceivedBadge::
	.string "{PLAYER} received the\n"
	.string "SEA RUBY BADGE from RUDY!$"

OrangeIslands_Trovita_Gym_Text_ExplainBadge::
	.string "The SEA RUBY BADGE. The ORANGE\n"
	.string "CREW will raise your level cap.\p"
	.string "Please, take this as well.$"

OrangeIslands_Trovita_Gym_Text_ReceivedTM10::
	.string "{PLAYER} received TM10\n"
	.string "from RUDY.$"

OrangeIslands_Trovita_Gym_Text_ExplainTM10::
	.string "TM10 is HIDDEN POWER. Its type\n"
	.string "is different for every POKeMON.\p"
	.string "A good way to cover a gap in\n"
	.string "your triad!$"

OrangeIslands_Trovita_Gym_Text_NoRoom::
	.string "Your BAG is full. Come back\n"
	.string "once you've made room.$"

OrangeIslands_Trovita_Gym_Text_RudyPostBattle::
	.string "The last gym is on KUMQUAT\n"
	.string "ISLAND. Good luck!$"

OrangeIslands_Trovita_Gym_Text_LimeIntro::
	.string "FIRE, WATER, GRASS! Can you\n"
	.string "handle all three?$"

OrangeIslands_Trovita_Gym_Text_LimeDefeat::
	.string "My triad fell apart!$"

OrangeIslands_Trovita_Gym_Text_LimePostBattle::
	.string "RUDY only lets you use three\n"
	.string "POKeMON. Pick them well!$"

OrangeIslands_Trovita_Gym_Text_RowanIntro::
	.string "Show me your balance!$"

OrangeIslands_Trovita_Gym_Text_RowanDefeat::
	.string "Out of balance...$"

OrangeIslands_Trovita_Gym_Text_RowanPostBattle::
	.string "RUDY's team doesn't follow the\n"
	.string "triad. Watch for ELECTRIC and\l"
	.string "PSYCHIC moves!$"

OrangeIslands_Trovita_Gym_Text_GymGuyAdvice::
	.string "Yo! RUDY's rule: you battle him\n"
	.string "with one FIRE, one WATER and one\l"
	.string "GRASS POKeMON, nothing else.\p"
	.string "His own team is a mix, so think\n"
	.string "about which of your three\l"
	.string "covers what!$"

OrangeIslands_Trovita_Gym_Text_GymGuyAfter::
	.string "Three POKeMON against RUDY, and\n"
	.string "you won! Nicely done!$"

OrangeIslands_Trovita_Gym_Text_GymStatue::
	.string "TROVITA ISLAND POKeMON GYM\n"
	.string "LEADER: RUDY\p"
	.string "WINNING TRAINERS:\n"
	.string "{RIVAL}$"

OrangeIslands_Trovita_Gym_Text_GymStatueAfter::
	.string "TROVITA ISLAND POKeMON GYM\n"
	.string "LEADER: RUDY\p"
	.string "WINNING TRAINERS:\n"
	.string "{RIVAL}, {PLAYER}$"
'''

KUMQUAT = r'''.equ LOCALID_LUANA, 1

OrangeIslands_Kumquat_Gym_MapScripts::
	.byte 0

@ Every battle here is a double battle.
OrangeIslands_Kumquat_Gym_EventScript_Luana::
	goto_if_defeated TRAINER_LEADER_LUANA, OrangeIslands_Kumquat_Gym_EventScript_LuanaAfter
	trainerbattle_double TRAINER_LEADER_LUANA, OrangeIslands_Kumquat_Gym_Text_LuanaIntro, OrangeIslands_Kumquat_Gym_Text_LuanaDefeat, OrangeIslands_Kumquat_Gym_Text_LuanaNotEnough, OrangeIslands_Kumquat_Gym_EventScript_LuanaDefeated, NO_MUSIC
	end

OrangeIslands_Kumquat_Gym_EventScript_LuanaDefeated::
	setflag FLAG_BADGE_JADE_STAR
	message OrangeIslands_Kumquat_Gym_Text_ReceivedBadge
	waitmessage
	playfanfare MUS_OBTAIN_BADGE
	waitfanfare
	msgbox OrangeIslands_Kumquat_Gym_Text_ExplainBadge
	goto OrangeIslands_Kumquat_Gym_EventScript_GiveTM33
	end

OrangeIslands_Kumquat_Gym_EventScript_LuanaAfter::
	lock
	faceplayer
	goto_if_unset FLAG_GOT_TM33_FROM_LUANA, OrangeIslands_Kumquat_Gym_EventScript_GiveTM33
	msgbox OrangeIslands_Kumquat_Gym_Text_LuanaPostBattle
	release
	end

OrangeIslands_Kumquat_Gym_EventScript_GiveTM33::
	checkitemspace ITEM_TM33
	goto_if_eq VAR_RESULT, FALSE, OrangeIslands_Kumquat_Gym_EventScript_NoRoom
	giveitem_msg OrangeIslands_Kumquat_Gym_Text_ReceivedTM33, ITEM_TM33
	setflag FLAG_GOT_TM33_FROM_LUANA
	msgbox OrangeIslands_Kumquat_Gym_Text_ExplainTM33
	release
	end

OrangeIslands_Kumquat_Gym_EventScript_NoRoom::
	msgbox OrangeIslands_Kumquat_Gym_Text_NoRoom
	release
	end

OrangeIslands_Kumquat_Gym_EventScript_Ana::
	trainerbattle_double TRAINER_TWINS_KUMQUAT_ANA_BEL, OrangeIslands_Kumquat_Gym_Text_AnaIntro, OrangeIslands_Kumquat_Gym_Text_AnaDefeat, OrangeIslands_Kumquat_Gym_Text_TwinsNotEnough
	msgbox OrangeIslands_Kumquat_Gym_Text_AnaPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Kumquat_Gym_EventScript_Bel::
	trainerbattle_double TRAINER_TWINS_KUMQUAT_ANA_BEL, OrangeIslands_Kumquat_Gym_Text_BelIntro, OrangeIslands_Kumquat_Gym_Text_BelDefeat, OrangeIslands_Kumquat_Gym_Text_TwinsNotEnough
	msgbox OrangeIslands_Kumquat_Gym_Text_BelPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Kumquat_Gym_EventScript_Lex::
	trainerbattle_double TRAINER_COOL_COUPLE_KUMQUAT_LEX_NYA, OrangeIslands_Kumquat_Gym_Text_LexIntro, OrangeIslands_Kumquat_Gym_Text_LexDefeat, OrangeIslands_Kumquat_Gym_Text_CoupleNotEnough
	msgbox OrangeIslands_Kumquat_Gym_Text_LexPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Kumquat_Gym_EventScript_Nya::
	trainerbattle_double TRAINER_COOL_COUPLE_KUMQUAT_LEX_NYA, OrangeIslands_Kumquat_Gym_Text_NyaIntro, OrangeIslands_Kumquat_Gym_Text_NyaDefeat, OrangeIslands_Kumquat_Gym_Text_CoupleNotEnough
	msgbox OrangeIslands_Kumquat_Gym_Text_NyaPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Kumquat_Gym_EventScript_GymGuy::
	lock
	faceplayer
	goto_if_set FLAG_BADGE_JADE_STAR, OrangeIslands_Kumquat_Gym_EventScript_GymGuyAfter
	msgbox OrangeIslands_Kumquat_Gym_Text_GymGuyAdvice
	release
	end

OrangeIslands_Kumquat_Gym_EventScript_GymGuyAfter::
	msgbox OrangeIslands_Kumquat_Gym_Text_GymGuyAfter
	release
	end

OrangeIslands_Kumquat_Gym_Text_LuanaIntro::
	.string "Welcome to KUMQUAT. I'm LUANA.\p"
	.string "Here, POKeMON battle two at a\n"
	.string "time. Partners have to work\l"
	.string "together, not just hit hard.\p"
	.string "Show me your best pair!$"

OrangeIslands_Kumquat_Gym_Text_LuanaDefeat::
	.string "Wonderful teamwork. You've\n"
	.string "earned the last badge.$"

OrangeIslands_Kumquat_Gym_Text_LuanaNotEnough::
	.string "Oh, you need at least two\n"
	.string "POKeMON for a battle here.$"

OrangeIslands_Kumquat_Gym_Text_ReceivedBadge::
	.string "{PLAYER} received the\n"
	.string "JADE STAR BADGE from LUANA!$"

OrangeIslands_Kumquat_Gym_Text_ExplainBadge::
	.string "With four badges, you can take a\n"
	.string "ferry to PUMMELO ISLAND and face\l"
	.string "the head of the ORANGE CREW.\p"
	.string "Here, this will help your pairs.$"

OrangeIslands_Kumquat_Gym_Text_ReceivedTM33::
	.string "{PLAYER} received TM33\n"
	.string "from LUANA.$"

OrangeIslands_Kumquat_Gym_Text_ExplainTM33::
	.string "TM33 is REFLECT. It halves\n"
	.string "physical damage to your side\l"
	.string "for five turns.$"

OrangeIslands_Kumquat_Gym_Text_NoRoom::
	.string "Your BAG is full. Come back\n"
	.string "once you've made room.$"

OrangeIslands_Kumquat_Gym_Text_LuanaPostBattle::
	.string "DRAKE is waiting on PUMMELO\n"
	.string "ISLAND. Give him my best!$"

OrangeIslands_Kumquat_Gym_Text_AnaIntro::
	.string "ANA: Two of us...$"

OrangeIslands_Kumquat_Gym_Text_BelIntro::
	.string "BEL: ...against two of yours!$"

OrangeIslands_Kumquat_Gym_Text_AnaDefeat::
	.string "ANA: We lost together!$"

OrangeIslands_Kumquat_Gym_Text_BelDefeat::
	.string "BEL: Together, we lost!$"

OrangeIslands_Kumquat_Gym_Text_TwinsNotEnough::
	.string "We battle two at a time! Come\n"
	.string "back with more POKeMON!$"

OrangeIslands_Kumquat_Gym_Text_AnaPostBattle::
	.string "ANA: LUANA's partners never\n"
	.string "get in each other's way.$"

OrangeIslands_Kumquat_Gym_Text_BelPostBattle::
	.string "BEL: Moves that hit both foes\n"
	.string "are great here!$"

OrangeIslands_Kumquat_Gym_Text_LexIntro::
	.string "LEX: We've trained as a pair\n"
	.string "for years!$"

OrangeIslands_Kumquat_Gym_Text_NyaIntro::
	.string "NYA: Ready, partner?$"

OrangeIslands_Kumquat_Gym_Text_LexDefeat::
	.string "LEX: Out of sync...$"

OrangeIslands_Kumquat_Gym_Text_NyaDefeat::
	.string "NYA: We'll practice more.$"

OrangeIslands_Kumquat_Gym_Text_CoupleNotEnough::
	.string "A double battle needs two\n"
	.string "POKeMON on your side!$"

OrangeIslands_Kumquat_Gym_Text_LexPostBattle::
	.string "LEX: LUANA's ALAKAZAM can put\n"
	.string "up REFLECT for her whole team.$"

OrangeIslands_Kumquat_Gym_Text_NyaPostBattle::
	.string "NYA: Watch out for her\n"
	.string "MAROWAK. It hits hard!$"

OrangeIslands_Kumquat_Gym_Text_GymGuyAdvice::
	.string "Yo! Every battle in LUANA's gym\n"
	.string "is a double battle.\p"
	.string "Her STEELIX and MAROWAK hit\n"
	.string "hard, and ALAKAZAM backs them\l"
	.string "up. Bring partners that cover\l"
	.string "each other!$"

OrangeIslands_Kumquat_Gym_Text_GymGuyAfter::
	.string "Four badges! You're ready for\n"
	.string "PUMMELO ISLAND!$"
'''
for name, text in [('OrangeIslands_Navel_Gym', NAVEL), ('OrangeIslands_Trovita_Gym', TROVITA), ('OrangeIslands_Kumquat_Gym', KUMQUAT)]:
    p = 'data/maps/%s/scripts.inc' % name
    if not os.path.exists(p):
        wr(p, text.replace('POKeMON', 'POKéMON'))
p = 'data/event_scripts.s'; t = rd(p)
for name in ('OrangeIslands_Navel_Gym', 'OrangeIslands_Trovita_Gym', 'OrangeIslands_Kumquat_Gym'):
    line = '\t.include "data/maps/%s/scripts.inc"\n' % name
    if line not in t:
        anchor = '\t.include "data/maps/OrangeIslands_Mikan_Gym/scripts.inc"\n'
        t = sub1(t, anchor, anchor + line, name)
wr(p, t)
print('orange gyms ok')
