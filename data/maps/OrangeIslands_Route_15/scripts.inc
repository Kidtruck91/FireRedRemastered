OrangeIslands_Route_15_MapScripts::
	.byte 0

@ ---- Orange Islands trainers (orange_route_trainers.py) ----
OrangeIslands_Route_15_EventScript_OrangeNick::
	trainerbattle_single TRAINER_ORANGE_BIRD_KEEPER_NICK, OrangeIslands_Route_15_Text_OrangeNickIntro, OrangeIslands_Route_15_Text_OrangeNickDefeat
	msgbox OrangeIslands_Route_15_Text_OrangeNickPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Route_15_EventScript_OrangeEvan::
	trainerbattle_single TRAINER_ORANGE_FISHERMAN_EVAN, OrangeIslands_Route_15_Text_OrangeEvanIntro, OrangeIslands_Route_15_Text_OrangeEvanDefeat
	msgbox OrangeIslands_Route_15_Text_OrangeEvanPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Route_15_EventScript_OrangeKira::
	trainerbattle_single TRAINER_ORANGE_SWIMMER_F_KIRA, OrangeIslands_Route_15_Text_OrangeKiraIntro, OrangeIslands_Route_15_Text_OrangeKiraDefeat
	msgbox OrangeIslands_Route_15_Text_OrangeKiraPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Route_15_EventScript_OrangeElise::
	trainerbattle_single TRAINER_ORANGE_SWIMMER_F_ELISE, OrangeIslands_Route_15_Text_OrangeEliseIntro, OrangeIslands_Route_15_Text_OrangeEliseDefeat
	msgbox OrangeIslands_Route_15_Text_OrangeElisePostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Route_15_Text_OrangeNickIntro::
	.string "The wind here is perfect\n"
	.string "for flying!$"

OrangeIslands_Route_15_Text_OrangeNickDefeat::
	.string "Feathers everywhere...$"

OrangeIslands_Route_15_Text_OrangeNickPostBattle::
	.string "A good flyer reads the\n"
	.string "sea wind.$"

OrangeIslands_Route_15_Text_OrangeEvanIntro::
	.string "Look at this catch!\n"
	.string "Want to see it in battle?$"

OrangeIslands_Route_15_Text_OrangeEvanDefeat::
	.string "It got away...$"

OrangeIslands_Route_15_Text_OrangeEvanPostBattle::
	.string "Fishing is like battling.\n"
	.string "Timing is everything.$"

OrangeIslands_Route_15_Text_OrangeKiraIntro::
	.string "Swimming in the ORANGE\n"
	.string "ISLANDS is the best!$"

OrangeIslands_Route_15_Text_OrangeKiraDefeat::
	.string "Oh, I lost!$"

OrangeIslands_Route_15_Text_OrangeKiraPostBattle::
	.string "The reefs are so pretty\n"
	.string "from out here.$"

OrangeIslands_Route_15_Text_OrangeEliseIntro::
	.string "Swimming in the ORANGE\n"
	.string "ISLANDS is the best!$"

OrangeIslands_Route_15_Text_OrangeEliseDefeat::
	.string "Oh, I lost!$"

OrangeIslands_Route_15_Text_OrangeElisePostBattle::
	.string "The reefs are so pretty\n"
	.string "from out here.$"

