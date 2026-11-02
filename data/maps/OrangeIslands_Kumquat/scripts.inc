OrangeIslands_Kumquat_MapScripts::
	.byte 0

OrangeIslands_Kumquat_EventScript_Beach::
	msgbox OrangeIslands_Kumquat_Text_Beach, MSGBOX_SIGN
	end

OrangeIslands_Kumquat_EventScript_Town::
	msgbox OrangeIslands_Kumquat_Text_Town, MSGBOX_SIGN
	end

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Route 13
OrangeIslands_Kumquat_EventScript_SeaEdgeToRoute13::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8004, 20
	goto_if_lt OrangeIslands_Kumquat_EventScript_SeaEdgeToRoute13_Lo
	compare VAR_0x8004, 67
	goto_if_gt OrangeIslands_Kumquat_EventScript_SeaEdgeToRoute13_Hi
OrangeIslands_Kumquat_EventScript_SeaEdgeToRoute13_Go:
	setvar VAR_0x8005, 1
	warp MAP_ORANGE_ISLANDS_ROUTE_13, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Kumquat_EventScript_SeaEdgeToRoute13_Lo:
	setvar VAR_0x8004, 20
	goto OrangeIslands_Kumquat_EventScript_SeaEdgeToRoute13_Go
OrangeIslands_Kumquat_EventScript_SeaEdgeToRoute13_Hi:
	setvar VAR_0x8004, 67
	goto OrangeIslands_Kumquat_EventScript_SeaEdgeToRoute13_Go

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Route 14
OrangeIslands_Kumquat_EventScript_SeaEdgeToRoute14::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8004, 4
	goto_if_lt OrangeIslands_Kumquat_EventScript_SeaEdgeToRoute14_Lo
	compare VAR_0x8004, 51
	goto_if_gt OrangeIslands_Kumquat_EventScript_SeaEdgeToRoute14_Hi
OrangeIslands_Kumquat_EventScript_SeaEdgeToRoute14_Go:
	addvar VAR_0x8004, 16
	setvar VAR_0x8005, 70
	warp MAP_ORANGE_ISLANDS_ROUTE_14, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Kumquat_EventScript_SeaEdgeToRoute14_Lo:
	setvar VAR_0x8004, 4
	goto OrangeIslands_Kumquat_EventScript_SeaEdgeToRoute14_Go
OrangeIslands_Kumquat_EventScript_SeaEdgeToRoute14_Hi:
	setvar VAR_0x8004, 51
	goto OrangeIslands_Kumquat_EventScript_SeaEdgeToRoute14_Go

@ Sea edge: surfing off the east side crosses to Kumquat Beach.
@ A trigger, not a map connection: this map is on gTileset_HoennGeneral and the beach
@ on gTileset_OrangeGeneral, and a connected map's edge strip is drawn with the
@ tilesets of the map the player is standing on, so a connection would show the wrong
@ graphics and palettes along the whole seam.
@ The landing spot is fixed rather than matched to the row the player left on: the
@ beach's west end is broken up by sandbars, so no column there is open water for all
@ 60 rows of this edge.
OrangeIslands_Kumquat_EventScript_SeaEdgeToKumquatBeach::
	warp MAP_ORANGE_ISLANDS_KUMQUAT_BEACH, 15, 21
	waitstate
	end


@ ---- Orange Islands trainers (orange_route_trainers.py) ----
OrangeIslands_Kumquat_EventScript_OrangeGrace::
	trainerbattle_single TRAINER_ORANGE_COOLTRAINER_GRACE, OrangeIslands_Kumquat_Text_OrangeGraceIntro, OrangeIslands_Kumquat_Text_OrangeGraceDefeat
	msgbox OrangeIslands_Kumquat_Text_OrangeGracePostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Kumquat_EventScript_OrangeEddie::
	trainerbattle_single TRAINER_ORANGE_BIRD_KEEPER_EDDIE, OrangeIslands_Kumquat_Text_OrangeEddieIntro, OrangeIslands_Kumquat_Text_OrangeEddieDefeat
	msgbox OrangeIslands_Kumquat_Text_OrangeEddiePostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Kumquat_EventScript_OrangeGary::
	trainerbattle_single TRAINER_ORANGE_CAMPER_GARY, OrangeIslands_Kumquat_Text_OrangeGaryIntro, OrangeIslands_Kumquat_Text_OrangeGaryDefeat
	msgbox OrangeIslands_Kumquat_Text_OrangeGaryPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Kumquat_EventScript_OrangeNaomi::
	trainerbattle_single TRAINER_ORANGE_PICNICKER_NAOMI, OrangeIslands_Kumquat_Text_OrangeNaomiIntro, OrangeIslands_Kumquat_Text_OrangeNaomiDefeat
	msgbox OrangeIslands_Kumquat_Text_OrangeNaomiPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Kumquat_EventScript_OrangeTeddy::
	trainerbattle_single TRAINER_ORANGE_SWIMMER_M_TEDDY, OrangeIslands_Kumquat_Text_OrangeTeddyIntro, OrangeIslands_Kumquat_Text_OrangeTeddyDefeat
	msgbox OrangeIslands_Kumquat_Text_OrangeTeddyPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Kumquat_Text_OrangeGraceIntro::
	.string "I've beaten trainers on\n"
	.string "every island. You're next!$"

OrangeIslands_Kumquat_Text_OrangeGraceDefeat::
	.string "Well done.$"

OrangeIslands_Kumquat_Text_OrangeGracePostBattle::
	.string "Strategy matters more than\n"
	.string "levels here.$"

OrangeIslands_Kumquat_Text_OrangeEddieIntro::
	.string "Look up! My POKéMON rule\n"
	.string "the island skies!$"

OrangeIslands_Kumquat_Text_OrangeEddieDefeat::
	.string "Shot down!$"

OrangeIslands_Kumquat_Text_OrangeEddiePostBattle::
	.string "PELIPPER and WINGULL nest\n"
	.string "on the cliffs here.$"

OrangeIslands_Kumquat_Text_OrangeGaryIntro::
	.string "Hey! Over here! Let's have\n"
	.string "a battle!$"

OrangeIslands_Kumquat_Text_OrangeGaryDefeat::
	.string "Darn it!$"

OrangeIslands_Kumquat_Text_OrangeGaryPostBattle::
	.string "The hills here have great\n"
	.string "views of the sea.$"

OrangeIslands_Kumquat_Text_OrangeNaomiIntro::
	.string "The sun's out! Perfect\n"
	.string "battle weather!$"

OrangeIslands_Kumquat_Text_OrangeNaomiDefeat::
	.string "Oh, you're good!$"

OrangeIslands_Kumquat_Text_OrangeNaomiPostBattle::
	.string "I love the island fruit\n"
	.string "here. So juicy!$"

OrangeIslands_Kumquat_Text_OrangeTeddyIntro::
	.string "I train my POKéMON in the\n"
	.string "open sea. Ready?$"

OrangeIslands_Kumquat_Text_OrangeTeddyDefeat::
	.string "Washed out!$"

OrangeIslands_Kumquat_Text_OrangeTeddyPostBattle::
	.string "Out here, WATER POKéMON are\n"
	.string "the real champions.$"

@ ---- League boat to Pummelo (orange_trip_story.py): all four Orange badges
OrangeIslands_Kumquat_EventScript_LeagueBoat::
	lock
	faceplayer
	goto_if_unset FLAG_BADGE_CORAL_EYE, OrangeIslands_Kumquat_EventScript_LeagueBoatNoBadges
	goto_if_unset FLAG_BADGE_SPIKE_SHELL, OrangeIslands_Kumquat_EventScript_LeagueBoatNoBadges
	goto_if_unset FLAG_BADGE_SEA_RUBY, OrangeIslands_Kumquat_EventScript_LeagueBoatNoBadges
	goto_if_unset FLAG_BADGE_JADE_STAR, OrangeIslands_Kumquat_EventScript_LeagueBoatNoBadges
	msgbox OrangeIslands_Kumquat_Text_LeagueBoatAsk, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, OrangeIslands_Kumquat_EventScript_LeagueBoatNo
	msgbox OrangeIslands_Kumquat_Text_LeagueBoatDeparting
	setvar VAR_0x8004, SEAGALLOP_KUMQUAT
	setvar VAR_0x8006, SEAGALLOP_PUMMELO
	goto EventScript_SetSail
	end

OrangeIslands_Kumquat_EventScript_LeagueBoatNoBadges::
	msgbox OrangeIslands_Kumquat_Text_LeagueBoatNoBadges
	release
	end

OrangeIslands_Kumquat_EventScript_LeagueBoatNo::
	msgbox OrangeIslands_Kumquat_Text_LeagueBoatNo
	release
	end

OrangeIslands_Kumquat_Text_LeagueBoatNoBadges::
	.string "This boat goes to PUMMELO ISLAND,\n"
	.string "home of the ORANGE LEAGUE.\p"
	.string "Only trainers with all four\n"
	.string "ORANGE badges may board.$"

OrangeIslands_Kumquat_Text_LeagueBoatAsk::
	.string "Four badges! You've earned a ride.\p"
	.string "Sail to PUMMELO ISLAND?$"

OrangeIslands_Kumquat_Text_LeagueBoatDeparting::
	.string "Hold on tight!\n"
	.string "Next stop, PUMMELO ISLAND!$"

OrangeIslands_Kumquat_Text_LeagueBoatNo::
	.string "Come back when you're ready.$"
