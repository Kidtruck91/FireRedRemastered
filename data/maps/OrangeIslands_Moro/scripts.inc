OrangeIslands_Moro_MapScripts::
	.byte 0

OrangeIslands_Moro_EventScript_Beach::
	msgbox OrangeIslands_Moro_Text_Beach, MSGBOX_SIGN
	end

OrangeIslands_Moro_EventScript_Town::
	msgbox OrangeIslands_Moro_Text_Town, MSGBOX_SIGN
	end

OrangeIslands_Moro_EventScript_BoardGhostShip::
	warp MAP_ORANGE_ISLANDS_MORO_GHOST_SHIP, 8, 10
	waitstate
	end

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Route 7
OrangeIslands_Moro_EventScript_SeaEdgeToRoute7::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8004, 12
	goto_if_lt OrangeIslands_Moro_EventScript_SeaEdgeToRoute7_Lo
	compare VAR_0x8004, 63
	goto_if_gt OrangeIslands_Moro_EventScript_SeaEdgeToRoute7_Hi
OrangeIslands_Moro_EventScript_SeaEdgeToRoute7_Go:
	addvar VAR_0x8004, 1
	setvar VAR_0x8005, 1
	warp MAP_ORANGE_ISLANDS_ROUTE_7, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Moro_EventScript_SeaEdgeToRoute7_Lo:
	setvar VAR_0x8004, 12
	goto OrangeIslands_Moro_EventScript_SeaEdgeToRoute7_Go
OrangeIslands_Moro_EventScript_SeaEdgeToRoute7_Hi:
	setvar VAR_0x8004, 63
	goto OrangeIslands_Moro_EventScript_SeaEdgeToRoute7_Go

@ ---- Orange Islands trainers (orange_route_trainers.py) ----
OrangeIslands_Moro_EventScript_OrangeDerek::
	trainerbattle_single TRAINER_ORANGE_BIRD_KEEPER_DEREK, OrangeIslands_Moro_Text_OrangeDerekIntro, OrangeIslands_Moro_Text_OrangeDerekDefeat
	msgbox OrangeIslands_Moro_Text_OrangeDerekPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Moro_EventScript_OrangeOscar::
	trainerbattle_single TRAINER_ORANGE_BIRD_KEEPER_OSCAR, OrangeIslands_Moro_Text_OrangeOscarIntro, OrangeIslands_Moro_Text_OrangeOscarDefeat
	msgbox OrangeIslands_Moro_Text_OrangeOscarPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Moro_EventScript_OrangeKyle::
	trainerbattle_single TRAINER_ORANGE_BLACK_BELT_KYLE, OrangeIslands_Moro_Text_OrangeKyleIntro, OrangeIslands_Moro_Text_OrangeKyleDefeat
	msgbox OrangeIslands_Moro_Text_OrangeKylePostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Moro_EventScript_OrangeHolt::
	trainerbattle_single TRAINER_ORANGE_SWIMMER_M_HOLT, OrangeIslands_Moro_Text_OrangeHoltIntro, OrangeIslands_Moro_Text_OrangeHoltDefeat
	msgbox OrangeIslands_Moro_Text_OrangeHoltPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Moro_Text_OrangeDerekIntro::
	.string "The wind here is perfect\n"
	.string "for flying!$"

OrangeIslands_Moro_Text_OrangeDerekDefeat::
	.string "Feathers everywhere...$"

OrangeIslands_Moro_Text_OrangeDerekPostBattle::
	.string "A good flyer reads the\n"
	.string "sea wind.$"

OrangeIslands_Moro_Text_OrangeOscarIntro::
	.string "The wind here is perfect\n"
	.string "for flying!$"

OrangeIslands_Moro_Text_OrangeOscarDefeat::
	.string "Feathers everywhere...$"

OrangeIslands_Moro_Text_OrangeOscarPostBattle::
	.string "A good flyer reads the\n"
	.string "sea wind.$"

OrangeIslands_Moro_Text_OrangeKyleIntro::
	.string "My fists are ready!\n"
	.string "Are yours?$"

OrangeIslands_Moro_Text_OrangeKyleDefeat::
	.string "Argh!$"

OrangeIslands_Moro_Text_OrangeKylePostBattle::
	.string "A true fighter never stops\n"
	.string "training.$"

OrangeIslands_Moro_Text_OrangeHoltIntro::
	.string "The water's perfect today!\n"
	.string "Let's battle out here!$"

OrangeIslands_Moro_Text_OrangeHoltDefeat::
	.string "I'm sinking!$"

OrangeIslands_Moro_Text_OrangeHoltPostBattle::
	.string "I swim between the islands\n"
	.string "every morning.$"

