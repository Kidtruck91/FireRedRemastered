Route40_MapScripts::
	.byte 0

@ ---- Johto route trainers (johto_route_trainers.py) ----
Route40_EventScript_JohtoSimon::
	trainerbattle_single TRAINER_JOHTO_SWIMMER_M_SIMON, Route40_Text_JohtoSimonIntro, Route40_Text_JohtoSimonDefeat
	msgbox Route40_Text_JohtoSimonPostBattle, MSGBOX_AUTOCLOSE
	end

Route40_EventScript_JohtoRandall::
	trainerbattle_single TRAINER_JOHTO_SWIMMER_M_RANDALL, Route40_Text_JohtoRandallIntro, Route40_Text_JohtoRandallDefeat
	msgbox Route40_Text_JohtoRandallPostBattle, MSGBOX_AUTOCLOSE
	end

Route40_EventScript_JohtoElaine::
	trainerbattle_single TRAINER_JOHTO_SWIMMER_F_ELAINE, Route40_Text_JohtoElaineIntro, Route40_Text_JohtoElaineDefeat
	msgbox Route40_Text_JohtoElainePostBattle, MSGBOX_AUTOCLOSE
	end

Route40_EventScript_JohtoPaula::
	trainerbattle_single TRAINER_JOHTO_SWIMMER_F_PAULA, Route40_Text_JohtoPaulaIntro, Route40_Text_JohtoPaulaDefeat
	msgbox Route40_Text_JohtoPaulaPostBattle, MSGBOX_AUTOCLOSE
	end

Route40_Text_JohtoSimonIntro::
	.string "You have to warm up before\n"
	.string "going into the water.\p"
	.string "That's basic.$"

Route40_Text_JohtoSimonDefeat::
	.string "OK! Uncle! I give!$"

Route40_Text_JohtoSimonPostBattle::
	.string "CIANWOOD CITY is a good\n"
	.string "distance away from here.$"

Route40_Text_JohtoRandallIntro::
	.string "Hey, you're young and fit!\p"
	.string "Don't ride your POKéMON! Swim!$"

Route40_Text_JohtoRandallDefeat::
	.string "Uh-oh. I lost…$"

Route40_Text_JohtoRandallPostBattle::
	.string "Swimming exercises your entire\n"
	.string "body. It's healthy.$"

Route40_Text_JohtoElaineIntro::
	.string "Are you going to CIANWOOD?\p"
	.string "How about a quick battle\n"
	.string "first?$"

Route40_Text_JohtoElaineDefeat::
	.string "I lost that one!$"

Route40_Text_JohtoElainePostBattle::
	.string "I'd say I'm a bet- ter swimmer\n"
	.string "than you. Yeah!$"

Route40_Text_JohtoPaulaIntro::
	.string "No inner tube for me.\p"
	.string "I'm hanging on to a sea\n"
	.string "POKéMON!$"

Route40_Text_JohtoPaulaDefeat::
	.string "Ooh, I'm feeling dizzy!$"

Route40_Text_JohtoPaulaPostBattle::
	.string "While I float like this, the\n"
	.string "waves carry me along.$"

