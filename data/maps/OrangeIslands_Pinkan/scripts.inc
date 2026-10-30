OrangeIslands_Pinkan_MapScripts::
	.byte 0

OrangeIslands_Pinkan_EventScript_Beach::
	msgbox OrangeIslands_Pinkan_Text_Beach, MSGBOX_SIGN
	end

OrangeIslands_Pinkan_EventScript_Town::
	msgbox OrangeIslands_Pinkan_Text_Town, MSGBOX_SIGN
	end

@ ---- Orange Islands trainers (orange_route_trainers.py) ----
OrangeIslands_Pinkan_EventScript_OrangeNate::
	trainerbattle_single TRAINER_ORANGE_COOLTRAINER_NATE, OrangeIslands_Pinkan_Text_OrangeNateIntro, OrangeIslands_Pinkan_Text_OrangeNateDefeat
	msgbox OrangeIslands_Pinkan_Text_OrangeNatePostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Pinkan_EventScript_OrangeMeg::
	trainerbattle_single TRAINER_ORANGE_PICNICKER_MEG, OrangeIslands_Pinkan_Text_OrangeMegIntro, OrangeIslands_Pinkan_Text_OrangeMegDefeat
	msgbox OrangeIslands_Pinkan_Text_OrangeMegPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Pinkan_EventScript_OrangeGlenn::
	trainerbattle_single TRAINER_ORANGE_SWIMMER_M_GLENN, OrangeIslands_Pinkan_Text_OrangeGlennIntro, OrangeIslands_Pinkan_Text_OrangeGlennDefeat
	msgbox OrangeIslands_Pinkan_Text_OrangeGlennPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Pinkan_Text_OrangeNateIntro::
	.string "I came here to beat the\n"
	.string "ORANGE CREW. You're first!$"

OrangeIslands_Pinkan_Text_OrangeNateDefeat::
	.string "Not bad at all.$"

OrangeIslands_Pinkan_Text_OrangeNatePostBattle::
	.string "Every gym here has its own\n"
	.string "rules. Be ready.$"

OrangeIslands_Pinkan_Text_OrangeMegIntro::
	.string "The sun's out! Perfect\n"
	.string "battle weather!$"

OrangeIslands_Pinkan_Text_OrangeMegDefeat::
	.string "Oh, you're good!$"

OrangeIslands_Pinkan_Text_OrangeMegPostBattle::
	.string "I love the island fruit\n"
	.string "here. So juicy!$"

OrangeIslands_Pinkan_Text_OrangeGlennIntro::
	.string "The water's perfect today!\n"
	.string "Let's battle out here!$"

OrangeIslands_Pinkan_Text_OrangeGlennDefeat::
	.string "I'm sinking!$"

OrangeIslands_Pinkan_Text_OrangeGlennPostBattle::
	.string "I swim between the islands\n"
	.string "every morning.$"

@ ---- Pinkan reserve (orange_orphans.py) ----
OrangeIslands_Pinkan_EventScript_Warden::
	lock
	faceplayer
	msgbox OrangeIslands_Pinkan_Text_Warden
	release
	end

OrangeIslands_Pinkan_Text_Warden::
	.string "Hold it. This whole island is a\n"
	.string "reserve, and I'm its warden.\p"
	.string "The berries here turn whatever\n"
	.string "eats them pink. Every POKéMON on\l"
	.string "PINKAN is pink to the tail.\p"
	.string "You can look. You can battle.\n"
	.string "You do not take one off this\l"
	.string "island. Understood?\p"
	.string "The shore path north runs down to\n"
	.string "the beach if you want the long\l"
	.string "way round.$"
