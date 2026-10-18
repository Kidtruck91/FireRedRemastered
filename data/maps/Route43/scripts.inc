Route43_MapScripts::
	.byte 0

@ ---- Johto route trainers (johto_route_trainers.py) ----
Route43_EventScript_JohtoBen::
	trainerbattle_single TRAINER_JOHTO_POKEMANIAC_BEN, Route43_Text_JohtoBenIntro, Route43_Text_JohtoBenDefeat
	msgbox Route43_Text_JohtoBenPostBattle, MSGBOX_AUTOCLOSE
	end

Route43_EventScript_JohtoBrent::
	trainerbattle_single TRAINER_JOHTO_POKEMANIAC_BRENT, Route43_Text_JohtoBrentIntro, Route43_Text_JohtoBrentDefeat
	msgbox Route43_Text_JohtoBrentPostBattle, MSGBOX_AUTOCLOSE
	end

Route43_EventScript_JohtoRon::
	trainerbattle_single TRAINER_JOHTO_POKEMANIAC_RON, Route43_Text_JohtoRonIntro, Route43_Text_JohtoRonDefeat
	msgbox Route43_Text_JohtoRonPostBattle, MSGBOX_AUTOCLOSE
	end

Route43_EventScript_JohtoMarvin::
	trainerbattle_single TRAINER_JOHTO_FISHERMAN_MARVIN, Route43_Text_JohtoMarvinIntro, Route43_Text_JohtoMarvinDefeat
	msgbox Route43_Text_JohtoMarvinPostBattle, MSGBOX_AUTOCLOSE
	end

Route43_EventScript_JohtoTiffany::
	trainerbattle_single TRAINER_JOHTO_PICNICKER_TIFFANY, Route43_Text_JohtoTiffanyIntro, Route43_Text_JohtoTiffanyDefeat
	msgbox Route43_Text_JohtoTiffanyPostBattle, MSGBOX_AUTOCLOSE
	end

Route43_EventScript_JohtoSpencer::
	trainerbattle_single TRAINER_JOHTO_CAMPER_SPENCER, Route43_Text_JohtoSpencerIntro, Route43_Text_JohtoSpencerDefeat
	msgbox Route43_Text_JohtoSpencerPostBattle, MSGBOX_AUTOCLOSE
	end

Route43_Text_JohtoBenIntro::
	.string "I love POKéMON!\p"
	.string "That's why I started--and why\p"
	.string "I'll keep on col- lecting\n"
	.string "POKéMON!$"

Route43_Text_JohtoBenDefeat::
	.string "How could you do this to me?$"

Route43_Text_JohtoBenPostBattle::
	.string "What else do I like besides\n"
	.string "POKéMON?\p"
	.string "MARY on the radio. I bet she's\n"
	.string "cute!$"

Route43_Text_JohtoBrentIntro::
	.string "Hey! Do you have any rare\n"
	.string "POKéMON?$"

Route43_Text_JohtoBrentDefeat::
	.string "Oh, my poor POKé- MON!\n"
	.string "Darlings!$"

Route43_Text_JohtoBrentPostBattle::
	.string "I'd be happy just to own a\n"
	.string "single rare POKéMON.$"

Route43_Text_JohtoRonIntro::
	.string "Would you get this?\p"
	.string "Some {RIVAL} guy made fun of\n"
	.string "my POKéMON!\p"
	.string "Darn it! My POKé- MON's great!$"

Route43_Text_JohtoRonDefeat::
	.string "My NIDOKING did pretty right\n"
	.string "on!$"

Route43_Text_JohtoRonPostBattle::
	.string "It's okay for people to like\p"
	.string "different types of POKéMON.\p"
	.string "POKéMON isn't just about\n"
	.string "having the most powerful one.$"

Route43_Text_JohtoMarvinIntro::
	.string "I'm in a slump.\p"
	.string "Maybe it's the gear I'm using.\p"
	.string "Let's battle for a change of\n"
	.string "pace!$"

Route43_Text_JohtoMarvinDefeat::
	.string "I lost, but I feel better\n"
	.string "anyway.$"

Route43_Text_JohtoMarvinPostBattle::
	.string "KURT's LURE BALL is the best\n"
	.string "for\p"
	.string "catching hooked POKéMON.\p"
	.string "It's much more effective than\n"
	.string "a ULTRA BALL.$"

Route43_Text_JohtoTiffanyIntro::
	.string "Are you going to LAKE OF RAGE\n"
	.string "too?\p"
	.string "Let's play for a little while!$"

Route43_Text_JohtoTiffanyDefeat::
	.string "I played too much!$"

Route43_Text_JohtoTiffanyPostBattle::
	.string "I'm having a pic- nic with\n"
	.string "POKéMON.\p"
	.string "Won't you join us?$"

Route43_Text_JohtoSpencerIntro::
	.string "I can do so much with my\n"
	.string "POKéMON-- it's super-fun!$"

Route43_Text_JohtoSpencerDefeat::
	.string "Losing isn't fun at all…$"

Route43_Text_JohtoSpencerPostBattle::
	.string "What is going on at LAKE OF\n"
	.string "RAGE?\p"
	.string "We were planning to camp\n"
	.string "there.$"

