OrangeIslands_Route_8_MapScripts::
	.byte 0

OrangeIslands_Route_8_EventScript_West::
	msgbox OrangeIslands_Route_8_Text_West, MSGBOX_SIGN
	end

OrangeIslands_Route_8_EventScript_East::
	msgbox OrangeIslands_Route_8_Text_East, MSGBOX_SIGN
	end

OrangeIslands_Route_8_EventScript_Rest::
	msgbox OrangeIslands_Route_8_Text_Rest, MSGBOX_SIGN
	end

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Mandarin South
OrangeIslands_Route_8_EventScript_SeaEdgeToMandarinSouth::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8004, 39
	goto_if_lt OrangeIslands_Route_8_EventScript_SeaEdgeToMandarinSouth_Lo
	compare VAR_0x8004, 128
	goto_if_gt OrangeIslands_Route_8_EventScript_SeaEdgeToMandarinSouth_Hi
OrangeIslands_Route_8_EventScript_SeaEdgeToMandarinSouth_Go:
	subvar VAR_0x8004, 36
	setvar VAR_0x8005, 74
	warp MAP_ORANGE_ISLANDS_MANDARIN_SOUTH, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Route_8_EventScript_SeaEdgeToMandarinSouth_Lo:
	setvar VAR_0x8004, 39
	goto OrangeIslands_Route_8_EventScript_SeaEdgeToMandarinSouth_Go
OrangeIslands_Route_8_EventScript_SeaEdgeToMandarinSouth_Hi:
	setvar VAR_0x8004, 128
	goto OrangeIslands_Route_8_EventScript_SeaEdgeToMandarinSouth_Go

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Kinnow
OrangeIslands_Route_8_EventScript_SeaEdgeToKinnow::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8005, 0
	goto_if_lt OrangeIslands_Route_8_EventScript_SeaEdgeToKinnow_Lo
	compare VAR_0x8005, 19
	goto_if_gt OrangeIslands_Route_8_EventScript_SeaEdgeToKinnow_Hi
OrangeIslands_Route_8_EventScript_SeaEdgeToKinnow_Go:
	addvar VAR_0x8005, 46
	setvar VAR_0x8004, 70
	warp MAP_ORANGE_ISLANDS_KINNOW, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Route_8_EventScript_SeaEdgeToKinnow_Lo:
	setvar VAR_0x8005, 0
	goto OrangeIslands_Route_8_EventScript_SeaEdgeToKinnow_Go
OrangeIslands_Route_8_EventScript_SeaEdgeToKinnow_Hi:
	setvar VAR_0x8005, 19
	goto OrangeIslands_Route_8_EventScript_SeaEdgeToKinnow_Go

@ ---- Orange Islands trainers (orange_route_trainers.py) ----
OrangeIslands_Route_8_EventScript_OrangeKate::
	trainerbattle_single TRAINER_ORANGE_SWIMMER_F_KATE, OrangeIslands_Route_8_Text_OrangeKateIntro, OrangeIslands_Route_8_Text_OrangeKateDefeat
	msgbox OrangeIslands_Route_8_Text_OrangeKatePostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Route_8_EventScript_OrangeCarmen::
	trainerbattle_single TRAINER_ORANGE_SWIMMER_F_CARMEN, OrangeIslands_Route_8_Text_OrangeCarmenIntro, OrangeIslands_Route_8_Text_OrangeCarmenDefeat
	msgbox OrangeIslands_Route_8_Text_OrangeCarmenPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Route_8_EventScript_OrangeHeidi::
	trainerbattle_single TRAINER_ORANGE_SWIMMER_F_HEIDI, OrangeIslands_Route_8_Text_OrangeHeidiIntro, OrangeIslands_Route_8_Text_OrangeHeidiDefeat
	msgbox OrangeIslands_Route_8_Text_OrangeHeidiPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Route_8_EventScript_OrangeAlbert::
	trainerbattle_single TRAINER_ORANGE_SWIMMER_M_ALBERT, OrangeIslands_Route_8_Text_OrangeAlbertIntro, OrangeIslands_Route_8_Text_OrangeAlbertDefeat
	msgbox OrangeIslands_Route_8_Text_OrangeAlbertPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Route_8_EventScript_OrangeRex::
	trainerbattle_single TRAINER_ORANGE_SWIMMER_M_REX, OrangeIslands_Route_8_Text_OrangeRexIntro, OrangeIslands_Route_8_Text_OrangeRexDefeat
	msgbox OrangeIslands_Route_8_Text_OrangeRexPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Route_8_Text_OrangeKateIntro::
	.string "Hey! You splashed me!\n"
	.string "That means a battle!$"

OrangeIslands_Route_8_Text_OrangeKateDefeat::
	.string "Fine, you win.$"

OrangeIslands_Route_8_Text_OrangeKatePostBattle::
	.string "Watch out for TENTACOOL\n"
	.string "near the reefs.$"

OrangeIslands_Route_8_Text_OrangeCarmenIntro::
	.string "My POKéMON and I swim\n"
	.string "together every day!$"

OrangeIslands_Route_8_Text_OrangeCarmenDefeat::
	.string "We'll swim harder!$"

OrangeIslands_Route_8_Text_OrangeCarmenPostBattle::
	.string "Each island has its own\n"
	.string "kind of water. Isn't it neat?$"

OrangeIslands_Route_8_Text_OrangeHeidiIntro::
	.string "Hey! You splashed me!\n"
	.string "That means a battle!$"

OrangeIslands_Route_8_Text_OrangeHeidiDefeat::
	.string "Fine, you win.$"

OrangeIslands_Route_8_Text_OrangeHeidiPostBattle::
	.string "Watch out for TENTACOOL\n"
	.string "near the reefs.$"

OrangeIslands_Route_8_Text_OrangeAlbertIntro::
	.string "I train my POKéMON in the\n"
	.string "open sea. Ready?$"

OrangeIslands_Route_8_Text_OrangeAlbertDefeat::
	.string "Washed out!$"

OrangeIslands_Route_8_Text_OrangeAlbertPostBattle::
	.string "Out here, WATER POKéMON are\n"
	.string "the real champions.$"

OrangeIslands_Route_8_Text_OrangeRexIntro::
	.string "You're surfing? Then you\n"
	.string "can't escape me!$"

OrangeIslands_Route_8_Text_OrangeRexDefeat::
	.string "Glub... You win.$"

OrangeIslands_Route_8_Text_OrangeRexPostBattle::
	.string "The currents here are strong.\n"
	.string "Watch where they push you.$"

