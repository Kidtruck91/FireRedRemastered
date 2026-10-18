Route27_MapScripts::
	.byte 0

@ ---- Johto route trainers (johto_route_trainers.py) ----
Route27_EventScript_JohtoBlake::
	trainerbattle_single TRAINER_JOHTO_COOLTRAINER_BLAKE, Route27_Text_JohtoBlakeIntro, Route27_Text_JohtoBlakeDefeat
	msgbox Route27_Text_JohtoBlakePostBattle, MSGBOX_AUTOCLOSE
	end

Route27_EventScript_JohtoBrian::
	trainerbattle_single TRAINER_JOHTO_COOLTRAINER_BRIAN, Route27_Text_JohtoBrianIntro, Route27_Text_JohtoBrianDefeat
	msgbox Route27_Text_JohtoBrianPostBattle, MSGBOX_AUTOCLOSE
	end

Route27_EventScript_JohtoReena::
	trainerbattle_single TRAINER_JOHTO_COOLTRAINER_REENA, Route27_Text_JohtoReenaIntro, Route27_Text_JohtoReenaDefeat
	msgbox Route27_Text_JohtoReenaPostBattle, MSGBOX_AUTOCLOSE
	end

Route27_EventScript_JohtoMegan::
	trainerbattle_single TRAINER_JOHTO_COOLTRAINER_MEGAN, Route27_Text_JohtoMeganIntro, Route27_Text_JohtoMeganDefeat
	msgbox Route27_Text_JohtoMeganPostBattle, MSGBOX_AUTOCLOSE
	end

Route27_EventScript_JohtoGilbert::
	trainerbattle_single TRAINER_JOHTO_PSYCHIC_GILBERT, Route27_Text_JohtoGilbertIntro, Route27_Text_JohtoGilbertDefeat
	msgbox Route27_Text_JohtoGilbertPostBattle, MSGBOX_AUTOCLOSE
	end

Route27_EventScript_JohtoJose::
	trainerbattle_single TRAINER_JOHTO_BIRD_KEEPER_JOSE, Route27_Text_JohtoJoseIntro, Route27_Text_JohtoJoseDefeat
	msgbox Route27_Text_JohtoJosePostBattle, MSGBOX_AUTOCLOSE
	end

Route27_Text_JohtoBlakeIntro::
	.string "You look pretty strong. Let me\n"
	.string "battle you!$"

Route27_Text_JohtoBlakeDefeat::
	.string "Yow!$"

Route27_Text_JohtoBlakePostBattle::
	.string "If you prevail on this harsh\n"
	.string "trek,\p"
	.string "the truth will be revealed!\p"
	.string "Heh, sorry, I just wanted to\n"
	.string "say something cool.$"

Route27_Text_JohtoBrianIntro::
	.string "Hm? You're good, aren't you?$"

Route27_Text_JohtoBrianDefeat::
	.string "Just as I thought!$"

Route27_Text_JohtoBrianPostBattle::
	.string "A good trainer can recognize\n"
	.string "other good trainers.$"

Route27_Text_JohtoReenaIntro::
	.string "You shouldn't underestimate\n"
	.string "the\p"
	.string "wild POKéMON in these parts.$"

Route27_Text_JohtoReenaDefeat::
	.string "Oh! You're much too strong!$"

Route27_Text_JohtoReenaPostBattle::
	.string "You're just a kid, but you're\n"
	.string "not to\p"
	.string "be underestimated either.$"

Route27_Text_JohtoMeganIntro::
	.string "It's rare to see anyone come\n"
	.string "here.\p"
	.string "Are you training on your own?$"

Route27_Text_JohtoMeganDefeat::
	.string "Oh! You're really strong!$"

Route27_Text_JohtoMeganPostBattle::
	.string "I'm checking out pre- and\n"
	.string "post- evolution POKéMON.\p"
	.string "Evolution really does make\n"
	.string "POKéMON stronger.\p"
	.string "But evolved forms also learn\n"
	.string "moves later on.$"

Route27_Text_JohtoGilbertIntro::
	.string "Don't say a thing!\p"
	.string "Let me guess what you're\n"
	.string "thinking.\p"
	.string "Mmmmmmm…\p"
	.string "I got it! You're on the\n"
	.string "POKéMON LEAGUE challenge!$"

Route27_Text_JohtoGilbertDefeat::
	.string "You're too much!$"

Route27_Text_JohtoGilbertPostBattle::
	.string "With your skills, you'll do\n"
	.string "well at the LEAGUE.\p"
	.string "That's what my premonition\n"
	.string "says.$"

Route27_Text_JohtoJoseIntro::
	.string "Tweet! Tweet! Tetweet!$"

Route27_Text_JohtoJoseDefeat::
	.string "Tweet!$"

Route27_Text_JohtoJosePostBattle::
	.string "BIRD KEEPERS like me mimic\n"
	.string "bird\p"
	.string "whistles to com- mand POKéMON.$"

