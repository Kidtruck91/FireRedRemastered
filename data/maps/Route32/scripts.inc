Route32_MapScripts::
	.byte 0

@ ---- Johto route trainers (johto_route_trainers.py) ----
Route32_EventScript_JohtoJustin::
	trainerbattle_single TRAINER_JOHTO_FISHERMAN_JUSTIN, Route32_Text_JohtoJustinIntro, Route32_Text_JohtoJustinDefeat
	msgbox Route32_Text_JohtoJustinPostBattle, MSGBOX_AUTOCLOSE
	end

Route32_EventScript_JohtoRalph::
	trainerbattle_single TRAINER_JOHTO_FISHERMAN_RALPH, Route32_Text_JohtoRalphIntro, Route32_Text_JohtoRalphDefeat
	msgbox Route32_Text_JohtoRalphPostBattle, MSGBOX_AUTOCLOSE
	end

Route32_EventScript_JohtoHenry::
	trainerbattle_single TRAINER_JOHTO_FISHERMAN_HENRY, Route32_Text_JohtoHenryIntro, Route32_Text_JohtoHenryDefeat
	msgbox Route32_Text_JohtoHenryPostBattle, MSGBOX_AUTOCLOSE
	end

Route32_EventScript_JohtoAlbert::
	trainerbattle_single TRAINER_JOHTO_YOUNGSTER_ALBERT, Route32_Text_JohtoAlbertIntro, Route32_Text_JohtoAlbertDefeat
	msgbox Route32_Text_JohtoAlbertPostBattle, MSGBOX_AUTOCLOSE
	end

Route32_EventScript_JohtoGordon::
	trainerbattle_single TRAINER_JOHTO_YOUNGSTER_GORDON, Route32_Text_JohtoGordonIntro, Route32_Text_JohtoGordonDefeat
	msgbox Route32_Text_JohtoGordonPostBattle, MSGBOX_AUTOCLOSE
	end

Route32_EventScript_JohtoRoland::
	trainerbattle_single TRAINER_JOHTO_CAMPER_ROLAND, Route32_Text_JohtoRolandIntro, Route32_Text_JohtoRolandDefeat
	msgbox Route32_Text_JohtoRolandPostBattle, MSGBOX_AUTOCLOSE
	end

Route32_EventScript_JohtoLiz::
	trainerbattle_single TRAINER_JOHTO_PICNICKER_LIZ, Route32_Text_JohtoLizIntro, Route32_Text_JohtoLizDefeat
	msgbox Route32_Text_JohtoLizPostBattle, MSGBOX_AUTOCLOSE
	end

Route32_EventScript_JohtoPeter::
	trainerbattle_single TRAINER_JOHTO_BIRD_KEEPER_PETER, Route32_Text_JohtoPeterIntro, Route32_Text_JohtoPeterDefeat
	msgbox Route32_Text_JohtoPeterPostBattle, MSGBOX_AUTOCLOSE
	end

Route32_Text_JohtoJustinIntro::
	.string "Whoa!\p"
	.string "You made me lose that fish!$"

Route32_Text_JohtoJustinDefeat::
	.string "Sploosh!$"

Route32_Text_JohtoJustinPostBattle::
	.string "Calm, collected… The essence\n"
	.string "of\p"
	.string "fishing and POKé- MON is the\n"
	.string "same.$"

Route32_Text_JohtoRalphIntro::
	.string "I'm really good at both\n"
	.string "fishing and POKéMON.\p"
	.string "I'm not about to lose to any\n"
	.string "kid!$"

Route32_Text_JohtoRalphDefeat::
	.string "Tch! I tried to rush things…$"

Route32_Text_JohtoRalphPostBattle::
	.string "Fishing is a life- long\n"
	.string "passion.\p"
	.string "POKéMON are life- long\n"
	.string "friends!$"

Route32_Text_JohtoHenryIntro::
	.string "My POKéMON? Freshly caught!$"

Route32_Text_JohtoHenryDefeat::
	.string "SPLASH?$"

Route32_Text_JohtoHenryPostBattle::
	.string "Freshly caught POKéMON are no\p"
	.string "match for properly raised\n"
	.string "ones.$"

Route32_Text_JohtoAlbertIntro::
	.string "I haven't seen you around\n"
	.string "before.\p"
	.string "So you think you are pretty\n"
	.string "tough?$"

Route32_Text_JohtoAlbertDefeat::
	.string "You're strong!$"

Route32_Text_JohtoAlbertPostBattle::
	.string "I'm going to try to be the\n"
	.string "best with my favorites.\p"
	.string "I'm not using the same tough\n"
	.string "POKéMON as everyone else.$"

Route32_Text_JohtoGordonIntro::
	.string "I found some good POKéMON in\n"
	.string "the grass!\p"
	.string "I think they'll do it for me!$"

Route32_Text_JohtoGordonDefeat::
	.string "Darn. I thought I could win.$"

Route32_Text_JohtoGordonPostBattle::
	.string "The grass is full of clingy\n"
	.string "things.$"

Route32_Text_JohtoRolandIntro::
	.string "That glance… It's intriguing.$"

Route32_Text_JohtoRolandDefeat::
	.string "Hmmm. This is disappointing.$"

Route32_Text_JohtoRolandPostBattle::
	.string "If you don't want to battle,\n"
	.string "just avoid eye contact.$"

Route32_Text_JohtoLizIntro::
	.string "Uh-huh. Yeah, and you know…\p"
	.string "Pardon? Battle? I'm on the\n"
	.string "phone.\p"
	.string "Oh, all right. But make it\n"
	.string "fast.$"

Route32_Text_JohtoLizDefeat::
	.string "Oh! I've got to relieve my\n"
	.string "anger!$"

Route32_Text_JohtoLizPostBattle::
	.string "I was having a nice chat too.$"

Route32_Text_JohtoPeterIntro::
	.string "That BADGE! It's from VIOLET\n"
	.string "CITY!\p"
	.string "You beat FALKNER?$"

Route32_Text_JohtoPeterDefeat::
	.string "I know what my weaknesses are.$"

Route32_Text_JohtoPeterPostBattle::
	.string "I should train again at the\n"
	.string "GYM in VIOLET CITY.$"

