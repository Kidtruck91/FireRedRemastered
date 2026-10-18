Route45_MapScripts::
	.byte 0

@ ---- Johto route trainers (johto_route_trainers.py) ----
Route45_EventScript_JohtoErik::
	trainerbattle_single TRAINER_JOHTO_HIKER_ERIK, Route45_Text_JohtoErikIntro, Route45_Text_JohtoErikDefeat
	msgbox Route45_Text_JohtoErikPostBattle, MSGBOX_AUTOCLOSE
	end

Route45_EventScript_JohtoMichael::
	trainerbattle_single TRAINER_JOHTO_HIKER_MICHAEL, Route45_Text_JohtoMichaelIntro, Route45_Text_JohtoMichaelDefeat
	msgbox Route45_Text_JohtoMichaelPostBattle, MSGBOX_AUTOCLOSE
	end

Route45_EventScript_JohtoParry::
	trainerbattle_single TRAINER_JOHTO_HIKER_PARRY, Route45_Text_JohtoParryIntro, Route45_Text_JohtoParryDefeat
	msgbox Route45_Text_JohtoParryPostBattle, MSGBOX_AUTOCLOSE
	end

Route45_EventScript_JohtoTimothy::
	trainerbattle_single TRAINER_JOHTO_HIKER_TIMOTHY, Route45_Text_JohtoTimothyIntro, Route45_Text_JohtoTimothyDefeat
	msgbox Route45_Text_JohtoTimothyPostBattle, MSGBOX_AUTOCLOSE
	end

Route45_EventScript_JohtoKenji::
	trainerbattle_single TRAINER_JOHTO_BLACK_BELT_KENJI, Route45_Text_JohtoKenjiIntro, Route45_Text_JohtoKenjiDefeat
	msgbox Route45_Text_JohtoKenjiPostBattle, MSGBOX_AUTOCLOSE
	end

Route45_EventScript_JohtoRyan::
	trainerbattle_single TRAINER_JOHTO_COOLTRAINER_RYAN, Route45_Text_JohtoRyanIntro, Route45_Text_JohtoRyanDefeat
	msgbox Route45_Text_JohtoRyanPostBattle, MSGBOX_AUTOCLOSE
	end

Route45_EventScript_JohtoKelly::
	trainerbattle_single TRAINER_JOHTO_COOLTRAINER_KELLY, Route45_Text_JohtoKellyIntro, Route45_Text_JohtoKellyDefeat
	msgbox Route45_Text_JohtoKellyPostBattle, MSGBOX_AUTOCLOSE
	end

Route45_Text_JohtoErikIntro::
	.string "Be prepared for anything!\p"
	.string "Let me see if your POKéMON\n"
	.string "have been raised properly!$"

Route45_Text_JohtoErikDefeat::
	.string "Oh, I lost that!$"

Route45_Text_JohtoErikPostBattle::
	.string "I'll head back to BLACKTHORN's\n"
	.string "ICE\p"
	.string "PATH and train some more.$"

Route45_Text_JohtoMichaelIntro::
	.string "Yo! You're spunky! But you\n"
	.string "know what?\p"
	.string "When it comes to sheer\n"
	.string "spunkiness, I'm the man!$"

Route45_Text_JohtoMichaelDefeat::
	.string "My POKéMON weren't spunky\n"
	.string "enough!$"

Route45_Text_JohtoMichaelPostBattle::
	.string "Boy, do I love HP UP! Mmmm,\n"
	.string "yum!\p"
	.string "I keep drinking my POKéMON's!\p"
	.string "I can't help it!$"

Route45_Text_JohtoParryIntro::
	.string "My POKéMON are power packed!$"

Route45_Text_JohtoParryDefeat::
	.string "Wahahah! I'm the big loser!$"

Route45_Text_JohtoParryPostBattle::
	.string "I'm not much good at thinking,\n"
	.string "see?\p"
	.string "So, I just plow ahead with\n"
	.string "power!$"

Route45_Text_JohtoTimothyIntro::
	.string "Why do I climb mountains?\p"
	.string "Because they're there.\p"
	.string "Why do I train POKéMON?\p"
	.string "Because they're there!$"

Route45_Text_JohtoTimothyDefeat::
	.string "Losses… They're there too!$"

Route45_Text_JohtoTimothyPostBattle::
	.string "The best thing to ever happen\n"
	.string "to me\p"
	.string "was discovering POKéMON.$"

Route45_Text_JohtoKenjiIntro::
	.string "I was training here alone.\p"
	.string "Behold the fruits of my labor!$"

Route45_Text_JohtoKenjiDefeat::
	.string "Waaaargh!$"

Route45_Text_JohtoKenjiPostBattle::
	.string "This calls for extreme\n"
	.string "measures.\p"
	.string "I must take to the hills and\n"
	.string "train in solitude.$"

Route45_Text_JohtoRyanIntro::
	.string "What are your thoughts on\n"
	.string "rais- ing POKéMON?$"

Route45_Text_JohtoRyanDefeat::
	.string "You've won my respect.$"

Route45_Text_JohtoRyanPostBattle::
	.string "I see you're rais- ing your\n"
	.string "POKéMON with care.\p"
	.string "The bond you build will save\n"
	.string "you in tough situations.$"

Route45_Text_JohtoKellyIntro::
	.string "What is your battle strategy?\p"
	.string "It is foolish to use strong\n"
	.string "moves indiscriminately.$"

Route45_Text_JohtoKellyDefeat::
	.string "Fine. I lost.$"

Route45_Text_JohtoKellyPostBattle::
	.string "I'm not in favor of overly\n"
	.string "power- ful moves.\p"
	.string "I want to win, but I also\n"
	.string "don't want to harm POKéMON.$"

