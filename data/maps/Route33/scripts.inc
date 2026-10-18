Route33_MapScripts::
	.byte 0

@ ---- Johto route trainers (johto_route_trainers.py) ----
Route33_EventScript_JohtoAnthony::
	trainerbattle_single TRAINER_JOHTO_HIKER_ANTHONY, Route33_Text_JohtoAnthonyIntro, Route33_Text_JohtoAnthonyDefeat
	msgbox Route33_Text_JohtoAnthonyPostBattle, MSGBOX_AUTOCLOSE
	end

Route33_Text_JohtoAnthonyIntro::
	.string "I came through the tunnel, but\n"
	.string "I\p"
	.string "still have plenty of energy\n"
	.string "left.$"

Route33_Text_JohtoAnthonyDefeat::
	.string "Whoa! You've got more zip than\n"
	.string "me!$"

Route33_Text_JohtoAnthonyPostBattle::
	.string "We HIKERS are at our best in\n"
	.string "the mountains.$"

