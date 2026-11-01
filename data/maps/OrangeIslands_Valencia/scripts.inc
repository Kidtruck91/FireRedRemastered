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

@ Edge warp: different tilesets, so no seamless edge
OrangeIslands_Valencia_EventScript_WalkToRoute2_28_81::
	warp MAP_ORANGE_ISLANDS_ROUTE_2, 28, 81
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

@ ---- Orange Islands trainers (orange_route_trainers.py) ----
OrangeIslands_Valencia_EventScript_OrangeSeth::
	trainerbattle_single TRAINER_ORANGE_BIRD_KEEPER_SETH, OrangeIslands_Valencia_Text_OrangeSethIntro, OrangeIslands_Valencia_Text_OrangeSethDefeat
	msgbox OrangeIslands_Valencia_Text_OrangeSethPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Valencia_EventScript_OrangeDora::
	trainerbattle_single TRAINER_ORANGE_LASS_DORA, OrangeIslands_Valencia_Text_OrangeDoraIntro, OrangeIslands_Valencia_Text_OrangeDoraDefeat
	msgbox OrangeIslands_Valencia_Text_OrangeDoraPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Valencia_EventScript_OrangeTerry::
	trainerbattle_single TRAINER_ORANGE_FISHERMAN_TERRY, OrangeIslands_Valencia_Text_OrangeTerryIntro, OrangeIslands_Valencia_Text_OrangeTerryDefeat
	msgbox OrangeIslands_Valencia_Text_OrangeTerryPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Valencia_Text_OrangeSethIntro::
	.string "My birds fly between the\n"
	.string "islands every day!$"

OrangeIslands_Valencia_Text_OrangeSethDefeat::
	.string "Grounded!$"

OrangeIslands_Valencia_Text_OrangeSethPostBattle::
	.string "Sea birds are tougher than\n"
	.string "they look.$"

OrangeIslands_Valencia_Text_OrangeDoraIntro::
	.string "Ooh, a new face!\n"
	.string "Let's battle!$"

OrangeIslands_Valencia_Text_OrangeDoraDefeat::
	.string "You're so mean!$"

OrangeIslands_Valencia_Text_OrangeDoraPostBattle::
	.string "I love the flowers on this\n"
	.string "island.$"

OrangeIslands_Valencia_Text_OrangeTerryIntro::
	.string "Shh! You'll scare the fish!\p"
	.string "Well, now that they're gone,\n"
	.string "let's battle!$"

OrangeIslands_Valencia_Text_OrangeTerryDefeat::
	.string "Reeled in!$"

OrangeIslands_Valencia_Text_OrangeTerryPostBattle::
	.string "The big ones bite at dawn.\n"
	.string "Patience is everything.$"

@ ---- Ferry sailor (orange_trip_story.py): no ship until Drake hands over the GS BALL
OrangeIslands_Valencia_EventScript_FerrySailor::
	lock
	faceplayer
	goto_if_set FLAG_GOT_GS_BALL, OrangeIslands_Valencia_EventScript_FerryHome
	msgbox OrangeIslands_Valencia_Text_ShipLeft
	release
	end

OrangeIslands_Valencia_EventScript_FerryHome::
	msgbox OrangeIslands_Valencia_Text_FerryHomeAsk, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, OrangeIslands_Valencia_EventScript_FerryNotNow
	goto_if_unset FLAG_ORANGE_CHALLENGE, OrangeIslands_Valencia_EventScript_FerrySail
	msgbox OrangeIslands_Valencia_Text_TeamsSwap
	callnative OrangeChallenge_End
	goto_if_eq VAR_RESULT, FALSE, OrangeIslands_Valencia_EventScript_FerrySaveError
	playse SE_PC_LOGIN
	waitse
OrangeIslands_Valencia_EventScript_FerrySail::
	msgbox OrangeIslands_Valencia_Text_FerryDeparting
	setvar VAR_0x8004, SEAGALLOP_VALENCIA
	setvar VAR_0x8006, SEAGALLOP_VERMILION_CITY
	goto EventScript_SetSail
	end

OrangeIslands_Valencia_EventScript_FerryNotNow::
	msgbox OrangeIslands_Valencia_Text_FerryNotNow
	release
	end

OrangeIslands_Valencia_EventScript_FerrySaveError::
	msgbox OrangeIslands_Valencia_Text_FerrySaveError
	release
	end

OrangeIslands_Valencia_Text_ShipLeft::
	.string "The SEAGALLOP that brought you\n"
	.string "has already sailed back to KANTO.\p"
	.string "It won't be back for several days.\p"
	.string "The captain says he's waiting on\n"
	.string "something from PUMMELO ISLAND.\l"
	.string "Enjoy the islands until then!$"

OrangeIslands_Valencia_Text_FerryHomeAsk::
	.string "Ahoy! The SEAGALLOP is back.\p"
	.string "Sailing home to VERMILION CITY?$"

OrangeIslands_Valencia_Text_TeamsSwap::
	.string "Your ORANGE ISLANDS team and PC\n"
	.string "will wait here for you.\p"
	.string "Your KANTO team and PC go home\n"
	.string "with you.$"

OrangeIslands_Valencia_Text_FerryDeparting::
	.string "All right!\n"
	.string "All aboard for VERMILION CITY!$"

OrangeIslands_Valencia_Text_FerryNotNow::
	.string "No rush. The ship will wait.$"

OrangeIslands_Valencia_Text_FerrySaveError::
	.string "Hm, something went wrong storing\n"
	.string "your POKéMON. Nothing was changed.\p"
	.string "Please try again.$"
