OrangeIslands_Route_6_MapScripts::
	.byte 0

OrangeIslands_Route_6_EventScript_South::
	msgbox OrangeIslands_Route_6_Text_South, MSGBOX_SIGN
	end

OrangeIslands_Route_6_EventScript_East::
	msgbox OrangeIslands_Route_6_Text_East, MSGBOX_SIGN
	end

OrangeIslands_Route_6_EventScript_Interior::
	msgbox OrangeIslands_Route_6_Text_Interior, MSGBOX_SIGN
	end

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Route 5
OrangeIslands_Route_6_EventScript_SeaEdgeToRoute5::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8005, 0
	goto_if_lt OrangeIslands_Route_6_EventScript_SeaEdgeToRoute5_Lo
	compare VAR_0x8005, 71
	goto_if_gt OrangeIslands_Route_6_EventScript_SeaEdgeToRoute5_Hi
OrangeIslands_Route_6_EventScript_SeaEdgeToRoute5_Go:
	addvar VAR_0x8005, 33
	setvar VAR_0x8004, 1
	warp MAP_ORANGE_ISLANDS_ROUTE_5, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Route_6_EventScript_SeaEdgeToRoute5_Lo:
	setvar VAR_0x8005, 0
	goto OrangeIslands_Route_6_EventScript_SeaEdgeToRoute5_Go
OrangeIslands_Route_6_EventScript_SeaEdgeToRoute5_Hi:
	setvar VAR_0x8005, 71
	goto OrangeIslands_Route_6_EventScript_SeaEdgeToRoute5_Go

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Mandarin South
OrangeIslands_Route_6_EventScript_SeaEdgeToMandarinSouth::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8005, 0
	goto_if_lt OrangeIslands_Route_6_EventScript_SeaEdgeToMandarinSouth_Lo
	compare VAR_0x8005, 65
	goto_if_gt OrangeIslands_Route_6_EventScript_SeaEdgeToMandarinSouth_Hi
OrangeIslands_Route_6_EventScript_SeaEdgeToMandarinSouth_Go:
	addvar VAR_0x8005, 10
	setvar VAR_0x8004, 94
	warp MAP_ORANGE_ISLANDS_MANDARIN_SOUTH, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Route_6_EventScript_SeaEdgeToMandarinSouth_Lo:
	setvar VAR_0x8005, 0
	goto OrangeIslands_Route_6_EventScript_SeaEdgeToMandarinSouth_Go
OrangeIslands_Route_6_EventScript_SeaEdgeToMandarinSouth_Hi:
	setvar VAR_0x8005, 65
	goto OrangeIslands_Route_6_EventScript_SeaEdgeToMandarinSouth_Go

@ ---- Orange Islands trainers (orange_route_trainers.py) ----
OrangeIslands_Route_6_EventScript_OrangeChase::
	trainerbattle_single TRAINER_ORANGE_SWIMMER_M_CHASE, OrangeIslands_Route_6_Text_OrangeChaseIntro, OrangeIslands_Route_6_Text_OrangeChaseDefeat
	msgbox OrangeIslands_Route_6_Text_OrangeChasePostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Route_6_EventScript_OrangeAndy::
	trainerbattle_single TRAINER_ORANGE_SWIMMER_M_ANDY, OrangeIslands_Route_6_Text_OrangeAndyIntro, OrangeIslands_Route_6_Text_OrangeAndyDefeat
	msgbox OrangeIslands_Route_6_Text_OrangeAndyPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Route_6_EventScript_OrangeRob::
	trainerbattle_single TRAINER_ORANGE_SWIMMER_M_ROB, OrangeIslands_Route_6_Text_OrangeRobIntro, OrangeIslands_Route_6_Text_OrangeRobDefeat
	msgbox OrangeIslands_Route_6_Text_OrangeRobPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Route_6_EventScript_OrangeAda::
	trainerbattle_single TRAINER_ORANGE_SWIMMER_F_ADA, OrangeIslands_Route_6_Text_OrangeAdaIntro, OrangeIslands_Route_6_Text_OrangeAdaDefeat
	msgbox OrangeIslands_Route_6_Text_OrangeAdaPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Route_6_EventScript_OrangeGrant::
	trainerbattle_single TRAINER_ORANGE_SWIMMER_M_GRANT, OrangeIslands_Route_6_Text_OrangeGrantIntro, OrangeIslands_Route_6_Text_OrangeGrantDefeat
	msgbox OrangeIslands_Route_6_Text_OrangeGrantPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Route_6_EventScript_OrangeLila::
	trainerbattle_single TRAINER_ORANGE_SWIMMER_F_LILA, OrangeIslands_Route_6_Text_OrangeLilaIntro, OrangeIslands_Route_6_Text_OrangeLilaDefeat
	msgbox OrangeIslands_Route_6_Text_OrangeLilaPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Route_6_Text_OrangeChaseIntro::
	.string "You're surfing? Then you\n"
	.string "can't escape me!$"

OrangeIslands_Route_6_Text_OrangeChaseDefeat::
	.string "Glub... You win.$"

OrangeIslands_Route_6_Text_OrangeChasePostBattle::
	.string "The currents here are strong.\n"
	.string "Watch where they push you.$"

OrangeIslands_Route_6_Text_OrangeAndyIntro::
	.string "I train my POKéMON in the\n"
	.string "open sea. Ready?$"

OrangeIslands_Route_6_Text_OrangeAndyDefeat::
	.string "Washed out!$"

OrangeIslands_Route_6_Text_OrangeAndyPostBattle::
	.string "Out here, WATER POKéMON are\n"
	.string "the real champions.$"

OrangeIslands_Route_6_Text_OrangeRobIntro::
	.string "You're surfing? Then you\n"
	.string "can't escape me!$"

OrangeIslands_Route_6_Text_OrangeRobDefeat::
	.string "Glub... You win.$"

OrangeIslands_Route_6_Text_OrangeRobPostBattle::
	.string "The currents here are strong.\n"
	.string "Watch where they push you.$"

OrangeIslands_Route_6_Text_OrangeAdaIntro::
	.string "Hey! You splashed me!\n"
	.string "That means a battle!$"

OrangeIslands_Route_6_Text_OrangeAdaDefeat::
	.string "Fine, you win.$"

OrangeIslands_Route_6_Text_OrangeAdaPostBattle::
	.string "Watch out for TENTACOOL\n"
	.string "near the reefs.$"

OrangeIslands_Route_6_Text_OrangeGrantIntro::
	.string "The water's perfect today!\n"
	.string "Let's battle out here!$"

OrangeIslands_Route_6_Text_OrangeGrantDefeat::
	.string "I'm sinking!$"

OrangeIslands_Route_6_Text_OrangeGrantPostBattle::
	.string "I swim between the islands\n"
	.string "every morning.$"

OrangeIslands_Route_6_Text_OrangeLilaIntro::
	.string "Hey! You splashed me!\n"
	.string "That means a battle!$"

OrangeIslands_Route_6_Text_OrangeLilaDefeat::
	.string "Fine, you win.$"

OrangeIslands_Route_6_Text_OrangeLilaPostBattle::
	.string "Watch out for TENTACOOL\n"
	.string "near the reefs.$"

