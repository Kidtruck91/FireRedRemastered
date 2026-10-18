OrangeIslands_Trovita_Beach_MapScripts::
	.byte 0

OrangeIslands_Trovita_Beach_EventScript_Town::
	msgbox OrangeIslands_Trovita_Beach_Text_Town, MSGBOX_SIGN
	end

OrangeIslands_Trovita_Beach_EventScript_Harbor::
	msgbox OrangeIslands_Trovita_Beach_Text_Harbor, MSGBOX_SIGN
	end

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Route 11
OrangeIslands_Trovita_Beach_EventScript_SeaEdgeToRoute11::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8004, 27
	goto_if_lt OrangeIslands_Trovita_Beach_EventScript_SeaEdgeToRoute11_Lo
	compare VAR_0x8004, 66
	goto_if_gt OrangeIslands_Trovita_Beach_EventScript_SeaEdgeToRoute11_Hi
OrangeIslands_Trovita_Beach_EventScript_SeaEdgeToRoute11_Go:
	subvar VAR_0x8004, 7
	setvar VAR_0x8005, 1
	warp MAP_ORANGE_ISLANDS_ROUTE_11, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Trovita_Beach_EventScript_SeaEdgeToRoute11_Lo:
	setvar VAR_0x8004, 27
	goto OrangeIslands_Trovita_Beach_EventScript_SeaEdgeToRoute11_Go
OrangeIslands_Trovita_Beach_EventScript_SeaEdgeToRoute11_Hi:
	setvar VAR_0x8004, 66
	goto OrangeIslands_Trovita_Beach_EventScript_SeaEdgeToRoute11_Go

@ ---- Orange Islands trainers (orange_route_trainers.py) ----
OrangeIslands_Trovita_Beach_EventScript_OrangeLogan::
	trainerbattle_single TRAINER_ORANGE_SWIMMER_M_LOGAN, OrangeIslands_Trovita_Beach_Text_OrangeLoganIntro, OrangeIslands_Trovita_Beach_Text_OrangeLoganDefeat
	msgbox OrangeIslands_Trovita_Beach_Text_OrangeLoganPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Trovita_Beach_EventScript_OrangePearl::
	trainerbattle_single TRAINER_ORANGE_BEAUTY_PEARL, OrangeIslands_Trovita_Beach_Text_OrangePearlIntro, OrangeIslands_Trovita_Beach_Text_OrangePearlDefeat
	msgbox OrangeIslands_Trovita_Beach_Text_OrangePearlPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Trovita_Beach_EventScript_OrangeCindy::
	trainerbattle_single TRAINER_ORANGE_SWIMMER_F_CINDY, OrangeIslands_Trovita_Beach_Text_OrangeCindyIntro, OrangeIslands_Trovita_Beach_Text_OrangeCindyDefeat
	msgbox OrangeIslands_Trovita_Beach_Text_OrangeCindyPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Trovita_Beach_EventScript_OrangeIris::
	trainerbattle_single TRAINER_ORANGE_SWIMMER_F_IRIS, OrangeIslands_Trovita_Beach_Text_OrangeIrisIntro, OrangeIslands_Trovita_Beach_Text_OrangeIrisDefeat
	msgbox OrangeIslands_Trovita_Beach_Text_OrangeIrisPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Trovita_Beach_Text_OrangeLoganIntro::
	.string "You're surfing? Then you\n"
	.string "can't escape me!$"

OrangeIslands_Trovita_Beach_Text_OrangeLoganDefeat::
	.string "Glub... You win.$"

OrangeIslands_Trovita_Beach_Text_OrangeLoganPostBattle::
	.string "The currents here are strong.\n"
	.string "Watch where they push you.$"

OrangeIslands_Trovita_Beach_Text_OrangePearlIntro::
	.string "My POKéMON are as pretty\n"
	.string "as they are strong.$"

OrangeIslands_Trovita_Beach_Text_OrangePearlDefeat::
	.string "How disappointing.$"

OrangeIslands_Trovita_Beach_Text_OrangePearlPostBattle::
	.string "Did you see the sunsets\n"
	.string "here? Simply perfect.$"

OrangeIslands_Trovita_Beach_Text_OrangeCindyIntro::
	.string "Hey! You splashed me!\n"
	.string "That means a battle!$"

OrangeIslands_Trovita_Beach_Text_OrangeCindyDefeat::
	.string "Fine, you win.$"

OrangeIslands_Trovita_Beach_Text_OrangeCindyPostBattle::
	.string "Watch out for TENTACOOL\n"
	.string "near the reefs.$"

OrangeIslands_Trovita_Beach_Text_OrangeIrisIntro::
	.string "Hey! You splashed me!\n"
	.string "That means a battle!$"

OrangeIslands_Trovita_Beach_Text_OrangeIrisDefeat::
	.string "Fine, you win.$"

OrangeIslands_Trovita_Beach_Text_OrangeIrisPostBattle::
	.string "Watch out for TENTACOOL\n"
	.string "near the reefs.$"

