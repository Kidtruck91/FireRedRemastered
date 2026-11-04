OrangeIslands_Route_14_MapScripts::
	.byte 0

OrangeIslands_Route_14_EventScript_Resort::
	msgbox OrangeIslands_Route_14_Text_Resort, MSGBOX_SIGN
	end

OrangeIslands_Route_14_EventScript_South::
	msgbox OrangeIslands_Route_14_Text_South, MSGBOX_SIGN
	end

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Kumquat
OrangeIslands_Route_14_EventScript_SeaEdgeToKumquat::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8004, 16
	goto_if_lt OrangeIslands_Route_14_EventScript_SeaEdgeToKumquat_Lo
	compare VAR_0x8004, 87
	goto_if_gt OrangeIslands_Route_14_EventScript_SeaEdgeToKumquat_Hi
OrangeIslands_Route_14_EventScript_SeaEdgeToKumquat_Go:
	subvar VAR_0x8004, 16
	setvar VAR_0x8005, 1
	warp MAP_ORANGE_ISLANDS_KUMQUAT, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Route_14_EventScript_SeaEdgeToKumquat_Lo:
	setvar VAR_0x8004, 16
	goto OrangeIslands_Route_14_EventScript_SeaEdgeToKumquat_Go
OrangeIslands_Route_14_EventScript_SeaEdgeToKumquat_Hi:
	setvar VAR_0x8004, 87
	goto OrangeIslands_Route_14_EventScript_SeaEdgeToKumquat_Go

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Trovita
@ Column 7, not 1. Landing on column 1 put six columns of border on screen; 7 is the
@ first column that shows none on a 72-wide map, and it is open water for every row
@ this can arrive on. The band is clamped to the rows Trovita's west edge actually
@ opens on -- 2-14 here, which is 42-54 there -- since the cliff now closes the rest.
OrangeIslands_Route_14_EventScript_SeaEdgeToTrovita::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8005, 2
	goto_if_lt OrangeIslands_Route_14_EventScript_SeaEdgeToTrovita_Lo
	compare VAR_0x8005, 14
	goto_if_gt OrangeIslands_Route_14_EventScript_SeaEdgeToTrovita_Hi
OrangeIslands_Route_14_EventScript_SeaEdgeToTrovita_Go:
	addvar VAR_0x8005, 40
	setvar VAR_0x8004, 7
	warp MAP_ORANGE_ISLANDS_TROVITA, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Route_14_EventScript_SeaEdgeToTrovita_Lo:
	setvar VAR_0x8005, 2
	goto OrangeIslands_Route_14_EventScript_SeaEdgeToTrovita_Go
OrangeIslands_Route_14_EventScript_SeaEdgeToTrovita_Hi:
	setvar VAR_0x8005, 14
	goto OrangeIslands_Route_14_EventScript_SeaEdgeToTrovita_Go

@ ---- Orange Islands trainers (orange_route_trainers.py) ----
OrangeIslands_Route_14_EventScript_OrangeBruce::
	trainerbattle_single TRAINER_ORANGE_SWIMMER_M_BRUCE, OrangeIslands_Route_14_Text_OrangeBruceIntro, OrangeIslands_Route_14_Text_OrangeBruceDefeat
	msgbox OrangeIslands_Route_14_Text_OrangeBrucePostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Route_14_EventScript_OrangeAlex::
	trainerbattle_single TRAINER_ORANGE_SWIMMER_M_ALEX, OrangeIslands_Route_14_Text_OrangeAlexIntro, OrangeIslands_Route_14_Text_OrangeAlexDefeat
	msgbox OrangeIslands_Route_14_Text_OrangeAlexPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Route_14_EventScript_OrangeAbby::
	trainerbattle_single TRAINER_ORANGE_SWIMMER_F_ABBY, OrangeIslands_Route_14_Text_OrangeAbbyIntro, OrangeIslands_Route_14_Text_OrangeAbbyDefeat
	msgbox OrangeIslands_Route_14_Text_OrangeAbbyPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Route_14_EventScript_OrangeOlivia::
	trainerbattle_single TRAINER_ORANGE_SWIMMER_F_OLIVIA, OrangeIslands_Route_14_Text_OrangeOliviaIntro, OrangeIslands_Route_14_Text_OrangeOliviaDefeat
	msgbox OrangeIslands_Route_14_Text_OrangeOliviaPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Route_14_EventScript_OrangeMia::
	trainerbattle_single TRAINER_ORANGE_SWIMMER_F_MIA, OrangeIslands_Route_14_Text_OrangeMiaIntro, OrangeIslands_Route_14_Text_OrangeMiaDefeat
	msgbox OrangeIslands_Route_14_Text_OrangeMiaPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Route_14_EventScript_OrangeLuke::
	trainerbattle_single TRAINER_ORANGE_SWIMMER_M_LUKE, OrangeIslands_Route_14_Text_OrangeLukeIntro, OrangeIslands_Route_14_Text_OrangeLukeDefeat
	msgbox OrangeIslands_Route_14_Text_OrangeLukePostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Route_14_Text_OrangeBruceIntro::
	.string "I train my POKéMON in the\n"
	.string "open sea. Ready?$"

OrangeIslands_Route_14_Text_OrangeBruceDefeat::
	.string "Washed out!$"

OrangeIslands_Route_14_Text_OrangeBrucePostBattle::
	.string "Out here, WATER POKéMON are\n"
	.string "the real champions.$"

OrangeIslands_Route_14_Text_OrangeAlexIntro::
	.string "The water's perfect today!\n"
	.string "Let's battle out here!$"

OrangeIslands_Route_14_Text_OrangeAlexDefeat::
	.string "I'm sinking!$"

OrangeIslands_Route_14_Text_OrangeAlexPostBattle::
	.string "I swim between the islands\n"
	.string "every morning.$"

OrangeIslands_Route_14_Text_OrangeAbbyIntro::
	.string "My POKéMON and I swim\n"
	.string "together every day!$"

OrangeIslands_Route_14_Text_OrangeAbbyDefeat::
	.string "We'll swim harder!$"

OrangeIslands_Route_14_Text_OrangeAbbyPostBattle::
	.string "Each island has its own\n"
	.string "kind of water. Isn't it neat?$"

OrangeIslands_Route_14_Text_OrangeOliviaIntro::
	.string "My POKéMON and I swim\n"
	.string "together every day!$"

OrangeIslands_Route_14_Text_OrangeOliviaDefeat::
	.string "We'll swim harder!$"

OrangeIslands_Route_14_Text_OrangeOliviaPostBattle::
	.string "Each island has its own\n"
	.string "kind of water. Isn't it neat?$"

OrangeIslands_Route_14_Text_OrangeMiaIntro::
	.string "Swimming in the ORANGE\n"
	.string "ISLANDS is the best!$"

OrangeIslands_Route_14_Text_OrangeMiaDefeat::
	.string "Oh, I lost!$"

OrangeIslands_Route_14_Text_OrangeMiaPostBattle::
	.string "The reefs are so pretty\n"
	.string "from out here.$"

OrangeIslands_Route_14_Text_OrangeLukeIntro::
	.string "I train my POKéMON in the\n"
	.string "open sea. Ready?$"

OrangeIslands_Route_14_Text_OrangeLukeDefeat::
	.string "Washed out!$"

OrangeIslands_Route_14_Text_OrangeLukePostBattle::
	.string "Out here, WATER POKéMON are\n"
	.string "the real champions.$"

