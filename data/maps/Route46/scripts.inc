Route46_MapScripts::
	.byte 0

@ ---- Johto route trainers (johto_route_trainers.py) ----
Route46_EventScript_JohtoBailey::
	trainerbattle_single TRAINER_JOHTO_HIKER_BAILEY, Route46_Text_JohtoBaileyIntro, Route46_Text_JohtoBaileyDefeat
	msgbox Route46_Text_JohtoBaileyPostBattle, MSGBOX_AUTOCLOSE
	end

Route46_EventScript_JohtoTed::
	trainerbattle_single TRAINER_JOHTO_CAMPER_TED, Route46_Text_JohtoTedIntro, Route46_Text_JohtoTedDefeat
	msgbox Route46_Text_JohtoTedPostBattle, MSGBOX_AUTOCLOSE
	end

Route46_EventScript_JohtoErin::
	trainerbattle_single TRAINER_JOHTO_PICNICKER_ERIN, Route46_Text_JohtoErinIntro, Route46_Text_JohtoErinDefeat
	msgbox Route46_Text_JohtoErinPostBattle, MSGBOX_AUTOCLOSE
	end

Route46_Text_JohtoBaileyIntro::
	.string "Awright! I'll show you the\n"
	.string "power of mountain POKéMON!$"

Route46_Text_JohtoBaileyDefeat::
	.string "Mercy! You showed me your\n"
	.string "power!$"

Route46_Text_JohtoBaileyPostBattle::
	.string "It's over. I don't mind. We\n"
	.string "HIKERS are like that.$"

Route46_Text_JohtoTedIntro::
	.string "I'm raising POKé- MON too!\p"
	.string "Will you battle with me?$"

Route46_Text_JohtoTedDefeat::
	.string "Wha…?$"

Route46_Text_JohtoTedPostBattle::
	.string "I did my best but came up\n"
	.string "short.\p"
	.string "No excuses--I admit I lost.$"

Route46_Text_JohtoErinIntro::
	.string "I raise POKéMON too!\p"
	.string "Will you battle with me?$"

Route46_Text_JohtoErinDefeat::
	.string "Oh, rats!$"

Route46_Text_JohtoErinPostBattle::
	.string "I've been to many GYMS, but\n"
	.string "the GYM\p"
	.string "in GOLDENROD is my favorite.\p"
	.string "It's filled with pretty\n"
	.string "flowers!$"

