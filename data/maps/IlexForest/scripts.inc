IlexForest_MapScripts::
	.byte 0

@ ---- Johto route trainers (johto_route_trainers.py) ----
IlexForest_EventScript_JohtoWayne::
	trainerbattle_single TRAINER_JOHTO_BUG_CATCHER_WAYNE, IlexForest_Text_JohtoWayneIntro, IlexForest_Text_JohtoWayneDefeat
	msgbox IlexForest_Text_JohtoWaynePostBattle, MSGBOX_AUTOCLOSE
	end

IlexForest_Text_JohtoWayneIntro::
	.string "Don't sneak up on me like\n"
	.string "that!\p"
	.string "You frightened a POKéMON away!$"

IlexForest_Text_JohtoWayneDefeat::
	.string "I hadn't seen that POKéMON\n"
	.string "before…$"

IlexForest_Text_JohtoWaynePostBattle::
	.string "A POKéMON I've never seen\n"
	.string "before\p"
	.string "fell out of the tree when I\n"
	.string "used HEADBUTT.\p"
	.string "I ought to use HEADBUTT in\n"
	.string "other places too.$"

