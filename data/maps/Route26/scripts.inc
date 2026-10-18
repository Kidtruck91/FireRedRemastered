Route26_MapScripts::
	.byte 0

@ ---- Johto route trainers (johto_route_trainers.py) ----
Route26_EventScript_JohtoJake::
	trainerbattle_single TRAINER_JOHTO_COOLTRAINER_JAKE, Route26_Text_JohtoJakeIntro, Route26_Text_JohtoJakeDefeat
	msgbox Route26_Text_JohtoJakePostBattle, MSGBOX_AUTOCLOSE
	end

Route26_EventScript_JohtoGaven::
	trainerbattle_single TRAINER_JOHTO_COOLTRAINER_GAVEN, Route26_Text_JohtoGavenIntro, Route26_Text_JohtoGavenDefeat
	msgbox Route26_Text_JohtoGavenPostBattle, MSGBOX_AUTOCLOSE
	end

Route26_EventScript_JohtoJoyce::
	trainerbattle_single TRAINER_JOHTO_COOLTRAINER_JOYCE, Route26_Text_JohtoJoyceIntro, Route26_Text_JohtoJoyceDefeat
	msgbox Route26_Text_JohtoJoycePostBattle, MSGBOX_AUTOCLOSE
	end

Route26_EventScript_JohtoBeth::
	trainerbattle_single TRAINER_JOHTO_COOLTRAINER_BETH, Route26_Text_JohtoBethIntro, Route26_Text_JohtoBethDefeat
	msgbox Route26_Text_JohtoBethPostBattle, MSGBOX_AUTOCLOSE
	end

Route26_EventScript_JohtoRichard::
	trainerbattle_single TRAINER_JOHTO_PSYCHIC_RICHARD, Route26_Text_JohtoRichardIntro, Route26_Text_JohtoRichardDefeat
	msgbox Route26_Text_JohtoRichardPostBattle, MSGBOX_AUTOCLOSE
	end

Route26_EventScript_JohtoScott::
	trainerbattle_single TRAINER_JOHTO_FISHERMAN_SCOTT, Route26_Text_JohtoScottIntro, Route26_Text_JohtoScottDefeat
	msgbox Route26_Text_JohtoScottPostBattle, MSGBOX_AUTOCLOSE
	end

Route26_Text_JohtoJakeIntro::
	.string "I'm making my final\n"
	.string "preparations\p"
	.string "for the POKéMON LEAGUE.$"

Route26_Text_JohtoJakeDefeat::
	.string "I blew it!$"

Route26_Text_JohtoJakePostBattle::
	.string "It's going to be tough to win\n"
	.string "at the LEAGUE.\p"
	.string "I need to do some more\n"
	.string "training.\p"
	.string "I hear that the LEAGUE's ELITE\p"
	.string "FOUR are tougher than GYM\n"
	.string "LEADERS.$"

Route26_Text_JohtoGavenIntro::
	.string "By experiencing tough battles,\n"
	.string "you gain power.$"

Route26_Text_JohtoGavenDefeat::
	.string "Gaah! Life is even tougher!$"

Route26_Text_JohtoGavenPostBattle::
	.string "To get to POKéMON LEAGUE, you\n"
	.string "have\p"
	.string "to get through VICTORY ROAD.\p"
	.string "But VICTORY ROAD is tough.\p"
	.string "Practically nobody goes there!$"

Route26_Text_JohtoJoyceIntro::
	.string "Since you've come this far,\n"
	.string "you must be good.\p"
	.string "I'm going to give this battle\n"
	.string "every- thing I've got!$"

Route26_Text_JohtoJoyceDefeat::
	.string "No! I don't believe this!$"

Route26_Text_JohtoJoycePostBattle::
	.string "I've defeated eight GYM\n"
	.string "LEADERS,\p"
	.string "so I was feeling confident.\p"
	.string "I'll have to try harder next\n"
	.string "time.$"

Route26_Text_JohtoBethIntro::
	.string "I lost to a train- er named\n"
	.string "{RIVAL}.\p"
	.string "He was really strong, but…\p"
	.string "It was as if he absolutely had\n"
	.string "to win at any cost.\p"
	.string "I felt sorry for his POKéMON.$"

Route26_Text_JohtoBethDefeat::
	.string "POKéMON aren't tools of war.$"

Route26_Text_JohtoBethPostBattle::
	.string "POKéMON are in- valuable,\n"
	.string "lifelong partners.$"

Route26_Text_JohtoRichardIntro::
	.string "Wow, look at all those BADGES!\n"
	.string "I'm impressed.\p"
	.string "But you're not satisfied by\n"
	.string "just\p"
	.string "collecting them, right?$"

Route26_Text_JohtoRichardDefeat::
	.string "Good battle!$"

Route26_Text_JohtoRichardPostBattle::
	.string "People and POKéMON grow from\n"
	.string "their experiences.\p"
	.string "Don't get lazy and complacent.$"

Route26_Text_JohtoScottIntro::
	.string "I'm feeling great today!\p"
	.string "I feel like I could boot even\n"
	.string "the LEAGUE CHAMP!$"

Route26_Text_JohtoScottDefeat::
	.string "No! Not in this battle!$"

Route26_Text_JohtoScottPostBattle::
	.string "Just like in fish- ing, it's\n"
	.string "all over\p"
	.string "in POKéMON if you give up.$"

