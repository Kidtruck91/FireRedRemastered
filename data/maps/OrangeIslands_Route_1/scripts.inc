OrangeIslands_Route_1_MapScripts::
	.byte 0

OrangeIslands_Route_1_EventScript_Ferry::
	msgbox OrangeIslands_Route_1_Text_Ferry, MSGBOX_SIGN
	end

OrangeIslands_Route_1_EventScript_Beach::
	msgbox OrangeIslands_Route_1_Text_Beach, MSGBOX_SIGN
	end

@ ---- Orange Islands trainers (orange_route_trainers.py) ----
OrangeIslands_Route_1_EventScript_OrangeJack::
	trainerbattle_single TRAINER_ORANGE_FISHERMAN_JACK, OrangeIslands_Route_1_Text_OrangeJackIntro, OrangeIslands_Route_1_Text_OrangeJackDefeat
	msgbox OrangeIslands_Route_1_Text_OrangeJackPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Route_1_EventScript_OrangeOwen::
	trainerbattle_single TRAINER_ORANGE_BIRD_KEEPER_OWEN, OrangeIslands_Route_1_Text_OrangeOwenIntro, OrangeIslands_Route_1_Text_OrangeOwenDefeat
	msgbox OrangeIslands_Route_1_Text_OrangeOwenPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Route_1_EventScript_OrangeDusty::
	trainerbattle_single TRAINER_ORANGE_SWIMMER_M_DUSTY, OrangeIslands_Route_1_Text_OrangeDustyIntro, OrangeIslands_Route_1_Text_OrangeDustyDefeat
	msgbox OrangeIslands_Route_1_Text_OrangeDustyPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Route_1_EventScript_OrangeHarvey::
	trainerbattle_single TRAINER_ORANGE_SWIMMER_M_HARVEY, OrangeIslands_Route_1_Text_OrangeHarveyIntro, OrangeIslands_Route_1_Text_OrangeHarveyDefeat
	msgbox OrangeIslands_Route_1_Text_OrangeHarveyPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Route_1_Text_OrangeJackIntro::
	.string "Look at this catch!\n"
	.string "Want to see it in battle?$"

OrangeIslands_Route_1_Text_OrangeJackDefeat::
	.string "It got away...$"

OrangeIslands_Route_1_Text_OrangeJackPostBattle::
	.string "Fishing is like battling.\n"
	.string "Timing is everything.$"

OrangeIslands_Route_1_Text_OrangeOwenIntro::
	.string "Look up! My POKéMON rule\n"
	.string "the island skies!$"

OrangeIslands_Route_1_Text_OrangeOwenDefeat::
	.string "Shot down!$"

OrangeIslands_Route_1_Text_OrangeOwenPostBattle::
	.string "PELIPPER and WINGULL nest\n"
	.string "on the cliffs here.$"

OrangeIslands_Route_1_Text_OrangeDustyIntro::
	.string "I train my POKéMON in the\n"
	.string "open sea. Ready?$"

OrangeIslands_Route_1_Text_OrangeDustyDefeat::
	.string "Washed out!$"

OrangeIslands_Route_1_Text_OrangeDustyPostBattle::
	.string "Out here, WATER POKéMON are\n"
	.string "the real champions.$"

OrangeIslands_Route_1_Text_OrangeHarveyIntro::
	.string "I train my POKéMON in the\n"
	.string "open sea. Ready?$"

OrangeIslands_Route_1_Text_OrangeHarveyDefeat::
	.string "Washed out!$"

OrangeIslands_Route_1_Text_OrangeHarveyPostBattle::
	.string "Out here, WATER POKéMON are\n"
	.string "the real champions.$"

