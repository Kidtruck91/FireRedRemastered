Route38_MapScripts::
	.byte 0

@ ---- Johto route trainers (johto_route_trainers.py) ----
Route38_EventScript_JohtoChad::
	trainerbattle_single TRAINER_JOHTO_SCHOOL_KID_CHAD, Route38_Text_JohtoChadIntro, Route38_Text_JohtoChadDefeat
	msgbox Route38_Text_JohtoChadPostBattle, MSGBOX_AUTOCLOSE
	end

Route38_EventScript_JohtoDana::
	trainerbattle_single TRAINER_JOHTO_LASS_DANA, Route38_Text_JohtoDanaIntro, Route38_Text_JohtoDanaDefeat
	msgbox Route38_Text_JohtoDanaPostBattle, MSGBOX_AUTOCLOSE
	end

Route38_EventScript_JohtoToby::
	trainerbattle_single TRAINER_JOHTO_BIRD_KEEPER_TOBY, Route38_Text_JohtoTobyIntro, Route38_Text_JohtoTobyDefeat
	msgbox Route38_Text_JohtoTobyPostBattle, MSGBOX_AUTOCLOSE
	end

Route38_EventScript_JohtoValerie::
	trainerbattle_single TRAINER_JOHTO_BEAUTY_VALERIE, Route38_Text_JohtoValerieIntro, Route38_Text_JohtoValerieDefeat
	msgbox Route38_Text_JohtoValeriePostBattle, MSGBOX_AUTOCLOSE
	end

Route38_EventScript_JohtoHarry::
	trainerbattle_single TRAINER_JOHTO_SAILOR_HARRY, Route38_Text_JohtoHarryIntro, Route38_Text_JohtoHarryDefeat
	msgbox Route38_Text_JohtoHarryPostBattle, MSGBOX_AUTOCLOSE
	end

Route38_EventScript_JohtoOlivia::
	trainerbattle_single TRAINER_JOHTO_BEAUTY_OLIVIA, Route38_Text_JohtoOliviaIntro, Route38_Text_JohtoOliviaDefeat
	msgbox Route38_Text_JohtoOliviaPostBattle, MSGBOX_AUTOCLOSE
	end

Route38_Text_JohtoChadIntro::
	.string "Let me try some- thing I\n"
	.string "learned today.$"

Route38_Text_JohtoChadDefeat::
	.string "I didn't study enough, I\n"
	.string "guess.$"

Route38_Text_JohtoChadPostBattle::
	.string "I have to take so many tests,\n"
	.string "I\p"
	.string "don't have much time for\n"
	.string "POKéMON.\p"
	.string "So when I do get to play, I\n"
	.string "really concentrate.$"

Route38_Text_JohtoDanaIntro::
	.string "You seem to be good at\n"
	.string "POKéMON.\p"
	.string "If you are, how about giving\n"
	.string "me some advice?$"

Route38_Text_JohtoDanaDefeat::
	.string "I see. So you can battle that\n"
	.string "way.$"

Route38_Text_JohtoDanaPostBattle::
	.string "I know something good!\p"
	.string "MOOMOO FARM's milk is famous\n"
	.string "for its flavor.$"

Route38_Text_JohtoTobyIntro::
	.string "Fly high into the sky, my\n"
	.string "beloved bird POKéMON!$"

Route38_Text_JohtoTobyDefeat::
	.string "I feel like just flying away\n"
	.string "now.$"

Route38_Text_JohtoTobyPostBattle::
	.string "I plan to train in CIANWOOD\n"
	.string "CITY to\p"
	.string "teach my POKéMON how to FLY.$"

Route38_Text_JohtoValerieIntro::
	.string "Hi! Aren't you a cute trainer!\p"
	.string "May I see your POKéMON?$"

Route38_Text_JohtoValerieDefeat::
	.string "I'm glad I got to see your\n"
	.string "POKéMON!$"

Route38_Text_JohtoValeriePostBattle::
	.string "When I see POKé- MON, it seems\n"
	.string "to soothe my nerves.$"

Route38_Text_JohtoHarryIntro::
	.string "I've been over- seas, so I\n"
	.string "know\p"
	.string "about all sorts of POKéMON!$"

Route38_Text_JohtoHarryDefeat::
	.string "Your skill is world class!$"

Route38_Text_JohtoHarryPostBattle::
	.string "All kinds of peo- ple around\n"
	.string "the\p"
	.string "world live happily with\n"
	.string "POKéMON.$"

Route38_Text_JohtoOliviaIntro::
	.string "Don't you think my POKéMON and\n"
	.string "I are beautiful?$"

Route38_Text_JohtoOliviaDefeat::
	.string "We drink MOOMOO MILK every\n"
	.string "day.$"

Route38_Text_JohtoOliviaPostBattle::
	.string "MOOMOO MILK is good for beauty\p"
	.string "and health, but\n"
	.string "inconveniently,\p"
	.string "they only sell a bottle at a\n"
	.string "time.$"

