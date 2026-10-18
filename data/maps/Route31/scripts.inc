Route31_MapScripts::
	.byte 0

@ ---- Johto route trainers (johto_route_trainers.py) ----
Route31_EventScript_JohtoWade::
	trainerbattle_single TRAINER_JOHTO_BUG_CATCHER_WADE, Route31_Text_JohtoWadeIntro, Route31_Text_JohtoWadeDefeat
	msgbox Route31_Text_JohtoWadePostBattle, MSGBOX_AUTOCLOSE
	end

Route31_Text_JohtoWadeIntro::
	.string "I caught a bunch of POKéMON.\n"
	.string "Let me battle with you!$"

Route31_Text_JohtoWadeDefeat::
	.string "Awwwww…$"

Route31_Text_JohtoWadePostBattle::
	.string "You can catch POKéMON even if\p"
	.string "you have six with you.\p"
	.string "If you catch one, it'll go to\n"
	.string "your BOX automatically.$"

