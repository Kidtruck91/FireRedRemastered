Route37_MapScripts::
	.byte 0

@ ---- Johto route trainers (johto_route_trainers.py) ----
Route37_EventScript_JohtoAnnAnne::
	trainerbattle_double TRAINER_JOHTO_TWINS_ANN_ANNE, Route37_Text_JohtoAnnAnneIntro, Route37_Text_JohtoAnnAnneDefeat, Route37_Text_JohtoAnnAnneNotEnough
	msgbox Route37_Text_JohtoAnnAnnePostBattle, MSGBOX_AUTOCLOSE
	end

Route37_EventScript_JohtoAnnAnne2::
	trainerbattle_double TRAINER_JOHTO_TWINS_ANN_ANNE, Route37_Text_JohtoAnnAnne2Intro, Route37_Text_JohtoAnnAnne2Defeat, Route37_Text_JohtoAnnAnne2NotEnough
	msgbox Route37_Text_JohtoAnnAnne2PostBattle, MSGBOX_AUTOCLOSE
	end

Route37_EventScript_JohtoGreg::
	trainerbattle_single TRAINER_JOHTO_PSYCHIC_GREG, Route37_Text_JohtoGregIntro, Route37_Text_JohtoGregDefeat
	msgbox Route37_Text_JohtoGregPostBattle, MSGBOX_AUTOCLOSE
	end

Route37_Text_JohtoAnnAnneIntro::
	.string "ANN: ANNE and I are in this\n"
	.string "to- gether!$"

Route37_Text_JohtoAnnAnneDefeat::
	.string "ANN & ANNE: Nnn… A little too\n"
	.string "strong.$"

Route37_Text_JohtoAnnAnnePostBattle::
	.string "ANN: I can tell what my sister\n"
	.string "and\p"
	.string "my POKéMON are thinking.$"

Route37_Text_JohtoAnnAnneNotEnough::
	.string "We battle two on two! Come\n"
	.string "back with two POKéMON!$"

Route37_Text_JohtoAnnAnne2Intro::
	.string "ANNE: ANN and I are in this\n"
	.string "to- gether!$"

Route37_Text_JohtoAnnAnne2Defeat::
	.string "ANN & ANNE: Nnn… A little too\n"
	.string "strong.$"

Route37_Text_JohtoAnnAnne2PostBattle::
	.string "ANNE: We share the same\n"
	.string "feelings as our POKéMON.$"

Route37_Text_JohtoAnnAnne2NotEnough::
	.string "We battle two on two! Come\n"
	.string "back with two POKéMON!$"

Route37_Text_JohtoGregIntro::
	.string "POKéMON can't do a thing if\n"
	.string "they are asleep.\p"
	.string "I'll show you how scary that\n"
	.string "is!$"

Route37_Text_JohtoGregDefeat::
	.string "I lost. That's pretty sad…$"

Route37_Text_JohtoGregPostBattle::
	.string "Putting POKéMON to sleep or\n"
	.string "paralyz- ing them are good\l"
	.string "battle techniques.$"

