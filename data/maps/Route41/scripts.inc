Route41_MapScripts::
	.byte 0

@ ---- Johto route trainers (johto_route_trainers.py) ----
Route41_EventScript_JohtoCharlie::
	trainerbattle_single TRAINER_JOHTO_SWIMMER_M_CHARLIE, Route41_Text_JohtoCharlieIntro, Route41_Text_JohtoCharlieDefeat
	msgbox Route41_Text_JohtoCharliePostBattle, MSGBOX_AUTOCLOSE
	end

Route41_EventScript_JohtoGeorge::
	trainerbattle_single TRAINER_JOHTO_SWIMMER_M_GEORGE, Route41_Text_JohtoGeorgeIntro, Route41_Text_JohtoGeorgeDefeat
	msgbox Route41_Text_JohtoGeorgePostBattle, MSGBOX_AUTOCLOSE
	end

Route41_EventScript_JohtoBerke::
	trainerbattle_single TRAINER_JOHTO_SWIMMER_M_BERKE, Route41_Text_JohtoBerkeIntro, Route41_Text_JohtoBerkeDefeat
	msgbox Route41_Text_JohtoBerkePostBattle, MSGBOX_AUTOCLOSE
	end

Route41_EventScript_JohtoKirk::
	trainerbattle_single TRAINER_JOHTO_SWIMMER_M_KIRK, Route41_Text_JohtoKirkIntro, Route41_Text_JohtoKirkDefeat
	msgbox Route41_Text_JohtoKirkPostBattle, MSGBOX_AUTOCLOSE
	end

Route41_EventScript_JohtoMathew::
	trainerbattle_single TRAINER_JOHTO_SWIMMER_M_MATHEW, Route41_Text_JohtoMathewIntro, Route41_Text_JohtoMathewDefeat
	msgbox Route41_Text_JohtoMathewPostBattle, MSGBOX_AUTOCLOSE
	end

Route41_EventScript_JohtoKaylee::
	trainerbattle_single TRAINER_JOHTO_SWIMMER_F_KAYLEE, Route41_Text_JohtoKayleeIntro, Route41_Text_JohtoKayleeDefeat
	msgbox Route41_Text_JohtoKayleePostBattle, MSGBOX_AUTOCLOSE
	end

Route41_EventScript_JohtoSusie::
	trainerbattle_single TRAINER_JOHTO_SWIMMER_F_SUSIE, Route41_Text_JohtoSusieIntro, Route41_Text_JohtoSusieDefeat
	msgbox Route41_Text_JohtoSusiePostBattle, MSGBOX_AUTOCLOSE
	end

Route41_EventScript_JohtoDenise::
	trainerbattle_single TRAINER_JOHTO_SWIMMER_F_DENISE, Route41_Text_JohtoDeniseIntro, Route41_Text_JohtoDeniseDefeat
	msgbox Route41_Text_JohtoDenisePostBattle, MSGBOX_AUTOCLOSE
	end

Route41_EventScript_JohtoKara::
	trainerbattle_single TRAINER_JOHTO_SWIMMER_F_KARA, Route41_Text_JohtoKaraIntro, Route41_Text_JohtoKaraDefeat
	msgbox Route41_Text_JohtoKaraPostBattle, MSGBOX_AUTOCLOSE
	end

Route41_EventScript_JohtoWendy::
	trainerbattle_single TRAINER_JOHTO_SWIMMER_F_WENDY, Route41_Text_JohtoWendyIntro, Route41_Text_JohtoWendyDefeat
	msgbox Route41_Text_JohtoWendyPostBattle, MSGBOX_AUTOCLOSE
	end

Route41_Text_JohtoCharlieIntro::
	.string "The water's warm here. I'm\n"
	.string "loose and limber.\p"
	.string "Sure, I'll take you on!$"

Route41_Text_JohtoCharlieDefeat::
	.string "Yikes! I've got prune skin!$"

Route41_Text_JohtoCharliePostBattle::
	.string "Isn't it relaxing just\n"
	.string "floating like this?$"

Route41_Text_JohtoGeorgeIntro::
	.string "I'm a bit tired. If I win,\n"
	.string "lend me your POKéMON.$"

Route41_Text_JohtoGeorgeDefeat::
	.string "Pant, pant…$"

Route41_Text_JohtoGeorgePostBattle::
	.string "It's so far to CIANWOOD.\p"
	.string "But it's no easy return trip\n"
	.string "to OLIVINE either.\p"
	.string "What should I do?$"

Route41_Text_JohtoBerkeIntro::
	.string "See those islands that are\n"
	.string "blocked by whirlpools?\p"
	.string "There just has to be a secret!$"

Route41_Text_JohtoBerkeDefeat::
	.string "What's the secret to your\n"
	.string "strength?$"

Route41_Text_JohtoBerkePostBattle::
	.string "It was a dark and stormy\n"
	.string "night…\p"
	.string "I saw this giant POKéMON\n"
	.string "flying from the islands.\p"
	.string "It was scattering feathers\n"
	.string "from its silver wings.$"

Route41_Text_JohtoKirkIntro::
	.string "The waves are wild here.\p"
	.string "They tire you out while you\n"
	.string "swim.$"

Route41_Text_JohtoKirkDefeat::
	.string "I'm beat!$"

Route41_Text_JohtoKirkPostBattle::
	.string "The currents keep me from\n"
	.string "reaching that island.$"

Route41_Text_JohtoMathewIntro::
	.string "Are you seeking the secrets of\n"
	.string "WHIRL ISLANDS?$"

Route41_Text_JohtoMathewDefeat::
	.string "Ooh, you've got great\n"
	.string "endurance!$"

Route41_Text_JohtoMathewPostBattle::
	.string "A secret about WHIRL ISLANDS…\p"
	.string "It's pitch-black inside!$"

Route41_Text_JohtoKayleeIntro::
	.string "I'm on my way to WHIRL\n"
	.string "ISLANDS.\p"
	.string "I'm going explor- ing with\n"
	.string "friends.$"

Route41_Text_JohtoKayleeDefeat::
	.string "Is that how you do it?$"

Route41_Text_JohtoKayleePostBattle::
	.string "There's supposed to be a big\n"
	.string "POKé- MON deep beneath WHIRL\l"
	.string "ISLANDS.\p"
	.string "I wonder what it could be?$"

Route41_Text_JohtoSusieIntro::
	.string "You look so ele- gant, riding\n"
	.string "your POKéMON.$"

Route41_Text_JohtoSusieDefeat::
	.string "I'm crushed…$"

Route41_Text_JohtoSusiePostBattle::
	.string "Wasn't there a hit song about\n"
	.string "a boy riding a LAPRAS?$"

Route41_Text_JohtoDeniseIntro::
	.string "The weather is so beautiful,\n"
	.string "I'm in a daze!$"

Route41_Text_JohtoDeniseDefeat::
	.string "Ohhh!$"

Route41_Text_JohtoDenisePostBattle::
	.string "A sunburn is the worst for\n"
	.string "skin.\p"
	.string "But I don't use a sunblock.\p"
	.string "I won't pollute the water.$"

Route41_Text_JohtoKaraIntro::
	.string "If you need to rest, just\n"
	.string "tread water.\p"
	.string "You'll get your wind back, so\n"
	.string "you\p"
	.string "can keep on going strong.$"

Route41_Text_JohtoKaraDefeat::
	.string "Oh! You have more energy than\n"
	.string "I do.$"

Route41_Text_JohtoKaraPostBattle::
	.string "I heard roars from deep inside\n"
	.string "the ISLANDS.$"

Route41_Text_JohtoWendyIntro::
	.string "At night, STARYU gather near\n"
	.string "the water's surface.$"

Route41_Text_JohtoWendyDefeat::
	.string "Oh, dear…$"

Route41_Text_JohtoWendyPostBattle::
	.string "The clusters of STARYU light\n"
	.string "up at the same time.\p"
	.string "It's so beautiful, it's scary.$"

