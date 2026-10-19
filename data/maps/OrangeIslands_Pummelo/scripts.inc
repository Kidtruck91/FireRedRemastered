OrangeIslands_Pummelo_MapScripts::
	.byte 0

OrangeIslands_Pummelo_EventScript_Beach::
	msgbox OrangeIslands_Pummelo_Text_Beach, MSGBOX_SIGN
	end

OrangeIslands_Pummelo_EventScript_Town::
	msgbox OrangeIslands_Pummelo_Text_Town, MSGBOX_SIGN
	end

@ ---- Orange Islands trainers (orange_route_trainers.py) ----
OrangeIslands_Pummelo_EventScript_OrangeAriel::
	trainerbattle_single TRAINER_ORANGE_PICNICKER_ARIEL, OrangeIslands_Pummelo_Text_OrangeArielIntro, OrangeIslands_Pummelo_Text_OrangeArielDefeat
	msgbox OrangeIslands_Pummelo_Text_OrangeArielPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Pummelo_EventScript_OrangeTess::
	trainerbattle_single TRAINER_ORANGE_BEAUTY_TESS, OrangeIslands_Pummelo_Text_OrangeTessIntro, OrangeIslands_Pummelo_Text_OrangeTessDefeat
	msgbox OrangeIslands_Pummelo_Text_OrangeTessPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Pummelo_EventScript_OrangeJeff::
	trainerbattle_single TRAINER_ORANGE_COOLTRAINER_JEFF, OrangeIslands_Pummelo_Text_OrangeJeffIntro, OrangeIslands_Pummelo_Text_OrangeJeffDefeat
	msgbox OrangeIslands_Pummelo_Text_OrangeJeffPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Pummelo_EventScript_OrangeEmmet::
	trainerbattle_single TRAINER_ORANGE_SWIMMER_M_EMMET, OrangeIslands_Pummelo_Text_OrangeEmmetIntro, OrangeIslands_Pummelo_Text_OrangeEmmetDefeat
	msgbox OrangeIslands_Pummelo_Text_OrangeEmmetPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Pummelo_Text_OrangeArielIntro::
	.string "Let's have a fun battle!$"

OrangeIslands_Pummelo_Text_OrangeArielDefeat::
	.string "That was fun anyway!$"

OrangeIslands_Pummelo_Text_OrangeArielPostBattle::
	.string "I'm visiting every island\n"
	.string "on my holiday!$"

OrangeIslands_Pummelo_Text_OrangeTessIntro::
	.string "I came to the islands on\n"
	.string "vacation, but a battle's fine.$"

OrangeIslands_Pummelo_Text_OrangeTessDefeat::
	.string "Not my day...$"

OrangeIslands_Pummelo_Text_OrangeTessPostBattle::
	.string "Even on vacation, a trainer\n"
	.string "keeps training.$"

OrangeIslands_Pummelo_Text_OrangeJeffIntro::
	.string "You're doing the ORANGE\n"
	.string "LEAGUE? Show me your best!$"

OrangeIslands_Pummelo_Text_OrangeJeffDefeat::
	.string "Impressive.$"

OrangeIslands_Pummelo_Text_OrangeJeffPostBattle::
	.string "The ORANGE CREW won't go\n"
	.string "easy on you.$"

OrangeIslands_Pummelo_Text_OrangeEmmetIntro::
	.string "You're surfing? Then you\n"
	.string "can't escape me!$"

OrangeIslands_Pummelo_Text_OrangeEmmetDefeat::
	.string "Glub... You win.$"

OrangeIslands_Pummelo_Text_OrangeEmmetPostBattle::
	.string "The currents here are strong.\n"
	.string "Watch where they push you.$"

@ ---- League boat back to Kumquat (orange_trip_story.py)
OrangeIslands_Pummelo_EventScript_LeagueBoat::
	lock
	faceplayer
	msgbox OrangeIslands_Pummelo_Text_LeagueBoatAsk, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, OrangeIslands_Pummelo_EventScript_LeagueBoatNo
	msgbox OrangeIslands_Pummelo_Text_LeagueBoatDeparting
	setvar VAR_0x8004, SEAGALLOP_PUMMELO
	setvar VAR_0x8006, SEAGALLOP_KUMQUAT
	goto EventScript_SetSail
	end

OrangeIslands_Pummelo_EventScript_LeagueBoatNo::
	msgbox OrangeIslands_Pummelo_Text_LeagueBoatNo
	release
	end

OrangeIslands_Pummelo_Text_LeagueBoatAsk::
	.string "Heading back to KUMQUAT ISLAND?$"

OrangeIslands_Pummelo_Text_LeagueBoatDeparting::
	.string "Hold on tight!\n"
	.string "Next stop, KUMQUAT ISLAND!$"

OrangeIslands_Pummelo_Text_LeagueBoatNo::
	.string "I'll be right here.$"
