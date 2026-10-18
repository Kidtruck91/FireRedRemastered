.equ LOCALID_IVY_DOCK, 1

OrangeIslands_Valencia_MapScripts::
	map_script MAP_SCRIPT_ON_FRAME_TABLE, OrangeIslands_Valencia_OnFrame
	.byte 0

OrangeIslands_Valencia_EventScript_Beach::
	msgbox OrangeIslands_Valencia_Text_Beach, MSGBOX_SIGN
	end

OrangeIslands_Valencia_EventScript_Town::
	msgbox OrangeIslands_Valencia_Text_Town, MSGBOX_SIGN
	end

OrangeIslands_Valencia_EventScript_EdgeToValenciaBeach_49::
	warp MAP_ORANGE_ISLANDS_VALENCIA_BEACH, 8, 33
	waitstate
	end

OrangeIslands_Valencia_EventScript_EdgeToValenciaBeach_50::
	warp MAP_ORANGE_ISLANDS_VALENCIA_BEACH, 8, 34
	waitstate
	end

OrangeIslands_Valencia_EventScript_EdgeToValenciaBeach_51::
	warp MAP_ORANGE_ISLANDS_VALENCIA_BEACH, 8, 35
	waitstate
	end

OrangeIslands_Valencia_EventScript_EdgeToValenciaBeach_52::
	warp MAP_ORANGE_ISLANDS_VALENCIA_BEACH, 8, 36
	waitstate
	end

OrangeIslands_Valencia_EventScript_EdgeToValenciaBeach_53::
	warp MAP_ORANGE_ISLANDS_VALENCIA_BEACH, 8, 37
	waitstate
	end

@ Edge warp: different tilesets, so no seamless edge
OrangeIslands_Valencia_EventScript_WalkToRoute2_27_44::
	warp MAP_ORANGE_ISLANDS_ROUTE_2, 27, 44
	waitstate
	end

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
