OrangeIslands_Kinnow_MapScripts::
	.byte 0

OrangeIslands_Kinnow_EventScript_Beach::
	msgbox OrangeIslands_Kinnow_Text_Beach, MSGBOX_SIGN
	end

OrangeIslands_Kinnow_EventScript_Town::
	msgbox OrangeIslands_Kinnow_Text_Town, MSGBOX_SIGN
	end

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Route 8
@ the return of the crossing above
OrangeIslands_Kinnow_EventScript_SeaEdgeToRoute8::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8005, 48
	goto_if_lt OrangeIslands_Kinnow_EventScript_SeaEdgeToRoute8_Lo
	compare VAR_0x8005, 63
	goto_if_gt OrangeIslands_Kinnow_EventScript_SeaEdgeToRoute8_Hi
OrangeIslands_Kinnow_EventScript_SeaEdgeToRoute8_Go:
	subvar VAR_0x8005, 46
	setvar VAR_0x8004, 1
	warp MAP_ORANGE_ISLANDS_ROUTE_8, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Kinnow_EventScript_SeaEdgeToRoute8_Lo:
	setvar VAR_0x8005, 48
	goto OrangeIslands_Kinnow_EventScript_SeaEdgeToRoute8_Go
OrangeIslands_Kinnow_EventScript_SeaEdgeToRoute8_Hi:
	setvar VAR_0x8005, 63
	goto OrangeIslands_Kinnow_EventScript_SeaEdgeToRoute8_Go

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Route 10
@ the return of the crossing above
OrangeIslands_Kinnow_EventScript_SeaEdgeToRoute10::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8005, 3
	goto_if_lt OrangeIslands_Kinnow_EventScript_SeaEdgeToRoute10_Lo
	compare VAR_0x8005, 17
	goto_if_gt OrangeIslands_Kinnow_EventScript_SeaEdgeToRoute10_Hi
OrangeIslands_Kinnow_EventScript_SeaEdgeToRoute10_Go:
	addvar VAR_0x8005, 40
	setvar VAR_0x8004, 102
	warp MAP_ORANGE_ISLANDS_ROUTE_10, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Kinnow_EventScript_SeaEdgeToRoute10_Lo:
	setvar VAR_0x8005, 3
	goto OrangeIslands_Kinnow_EventScript_SeaEdgeToRoute10_Go
OrangeIslands_Kinnow_EventScript_SeaEdgeToRoute10_Hi:
	setvar VAR_0x8005, 17
	goto OrangeIslands_Kinnow_EventScript_SeaEdgeToRoute10_Go

@ ---- Orange Islands trainers (orange_route_trainers.py) ----
OrangeIslands_Kinnow_EventScript_OrangeKane::
	trainerbattle_single TRAINER_ORANGE_YOUNGSTER_KANE, OrangeIslands_Kinnow_Text_OrangeKaneIntro, OrangeIslands_Kinnow_Text_OrangeKaneDefeat
	msgbox OrangeIslands_Kinnow_Text_OrangeKanePostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Kinnow_EventScript_OrangeIvan::
	trainerbattle_single TRAINER_ORANGE_BIRD_KEEPER_IVAN, OrangeIslands_Kinnow_Text_OrangeIvanIntro, OrangeIslands_Kinnow_Text_OrangeIvanDefeat
	msgbox OrangeIslands_Kinnow_Text_OrangeIvanPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Kinnow_EventScript_OrangeDean::
	trainerbattle_single TRAINER_ORANGE_CAMPER_DEAN, OrangeIslands_Kinnow_Text_OrangeDeanIntro, OrangeIslands_Kinnow_Text_OrangeDeanDefeat
	msgbox OrangeIslands_Kinnow_Text_OrangeDeanPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Kinnow_EventScript_OrangeMiles::
	trainerbattle_single TRAINER_ORANGE_BUG_CATCHER_MILES, OrangeIslands_Kinnow_Text_OrangeMilesIntro, OrangeIslands_Kinnow_Text_OrangeMilesDefeat
	msgbox OrangeIslands_Kinnow_Text_OrangeMilesPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Kinnow_EventScript_OrangeFlora::
	trainerbattle_single TRAINER_ORANGE_SWIMMER_F_FLORA, OrangeIslands_Kinnow_Text_OrangeFloraIntro, OrangeIslands_Kinnow_Text_OrangeFloraDefeat
	msgbox OrangeIslands_Kinnow_Text_OrangeFloraPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Kinnow_EventScript_OrangeSean::
	trainerbattle_single TRAINER_ORANGE_SWIMMER_M_SEAN, OrangeIslands_Kinnow_Text_OrangeSeanIntro, OrangeIslands_Kinnow_Text_OrangeSeanDefeat
	msgbox OrangeIslands_Kinnow_Text_OrangeSeanPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Kinnow_Text_OrangeKaneIntro::
	.string "My POKéMON grew up on this\n"
	.string "island! They're tough!$"

OrangeIslands_Kinnow_Text_OrangeKaneDefeat::
	.string "No way!$"

OrangeIslands_Kinnow_Text_OrangeKanePostBattle::
	.string "There are POKéMON here\n"
	.string "you won't find in KANTO.$"

OrangeIslands_Kinnow_Text_OrangeIvanIntro::
	.string "My birds fly between the\n"
	.string "islands every day!$"

OrangeIslands_Kinnow_Text_OrangeIvanDefeat::
	.string "Grounded!$"

OrangeIslands_Kinnow_Text_OrangeIvanPostBattle::
	.string "Sea birds are tougher than\n"
	.string "they look.$"

OrangeIslands_Kinnow_Text_OrangeDeanIntro::
	.string "You look tough. Let's see\n"
	.string "how tough!$"

OrangeIslands_Kinnow_Text_OrangeDeanDefeat::
	.string "You win!$"

OrangeIslands_Kinnow_Text_OrangeDeanPostBattle::
	.string "I'm hiking around every\n"
	.string "island this summer.$"

OrangeIslands_Kinnow_Text_OrangeMilesIntro::
	.string "The bugs on these islands\n"
	.string "are huge! Look!$"

OrangeIslands_Kinnow_Text_OrangeMilesDefeat::
	.string "Squashed!$"

OrangeIslands_Kinnow_Text_OrangeMilesPostBattle::
	.string "Tropical bugs are the\n"
	.string "coolest bugs!$"

OrangeIslands_Kinnow_Text_OrangeFloraIntro::
	.string "My POKéMON and I swim\n"
	.string "together every day!$"

OrangeIslands_Kinnow_Text_OrangeFloraDefeat::
	.string "We'll swim harder!$"

OrangeIslands_Kinnow_Text_OrangeFloraPostBattle::
	.string "Each island has its own\n"
	.string "kind of water. Isn't it neat?$"

OrangeIslands_Kinnow_Text_OrangeSeanIntro::
	.string "The water's perfect today!\n"
	.string "Let's battle out here!$"

OrangeIslands_Kinnow_Text_OrangeSeanDefeat::
	.string "I'm sinking!$"

OrangeIslands_Kinnow_Text_OrangeSeanPostBattle::
	.string "I swim between the islands\n"
	.string "every morning.$"

@ the return of the crossing above
OrangeIslands_Kinnow_EventScript_SeaEdgeToRoute9::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8004, 13
	goto_if_lt OrangeIslands_Kinnow_EventScript_SeaEdgeToRoute9_Lo
	compare VAR_0x8004, 48
	goto_if_gt OrangeIslands_Kinnow_EventScript_SeaEdgeToRoute9_Hi
OrangeIslands_Kinnow_EventScript_SeaEdgeToRoute9_Go:
	subvar VAR_0x8004, 5
	setvar VAR_0x8005, 1
	warp MAP_ORANGE_ISLANDS_ROUTE_9, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Kinnow_EventScript_SeaEdgeToRoute9_Lo:
	setvar VAR_0x8004, 13
	goto OrangeIslands_Kinnow_EventScript_SeaEdgeToRoute9_Go
OrangeIslands_Kinnow_EventScript_SeaEdgeToRoute9_Hi:
	setvar VAR_0x8004, 48
	goto OrangeIslands_Kinnow_EventScript_SeaEdgeToRoute9_Go
