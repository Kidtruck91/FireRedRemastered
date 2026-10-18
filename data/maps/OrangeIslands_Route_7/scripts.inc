OrangeIslands_Route_7_MapScripts::
	.byte 0

OrangeIslands_Route_7_EventScript_Moro::
	msgbox OrangeIslands_Route_7_Text_Moro, MSGBOX_SIGN
	end

OrangeIslands_Route_7_EventScript_South::
	msgbox OrangeIslands_Route_7_Text_South, MSGBOX_SIGN
	end

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Sunburst
OrangeIslands_Route_7_EventScript_SeaEdgeToSunburst::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8004, 6
	goto_if_lt OrangeIslands_Route_7_EventScript_SeaEdgeToSunburst_Lo
	compare VAR_0x8004, 69
	goto_if_gt OrangeIslands_Route_7_EventScript_SeaEdgeToSunburst_Hi
OrangeIslands_Route_7_EventScript_SeaEdgeToSunburst_Go:
	subvar VAR_0x8004, 6
	setvar VAR_0x8005, 1
	warp MAP_ORANGE_ISLANDS_SUNBURST, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Route_7_EventScript_SeaEdgeToSunburst_Lo:
	setvar VAR_0x8004, 6
	goto OrangeIslands_Route_7_EventScript_SeaEdgeToSunburst_Go
OrangeIslands_Route_7_EventScript_SeaEdgeToSunburst_Hi:
	setvar VAR_0x8004, 69
	goto OrangeIslands_Route_7_EventScript_SeaEdgeToSunburst_Go

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Moro
OrangeIslands_Route_7_EventScript_SeaEdgeToMoro::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8004, 1
	goto_if_lt OrangeIslands_Route_7_EventScript_SeaEdgeToMoro_Lo
	compare VAR_0x8004, 64
	goto_if_gt OrangeIslands_Route_7_EventScript_SeaEdgeToMoro_Hi
OrangeIslands_Route_7_EventScript_SeaEdgeToMoro_Go:
	subvar VAR_0x8004, 1
	setvar VAR_0x8005, 54
	warp MAP_ORANGE_ISLANDS_MORO, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Route_7_EventScript_SeaEdgeToMoro_Lo:
	setvar VAR_0x8004, 1
	goto OrangeIslands_Route_7_EventScript_SeaEdgeToMoro_Go
OrangeIslands_Route_7_EventScript_SeaEdgeToMoro_Hi:
	setvar VAR_0x8004, 64
	goto OrangeIslands_Route_7_EventScript_SeaEdgeToMoro_Go

@ ---- Orange Islands trainers (orange_route_trainers.py) ----
OrangeIslands_Route_7_EventScript_OrangeNina::
	trainerbattle_single TRAINER_ORANGE_SWIMMER_F_NINA, OrangeIslands_Route_7_Text_OrangeNinaIntro, OrangeIslands_Route_7_Text_OrangeNinaDefeat
	msgbox OrangeIslands_Route_7_Text_OrangeNinaPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Route_7_EventScript_OrangeZane::
	trainerbattle_single TRAINER_ORANGE_SWIMMER_M_ZANE, OrangeIslands_Route_7_Text_OrangeZaneIntro, OrangeIslands_Route_7_Text_OrangeZaneDefeat
	msgbox OrangeIslands_Route_7_Text_OrangeZanePostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Route_7_EventScript_OrangeMira::
	trainerbattle_single TRAINER_ORANGE_SWIMMER_F_MIRA, OrangeIslands_Route_7_Text_OrangeMiraIntro, OrangeIslands_Route_7_Text_OrangeMiraDefeat
	msgbox OrangeIslands_Route_7_Text_OrangeMiraPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Route_7_EventScript_OrangeTroy::
	trainerbattle_single TRAINER_ORANGE_TUBER_TROY, OrangeIslands_Route_7_Text_OrangeTroyIntro, OrangeIslands_Route_7_Text_OrangeTroyDefeat
	msgbox OrangeIslands_Route_7_Text_OrangeTroyPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Route_7_EventScript_OrangeKai::
	trainerbattle_single TRAINER_ORANGE_SWIMMER_M_KAI, OrangeIslands_Route_7_Text_OrangeKaiIntro, OrangeIslands_Route_7_Text_OrangeKaiDefeat
	msgbox OrangeIslands_Route_7_Text_OrangeKaiPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Route_7_EventScript_OrangeZoe::
	trainerbattle_single TRAINER_ORANGE_SWIMMER_F_ZOE, OrangeIslands_Route_7_Text_OrangeZoeIntro, OrangeIslands_Route_7_Text_OrangeZoeDefeat
	msgbox OrangeIslands_Route_7_Text_OrangeZoePostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Route_7_Text_OrangeNinaIntro::
	.string "My POKéMON and I swim\n"
	.string "together every day!$"

OrangeIslands_Route_7_Text_OrangeNinaDefeat::
	.string "We'll swim harder!$"

OrangeIslands_Route_7_Text_OrangeNinaPostBattle::
	.string "Each island has its own\n"
	.string "kind of water. Isn't it neat?$"

OrangeIslands_Route_7_Text_OrangeZaneIntro::
	.string "I train my POKéMON in the\n"
	.string "open sea. Ready?$"

OrangeIslands_Route_7_Text_OrangeZaneDefeat::
	.string "Washed out!$"

OrangeIslands_Route_7_Text_OrangeZanePostBattle::
	.string "Out here, WATER POKéMON are\n"
	.string "the real champions.$"

OrangeIslands_Route_7_Text_OrangeMiraIntro::
	.string "Hey! You splashed me!\n"
	.string "That means a battle!$"

OrangeIslands_Route_7_Text_OrangeMiraDefeat::
	.string "Fine, you win.$"

OrangeIslands_Route_7_Text_OrangeMiraPostBattle::
	.string "Watch out for TENTACOOL\n"
	.string "near the reefs.$"

OrangeIslands_Route_7_Text_OrangeTroyIntro::
	.string "Splash splash!\n"
	.string "Battle me!$"

OrangeIslands_Route_7_Text_OrangeTroyDefeat::
	.string "My float popped!$"

OrangeIslands_Route_7_Text_OrangeTroyPostBattle::
	.string "My big sister swims way\n"
	.string "out past the rocks.$"

OrangeIslands_Route_7_Text_OrangeKaiIntro::
	.string "I train my POKéMON in the\n"
	.string "open sea. Ready?$"

OrangeIslands_Route_7_Text_OrangeKaiDefeat::
	.string "Washed out!$"

OrangeIslands_Route_7_Text_OrangeKaiPostBattle::
	.string "Out here, WATER POKéMON are\n"
	.string "the real champions.$"

OrangeIslands_Route_7_Text_OrangeZoeIntro::
	.string "My POKéMON and I swim\n"
	.string "together every day!$"

OrangeIslands_Route_7_Text_OrangeZoeDefeat::
	.string "We'll swim harder!$"

OrangeIslands_Route_7_Text_OrangeZoePostBattle::
	.string "Each island has its own\n"
	.string "kind of water. Isn't it neat?$"

