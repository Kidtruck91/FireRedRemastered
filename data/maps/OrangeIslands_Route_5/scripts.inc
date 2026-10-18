OrangeIslands_Route_5_MapScripts::
	.byte 0

OrangeIslands_Route_5_EventScript_Currents::
	msgbox OrangeIslands_Route_5_Text_Currents, MSGBOX_SIGN
	end

OrangeIslands_Route_5_EventScript_North::
	msgbox OrangeIslands_Route_5_Text_North, MSGBOX_SIGN
	end

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Mikan
OrangeIslands_Route_5_EventScript_SeaEdgeToMikan::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8004, 0
	goto_if_lt OrangeIslands_Route_5_EventScript_SeaEdgeToMikan_Lo
	compare VAR_0x8004, 47
	goto_if_gt OrangeIslands_Route_5_EventScript_SeaEdgeToMikan_Hi
OrangeIslands_Route_5_EventScript_SeaEdgeToMikan_Go:
	addvar VAR_0x8004, 8
	setvar VAR_0x8005, 1
	warp MAP_ORANGE_ISLANDS_MIKAN, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Route_5_EventScript_SeaEdgeToMikan_Lo:
	setvar VAR_0x8004, 0
	goto OrangeIslands_Route_5_EventScript_SeaEdgeToMikan_Go
OrangeIslands_Route_5_EventScript_SeaEdgeToMikan_Hi:
	setvar VAR_0x8004, 47
	goto OrangeIslands_Route_5_EventScript_SeaEdgeToMikan_Go

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Navel
OrangeIslands_Route_5_EventScript_SeaEdgeToNavel::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8004, 0
	goto_if_lt OrangeIslands_Route_5_EventScript_SeaEdgeToNavel_Lo
	compare VAR_0x8004, 47
	goto_if_gt OrangeIslands_Route_5_EventScript_SeaEdgeToNavel_Hi
OrangeIslands_Route_5_EventScript_SeaEdgeToNavel_Go:
	addvar VAR_0x8004, 21
	setvar VAR_0x8005, 63
	warp MAP_ORANGE_ISLANDS_NAVEL, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Route_5_EventScript_SeaEdgeToNavel_Lo:
	setvar VAR_0x8004, 0
	goto OrangeIslands_Route_5_EventScript_SeaEdgeToNavel_Go
OrangeIslands_Route_5_EventScript_SeaEdgeToNavel_Hi:
	setvar VAR_0x8004, 47
	goto OrangeIslands_Route_5_EventScript_SeaEdgeToNavel_Go

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Route 6
OrangeIslands_Route_5_EventScript_SeaEdgeToRoute6::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8005, 63
	goto_if_lt OrangeIslands_Route_5_EventScript_SeaEdgeToRoute6_Lo
	compare VAR_0x8005, 70
	goto_if_gt OrangeIslands_Route_5_EventScript_SeaEdgeToRoute6_Hi
OrangeIslands_Route_5_EventScript_SeaEdgeToRoute6_Go:
	subvar VAR_0x8005, 33
	setvar VAR_0x8004, 86
	warp MAP_ORANGE_ISLANDS_ROUTE_6, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Route_5_EventScript_SeaEdgeToRoute6_Lo:
	setvar VAR_0x8005, 63
	goto OrangeIslands_Route_5_EventScript_SeaEdgeToRoute6_Go
OrangeIslands_Route_5_EventScript_SeaEdgeToRoute6_Hi:
	setvar VAR_0x8005, 70
	goto OrangeIslands_Route_5_EventScript_SeaEdgeToRoute6_Go

@ ---- Orange Islands trainers (orange_route_trainers.py) ----
OrangeIslands_Route_5_EventScript_OrangeKeith::
	trainerbattle_single TRAINER_ORANGE_SWIMMER_M_KEITH, OrangeIslands_Route_5_Text_OrangeKeithIntro, OrangeIslands_Route_5_Text_OrangeKeithDefeat
	msgbox OrangeIslands_Route_5_Text_OrangeKeithPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Route_5_EventScript_OrangeRalph::
	trainerbattle_single TRAINER_ORANGE_TUBER_RALPH, OrangeIslands_Route_5_Text_OrangeRalphIntro, OrangeIslands_Route_5_Text_OrangeRalphDefeat
	msgbox OrangeIslands_Route_5_Text_OrangeRalphPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Route_5_EventScript_OrangeRafael::
	trainerbattle_single TRAINER_ORANGE_TUBER_RAFAEL, OrangeIslands_Route_5_Text_OrangeRafaelIntro, OrangeIslands_Route_5_Text_OrangeRafaelDefeat
	msgbox OrangeIslands_Route_5_Text_OrangeRafaelPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Route_5_EventScript_OrangeVince::
	trainerbattle_single TRAINER_ORANGE_SWIMMER_M_VINCE, OrangeIslands_Route_5_Text_OrangeVinceIntro, OrangeIslands_Route_5_Text_OrangeVinceDefeat
	msgbox OrangeIslands_Route_5_Text_OrangeVincePostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Route_5_EventScript_OrangeJune::
	trainerbattle_single TRAINER_ORANGE_SWIMMER_F_JUNE, OrangeIslands_Route_5_Text_OrangeJuneIntro, OrangeIslands_Route_5_Text_OrangeJuneDefeat
	msgbox OrangeIslands_Route_5_Text_OrangeJunePostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Route_5_Text_OrangeKeithIntro::
	.string "You're surfing? Then you\n"
	.string "can't escape me!$"

OrangeIslands_Route_5_Text_OrangeKeithDefeat::
	.string "Glub... You win.$"

OrangeIslands_Route_5_Text_OrangeKeithPostBattle::
	.string "The currents here are strong.\n"
	.string "Watch where they push you.$"

OrangeIslands_Route_5_Text_OrangeRalphIntro::
	.string "I can't swim yet, but my\n"
	.string "POKéMON can!$"

OrangeIslands_Route_5_Text_OrangeRalphDefeat::
	.string "Waaah!$"

OrangeIslands_Route_5_Text_OrangeRalphPostBattle::
	.string "When I grow up, I'll swim\n"
	.string "to every island!$"

OrangeIslands_Route_5_Text_OrangeRafaelIntro::
	.string "Splash splash!\n"
	.string "Battle me!$"

OrangeIslands_Route_5_Text_OrangeRafaelDefeat::
	.string "My float popped!$"

OrangeIslands_Route_5_Text_OrangeRafaelPostBattle::
	.string "My big sister swims way\n"
	.string "out past the rocks.$"

OrangeIslands_Route_5_Text_OrangeVinceIntro::
	.string "You're surfing? Then you\n"
	.string "can't escape me!$"

OrangeIslands_Route_5_Text_OrangeVinceDefeat::
	.string "Glub... You win.$"

OrangeIslands_Route_5_Text_OrangeVincePostBattle::
	.string "The currents here are strong.\n"
	.string "Watch where they push you.$"

OrangeIslands_Route_5_Text_OrangeJuneIntro::
	.string "Hey! You splashed me!\n"
	.string "That means a battle!$"

OrangeIslands_Route_5_Text_OrangeJuneDefeat::
	.string "Fine, you win.$"

OrangeIslands_Route_5_Text_OrangeJunePostBattle::
	.string "Watch out for TENTACOOL\n"
	.string "near the reefs.$"

