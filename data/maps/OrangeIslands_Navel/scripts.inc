OrangeIslands_Navel_MapScripts::
	.byte 0

OrangeIslands_Navel_EventScript_Gym::
	msgbox OrangeIslands_Navel_Text_Gym, MSGBOX_SIGN
	end

OrangeIslands_Navel_EventScript_Town::
	msgbox OrangeIslands_Navel_Text_Town, MSGBOX_SIGN
	end

OrangeIslands_Navel_EventScript_Beach::
	msgbox OrangeIslands_Navel_Text_Beach, MSGBOX_SIGN
	end

OrangeIslands_Navel_EventScript_WestTrail::
	msgbox OrangeIslands_Navel_Text_WestTrail, MSGBOX_SIGN
	end

OrangeIslands_Navel_EventScript_EastTrail::
	msgbox OrangeIslands_Navel_Text_EastTrail, MSGBOX_SIGN
	end

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Route 5
OrangeIslands_Navel_EventScript_SeaEdgeToRoute5::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8004, 23
	goto_if_lt OrangeIslands_Navel_EventScript_SeaEdgeToRoute5_Lo
	compare VAR_0x8004, 66
	goto_if_gt OrangeIslands_Navel_EventScript_SeaEdgeToRoute5_Hi
OrangeIslands_Navel_EventScript_SeaEdgeToRoute5_Go:
	subvar VAR_0x8004, 21
	setvar VAR_0x8005, 1
	warp MAP_ORANGE_ISLANDS_ROUTE_5, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Navel_EventScript_SeaEdgeToRoute5_Lo:
	setvar VAR_0x8004, 23
	goto OrangeIslands_Navel_EventScript_SeaEdgeToRoute5_Go
OrangeIslands_Navel_EventScript_SeaEdgeToRoute5_Hi:
	setvar VAR_0x8004, 66
	goto OrangeIslands_Navel_EventScript_SeaEdgeToRoute5_Go

@ ---- Orange Islands trainers (orange_route_trainers.py) ----
OrangeIslands_Navel_EventScript_OrangePedro::
	trainerbattle_single TRAINER_ORANGE_BLACK_BELT_PEDRO, OrangeIslands_Navel_Text_OrangePedroIntro, OrangeIslands_Navel_Text_OrangePedroDefeat
	msgbox OrangeIslands_Navel_Text_OrangePedroPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Navel_EventScript_OrangePablo::
	trainerbattle_single TRAINER_ORANGE_CAMPER_PABLO, OrangeIslands_Navel_Text_OrangePabloIntro, OrangeIslands_Navel_Text_OrangePabloDefeat
	msgbox OrangeIslands_Navel_Text_OrangePabloPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Navel_EventScript_OrangeCraig::
	trainerbattle_single TRAINER_ORANGE_YOUNGSTER_CRAIG, OrangeIslands_Navel_Text_OrangeCraigIntro, OrangeIslands_Navel_Text_OrangeCraigDefeat
	msgbox OrangeIslands_Navel_Text_OrangeCraigPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Navel_EventScript_OrangeSienna::
	trainerbattle_single TRAINER_ORANGE_LASS_SIENNA, OrangeIslands_Navel_Text_OrangeSiennaIntro, OrangeIslands_Navel_Text_OrangeSiennaDefeat
	msgbox OrangeIslands_Navel_Text_OrangeSiennaPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Navel_EventScript_OrangeOliver::
	trainerbattle_single TRAINER_ORANGE_FISHERMAN_OLIVER, OrangeIslands_Navel_Text_OrangeOliverIntro, OrangeIslands_Navel_Text_OrangeOliverDefeat
	msgbox OrangeIslands_Navel_Text_OrangeOliverPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Navel_EventScript_OrangeCody::
	trainerbattle_single TRAINER_ORANGE_TUBER_CODY, OrangeIslands_Navel_Text_OrangeCodyIntro, OrangeIslands_Navel_Text_OrangeCodyDefeat
	msgbox OrangeIslands_Navel_Text_OrangeCodyPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Navel_EventScript_OrangeLeo::
	trainerbattle_single TRAINER_ORANGE_TUBER_LEO, OrangeIslands_Navel_Text_OrangeLeoIntro, OrangeIslands_Navel_Text_OrangeLeoDefeat
	msgbox OrangeIslands_Navel_Text_OrangeLeoPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Navel_Text_OrangePedroIntro::
	.string "My fists are ready!\n"
	.string "Are yours?$"

OrangeIslands_Navel_Text_OrangePedroDefeat::
	.string "Argh!$"

OrangeIslands_Navel_Text_OrangePedroPostBattle::
	.string "A true fighter never stops\n"
	.string "training.$"

OrangeIslands_Navel_Text_OrangePabloIntro::
	.string "You look tough. Let's see\n"
	.string "how tough!$"

OrangeIslands_Navel_Text_OrangePabloDefeat::
	.string "You win!$"

OrangeIslands_Navel_Text_OrangePabloPostBattle::
	.string "I'm hiking around every\n"
	.string "island this summer.$"

OrangeIslands_Navel_Text_OrangeCraigIntro::
	.string "Let's battle! I've been\n"
	.string "waiting for a challenger!$"

OrangeIslands_Navel_Text_OrangeCraigDefeat::
	.string "You're strong!$"

OrangeIslands_Navel_Text_OrangeCraigPostBattle::
	.string "I'll train more and battle\n"
	.string "you again!$"

OrangeIslands_Navel_Text_OrangeSiennaIntro::
	.string "Ooh, a new face!\n"
	.string "Let's battle!$"

OrangeIslands_Navel_Text_OrangeSiennaDefeat::
	.string "You're so mean!$"

OrangeIslands_Navel_Text_OrangeSiennaPostBattle::
	.string "I love the flowers on this\n"
	.string "island.$"

OrangeIslands_Navel_Text_OrangeOliverIntro::
	.string "Shh! You'll scare the fish!\p"
	.string "Well, now that they're gone,\n"
	.string "let's battle!$"

OrangeIslands_Navel_Text_OrangeOliverDefeat::
	.string "Reeled in!$"

OrangeIslands_Navel_Text_OrangeOliverPostBattle::
	.string "The big ones bite at dawn.\n"
	.string "Patience is everything.$"

OrangeIslands_Navel_Text_OrangeCodyIntro::
	.string "I can't swim yet, but my\n"
	.string "POKéMON can!$"

OrangeIslands_Navel_Text_OrangeCodyDefeat::
	.string "Waaah!$"

OrangeIslands_Navel_Text_OrangeCodyPostBattle::
	.string "When I grow up, I'll swim\n"
	.string "to every island!$"

OrangeIslands_Navel_Text_OrangeLeoIntro::
	.string "Look at my float!\n"
	.string "Wanna battle?$"

OrangeIslands_Navel_Text_OrangeLeoDefeat::
	.string "No fair!$"

OrangeIslands_Navel_Text_OrangeLeoPostBattle::
	.string "Sand castles are fun too!$"

@ ---- HM06 backstop (rocksmash_backstop.py) ----
OrangeIslands_Navel_EventScript_RockSmashTip::
	lock
	faceplayer
	goto_if_unset FLAG_GOT_HM06, OrangeIslands_Navel_EventScript_RockSmashTipNeeded
	msgbox OrangeIslands_Navel_Text_RockSmashTipHave
	release
	end

OrangeIslands_Navel_EventScript_RockSmashTipNeeded::
	msgbox OrangeIslands_Navel_Text_RockSmashTipNeeded
	release
	end

OrangeIslands_Navel_Text_RockSmashTipNeeded::
	.string "Heading up to DANNY's? You'll\n"
	.string "get nowhere without ROCK SMASH.\p"
	.string "Don't have it? Nobody does.\n"
	.string "It's the one nobody makes you\l"
	.string "pick up.\p"
	.string "There's an old hiker deep in the\n"
	.string "cave on SUNBURST. He keeps a\l"
	.string "spare. Tell him I sent you.$"

OrangeIslands_Navel_Text_RockSmashTipHave::
	.string "You've got ROCK SMASH and\n"
	.string "STRENGTH? Then DANNY's gym is\l"
	.string "just a puzzle.$"

@ west of the stretch Route 5 uses
OrangeIslands_Navel_EventScript_SeaEdgeToRoute9::
	getplayerxy VAR_0x8005, VAR_0x8004
	compare VAR_0x8005, 2
	goto_if_lt OrangeIslands_Navel_EventScript_SeaEdgeToRoute9_Lo
	compare VAR_0x8005, 9
	goto_if_gt OrangeIslands_Navel_EventScript_SeaEdgeToRoute9_Hi
OrangeIslands_Navel_EventScript_SeaEdgeToRoute9_Go:
	addvar VAR_0x8005, 104
	setvar VAR_0x8004, 50
	warp MAP_ORANGE_ISLANDS_ROUTE_9, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Navel_EventScript_SeaEdgeToRoute9_Lo:
	setvar VAR_0x8005, 2
	goto OrangeIslands_Navel_EventScript_SeaEdgeToRoute9_Go
OrangeIslands_Navel_EventScript_SeaEdgeToRoute9_Hi:
	setvar VAR_0x8005, 9
	goto OrangeIslands_Navel_EventScript_SeaEdgeToRoute9_Go
