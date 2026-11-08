OrangeIslands_Sunburst_MapScripts::
	.byte 0

OrangeIslands_Sunburst_EventScript_Beach::
	msgbox OrangeIslands_Sunburst_Text_Beach, MSGBOX_SIGN
	end

OrangeIslands_Sunburst_EventScript_Town::
	msgbox OrangeIslands_Sunburst_Text_Town, MSGBOX_SIGN
	end

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Route 7
@ SUNBURST north x 50-59 <-> ROUTE 7's east channel, row 87
OrangeIslands_Sunburst_EventScript_SeaEdgeToRoute7::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8004, 50
	goto_if_lt OrangeIslands_Sunburst_EventScript_SeaEdgeToRoute7_Lo
	compare VAR_0x8004, 59
	goto_if_gt OrangeIslands_Sunburst_EventScript_SeaEdgeToRoute7_Hi
OrangeIslands_Sunburst_EventScript_SeaEdgeToRoute7_Go:
	addvar VAR_0x8004, 6
	setvar VAR_0x8005, 87
	warp MAP_ORANGE_ISLANDS_ROUTE_7, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Sunburst_EventScript_SeaEdgeToRoute7_Lo:
	setvar VAR_0x8004, 50
	goto OrangeIslands_Sunburst_EventScript_SeaEdgeToRoute7_Go
OrangeIslands_Sunburst_EventScript_SeaEdgeToRoute7_Hi:
	setvar VAR_0x8004, 59
	goto OrangeIslands_Sunburst_EventScript_SeaEdgeToRoute7_Go

@ ---- Orange Islands trainers (orange_route_trainers.py) ----
OrangeIslands_Sunburst_EventScript_OrangeBen::
	trainerbattle_single TRAINER_ORANGE_FISHERMAN_BEN, OrangeIslands_Sunburst_Text_OrangeBenIntro, OrangeIslands_Sunburst_Text_OrangeBenDefeat
	msgbox OrangeIslands_Sunburst_Text_OrangeBenPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Sunburst_EventScript_OrangeDrew::
	trainerbattle_single TRAINER_ORANGE_YOUNGSTER_DREW, OrangeIslands_Sunburst_Text_OrangeDrewIntro, OrangeIslands_Sunburst_Text_OrangeDrewDefeat
	msgbox OrangeIslands_Sunburst_Text_OrangeDrewPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Sunburst_EventScript_OrangeBella::
	trainerbattle_single TRAINER_ORANGE_PICNICKER_BELLA, OrangeIslands_Sunburst_Text_OrangeBellaIntro, OrangeIslands_Sunburst_Text_OrangeBellaDefeat
	msgbox OrangeIslands_Sunburst_Text_OrangeBellaPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Sunburst_EventScript_OrangeVictor::
	trainerbattle_single TRAINER_ORANGE_BIRD_KEEPER_VICTOR, OrangeIslands_Sunburst_Text_OrangeVictorIntro, OrangeIslands_Sunburst_Text_OrangeVictorDefeat
	msgbox OrangeIslands_Sunburst_Text_OrangeVictorPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Sunburst_EventScript_OrangeChris::
	trainerbattle_single TRAINER_ORANGE_SWIMMER_M_CHRIS, OrangeIslands_Sunburst_Text_OrangeChrisIntro, OrangeIslands_Sunburst_Text_OrangeChrisDefeat
	msgbox OrangeIslands_Sunburst_Text_OrangeChrisPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Sunburst_Text_OrangeBenIntro::
	.string "Look at this catch!\n"
	.string "Want to see it in battle?$"

OrangeIslands_Sunburst_Text_OrangeBenDefeat::
	.string "It got away...$"

OrangeIslands_Sunburst_Text_OrangeBenPostBattle::
	.string "Fishing is like battling.\n"
	.string "Timing is everything.$"

OrangeIslands_Sunburst_Text_OrangeDrewIntro::
	.string "Hey! You're not from\n"
	.string "around here, are you?$"

OrangeIslands_Sunburst_Text_OrangeDrewDefeat::
	.string "Aww, man!$"

OrangeIslands_Sunburst_Text_OrangeDrewPostBattle::
	.string "I want to beat the ORANGE\n"
	.string "CREW someday!$"

OrangeIslands_Sunburst_Text_OrangeBellaIntro::
	.string "A picnic by the sea, and\n"
	.string "now a battle!$"

OrangeIslands_Sunburst_Text_OrangeBellaDefeat::
	.string "My lunch!$"

OrangeIslands_Sunburst_Text_OrangeBellaPostBattle::
	.string "Want a sandwich? Just\n"
	.string "kidding, it's mine.$"

OrangeIslands_Sunburst_Text_OrangeVictorIntro::
	.string "My birds fly between the\n"
	.string "islands every day!$"

OrangeIslands_Sunburst_Text_OrangeVictorDefeat::
	.string "Grounded!$"

OrangeIslands_Sunburst_Text_OrangeVictorPostBattle::
	.string "Sea birds are tougher than\n"
	.string "they look.$"

OrangeIslands_Sunburst_Text_OrangeChrisIntro::
	.string "The water's perfect today!\n"
	.string "Let's battle out here!$"

OrangeIslands_Sunburst_Text_OrangeChrisDefeat::
	.string "I'm sinking!$"

OrangeIslands_Sunburst_Text_OrangeChrisPostBattle::
	.string "I swim between the islands\n"
	.string "every morning.$"

@ --- built by docs/orange-islands/build_sunburst_town.py ---
OrangeIslands_Sunburst_EventScript_Man::
	msgbox OrangeIslands_Sunburst_Text_Man, MSGBOX_NPC
	end

OrangeIslands_Sunburst_EventScript_Girl::
	msgbox OrangeIslands_Sunburst_Text_Girl, MSGBOX_NPC
	end

OrangeIslands_Sunburst_EventScript_OldWoman::
	msgbox OrangeIslands_Sunburst_Text_OldWoman, MSGBOX_NPC
	end

OrangeIslands_Sunburst_EventScript_Youngster::
	msgbox OrangeIslands_Sunburst_Text_Youngster, MSGBOX_NPC
	end

OrangeIslands_Sunburst_EventScript_Boy::
	msgbox OrangeIslands_Sunburst_Text_Boy, MSGBOX_NPC
	end
@ --- end build_sunburst_town.py ---
