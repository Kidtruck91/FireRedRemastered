# Run from the repo root, after orange_leaders.py, orange_leader_levels.py and orange_npc_gfx.py.
#  - Prof. Ivy: arrival scene on Valencia's dock, lab interior (Oak's lab layout) behind the
#    south-beach door, team sent to the PC (Orange challenge starts), pick 1 of 3 starters.
#  - Mikan Gym: gym building stamped onto Mikan, interior on the Cerulean Gym layout, two gym
#    trainers, Cissy in a rain arena, Coral-Eye Badge + TM18 Rain Dance.
import json, re, struct, os

def rd(p): return open(p, encoding='utf-8').read()
def wr(p, s): open(p, 'w', newline='\n', encoding='utf-8').write(s)

# ---------------------------------------------------------------- flags / vars
p = 'include/constants/flags.h'; t = rd(p)
for old, new in [
    ('#define FLAG_0x206               0x206', '#define FLAG_HIDE_IVY_AT_DOCK        0x206 // Valencia dock: Ivy gone after the arrival scene'),
    ('#define FLAG_0x207               0x207', '#define FLAG_HIDE_IVY_STARTER_BALLS  0x207 // Ivy\'s lab: starter picked'),
    ('#define FLAG_0x208               0x208', '#define FLAG_BADGE_CORAL_EYE         0x208 // Orange League badge 1 (Cissy, Mikan)'),
    ('#define FLAG_0x209               0x209', '#define FLAG_GOT_TM18_FROM_CISSY     0x209'),
]:
    if new.split()[1] not in t:
        assert old in t, old
        t = t.replace(old, new, 1)
wr(p, t)
p = 'include/constants/vars.h'; t = rd(p)
if 'VAR_ORANGE_STORY' not in t:
    old = '#define VAR_0x4093                 0x4093'
    assert old in t
    t = t.replace(old, '#define VAR_ORANGE_STORY           0x4093 // 0 not arrived, 1 to the lab, 2 pick a starter, 3 done, 5 PC full', 1)
wr(p, t)

# ---------------------------------------------------------------- gym trainers
NEW = [
 ('TRAINER_SWIMMER_M_MARCO', 'TRAINER_CLASS_SWIMMER_M', 'TRAINER_PIC_SWIMMER_M', 'MARCO', 'TRAINER_ENCOUNTER_MUSIC_SWIMMER',
  'sParty_SwimmerMMarco', [(15,'POLIWAG',['HYPNOSIS','WATER_GUN','DOUBLE_TEAM','BUBBLE'],1),
                           (16,'GOLDEEN',['RAIN_DANCE','WATER_PULSE','SUPERSONIC','HORN_ATTACK'],1)]),
 ('TRAINER_SWIMMER_F_CORAL', 'TRAINER_CLASS_SWIMMER_F', 'TRAINER_PIC_SWIMMER_F', 'CORAL', 'TRAINER_ENCOUNTER_MUSIC_SWIMMER | F_TRAINER_FEMALE',
  'sParty_SwimmerFCoral', [(16,'SHELLDER',['ICICLE_SPEAR','SUPERSONIC','WITHDRAW','DOUBLE_TEAM'],1),
                           (17,'SEEL',['ICY_WIND','HEADBUTT','ENCORE','RAIN_DANCE'],1),
                           (17,'TENTACOOL',['POISON_STING','SUPERSONIC','CONSTRICT','WATER_PULSE'],1)]),
]
p = 'include/constants/opponents.h'; t = rd(p)
if 'TRAINER_SWIMMER_M_MARCO' not in t:
    n = int(re.search(r'#define NUM_TRAINERS\s+(\d+)', t).group(1))
    old = re.search(r'#define TRAINER_CHAMPION_DRAKE\s+\d+\n', t).group(0)
    t = t.replace(old, old + ''.join('#define %-40s %d\n' % (c[0], n + i) for i, c in enumerate(NEW)), 1)
    t = re.sub(r'#define NUM_TRAINERS\s+\d+', '#define NUM_TRAINERS                             %d' % (n + len(NEW)), t)
    wr(p, t)
p = 'src/data/trainer_parties.h'; t = rd(p)
if 'sParty_SwimmerMMarco' not in t:
    for c, cls, pic, name, mus, sym, party in NEW:
        t = t.rstrip('\n') + '\n\nstatic const struct TrainerMonItemCustomMoves %s[] = {\n' % sym
        for lvl, sp, mv, ab in party:
            t += ('    {\n        .iv = 100,\n        .lvl = %d,\n        .ability = %d,\n        .species = SPECIES_%s,\n'
                  '        .heldItem = ITEM_NONE,\n        .moves = {%s},\n    },\n') % (lvl, ab, sp, ', '.join('MOVE_' + m for m in mv))
        t += '};\n'
    wr(p, t)
p = 'src/data/trainers.h'; t = rd(p)
if '[TRAINER_SWIMMER_M_MARCO]' not in t:
    ent = ''
    for c, cls, pic, name, mus, sym, party in NEW:
        ent += ('    [%s] = {\n        .trainerClass = %s,\n        .encounterMusic_gender = %s,\n        .trainerPic = %s,\n'
                '        .trainerName = _("%s"),\n        .items = {},\n        .doubleBattle = FALSE,\n'
                '        .aiFlags = AI_LEVEL_ADVANCED,\n        .party = ITEM_CUSTOM_MOVES(%s),\n    },\n') % (c, cls, mus, pic, name, sym)
    i = t.index('    [TRAINER_CHAMPION_DRAKE] = {'); j = t.index('    },\n', i) + len('    },\n')
    t = t[:i] + t[i:j] + ent + t[j:]
    wr(p, t)

# ---------------------------------------------------------------- maps
G = 'data/maps/map_groups.json'; g = json.loads(rd(G))
for name in ('OrangeIslands_Valencia_IvysLab', 'OrangeIslands_Mikan_Gym'):
    if name not in g['gMapGroup_OrangeIslands']:
        g['gMapGroup_OrangeIslands'].append(name)
wr(G, json.dumps(g, indent=2) + '\n')

def objev(gfx, x, y, script, flag='0', mt='MOVEMENT_TYPE_FACE_DOWN', tt='TRAINER_TYPE_NONE', sight='0', elev=3):
    return {"type": "object", "graphics_id": gfx, "x": x, "y": y, "elevation": elev, "movement_type": mt,
            "movement_range_x": 1, "movement_range_y": 1, "trainer_type": tt, "trainer_sight_or_berry_tree_id": sight,
            "script": script, "flag": flag}

val = json.loads(rd('data/maps/OrangeIslands_Valencia/map.json'))
mik = json.loads(rd('data/maps/OrangeIslands_Mikan/map.json'))

lab = {"id": "MAP_ORANGE_ISLANDS_VALENCIA_IVYS_LAB", "name": "OrangeIslands_Valencia_IvysLab",
       "layout": "LAYOUT_PALLET_TOWN_PROFESSOR_OAKS_LAB", "music": "MUS_OAK_LAB",
       "region_map_section": val["region_map_section"], "requires_flash": False, "weather": "WEATHER_NONE",
       "map_type": "MAP_TYPE_INDOOR", "allow_cycling": False, "allow_escaping": False, "allow_running": False,
       "show_map_name": False, "floor_number": 0, "battle_scene": "MAP_BATTLE_SCENE_NORMAL", "connections": None,
       "object_events": [
           objev("OBJ_EVENT_GFX_IVY", 7, 4, "OrangeIslands_Valencia_IvysLab_EventScript_Ivy"),
           objev("OBJ_EVENT_GFX_ITEM_BALL", 8, 4, "OrangeIslands_Valencia_IvysLab_EventScript_OddishBall", "FLAG_HIDE_IVY_STARTER_BALLS"),
           objev("OBJ_EVENT_GFX_ITEM_BALL", 9, 4, "OrangeIslands_Valencia_IvysLab_EventScript_VulpixBall", "FLAG_HIDE_IVY_STARTER_BALLS"),
           objev("OBJ_EVENT_GFX_ITEM_BALL", 10, 4, "OrangeIslands_Valencia_IvysLab_EventScript_MarillBall", "FLAG_HIDE_IVY_STARTER_BALLS"),
           objev("OBJ_EVENT_GFX_SCIENTIST", 3, 11, "OrangeIslands_Valencia_IvysLab_EventScript_Aide", mt="MOVEMENT_TYPE_LOOK_AROUND"),
       ],
       "warp_events": [{"x": x, "y": 12, "elevation": 3, "dest_map": "MAP_ORANGE_ISLANDS_VALENCIA", "dest_warp_id": "0"} for x in (6, 7, 5)],
       "coord_events": [{"type": "trigger", "x": x, "y": 11, "elevation": 3, "var": "VAR_ORANGE_STORY", "var_value": "2",
                         "script": "OrangeIslands_Valencia_IvysLab_EventScript_DontLeaveYet"} for x in (5, 6, 7)],
       "bg_events": [{"type": "sign", "x": x, "y": 1, "elevation": 0, "player_facing_dir": "BG_EVENT_PLAYER_FACING_NORTH",
                      "script": "OrangeIslands_Valencia_IvysLab_EventScript_Notes"} for x in (2, 3)]}

gym = {"id": "MAP_ORANGE_ISLANDS_MIKAN_GYM", "name": "OrangeIslands_Mikan_Gym",
       "layout": "LAYOUT_CERULEAN_CITY_GYM", "music": "MUS_GYM",
       "region_map_section": mik["region_map_section"], "requires_flash": False, "weather": "WEATHER_NONE",
       "map_type": "MAP_TYPE_INDOOR", "allow_cycling": False, "allow_escaping": False, "allow_running": False,
       "show_map_name": False, "floor_number": 0, "battle_scene": "MAP_BATTLE_SCENE_GYM", "connections": None,
       "object_events": [
           objev("OBJ_EVENT_GFX_SWIMMER_M_WATER", 10, 12, "OrangeIslands_Mikan_Gym_EventScript_Marco",
                 mt="MOVEMENT_TYPE_FACE_LEFT", tt="TRAINER_TYPE_NORMAL", sight="1", elev=0),
           objev("OBJ_EVENT_GFX_SWIMMER_F_LAND", 4, 7, "OrangeIslands_Mikan_Gym_EventScript_Coral",
                 mt="MOVEMENT_TYPE_FACE_RIGHT", tt="TRAINER_TYPE_NORMAL", sight="3", elev=0),
           objev("OBJ_EVENT_GFX_CISSY", 8, 6, "OrangeIslands_Mikan_Gym_EventScript_Cissy", elev=0),
           objev("OBJ_EVENT_GFX_GYM_GUY", 7, 16, "OrangeIslands_Mikan_Gym_EventScript_GymGuy", elev=0),
       ],
       "warp_events": [{"x": x, "y": 18, "elevation": 0, "dest_map": "MAP_ORANGE_ISLANDS_MIKAN", "dest_warp_id": "0"} for x in (7, 8, 9)],
       "coord_events": [],
       "bg_events": [{"type": "sign", "x": x, "y": 17, "elevation": 0, "player_facing_dir": "BG_EVENT_PLAYER_FACING_ANY",
                      "script": "OrangeIslands_Mikan_Gym_EventScript_GymStatue"} for x in (6, 10)]}

for m in (lab, gym):
    os.makedirs('data/maps/' + m['name'], exist_ok=True)
    wr('data/maps/%s/map.json' % m['name'], json.dumps(m, indent=2) + '\n')

# Valencia: Ivy on the dock, lab door warp
if not any(o['script'] == 'OrangeIslands_Valencia_EventScript_IvyDock' for o in val['object_events']):
    val['object_events'].append(objev("OBJ_EVENT_GFX_IVY", 22, 30, "OrangeIslands_Valencia_EventScript_IvyDock",
                                      "FLAG_HIDE_IVY_AT_DOCK", mt="MOVEMENT_TYPE_FACE_LEFT"))
if not any(w['dest_map'] == 'MAP_ORANGE_ISLANDS_VALENCIA_IVYS_LAB' for w in val['warp_events']):
    assert not val['warp_events'], 'lab warp must be Valencia warp 0'
    val['warp_events'].append({"x": 45, "y": 51, "elevation": 3, "dest_map": "MAP_ORANGE_ISLANDS_VALENCIA_IVYS_LAB", "dest_warp_id": "0"})
wr('data/maps/OrangeIslands_Valencia/map.json', json.dumps(val, indent=2) + '\n')
IVY_LOCALID = 1 + [o['script'] for o in val['object_events']].index('OrangeIslands_Valencia_EventScript_IvyDock')

# Mikan: gym building (copied from Navel's summit gym) + door warp
GX, GY = 22, 35
if not any(w['dest_map'] == 'MAP_ORANGE_ISLANDS_MIKAN_GYM' for w in mik['warp_events']):
    assert not mik['warp_events'], 'gym warp must be Mikan warp 0'
    mik['warp_events'].append({"x": GX + 3, "y": GY + 4, "elevation": 3, "dest_map": "MAP_ORANGE_ISLANDS_MIKAN_GYM", "dest_warp_id": "0"})
    wr('data/maps/OrangeIslands_Mikan/map.json', json.dumps(mik, indent=2) + '\n')
L = {l['id']: l for l in json.loads(rd('data/layouts/layouts.json'))['layouts'] if 'id' in l}
nav = L['LAYOUT_ORANGE_ISLANDS_NAVEL']; nd = open(nav['blockdata_filepath'], 'rb').read()
ml = L['LAYOUT_ORANGE_ISLANDS_MIKAN']; md = bytearray(open(ml['blockdata_filepath'], 'rb').read())
for dy in range(5):
    for dx in range(7):
        v = struct.unpack_from('<H', nd, ((9 + dy) * nav['width'] + 34 + dx) * 2)[0]
        struct.pack_into('<H', md, ((GY + dy) * ml['width'] + GX + dx) * 2, v)
door = struct.unpack_from('<H', md, ((GY + 4) * ml['width'] + GX + 3) * 2)[0] & 0x3ff
assert door == 0x15b, hex(door)
open(ml['blockdata_filepath'], 'wb').write(bytes(md))

# ---------------------------------------------------------------- scripts
VAL_SCRIPTS = r'''
@ ---------------------------------------------------------------- Prof. Ivy arrival
OrangeIslands_Valencia_OnFrame::
	map_script_2 VAR_ORANGE_STORY, 0, OrangeIslands_Valencia_EventScript_IvyArrival
	.2byte 0

OrangeIslands_Valencia_EventScript_IvyArrival::
	lockall
	textcolor NPC_TEXT_COLOR_FEMALE
	getplayerxy VAR_0x8004, VAR_0x8005
	goto_if_ne VAR_0x8004, 19, OrangeIslands_Valencia_EventScript_IvyArrivalTalk
	goto_if_ne VAR_0x8005, 30, OrangeIslands_Valencia_EventScript_IvyArrivalTalk
	playse SE_PIN
	applymovement LOCALID_IVY_DOCK, Common_Movement_ExclamationMark
	waitmovement 0
	applymovement LOCALID_IVY_DOCK, OrangeIslands_Valencia_Movement_IvyToPlayer
	waitmovement 0
	applymovement OBJ_EVENT_ID_PLAYER, Common_Movement_WalkInPlaceFasterRight
	waitmovement 0
OrangeIslands_Valencia_EventScript_IvyArrivalTalk::
	msgbox OrangeIslands_Valencia_Text_IvyWelcome
	closemessage
	fadescreen FADE_TO_BLACK
	setflag FLAG_HIDE_IVY_AT_DOCK
	removeobject LOCALID_IVY_DOCK
	setvar VAR_ORANGE_STORY, 1
	warp MAP_ORANGE_ISLANDS_VALENCIA_IVYS_LAB, 7, 7
	waitstate
	releaseall
	end

OrangeIslands_Valencia_EventScript_IvyDock::
	end

OrangeIslands_Valencia_Movement_IvyToPlayer:
	walk_left
	walk_left
	step_end

OrangeIslands_Valencia_Text_IvyWelcome::
	.string "Oh! You must be {PLAYER}!\n"
	.string "PROF. OAK said you'd be coming.\p"
	.string "I'm PROF. IVY. Welcome to the\n"
	.string "ORANGE ISLANDS!\p"
	.string "You're here for the ORANGE LEAGUE,\n"
	.string "aren't you?\p"
	.string "There's something you need to know\n"
	.string "first. Come on, my lab is just\l"
	.string "down the beach!$"
'''
p = 'data/maps/OrangeIslands_Valencia/scripts.inc'; t = rd(p)
if 'OrangeIslands_Valencia_EventScript_IvyArrival' not in t:
    old = 'OrangeIslands_Valencia_MapScripts::\n\t.byte 0\n'
    assert old in t
    t = ('.equ LOCALID_IVY_DOCK, %d\n\n' % IVY_LOCALID) + t.replace(old,
        'OrangeIslands_Valencia_MapScripts::\n\tmap_script MAP_SCRIPT_ON_FRAME_TABLE, OrangeIslands_Valencia_OnFrame\n\t.byte 0\n', 1)
    t = t.rstrip('\n') + '\n' + VAL_SCRIPTS
    wr(p, t)

LAB = r'''.equ LOCALID_IVY,          1
.equ LOCALID_ODDISH_BALL,  2
.equ LOCALID_VULPIX_BALL,  3
.equ LOCALID_MARILL_BALL,  4

.equ STARTER_SPECIES, VAR_TEMP_1

OrangeIslands_Valencia_IvysLab_MapScripts::
	map_script MAP_SCRIPT_ON_FRAME_TABLE, OrangeIslands_Valencia_IvysLab_OnFrame
	.byte 0

OrangeIslands_Valencia_IvysLab_OnFrame::
	map_script_2 VAR_ORANGE_STORY, 1, OrangeIslands_Valencia_IvysLab_EventScript_Intro
	.2byte 0

@ ---------------------------------------------------------------- the challenge
OrangeIslands_Valencia_IvysLab_EventScript_Intro::
	lockall
	textcolor NPC_TEXT_COLOR_FEMALE
	applymovement OBJ_EVENT_ID_PLAYER, OrangeIslands_Valencia_IvysLab_Movement_PlayerToIvy
	waitmovement 0
	msgbox OrangeIslands_Valencia_IvysLab_Text_Intro
	goto OrangeIslands_Valencia_IvysLab_EventScript_StoreTeam
	end

OrangeIslands_Valencia_IvysLab_EventScript_StoreTeam::
	msgbox OrangeIslands_Valencia_IvysLab_Text_ReadyToStore
	callnative OrangeChallenge_Begin
	goto_if_eq VAR_RESULT, FALSE, OrangeIslands_Valencia_IvysLab_EventScript_NoRoom
	playse SE_PC_LOGIN
	message OrangeIslands_Valencia_IvysLab_Text_TeamStored
	waitmessage
	waitse
	playfanfare MUS_OBTAIN_ITEM
	waitfanfare
	msgbox OrangeIslands_Valencia_IvysLab_Text_PickOne
	setvar VAR_ORANGE_STORY, 2
	releaseall
	end

OrangeIslands_Valencia_IvysLab_EventScript_NoRoom::
	msgbox OrangeIslands_Valencia_IvysLab_Text_NoRoom
	setvar VAR_ORANGE_STORY, 5
	releaseall
	end

OrangeIslands_Valencia_IvysLab_EventScript_Ivy::
	lock
	faceplayer
	textcolor NPC_TEXT_COLOR_FEMALE
	goto_if_eq VAR_ORANGE_STORY, 5, OrangeIslands_Valencia_IvysLab_EventScript_StoreTeam
	goto_if_eq VAR_ORANGE_STORY, 2, OrangeIslands_Valencia_IvysLab_EventScript_IvyPickOne
	goto_if_set FLAG_BADGE_CORAL_EYE, OrangeIslands_Valencia_IvysLab_EventScript_IvyAfterBadge
	msgbox OrangeIslands_Valencia_IvysLab_Text_IvyTips
	release
	end

OrangeIslands_Valencia_IvysLab_EventScript_IvyPickOne::
	msgbox OrangeIslands_Valencia_IvysLab_Text_PickOne
	release
	end

OrangeIslands_Valencia_IvysLab_EventScript_IvyAfterBadge::
	msgbox OrangeIslands_Valencia_IvysLab_Text_IvyAfterBadge
	release
	end

OrangeIslands_Valencia_IvysLab_EventScript_DontLeaveYet::
	lockall
	textcolor NPC_TEXT_COLOR_FEMALE
	msgbox OrangeIslands_Valencia_IvysLab_Text_DontLeaveYet
	closemessage
	applymovement OBJ_EVENT_ID_PLAYER, Common_Movement_WalkUp
	waitmovement 0
	releaseall
	end

@ ---------------------------------------------------------------- starters
OrangeIslands_Valencia_IvysLab_EventScript_OddishBall::
	setvar STARTER_SPECIES, SPECIES_ODDISH
	goto OrangeIslands_Valencia_IvysLab_EventScript_Ball
	end

OrangeIslands_Valencia_IvysLab_EventScript_VulpixBall::
	setvar STARTER_SPECIES, SPECIES_VULPIX
	goto OrangeIslands_Valencia_IvysLab_EventScript_Ball
	end

OrangeIslands_Valencia_IvysLab_EventScript_MarillBall::
	setvar STARTER_SPECIES, SPECIES_MARILL
	goto OrangeIslands_Valencia_IvysLab_EventScript_Ball
	end

OrangeIslands_Valencia_IvysLab_EventScript_Ball::
	lock
	textcolor NPC_TEXT_COLOR_FEMALE
	goto_if_ne VAR_ORANGE_STORY, 2, OrangeIslands_Valencia_IvysLab_EventScript_NotYours
	showmonpic STARTER_SPECIES, 10, 3
	bufferspeciesname STR_VAR_1, STARTER_SPECIES
	msgbox OrangeIslands_Valencia_IvysLab_Text_ThisOne, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, OrangeIslands_Valencia_IvysLab_EventScript_Declined
	hidemonpic
	givemon STARTER_SPECIES, 10
	callnative UpdateFollowerPokemonGraphic
	setflag FLAG_HIDE_IVY_STARTER_BALLS
	removeobject LOCALID_ODDISH_BALL
	removeobject LOCALID_VULPIX_BALL
	removeobject LOCALID_MARILL_BALL
	bufferspeciesname STR_VAR_1, STARTER_SPECIES
	message OrangeIslands_Valencia_IvysLab_Text_Received
	waitmessage
	playfanfare MUS_OBTAIN_KEY_ITEM
	waitfanfare
	msgbox Text_GiveNicknameToThisMon, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, OrangeIslands_Valencia_IvysLab_EventScript_AfterStarter
	setvar VAR_0x8004, 0
	call EventScript_ChangePokemonNickname
	goto OrangeIslands_Valencia_IvysLab_EventScript_AfterStarter
	end

OrangeIslands_Valencia_IvysLab_EventScript_AfterStarter::
	setvar VAR_ORANGE_STORY, 3
	textcolor NPC_TEXT_COLOR_FEMALE
	msgbox OrangeIslands_Valencia_IvysLab_Text_TakeBalls
	giveitem ITEM_POKE_BALL, 5
	textcolor NPC_TEXT_COLOR_FEMALE
	msgbox OrangeIslands_Valencia_IvysLab_Text_GoToMikan
	release
	end

OrangeIslands_Valencia_IvysLab_EventScript_Declined::
	hidemonpic
	release
	end

OrangeIslands_Valencia_IvysLab_EventScript_NotYours::
	msgbox OrangeIslands_Valencia_IvysLab_Text_Studying
	release
	end

OrangeIslands_Valencia_IvysLab_EventScript_Aide::
	msgbox OrangeIslands_Valencia_IvysLab_Text_Aide, MSGBOX_NPC
	end

OrangeIslands_Valencia_IvysLab_EventScript_Notes::
	msgbox OrangeIslands_Valencia_IvysLab_Text_Notes, MSGBOX_SIGN
	end

OrangeIslands_Valencia_IvysLab_Movement_PlayerToIvy:
	walk_up
	walk_up
	step_end

@ ---------------------------------------------------------------- text
OrangeIslands_Valencia_IvysLab_Text_Intro::
	.string "This is my lab. I study how\n"
	.string "POKéMON here differ from the ones\l"
	.string "on the mainland.\p"
	.string "Now, about the ORANGE LEAGUE.\n"
	.string "The ORANGE CREW has one rule for\l"
	.string "visiting trainers: you start fresh.\p"
	.string "The team you brought rests in the\n"
	.string "PC, and the PC stays locked until\l"
	.string "you leave the islands.\p"
	.string "You'll raise a new team from the\n"
	.string "POKéMON you meet here.\p"
	.string "And the CREW caps your POKéMON's\n"
	.string "levels. Each badge you win raises\l"
	.string "the cap a little.$"

OrangeIslands_Valencia_IvysLab_Text_ReadyToStore::
	.string "I'll send your team to the PC now.\n"
	.string "Don't worry, they'll be safe!$"

OrangeIslands_Valencia_IvysLab_Text_TeamStored::
	.string "{PLAYER}'s POKéMON were sent\n"
	.string "to the PC!$"

OrangeIslands_Valencia_IvysLab_Text_NoRoom::
	.string "Oh dear, your PC boxes are too full\n"
	.string "to take your whole team.\p"
	.string "Make some room at the POKéMON\n"
	.string "CENTER, then come talk to me.$"

OrangeIslands_Valencia_IvysLab_Text_PickOne::
	.string "You can't go out with no POKéMON!\n"
	.string "Pick one of the three on the table.\p"
	.string "They were all raised right here\n"
	.string "on VALENCIA.$"

OrangeIslands_Valencia_IvysLab_Text_ThisOne::
	.string "So you'd like {STR_VAR_1}?$"

OrangeIslands_Valencia_IvysLab_Text_Received::
	.string "{PLAYER} received {STR_VAR_1}\n"
	.string "from PROF. IVY!$"

OrangeIslands_Valencia_IvysLab_Text_TakeBalls::
	.string "Good choice! You'll want more\n"
	.string "teammates, so take these too.$"

OrangeIslands_Valencia_IvysLab_Text_GoToMikan::
	.string "The first gym is on MIKAN ISLAND,\n"
	.string "east past VALENCIA BEACH.\p"
	.string "The leader there, CISSY, battles\n"
	.string "in the rain. Be ready for it!$"

OrangeIslands_Valencia_IvysLab_Text_DontLeaveYet::
	.string "PROF. IVY: Wait, {PLAYER}!\n"
	.string "Pick a POKéMON before you go!$"

OrangeIslands_Valencia_IvysLab_Text_Studying::
	.string "Those are POKéMON I'm studying.$"

OrangeIslands_Valencia_IvysLab_Text_IvyTips::
	.string "MIKAN ISLAND is east, past\n"
	.string "VALENCIA BEACH.\p"
	.string "CISSY loves the rain. Watch out\n"
	.string "for THUNDER, and for POKéMON that\l"
	.string "are hard to hit!$"

OrangeIslands_Valencia_IvysLab_Text_IvyAfterBadge::
	.string "The CORAL-EYE BADGE! Nicely done.\n"
	.string "The ORANGE CREW will raise your\l"
	.string "level cap now.$"

OrangeIslands_Valencia_IvysLab_Text_Aide::
	.string "PROF. IVY found that POKéMON here\n"
	.string "can look different from the ones\l"
	.string "in KANTO. Fascinating, right?$"

OrangeIslands_Valencia_IvysLab_Text_Notes::
	.string "It's a research report on the\n"
	.string "POKéMON of the ORANGE ISLANDS.$"
'''
p = 'data/maps/OrangeIslands_Valencia_IvysLab/scripts.inc'
if not os.path.exists(p): wr(p, LAB)

GYM = r'''.equ LOCALID_CISSY, 3

OrangeIslands_Mikan_Gym_MapScripts::
	.byte 0

OrangeIslands_Mikan_Gym_EventScript_Cissy::
	goto_if_defeated TRAINER_LEADER_CISSY, OrangeIslands_Mikan_Gym_EventScript_CissyAfter
	setweather WEATHER_RAIN
	doweather
	trainerbattle_single TRAINER_LEADER_CISSY, OrangeIslands_Mikan_Gym_Text_CissyIntro, OrangeIslands_Mikan_Gym_Text_CissyDefeat, OrangeIslands_Mikan_Gym_EventScript_CissyDefeated, NO_MUSIC
	end

OrangeIslands_Mikan_Gym_EventScript_CissyAfter::
	lock
	faceplayer
	goto_if_unset FLAG_GOT_TM18_FROM_CISSY, OrangeIslands_Mikan_Gym_EventScript_GiveTM18
	msgbox OrangeIslands_Mikan_Gym_Text_CissyPostBattle
	release
	end

OrangeIslands_Mikan_Gym_EventScript_CissyDefeated::
	resetweather
	doweather
	setflag FLAG_BADGE_CORAL_EYE
	message OrangeIslands_Mikan_Gym_Text_ReceivedBadge
	waitmessage
	playfanfare MUS_OBTAIN_BADGE
	waitfanfare
	msgbox OrangeIslands_Mikan_Gym_Text_ExplainBadge
	goto OrangeIslands_Mikan_Gym_EventScript_GiveTM18
	end

OrangeIslands_Mikan_Gym_EventScript_GiveTM18::
	checkitemspace ITEM_TM18
	goto_if_eq VAR_RESULT, FALSE, OrangeIslands_Mikan_Gym_EventScript_NoRoomForTM18
	giveitem_msg OrangeIslands_Mikan_Gym_Text_ReceivedTM18, ITEM_TM18
	setflag FLAG_GOT_TM18_FROM_CISSY
	msgbox OrangeIslands_Mikan_Gym_Text_ExplainTM18
	release
	end

OrangeIslands_Mikan_Gym_EventScript_NoRoomForTM18::
	msgbox OrangeIslands_Mikan_Gym_Text_NoRoom
	release
	end

OrangeIslands_Mikan_Gym_EventScript_Marco::
	trainerbattle_single TRAINER_SWIMMER_M_MARCO, OrangeIslands_Mikan_Gym_Text_MarcoIntro, OrangeIslands_Mikan_Gym_Text_MarcoDefeat
	msgbox OrangeIslands_Mikan_Gym_Text_MarcoPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Mikan_Gym_EventScript_Coral::
	trainerbattle_single TRAINER_SWIMMER_F_CORAL, OrangeIslands_Mikan_Gym_Text_CoralIntro, OrangeIslands_Mikan_Gym_Text_CoralDefeat
	msgbox OrangeIslands_Mikan_Gym_Text_CoralPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Mikan_Gym_EventScript_GymGuy::
	lock
	faceplayer
	goto_if_set FLAG_BADGE_CORAL_EYE, OrangeIslands_Mikan_Gym_EventScript_GymGuyAfter
	msgbox OrangeIslands_Mikan_Gym_Text_GymGuyAdvice
	release
	end

OrangeIslands_Mikan_Gym_EventScript_GymGuyAfter::
	msgbox OrangeIslands_Mikan_Gym_Text_GymGuyAfter
	release
	end

OrangeIslands_Mikan_Gym_EventScript_GymStatue::
	lockall
	goto_if_set FLAG_BADGE_CORAL_EYE, OrangeIslands_Mikan_Gym_EventScript_GymStatueAfter
	msgbox OrangeIslands_Mikan_Gym_Text_GymStatue
	releaseall
	end

OrangeIslands_Mikan_Gym_EventScript_GymStatueAfter::
	msgbox OrangeIslands_Mikan_Gym_Text_GymStatueAfter
	releaseall
	end

@ ---------------------------------------------------------------- text
OrangeIslands_Mikan_Gym_Text_CissyIntro::
	.string "So you're taking on the ORANGE\n"
	.string "LEAGUE? I'm CISSY, MIKAN's LEADER.\p"
	.string "Out here, we battle with the sea\n"
	.string "and the sky on our side.\p"
	.string "Let's make it rain!$"

OrangeIslands_Mikan_Gym_Text_CissyDefeat::
	.string "I couldn't land a hit on you\n"
	.string "when it counted!$"

OrangeIslands_Mikan_Gym_Text_ReceivedBadge::
	.string "{PLAYER} received the CORAL-EYE\n"
	.string "BADGE from CISSY!$"

OrangeIslands_Mikan_Gym_Text_ExplainBadge::
	.string "That's proof you beat me. The\n"
	.string "ORANGE CREW will raise your level\l"
	.string "cap now.\p"
	.string "Take this too. It's what makes\n"
	.string "my battles so slippery!$"

OrangeIslands_Mikan_Gym_Text_ReceivedTM18::
	.string "{PLAYER} received TM18\n"
	.string "from CISSY.$"

OrangeIslands_Mikan_Gym_Text_ExplainTM18::
	.string "TM18 is RAIN DANCE. It makes it\n"
	.string "rain for five turns.\p"
	.string "WATER moves hit harder, FIRE moves\n"
	.string "fizzle, and THUNDER never misses!$"

OrangeIslands_Mikan_Gym_Text_NoRoom::
	.string "Your BAG is full. Come back for\n"
	.string "this when you've made room.$"

OrangeIslands_Mikan_Gym_Text_CissyPostBattle::
	.string "The next gym is on NAVEL ISLAND,\n"
	.string "north of here. Good luck!$"

OrangeIslands_Mikan_Gym_Text_MarcoIntro::
	.string "Can you hit what you can't see?$"

OrangeIslands_Mikan_Gym_Text_MarcoDefeat::
	.string "Guess you can!$"

OrangeIslands_Mikan_Gym_Text_MarcoPostBattle::
	.string "CISSY's POKéMON are even harder\n"
	.string "to pin down than mine.$"

OrangeIslands_Mikan_Gym_Text_CoralIntro::
	.string "Rain or shine, I'm ready!$"

OrangeIslands_Mikan_Gym_Text_CoralDefeat::
	.string "I'm all washed up!$"

OrangeIslands_Mikan_Gym_Text_CoralPostBattle::
	.string "When CISSY makes it rain, her\n"
	.string "THUNDER never misses.$"

OrangeIslands_Mikan_Gym_Text_GymGuyAdvice::
	.string "Hey! Going for the CORAL-EYE\n"
	.string "BADGE?\p"
	.string "CISSY battles in the rain. Her\n"
	.string "WATER moves get stronger, and\l"
	.string "THUNDER can't miss.\p"
	.string "She also loves to cut your\n"
	.string "accuracy and dodge your attacks.\p"
	.string "GRASS types are a safe bet here!$"

OrangeIslands_Mikan_Gym_Text_GymGuyAfter::
	.string "You beat CISSY in her own rain!\n"
	.string "That was something!$"

OrangeIslands_Mikan_Gym_Text_GymStatue::
	.string "MIKAN ISLAND POKéMON GYM\n"
	.string "LEADER: CISSY\p"
	.string "WINNING TRAINERS:\n"
	.string "{RIVAL}$"

OrangeIslands_Mikan_Gym_Text_GymStatueAfter::
	.string "MIKAN ISLAND POKéMON GYM\n"
	.string "LEADER: CISSY\p"
	.string "WINNING TRAINERS:\n"
	.string "{RIVAL}, {PLAYER}$"
'''
p = 'data/maps/OrangeIslands_Mikan_Gym/scripts.inc'
if not os.path.exists(p): wr(p, GYM)

# Follower: the home team's follower goes away when the team is stored, and the follower is
# refreshed when the teams swap back.
p = 'src/orange_islands.c'; t = rd(p)
if 'follow_me.h' not in t:
    t = t.replace('#include "event_data.h"\n', '#include "event_data.h"\n#include "follow_me.h"\n', 1)
    old = '    gPlayerPartyCount = 0;\n    FlagSet(FLAG_ORANGE_CHALLENGE);\n'
    assert old in t
    t = t.replace(old, '    gPlayerPartyCount = 0;\n    DestroyFollower(); // nothing left to follow; the starter brings it back\n    FlagSet(FLAG_ORANGE_CHALLENGE);\n', 1)
    old = '    CompactPartySlots();\n    CalculatePlayerPartyCount();\n    FlagClear(FLAG_ORANGE_CHALLENGE);\n'
    assert old in t
    t = t.replace(old, '    CompactPartySlots();\n    CalculatePlayerPartyCount();\n    FlagClear(FLAG_ORANGE_CHALLENGE);\n    UpdateFollowerPokemonGraphic();\n', 1)
    wr(p, t)

# Hook the new maps' scripts into the build (pokefirered includes each map's scripts.inc from data/event_scripts.s)
p = 'data/event_scripts.s'; t = rd(p)
for name in ('OrangeIslands_Valencia_IvysLab', 'OrangeIslands_Mikan_Gym'):
    line = '\t.include "data/maps/%s/scripts.inc"\n' % name
    if line not in t:
        anchor = '\t.include "data/maps/OrangeIslands_Valencia/scripts.inc"\n'
        assert anchor in t
        t = t.replace(anchor, anchor + line, 1)
wr(p, t)
print('orange ivy + mikan ok')
