NationalPark_MapScripts::
	.byte 0

@ ---- Johto route trainers (johto_route_trainers.py) ----
NationalPark_EventScript_JohtoJack::
	trainerbattle_single TRAINER_JOHTO_SCHOOL_KID_JACK, NationalPark_Text_JohtoJackIntro, NationalPark_Text_JohtoJackDefeat
	msgbox NationalPark_Text_JohtoJackPostBattle, MSGBOX_AUTOCLOSE
	end

NationalPark_EventScript_JohtoBeverly::
	trainerbattle_single TRAINER_JOHTO_POKEFAN_BEVERLY, NationalPark_Text_JohtoBeverlyIntro, NationalPark_Text_JohtoBeverlyDefeat
	msgbox NationalPark_Text_JohtoBeverlyPostBattle, MSGBOX_AUTOCLOSE
	end

NationalPark_EventScript_JohtoWilliam::
	trainerbattle_single TRAINER_JOHTO_POKEFAN_WILLIAM, NationalPark_Text_JohtoWilliamIntro, NationalPark_Text_JohtoWilliamDefeat
	msgbox NationalPark_Text_JohtoWilliamPostBattle, MSGBOX_AUTOCLOSE
	end

NationalPark_EventScript_JohtoKrise::
	trainerbattle_single TRAINER_JOHTO_LASS_KRISE, NationalPark_Text_JohtoKriseIntro, NationalPark_Text_JohtoKriseDefeat
	msgbox NationalPark_Text_JohtoKrisePostBattle, MSGBOX_AUTOCLOSE
	end

NationalPark_Text_JohtoJackIntro::
	.string "The world of POKéMON is deep.\p"
	.string "There are still lots of things\n"
	.string "we don't know.\p"
	.string "But I know more than you do!$"

NationalPark_Text_JohtoJackDefeat::
	.string "Wha-wha-what?$"

NationalPark_Text_JohtoJackPostBattle::
	.string "There is a lot to learn.\p"
	.string "For example…\p"
	.string "There are 50 kinds of TMs.\p"
	.string "Traded POKéMON level up\n"
	.string "faster.$"

NationalPark_Text_JohtoBeverlyIntro::
	.string "My POKéMON are simply darling.\p"
	.string "Let me tell you how proud my\n"
	.string "darlings make me.$"

NationalPark_Text_JohtoBeverlyDefeat::
	.string "I can beat you in pride, but…$"

NationalPark_Text_JohtoBeverlyPostBattle::
	.string "I must say, your POKéMON are\n"
	.string "quite cute, too.$"

NationalPark_Text_JohtoWilliamIntro::
	.string "We adore our POKé- MON, even\n"
	.string "if they dislike us.\p"
	.string "That's what being a FAN is\n"
	.string "about.$"

NationalPark_Text_JohtoWilliamDefeat::
	.string "M-my POKéMON!$"

NationalPark_Text_JohtoWilliamPostBattle::
	.string "I lost the battle, but my\n"
	.string "POKéMON win\p"
	.string "the prize for being most\n"
	.string "lovely.$"

NationalPark_Text_JohtoKriseIntro::
	.string "Hello? Why are you staring at\n"
	.string "me?\p"
	.string "Oh, a battle?$"

NationalPark_Text_JohtoKriseDefeat::
	.string "…Hmmm…$"

NationalPark_Text_JohtoKrisePostBattle::
	.string "I thought you were staring at\n"
	.string "me because I'm cute!$"

