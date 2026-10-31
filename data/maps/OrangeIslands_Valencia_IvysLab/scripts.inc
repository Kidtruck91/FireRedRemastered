.equ LOCALID_IVY,          1
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
	@ Until now the respawn is still wherever it was in Kanto, so whiting out
	@ anywhere in the Orange Islands sent the player back across the ocean.
	@ The Valencia Center sets this too, but by then it is too late if they
	@ never walked in.
	setrespawn SPAWN_ORANGE_VALENCIA
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
	.string "The team you brought waits for you\n"
	.string "in your KANTO PC, and while you're\l"
	.string "here you get an ORANGE PC instead.\p"
	.string "You'll raise a new team from the\n"
	.string "POKéMON you meet here.\p"
	.string "And the CREW caps your POKéMON's\n"
	.string "levels. Each badge you win raises\l"
	.string "the cap a little.$"

OrangeIslands_Valencia_IvysLab_Text_ReadyToStore::
	.string "I'll send your team to your KANTO\n"
	.string "PC now. Don't worry, they'll be safe!$"

OrangeIslands_Valencia_IvysLab_Text_TeamStored::
	.string "{PLAYER}'s POKéMON were sent\n"
	.string "to the PC!$"

OrangeIslands_Valencia_IvysLab_Text_NoRoom::
	.string "Hmm, the PC link didn't go\n"
	.string "through. Let's try that again.$"

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
