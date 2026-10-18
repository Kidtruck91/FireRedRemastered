Route36_MapScripts::
	.byte 0

@ ---- Johto route trainers (johto_route_trainers.py) ----
Route36_EventScript_JohtoMark::
	trainerbattle_single TRAINER_JOHTO_PSYCHIC_MARK, Route36_Text_JohtoMarkIntro, Route36_Text_JohtoMarkDefeat
	msgbox Route36_Text_JohtoMarkPostBattle, MSGBOX_AUTOCLOSE
	end

Route36_EventScript_JohtoAlan::
	trainerbattle_single TRAINER_JOHTO_SCHOOL_KID_ALAN, Route36_Text_JohtoAlanIntro, Route36_Text_JohtoAlanDefeat
	msgbox Route36_Text_JohtoAlanPostBattle, MSGBOX_AUTOCLOSE
	end

Route36_Text_JohtoMarkIntro::
	.string "I'm going to read your\n"
	.string "thoughts!$"

Route36_Text_JohtoMarkDefeat::
	.string "I misread you!$"

Route36_Text_JohtoMarkPostBattle::
	.string "I'd be strong if only I could\n"
	.string "tell\p"
	.string "what my opponent was thinking.$"

Route36_Text_JohtoAlanIntro::
	.string "Thanks to my stud- ies, I'm\n"
	.string "ready for any POKéMON!$"

Route36_Text_JohtoAlanDefeat::
	.string "Oops! Computation error?$"

Route36_Text_JohtoAlanPostBattle::
	.string "Darn. I study five hours a day\n"
	.string "too.\p"
	.string "There's more to learning than\n"
	.string "just reading books.$"

