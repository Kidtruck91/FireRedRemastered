OrangeIslands_Mikan_Beach_MapScripts::
	.byte 0

OrangeIslands_Mikan_Beach_EventScript_Town::
	msgbox OrangeIslands_Mikan_Beach_Text_Town, MSGBOX_SIGN
	end

OrangeIslands_Mikan_Beach_EventScript_South::
	msgbox OrangeIslands_Mikan_Beach_Text_South, MSGBOX_SIGN
	end

OrangeIslands_Mikan_Beach_EventScript_West::
	msgbox OrangeIslands_Mikan_Beach_Text_West, MSGBOX_SIGN
	end

OrangeIslands_Mikan_Beach_EventScript_North::
	msgbox OrangeIslands_Mikan_Beach_Text_North, MSGBOX_SIGN
	end

OrangeIslands_Mikan_Beach_EventScript_Harbor::
	msgbox OrangeIslands_Mikan_Beach_Text_Harbor, MSGBOX_SIGN
	end

@ ---- Orange Islands trainers (orange_route_trainers.py) ----
OrangeIslands_Mikan_Beach_EventScript_OrangeAlma::
	trainerbattle_single TRAINER_ORANGE_TUBER_ALMA, OrangeIslands_Mikan_Beach_Text_OrangeAlmaIntro, OrangeIslands_Mikan_Beach_Text_OrangeAlmaDefeat
	msgbox OrangeIslands_Mikan_Beach_Text_OrangeAlmaPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Mikan_Beach_EventScript_OrangeKirby::
	trainerbattle_single TRAINER_ORANGE_TUBER_KIRBY, OrangeIslands_Mikan_Beach_Text_OrangeKirbyIntro, OrangeIslands_Mikan_Beach_Text_OrangeKirbyDefeat
	msgbox OrangeIslands_Mikan_Beach_Text_OrangeKirbyPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Mikan_Beach_EventScript_OrangeNoah::
	trainerbattle_single TRAINER_ORANGE_SWIMMER_M_NOAH, OrangeIslands_Mikan_Beach_Text_OrangeNoahIntro, OrangeIslands_Mikan_Beach_Text_OrangeNoahDefeat
	msgbox OrangeIslands_Mikan_Beach_Text_OrangeNoahPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Mikan_Beach_EventScript_OrangeIan::
	trainerbattle_single TRAINER_ORANGE_SWIMMER_M_IAN, OrangeIslands_Mikan_Beach_Text_OrangeIanIntro, OrangeIslands_Mikan_Beach_Text_OrangeIanDefeat
	msgbox OrangeIslands_Mikan_Beach_Text_OrangeIanPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Mikan_Beach_Text_OrangeAlmaIntro::
	.string "I can't swim yet, but my\n"
	.string "POKéMON can!$"

OrangeIslands_Mikan_Beach_Text_OrangeAlmaDefeat::
	.string "Waaah!$"

OrangeIslands_Mikan_Beach_Text_OrangeAlmaPostBattle::
	.string "When I grow up, I'll swim\n"
	.string "to every island!$"

OrangeIslands_Mikan_Beach_Text_OrangeKirbyIntro::
	.string "I can't swim yet, but my\n"
	.string "POKéMON can!$"

OrangeIslands_Mikan_Beach_Text_OrangeKirbyDefeat::
	.string "Waaah!$"

OrangeIslands_Mikan_Beach_Text_OrangeKirbyPostBattle::
	.string "When I grow up, I'll swim\n"
	.string "to every island!$"

OrangeIslands_Mikan_Beach_Text_OrangeNoahIntro::
	.string "You're surfing? Then you\n"
	.string "can't escape me!$"

OrangeIslands_Mikan_Beach_Text_OrangeNoahDefeat::
	.string "Glub... You win.$"

OrangeIslands_Mikan_Beach_Text_OrangeNoahPostBattle::
	.string "The currents here are strong.\n"
	.string "Watch where they push you.$"

OrangeIslands_Mikan_Beach_Text_OrangeIanIntro::
	.string "I train my POKéMON in the\n"
	.string "open sea. Ready?$"

OrangeIslands_Mikan_Beach_Text_OrangeIanDefeat::
	.string "Washed out!$"

OrangeIslands_Mikan_Beach_Text_OrangeIanPostBattle::
	.string "Out here, WATER POKéMON are\n"
	.string "the real champions.$"

OrangeIslands_Mikan_Beach_EventScript_GymGuide::
	msgbox OrangeIslands_Mikan_Beach_Text_GymGuide, MSGBOX_NPC
	end

OrangeIslands_Mikan_Beach_EventScript_Swimmer::
	msgbox OrangeIslands_Mikan_Beach_Text_Swimmer, MSGBOX_NPC
	end

OrangeIslands_Mikan_Beach_EventScript_Child::
	msgbox OrangeIslands_Mikan_Beach_Text_Child, MSGBOX_NPC
	end

OrangeIslands_Mikan_Beach_EventScript_Fisher::
	msgbox OrangeIslands_Mikan_Beach_Text_Fisher, MSGBOX_NPC
	end

