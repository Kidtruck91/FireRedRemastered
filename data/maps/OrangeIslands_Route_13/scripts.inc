OrangeIslands_Route_13_MapScripts::
	.byte 0

OrangeIslands_Route_13_EventScript_Reserve::
	msgbox OrangeIslands_Route_13_Text_Reserve, MSGBOX_SIGN
	end

OrangeIslands_Route_13_EventScript_South::
	msgbox OrangeIslands_Route_13_Text_South, MSGBOX_SIGN
	end

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Route 18
OrangeIslands_Route_13_EventScript_SeaEdgeToRoute18::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8004, 0
	goto_if_lt OrangeIslands_Route_13_EventScript_SeaEdgeToRoute18_Lo
	compare VAR_0x8004, 87
	goto_if_gt OrangeIslands_Route_13_EventScript_SeaEdgeToRoute18_Hi
OrangeIslands_Route_13_EventScript_SeaEdgeToRoute18_Go:
	addvar VAR_0x8004, 4
	setvar VAR_0x8005, 1
	warp MAP_ORANGE_ISLANDS_ROUTE_18, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Route_13_EventScript_SeaEdgeToRoute18_Lo:
	setvar VAR_0x8004, 0
	goto OrangeIslands_Route_13_EventScript_SeaEdgeToRoute18_Go
OrangeIslands_Route_13_EventScript_SeaEdgeToRoute18_Hi:
	setvar VAR_0x8004, 87
	goto OrangeIslands_Route_13_EventScript_SeaEdgeToRoute18_Go

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Kumquat
OrangeIslands_Route_13_EventScript_SeaEdgeToKumquat::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8004, 0
	goto_if_lt OrangeIslands_Route_13_EventScript_SeaEdgeToKumquat_Lo
	compare VAR_0x8004, 71
	goto_if_gt OrangeIslands_Route_13_EventScript_SeaEdgeToKumquat_Hi
OrangeIslands_Route_13_EventScript_SeaEdgeToKumquat_Go:
	setvar VAR_0x8005, 58
	warp MAP_ORANGE_ISLANDS_KUMQUAT, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Route_13_EventScript_SeaEdgeToKumquat_Lo:
	setvar VAR_0x8004, 0
	goto OrangeIslands_Route_13_EventScript_SeaEdgeToKumquat_Go
OrangeIslands_Route_13_EventScript_SeaEdgeToKumquat_Hi:
	setvar VAR_0x8004, 71
	goto OrangeIslands_Route_13_EventScript_SeaEdgeToKumquat_Go

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Route 16
OrangeIslands_Route_13_EventScript_SeaEdgeToRoute16::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8005, 0
	goto_if_lt OrangeIslands_Route_13_EventScript_SeaEdgeToRoute16_Lo
	compare VAR_0x8005, 79
	goto_if_gt OrangeIslands_Route_13_EventScript_SeaEdgeToRoute16_Hi
OrangeIslands_Route_13_EventScript_SeaEdgeToRoute16_Go:
	addvar VAR_0x8005, 12
	setvar VAR_0x8004, 34
	warp MAP_ORANGE_ISLANDS_ROUTE_16, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Route_13_EventScript_SeaEdgeToRoute16_Lo:
	setvar VAR_0x8005, 0
	goto OrangeIslands_Route_13_EventScript_SeaEdgeToRoute16_Go
OrangeIslands_Route_13_EventScript_SeaEdgeToRoute16_Hi:
	setvar VAR_0x8005, 79
	goto OrangeIslands_Route_13_EventScript_SeaEdgeToRoute16_Go

@ ---- Orange Islands trainers (orange_route_trainers.py) ----
OrangeIslands_Route_13_EventScript_OrangeElena::
	trainerbattle_single TRAINER_ORANGE_SWIMMER_F_ELENA, OrangeIslands_Route_13_Text_OrangeElenaIntro, OrangeIslands_Route_13_Text_OrangeElenaDefeat
	msgbox OrangeIslands_Route_13_Text_OrangeElenaPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Route_13_EventScript_OrangePaula::
	trainerbattle_single TRAINER_ORANGE_SWIMMER_F_PAULA, OrangeIslands_Route_13_Text_OrangePaulaIntro, OrangeIslands_Route_13_Text_OrangePaulaDefeat
	msgbox OrangeIslands_Route_13_Text_OrangePaulaPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Route_13_EventScript_OrangeDevin::
	trainerbattle_single TRAINER_ORANGE_SWIMMER_M_DEVIN, OrangeIslands_Route_13_Text_OrangeDevinIntro, OrangeIslands_Route_13_Text_OrangeDevinDefeat
	msgbox OrangeIslands_Route_13_Text_OrangeDevinPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Route_13_EventScript_OrangeCelia::
	trainerbattle_single TRAINER_ORANGE_SWIMMER_F_CELIA, OrangeIslands_Route_13_Text_OrangeCeliaIntro, OrangeIslands_Route_13_Text_OrangeCeliaDefeat
	msgbox OrangeIslands_Route_13_Text_OrangeCeliaPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Route_13_EventScript_OrangeJonah::
	trainerbattle_single TRAINER_ORANGE_SWIMMER_M_JONAH, OrangeIslands_Route_13_Text_OrangeJonahIntro, OrangeIslands_Route_13_Text_OrangeJonahDefeat
	msgbox OrangeIslands_Route_13_Text_OrangeJonahPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Route_13_EventScript_OrangeLiam::
	trainerbattle_single TRAINER_ORANGE_SWIMMER_M_LIAM, OrangeIslands_Route_13_Text_OrangeLiamIntro, OrangeIslands_Route_13_Text_OrangeLiamDefeat
	msgbox OrangeIslands_Route_13_Text_OrangeLiamPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Route_13_Text_OrangeElenaIntro::
	.string "Hey! You splashed me!\n"
	.string "That means a battle!$"

OrangeIslands_Route_13_Text_OrangeElenaDefeat::
	.string "Fine, you win.$"

OrangeIslands_Route_13_Text_OrangeElenaPostBattle::
	.string "Watch out for TENTACOOL\n"
	.string "near the reefs.$"

OrangeIslands_Route_13_Text_OrangePaulaIntro::
	.string "Swimming in the ORANGE\n"
	.string "ISLANDS is the best!$"

OrangeIslands_Route_13_Text_OrangePaulaDefeat::
	.string "Oh, I lost!$"

OrangeIslands_Route_13_Text_OrangePaulaPostBattle::
	.string "The reefs are so pretty\n"
	.string "from out here.$"

OrangeIslands_Route_13_Text_OrangeDevinIntro::
	.string "You're surfing? Then you\n"
	.string "can't escape me!$"

OrangeIslands_Route_13_Text_OrangeDevinDefeat::
	.string "Glub... You win.$"

OrangeIslands_Route_13_Text_OrangeDevinPostBattle::
	.string "The currents here are strong.\n"
	.string "Watch where they push you.$"

OrangeIslands_Route_13_Text_OrangeCeliaIntro::
	.string "Hey! You splashed me!\n"
	.string "That means a battle!$"

OrangeIslands_Route_13_Text_OrangeCeliaDefeat::
	.string "Fine, you win.$"

OrangeIslands_Route_13_Text_OrangeCeliaPostBattle::
	.string "Watch out for TENTACOOL\n"
	.string "near the reefs.$"

OrangeIslands_Route_13_Text_OrangeJonahIntro::
	.string "I train my POKéMON in the\n"
	.string "open sea. Ready?$"

OrangeIslands_Route_13_Text_OrangeJonahDefeat::
	.string "Washed out!$"

OrangeIslands_Route_13_Text_OrangeJonahPostBattle::
	.string "Out here, WATER POKéMON are\n"
	.string "the real champions.$"

OrangeIslands_Route_13_Text_OrangeLiamIntro::
	.string "You're surfing? Then you\n"
	.string "can't escape me!$"

OrangeIslands_Route_13_Text_OrangeLiamDefeat::
	.string "Glub... You win.$"

OrangeIslands_Route_13_Text_OrangeLiamPostBattle::
	.string "The currents here are strong.\n"
	.string "Watch where they push you.$"

