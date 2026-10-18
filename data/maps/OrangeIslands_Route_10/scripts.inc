OrangeIslands_Route_10_MapScripts::
	.byte 0

OrangeIslands_Route_10_EventScript_West::
	msgbox OrangeIslands_Route_10_Text_West, MSGBOX_SIGN
	end

OrangeIslands_Route_10_EventScript_Debris::
	msgbox OrangeIslands_Route_10_Text_Debris, MSGBOX_SIGN
	end

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Kinnow
OrangeIslands_Route_10_EventScript_SeaEdgeToKinnow::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8005, 40
	goto_if_lt OrangeIslands_Route_10_EventScript_SeaEdgeToKinnow_Lo
	compare VAR_0x8005, 67
	goto_if_gt OrangeIslands_Route_10_EventScript_SeaEdgeToKinnow_Hi
OrangeIslands_Route_10_EventScript_SeaEdgeToKinnow_Go:
	subvar VAR_0x8005, 40
	setvar VAR_0x8004, 1
	warp MAP_ORANGE_ISLANDS_KINNOW, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Route_10_EventScript_SeaEdgeToKinnow_Lo:
	setvar VAR_0x8005, 40
	goto OrangeIslands_Route_10_EventScript_SeaEdgeToKinnow_Go
OrangeIslands_Route_10_EventScript_SeaEdgeToKinnow_Hi:
	setvar VAR_0x8005, 67
	goto OrangeIslands_Route_10_EventScript_SeaEdgeToKinnow_Go

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Route 17
OrangeIslands_Route_10_EventScript_SeaEdgeToRoute17::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8005, 0
	goto_if_lt OrangeIslands_Route_10_EventScript_SeaEdgeToRoute17_Lo
	compare VAR_0x8005, 13
	goto_if_gt OrangeIslands_Route_10_EventScript_SeaEdgeToRoute17_Hi
OrangeIslands_Route_10_EventScript_SeaEdgeToRoute17_Go:
	addvar VAR_0x8005, 42
	setvar VAR_0x8004, 38
	warp MAP_ORANGE_ISLANDS_ROUTE_17, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Route_10_EventScript_SeaEdgeToRoute17_Lo:
	setvar VAR_0x8005, 0
	goto OrangeIslands_Route_10_EventScript_SeaEdgeToRoute17_Go
OrangeIslands_Route_10_EventScript_SeaEdgeToRoute17_Hi:
	setvar VAR_0x8005, 13
	goto OrangeIslands_Route_10_EventScript_SeaEdgeToRoute17_Go

@ ---- Orange Islands trainers (orange_route_trainers.py) ----
OrangeIslands_Route_10_EventScript_OrangeVera::
	trainerbattle_single TRAINER_ORANGE_SWIMMER_F_VERA, OrangeIslands_Route_10_Text_OrangeVeraIntro, OrangeIslands_Route_10_Text_OrangeVeraDefeat
	msgbox OrangeIslands_Route_10_Text_OrangeVeraPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Route_10_EventScript_OrangeJade::
	trainerbattle_single TRAINER_ORANGE_SWIMMER_F_JADE, OrangeIslands_Route_10_Text_OrangeJadeIntro, OrangeIslands_Route_10_Text_OrangeJadeDefeat
	msgbox OrangeIslands_Route_10_Text_OrangeJadePostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Route_10_EventScript_OrangeCallie::
	trainerbattle_single TRAINER_ORANGE_SWIMMER_F_CALLIE, OrangeIslands_Route_10_Text_OrangeCallieIntro, OrangeIslands_Route_10_Text_OrangeCallieDefeat
	msgbox OrangeIslands_Route_10_Text_OrangeCalliePostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Route_10_EventScript_OrangeLance::
	trainerbattle_single TRAINER_ORANGE_SWIMMER_M_LANCE, OrangeIslands_Route_10_Text_OrangeLanceIntro, OrangeIslands_Route_10_Text_OrangeLanceDefeat
	msgbox OrangeIslands_Route_10_Text_OrangeLancePostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Route_10_EventScript_OrangeDana::
	trainerbattle_single TRAINER_ORANGE_SWIMMER_F_DANA, OrangeIslands_Route_10_Text_OrangeDanaIntro, OrangeIslands_Route_10_Text_OrangeDanaDefeat
	msgbox OrangeIslands_Route_10_Text_OrangeDanaPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Route_10_EventScript_OrangeJason::
	trainerbattle_single TRAINER_ORANGE_SWIMMER_M_JASON, OrangeIslands_Route_10_Text_OrangeJasonIntro, OrangeIslands_Route_10_Text_OrangeJasonDefeat
	msgbox OrangeIslands_Route_10_Text_OrangeJasonPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Route_10_Text_OrangeVeraIntro::
	.string "Swimming in the ORANGE\n"
	.string "ISLANDS is the best!$"

OrangeIslands_Route_10_Text_OrangeVeraDefeat::
	.string "Oh, I lost!$"

OrangeIslands_Route_10_Text_OrangeVeraPostBattle::
	.string "The reefs are so pretty\n"
	.string "from out here.$"

OrangeIslands_Route_10_Text_OrangeJadeIntro::
	.string "My POKéMON and I swim\n"
	.string "together every day!$"

OrangeIslands_Route_10_Text_OrangeJadeDefeat::
	.string "We'll swim harder!$"

OrangeIslands_Route_10_Text_OrangeJadePostBattle::
	.string "Each island has its own\n"
	.string "kind of water. Isn't it neat?$"

OrangeIslands_Route_10_Text_OrangeCallieIntro::
	.string "Swimming in the ORANGE\n"
	.string "ISLANDS is the best!$"

OrangeIslands_Route_10_Text_OrangeCallieDefeat::
	.string "Oh, I lost!$"

OrangeIslands_Route_10_Text_OrangeCalliePostBattle::
	.string "The reefs are so pretty\n"
	.string "from out here.$"

OrangeIslands_Route_10_Text_OrangeLanceIntro::
	.string "I train my POKéMON in the\n"
	.string "open sea. Ready?$"

OrangeIslands_Route_10_Text_OrangeLanceDefeat::
	.string "Washed out!$"

OrangeIslands_Route_10_Text_OrangeLancePostBattle::
	.string "Out here, WATER POKéMON are\n"
	.string "the real champions.$"

OrangeIslands_Route_10_Text_OrangeDanaIntro::
	.string "Hey! You splashed me!\n"
	.string "That means a battle!$"

OrangeIslands_Route_10_Text_OrangeDanaDefeat::
	.string "Fine, you win.$"

OrangeIslands_Route_10_Text_OrangeDanaPostBattle::
	.string "Watch out for TENTACOOL\n"
	.string "near the reefs.$"

OrangeIslands_Route_10_Text_OrangeJasonIntro::
	.string "I train my POKéMON in the\n"
	.string "open sea. Ready?$"

OrangeIslands_Route_10_Text_OrangeJasonDefeat::
	.string "Washed out!$"

OrangeIslands_Route_10_Text_OrangeJasonPostBattle::
	.string "Out here, WATER POKéMON are\n"
	.string "the real champions.$"

