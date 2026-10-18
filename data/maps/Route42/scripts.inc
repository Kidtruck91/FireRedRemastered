Route42_MapScripts::
	.byte 0

@ ---- Johto route trainers (johto_route_trainers.py) ----
Route42_EventScript_JohtoTully::
	trainerbattle_single TRAINER_JOHTO_FISHERMAN_TULLY, Route42_Text_JohtoTullyIntro, Route42_Text_JohtoTullyDefeat
	msgbox Route42_Text_JohtoTullyPostBattle, MSGBOX_AUTOCLOSE
	end

Route42_EventScript_JohtoBenjamin::
	trainerbattle_single TRAINER_JOHTO_HIKER_BENJAMIN, Route42_Text_JohtoBenjaminIntro, Route42_Text_JohtoBenjaminDefeat
	msgbox Route42_Text_JohtoBenjaminPostBattle, MSGBOX_AUTOCLOSE
	end

Route42_EventScript_JohtoShane::
	trainerbattle_single TRAINER_JOHTO_POKEMANIAC_SHANE, Route42_Text_JohtoShaneIntro, Route42_Text_JohtoShaneDefeat
	msgbox Route42_Text_JohtoShanePostBattle, MSGBOX_AUTOCLOSE
	end

Route42_Text_JohtoTullyIntro::
	.string "Let me demonstrate the power\n"
	.string "of the POKéMON I caught!$"

Route42_Text_JohtoTullyDefeat::
	.string "What? That's not right.$"

Route42_Text_JohtoTullyPostBattle::
	.string "I want to become the trainer\n"
	.string "CHAMP\p"
	.string "using the POKéMON I caught.\p"
	.string "That's the best part of\n"
	.string "fishing!$"

Route42_Text_JohtoBenjaminIntro::
	.string "Ah, it's good to be outside! I\n"
	.string "feel so free!$"

Route42_Text_JohtoBenjaminDefeat::
	.string "Gahahah!$"

Route42_Text_JohtoBenjaminPostBattle::
	.string "Losing feels in- significant\n"
	.string "if you\p"
	.string "look up at the big sky!$"

Route42_Text_JohtoShaneIntro::
	.string "HEY!\p"
	.string "This is my secret place! Get\n"
	.string "lost, you outsider!$"

Route42_Text_JohtoShaneDefeat::
	.string "I should have used my MOON\n"
	.string "STONE…$"

Route42_Text_JohtoShanePostBattle::
	.string "You're working on a POKéDEX?\p"
	.string "Wow, you must know some pretty\n"
	.string "rare POKéMON!\p"
	.string "May I please see it. Please?$"

