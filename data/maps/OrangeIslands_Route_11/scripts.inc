OrangeIslands_Route_11_MapScripts::
	.byte 0

OrangeIslands_Route_11_EventScript_Reef::
	msgbox OrangeIslands_Route_11_Text_Reef, MSGBOX_SIGN
	end

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Trovita Beach
OrangeIslands_Route_11_EventScript_SeaEdgeToTrovitaBeach::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8004, 0
	goto_if_lt OrangeIslands_Route_11_EventScript_SeaEdgeToTrovitaBeach_Lo
	compare VAR_0x8004, 64
	goto_if_gt OrangeIslands_Route_11_EventScript_SeaEdgeToTrovitaBeach_Hi
OrangeIslands_Route_11_EventScript_SeaEdgeToTrovitaBeach_Go:
	addvar VAR_0x8004, 7
	setvar VAR_0x8005, 34
	warp MAP_ORANGE_ISLANDS_TROVITA_BEACH, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Route_11_EventScript_SeaEdgeToTrovitaBeach_Lo:
	setvar VAR_0x8004, 0
	goto OrangeIslands_Route_11_EventScript_SeaEdgeToTrovitaBeach_Go
OrangeIslands_Route_11_EventScript_SeaEdgeToTrovitaBeach_Hi:
	setvar VAR_0x8004, 64
	goto OrangeIslands_Route_11_EventScript_SeaEdgeToTrovitaBeach_Go

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Navel Beach
OrangeIslands_Route_11_EventScript_SeaEdgeToNavelBeach::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8005, 44
	goto_if_lt OrangeIslands_Route_11_EventScript_SeaEdgeToNavelBeach_Lo
	compare VAR_0x8005, 51
	goto_if_gt OrangeIslands_Route_11_EventScript_SeaEdgeToNavelBeach_Hi
OrangeIslands_Route_11_EventScript_SeaEdgeToNavelBeach_Go:
	subvar VAR_0x8005, 42
	setvar VAR_0x8004, 1
	warp MAP_ORANGE_ISLANDS_NAVEL_BEACH, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Route_11_EventScript_SeaEdgeToNavelBeach_Lo:
	setvar VAR_0x8005, 44
	goto OrangeIslands_Route_11_EventScript_SeaEdgeToNavelBeach_Go
OrangeIslands_Route_11_EventScript_SeaEdgeToNavelBeach_Hi:
	setvar VAR_0x8005, 51
	goto OrangeIslands_Route_11_EventScript_SeaEdgeToNavelBeach_Go

@ ---- Orange Islands trainers (orange_route_trainers.py) ----
OrangeIslands_Route_11_EventScript_OrangeAlice::
	trainerbattle_single TRAINER_ORANGE_SWIMMER_F_ALICE, OrangeIslands_Route_11_Text_OrangeAliceIntro, OrangeIslands_Route_11_Text_OrangeAliceDefeat
	msgbox OrangeIslands_Route_11_Text_OrangeAlicePostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Route_11_EventScript_OrangeCora::
	trainerbattle_single TRAINER_ORANGE_SWIMMER_F_CORA, OrangeIslands_Route_11_Text_OrangeCoraIntro, OrangeIslands_Route_11_Text_OrangeCoraDefeat
	msgbox OrangeIslands_Route_11_Text_OrangeCoraPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Route_11_EventScript_OrangeBlair::
	trainerbattle_single TRAINER_ORANGE_SWIMMER_F_BLAIR, OrangeIslands_Route_11_Text_OrangeBlairIntro, OrangeIslands_Route_11_Text_OrangeBlairDefeat
	msgbox OrangeIslands_Route_11_Text_OrangeBlairPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Route_11_EventScript_OrangeJana::
	trainerbattle_single TRAINER_ORANGE_SWIMMER_F_JANA, OrangeIslands_Route_11_Text_OrangeJanaIntro, OrangeIslands_Route_11_Text_OrangeJanaDefeat
	msgbox OrangeIslands_Route_11_Text_OrangeJanaPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Route_11_EventScript_OrangeTheo::
	trainerbattle_single TRAINER_ORANGE_SWIMMER_M_THEO, OrangeIslands_Route_11_Text_OrangeTheoIntro, OrangeIslands_Route_11_Text_OrangeTheoDefeat
	msgbox OrangeIslands_Route_11_Text_OrangeTheoPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Route_11_Text_OrangeAliceIntro::
	.string "Swimming in the ORANGE\n"
	.string "ISLANDS is the best!$"

OrangeIslands_Route_11_Text_OrangeAliceDefeat::
	.string "Oh, I lost!$"

OrangeIslands_Route_11_Text_OrangeAlicePostBattle::
	.string "The reefs are so pretty\n"
	.string "from out here.$"

OrangeIslands_Route_11_Text_OrangeCoraIntro::
	.string "My POKéMON and I swim\n"
	.string "together every day!$"

OrangeIslands_Route_11_Text_OrangeCoraDefeat::
	.string "We'll swim harder!$"

OrangeIslands_Route_11_Text_OrangeCoraPostBattle::
	.string "Each island has its own\n"
	.string "kind of water. Isn't it neat?$"

OrangeIslands_Route_11_Text_OrangeBlairIntro::
	.string "Swimming in the ORANGE\n"
	.string "ISLANDS is the best!$"

OrangeIslands_Route_11_Text_OrangeBlairDefeat::
	.string "Oh, I lost!$"

OrangeIslands_Route_11_Text_OrangeBlairPostBattle::
	.string "The reefs are so pretty\n"
	.string "from out here.$"

OrangeIslands_Route_11_Text_OrangeJanaIntro::
	.string "Hey! You splashed me!\n"
	.string "That means a battle!$"

OrangeIslands_Route_11_Text_OrangeJanaDefeat::
	.string "Fine, you win.$"

OrangeIslands_Route_11_Text_OrangeJanaPostBattle::
	.string "Watch out for TENTACOOL\n"
	.string "near the reefs.$"

OrangeIslands_Route_11_Text_OrangeTheoIntro::
	.string "The water's perfect today!\n"
	.string "Let's battle out here!$"

OrangeIslands_Route_11_Text_OrangeTheoDefeat::
	.string "I'm sinking!$"

OrangeIslands_Route_11_Text_OrangeTheoPostBattle::
	.string "I swim between the islands\n"
	.string "every morning.$"

