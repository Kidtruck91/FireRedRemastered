OrangeIslands_Route_16_MapScripts::
	.byte 0

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Route 13
OrangeIslands_Route_16_EventScript_SeaEdgeToRoute13::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8005, 42
	goto_if_lt OrangeIslands_Route_16_EventScript_SeaEdgeToRoute13_Lo
	compare VAR_0x8005, 61
	goto_if_gt OrangeIslands_Route_16_EventScript_SeaEdgeToRoute13_Hi
OrangeIslands_Route_16_EventScript_SeaEdgeToRoute13_Go:
	subvar VAR_0x8005, 12
	setvar VAR_0x8004, 1
	warp MAP_ORANGE_ISLANDS_ROUTE_13, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Route_16_EventScript_SeaEdgeToRoute13_Lo:
	setvar VAR_0x8005, 42
	goto OrangeIslands_Route_16_EventScript_SeaEdgeToRoute13_Go
OrangeIslands_Route_16_EventScript_SeaEdgeToRoute13_Hi:
	setvar VAR_0x8005, 61
	goto OrangeIslands_Route_16_EventScript_SeaEdgeToRoute13_Go

@ ---- Orange Islands trainers (orange_route_trainers.py) ----
OrangeIslands_Route_16_EventScript_OrangeEric::
	trainerbattle_single TRAINER_ORANGE_FISHERMAN_ERIC, OrangeIslands_Route_16_Text_OrangeEricIntro, OrangeIslands_Route_16_Text_OrangeEricDefeat
	msgbox OrangeIslands_Route_16_Text_OrangeEricPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Route_16_EventScript_OrangeJim::
	trainerbattle_single TRAINER_ORANGE_HIKER_JIM, OrangeIslands_Route_16_Text_OrangeJimIntro, OrangeIslands_Route_16_Text_OrangeJimDefeat
	msgbox OrangeIslands_Route_16_Text_OrangeJimPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Route_16_EventScript_OrangeCaleb::
	trainerbattle_single TRAINER_ORANGE_SWIMMER_M_CALEB, OrangeIslands_Route_16_Text_OrangeCalebIntro, OrangeIslands_Route_16_Text_OrangeCalebDefeat
	msgbox OrangeIslands_Route_16_Text_OrangeCalebPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Route_16_EventScript_OrangeStan::
	trainerbattle_single TRAINER_ORANGE_SWIMMER_M_STAN, OrangeIslands_Route_16_Text_OrangeStanIntro, OrangeIslands_Route_16_Text_OrangeStanDefeat
	msgbox OrangeIslands_Route_16_Text_OrangeStanPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Route_16_Text_OrangeEricIntro::
	.string "I've been waiting all day\n"
	.string "for a bite. You'll do!$"

OrangeIslands_Route_16_Text_OrangeEricDefeat::
	.string "Snapped my line!$"

OrangeIslands_Route_16_Text_OrangeEricPostBattle::
	.string "Rumor says a huge GYARADOS\n"
	.string "lives near these islands.$"

OrangeIslands_Route_16_Text_OrangeJimIntro::
	.string "Hey, kid! Test your team\n"
	.string "against my rocks!$"

OrangeIslands_Route_16_Text_OrangeJimDefeat::
	.string "Crumbled!$"

OrangeIslands_Route_16_Text_OrangeJimPostBattle::
	.string "Good boots are a hiker's\n"
	.string "best friend.$"

OrangeIslands_Route_16_Text_OrangeCalebIntro::
	.string "I train my POKéMON in the\n"
	.string "open sea. Ready?$"

OrangeIslands_Route_16_Text_OrangeCalebDefeat::
	.string "Washed out!$"

OrangeIslands_Route_16_Text_OrangeCalebPostBattle::
	.string "Out here, WATER POKéMON are\n"
	.string "the real champions.$"

OrangeIslands_Route_16_Text_OrangeStanIntro::
	.string "You're surfing? Then you\n"
	.string "can't escape me!$"

OrangeIslands_Route_16_Text_OrangeStanDefeat::
	.string "Glub... You win.$"

OrangeIslands_Route_16_Text_OrangeStanPostBattle::
	.string "The currents here are strong.\n"
	.string "Watch where they push you.$"

