OrangeIslands_Route_18_MapScripts::
	.byte 0

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Route 13
OrangeIslands_Route_18_EventScript_SeaEdgeToRoute13::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8004, 24
	goto_if_lt OrangeIslands_Route_18_EventScript_SeaEdgeToRoute13_Lo
	compare VAR_0x8004, 71
	goto_if_gt OrangeIslands_Route_18_EventScript_SeaEdgeToRoute13_Hi
OrangeIslands_Route_18_EventScript_SeaEdgeToRoute13_Go:
	subvar VAR_0x8004, 4
	setvar VAR_0x8005, 78
	warp MAP_ORANGE_ISLANDS_ROUTE_13, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Route_18_EventScript_SeaEdgeToRoute13_Lo:
	setvar VAR_0x8004, 24
	goto OrangeIslands_Route_18_EventScript_SeaEdgeToRoute13_Go
OrangeIslands_Route_18_EventScript_SeaEdgeToRoute13_Hi:
	setvar VAR_0x8004, 71
	goto OrangeIslands_Route_18_EventScript_SeaEdgeToRoute13_Go

@ ---- Orange Islands trainers (orange_route_trainers.py) ----
OrangeIslands_Route_18_EventScript_OrangeHector::
	trainerbattle_single TRAINER_ORANGE_FISHERMAN_HECTOR, OrangeIslands_Route_18_Text_OrangeHectorIntro, OrangeIslands_Route_18_Text_OrangeHectorDefeat
	msgbox OrangeIslands_Route_18_Text_OrangeHectorPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Route_18_EventScript_OrangeWade::
	trainerbattle_single TRAINER_ORANGE_BIRD_KEEPER_WADE, OrangeIslands_Route_18_Text_OrangeWadeIntro, OrangeIslands_Route_18_Text_OrangeWadeDefeat
	msgbox OrangeIslands_Route_18_Text_OrangeWadePostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Route_18_EventScript_OrangeLeon::
	trainerbattle_single TRAINER_ORANGE_SWIMMER_M_LEON, OrangeIslands_Route_18_Text_OrangeLeonIntro, OrangeIslands_Route_18_Text_OrangeLeonDefeat
	msgbox OrangeIslands_Route_18_Text_OrangeLeonPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Route_18_EventScript_OrangeErica::
	trainerbattle_single TRAINER_ORANGE_SWIMMER_F_ERICA, OrangeIslands_Route_18_Text_OrangeEricaIntro, OrangeIslands_Route_18_Text_OrangeEricaDefeat
	msgbox OrangeIslands_Route_18_Text_OrangeEricaPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Route_18_Text_OrangeHectorIntro::
	.string "I've been waiting all day\n"
	.string "for a bite. You'll do!$"

OrangeIslands_Route_18_Text_OrangeHectorDefeat::
	.string "Snapped my line!$"

OrangeIslands_Route_18_Text_OrangeHectorPostBattle::
	.string "Rumor says a huge GYARADOS\n"
	.string "lives near these islands.$"

OrangeIslands_Route_18_Text_OrangeWadeIntro::
	.string "The wind here is perfect\n"
	.string "for flying!$"

OrangeIslands_Route_18_Text_OrangeWadeDefeat::
	.string "Feathers everywhere...$"

OrangeIslands_Route_18_Text_OrangeWadePostBattle::
	.string "A good flyer reads the\n"
	.string "sea wind.$"

OrangeIslands_Route_18_Text_OrangeLeonIntro::
	.string "I train my POKéMON in the\n"
	.string "open sea. Ready?$"

OrangeIslands_Route_18_Text_OrangeLeonDefeat::
	.string "Washed out!$"

OrangeIslands_Route_18_Text_OrangeLeonPostBattle::
	.string "Out here, WATER POKéMON are\n"
	.string "the real champions.$"

OrangeIslands_Route_18_Text_OrangeEricaIntro::
	.string "Swimming in the ORANGE\n"
	.string "ISLANDS is the best!$"

OrangeIslands_Route_18_Text_OrangeEricaDefeat::
	.string "Oh, I lost!$"

OrangeIslands_Route_18_Text_OrangeEricaPostBattle::
	.string "The reefs are so pretty\n"
	.string "from out here.$"

