OrangeIslands_Sunburst_Cave_MapScripts::
	.byte 0

@ ---- Orange Islands trainers (orange_route_trainers.py) ----
OrangeIslands_Sunburst_Cave_EventScript_OrangeMark::
	trainerbattle_single TRAINER_ORANGE_HIKER_MARK, OrangeIslands_Sunburst_Cave_Text_OrangeMarkIntro, OrangeIslands_Sunburst_Cave_Text_OrangeMarkDefeat
	msgbox OrangeIslands_Sunburst_Cave_Text_OrangeMarkPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Sunburst_Cave_EventScript_OrangeBrady::
	trainerbattle_single TRAINER_ORANGE_HIKER_BRADY, OrangeIslands_Sunburst_Cave_Text_OrangeBradyIntro, OrangeIslands_Sunburst_Cave_Text_OrangeBradyDefeat
	msgbox OrangeIslands_Sunburst_Cave_Text_OrangeBradyPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Sunburst_Cave_Text_OrangeMarkIntro::
	.string "Hey, kid! Test your team\n"
	.string "against my rocks!$"

OrangeIslands_Sunburst_Cave_Text_OrangeMarkDefeat::
	.string "Crumbled!$"

OrangeIslands_Sunburst_Cave_Text_OrangeMarkPostBattle::
	.string "Good boots are a hiker's\n"
	.string "best friend.$"

OrangeIslands_Sunburst_Cave_Text_OrangeBradyIntro::
	.string "Hey, kid! Test your team\n"
	.string "against my rocks!$"

OrangeIslands_Sunburst_Cave_Text_OrangeBradyDefeat::
	.string "Crumbled!$"

OrangeIslands_Sunburst_Cave_Text_OrangeBradyPostBattle::
	.string "Good boots are a hiker's\n"
	.string "best friend.$"

