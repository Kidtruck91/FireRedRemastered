Route30_MapScripts::
	.byte 0

@ ---- Johto route trainers (johto_route_trainers.py) ----
Route30_EventScript_JohtoJoey::
	trainerbattle_single TRAINER_JOHTO_YOUNGSTER_JOEY, Route30_Text_JohtoJoeyIntro, Route30_Text_JohtoJoeyDefeat
	msgbox Route30_Text_JohtoJoeyPostBattle, MSGBOX_AUTOCLOSE
	end

Route30_EventScript_JohtoMikey::
	trainerbattle_single TRAINER_JOHTO_YOUNGSTER_MIKEY, Route30_Text_JohtoMikeyIntro, Route30_Text_JohtoMikeyDefeat
	msgbox Route30_Text_JohtoMikeyPostBattle, MSGBOX_AUTOCLOSE
	end

Route30_EventScript_JohtoDon::
	trainerbattle_single TRAINER_JOHTO_BUG_CATCHER_DON, Route30_Text_JohtoDonIntro, Route30_Text_JohtoDonDefeat
	msgbox Route30_Text_JohtoDonPostBattle, MSGBOX_AUTOCLOSE
	end

Route30_Text_JohtoJoeyIntro::
	.string "I just lost, so I'm trying to\n"
	.string "find more POKéMON.\p"
	.string "Wait! You look weak! Come on,\n"
	.string "let's battle!$"

Route30_Text_JohtoJoeyDefeat::
	.string "Ack! I lost again! Doggone it!$"

Route30_Text_JohtoJoeyPostBattle::
	.string "Do I have to have more POKéMON\n"
	.string "in\p"
	.string "order to battle better?\p"
	.string "No! I'm sticking with this one\n"
	.string "no matter what!$"

Route30_Text_JohtoMikeyIntro::
	.string "You're a POKéMON trainer,\n"
	.string "right?\p"
	.string "Then you have to battle!$"

Route30_Text_JohtoMikeyDefeat::
	.string "That's strange. I won before.$"

Route30_Text_JohtoMikeyPostBattle::
	.string "Becoming a good trainer is\n"
	.string "really tough.\p"
	.string "I'm going to bat- tle other\n"
	.string "people to get better.$"

Route30_Text_JohtoDonIntro::
	.string "Instead of a bug POKéMON, I\n"
	.string "found a trainer!$"

Route30_Text_JohtoDonDefeat::
	.string "Argh! You're too strong!$"

Route30_Text_JohtoDonPostBattle::
	.string "I ran out of POKé BALLS while\n"
	.string "I was catching POKéMON.\p"
	.string "I should've bought some more…$"

