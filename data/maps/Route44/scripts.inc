Route44_MapScripts::
	.byte 0

@ ---- Johto route trainers (johto_route_trainers.py) ----
Route44_EventScript_JohtoWilton::
	trainerbattle_single TRAINER_JOHTO_FISHERMAN_WILTON, Route44_Text_JohtoWiltonIntro, Route44_Text_JohtoWiltonDefeat
	msgbox Route44_Text_JohtoWiltonPostBattle, MSGBOX_AUTOCLOSE
	end

Route44_EventScript_JohtoEdgar::
	trainerbattle_single TRAINER_JOHTO_FISHERMAN_EDGAR, Route44_Text_JohtoEdgarIntro, Route44_Text_JohtoEdgarDefeat
	msgbox Route44_Text_JohtoEdgarPostBattle, MSGBOX_AUTOCLOSE
	end

Route44_EventScript_JohtoPhil::
	trainerbattle_single TRAINER_JOHTO_PSYCHIC_PHIL, Route44_Text_JohtoPhilIntro, Route44_Text_JohtoPhilDefeat
	msgbox Route44_Text_JohtoPhilPostBattle, MSGBOX_AUTOCLOSE
	end

Route44_EventScript_JohtoZach::
	trainerbattle_single TRAINER_JOHTO_POKEMANIAC_ZACH, Route44_Text_JohtoZachIntro, Route44_Text_JohtoZachDefeat
	msgbox Route44_Text_JohtoZachPostBattle, MSGBOX_AUTOCLOSE
	end

Route44_EventScript_JohtoVance::
	trainerbattle_single TRAINER_JOHTO_BIRD_KEEPER_VANCE, Route44_Text_JohtoVanceIntro, Route44_Text_JohtoVanceDefeat
	msgbox Route44_Text_JohtoVancePostBattle, MSGBOX_AUTOCLOSE
	end

Route44_EventScript_JohtoAllen::
	trainerbattle_single TRAINER_JOHTO_COOLTRAINER_ALLEN, Route44_Text_JohtoAllenIntro, Route44_Text_JohtoAllenDefeat
	msgbox Route44_Text_JohtoAllenPostBattle, MSGBOX_AUTOCLOSE
	end

Route44_EventScript_JohtoCybil::
	trainerbattle_single TRAINER_JOHTO_COOLTRAINER_CYBIL, Route44_Text_JohtoCybilIntro, Route44_Text_JohtoCybilDefeat
	msgbox Route44_Text_JohtoCybilPostBattle, MSGBOX_AUTOCLOSE
	end

Route44_Text_JohtoWiltonIntro::
	.string "Aack! You made me lose a\n"
	.string "POLIWAG!\p"
	.string "What are you going to do about\n"
	.string "it?$"

Route44_Text_JohtoWiltonDefeat::
	.string "Just forget about it.$"

Route44_Text_JohtoWiltonPostBattle::
	.string "That POLIWAG that got away… It\n"
	.string "was huge.\p"
	.string "I swear it must've been 16\n"
	.string "feet long!$"

Route44_Text_JohtoEdgarIntro::
	.string "I fish until I can't anymore.\p"
	.string "I also battle until I drop.\p"
	.string "That's my relationship with my\n"
	.string "POKéMON.$"

Route44_Text_JohtoEdgarDefeat::
	.string "Hmmmm… This isn't right.$"

Route44_Text_JohtoEdgarPostBattle::
	.string "That's 100 wins to 101 losses.\p"
	.string "I won't battle again for a\n"
	.string "while.$"

Route44_Text_JohtoPhilIntro::
	.string "I'm gonna win, for sure!$"

Route44_Text_JohtoPhilDefeat::
	.string "Arrgh… That's a shocking loss…$"

Route44_Text_JohtoPhilPostBattle::
	.string "It's important to have\n"
	.string "conviction on your side.$"

Route44_Text_JohtoZachIntro::
	.string "I'll do anything to get my\n"
	.string "hands on rare POKéMON!$"

Route44_Text_JohtoZachDefeat::
	.string "Oooh, your POKéMON are so\n"
	.string "appealing.$"

Route44_Text_JohtoZachPostBattle::
	.string "If a POKéMON has different\n"
	.string "colors\p"
	.string "from usual, it's more\n"
	.string "valuable.\p"
	.string "What? You mean that's not\n"
	.string "true?$"

Route44_Text_JohtoVanceIntro::
	.string "Do you know about the\n"
	.string "legendary bird POKéMON?$"

Route44_Text_JohtoVanceDefeat::
	.string "Whew! You're hot stuff.$"

Route44_Text_JohtoVancePostBattle::
	.string "ARTICUNO, ZAPDOS and MOLTRES\n"
	.string "are\p"
	.string "the three legend- ary bird\n"
	.string "POKéMON.\p"
	.string "I heard there are other\n"
	.string "legendary birds, though.$"

Route44_Text_JohtoAllenIntro::
	.string "I can tell you're a good\n"
	.string "trainer by looking at you.\p"
	.string "I'm good at POKé- MON, and I\n"
	.string "can see that you are too.$"

Route44_Text_JohtoAllenDefeat::
	.string "Tch! It's a total loss on my\n"
	.string "part.$"

Route44_Text_JohtoAllenPostBattle::
	.string "Wow. You have a lot of BADGES.\p"
	.string "No wonder you're so tough.\p"
	.string "I wonder if ECRUTEAK GYM's\p"
	.string "MORTY is still in training?\p"
	.string "He really hammered me.$"

Route44_Text_JohtoCybilIntro::
	.string "You look strong.\p"
	.string "Good trainers seek tough\n"
	.string "opponents instinctively.$"

Route44_Text_JohtoCybilDefeat::
	.string "Nope! This won't do at all.$"

Route44_Text_JohtoCybilPostBattle::
	.string "We all get better by\n"
	.string "experiencing many battles.\p"
	.string "I battled a lot to become what\n"
	.string "I am today--an elite.$"

