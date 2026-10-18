OrangeIslands_Route_17_MapScripts::
	.byte 0

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Route 10
OrangeIslands_Route_17_EventScript_SeaEdgeToRoute10::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8005, 43
	goto_if_lt OrangeIslands_Route_17_EventScript_SeaEdgeToRoute10_Lo
	compare VAR_0x8005, 55
	goto_if_gt OrangeIslands_Route_17_EventScript_SeaEdgeToRoute10_Hi
OrangeIslands_Route_17_EventScript_SeaEdgeToRoute10_Go:
	subvar VAR_0x8005, 42
	setvar VAR_0x8004, 1
	warp MAP_ORANGE_ISLANDS_ROUTE_10, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Route_17_EventScript_SeaEdgeToRoute10_Lo:
	setvar VAR_0x8005, 43
	goto OrangeIslands_Route_17_EventScript_SeaEdgeToRoute10_Go
OrangeIslands_Route_17_EventScript_SeaEdgeToRoute10_Hi:
	setvar VAR_0x8005, 55
	goto OrangeIslands_Route_17_EventScript_SeaEdgeToRoute10_Go

@ ---- Orange Islands trainers (orange_route_trainers.py) ----
OrangeIslands_Route_17_EventScript_OrangeCarl::
	trainerbattle_single TRAINER_ORANGE_FISHERMAN_CARL, OrangeIslands_Route_17_Text_OrangeCarlIntro, OrangeIslands_Route_17_Text_OrangeCarlDefeat
	msgbox OrangeIslands_Route_17_Text_OrangeCarlPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Route_17_EventScript_OrangeClara::
	trainerbattle_single TRAINER_ORANGE_SWIMMER_F_CLARA, OrangeIslands_Route_17_Text_OrangeClaraIntro, OrangeIslands_Route_17_Text_OrangeClaraDefeat
	msgbox OrangeIslands_Route_17_Text_OrangeClaraPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Route_17_EventScript_OrangeLisa::
	trainerbattle_single TRAINER_ORANGE_SWIMMER_F_LISA, OrangeIslands_Route_17_Text_OrangeLisaIntro, OrangeIslands_Route_17_Text_OrangeLisaDefeat
	msgbox OrangeIslands_Route_17_Text_OrangeLisaPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Route_17_Text_OrangeCarlIntro::
	.string "Shh! You'll scare the fish!\p"
	.string "Well, now that they're gone,\n"
	.string "let's battle!$"

OrangeIslands_Route_17_Text_OrangeCarlDefeat::
	.string "Reeled in!$"

OrangeIslands_Route_17_Text_OrangeCarlPostBattle::
	.string "The big ones bite at dawn.\n"
	.string "Patience is everything.$"

OrangeIslands_Route_17_Text_OrangeClaraIntro::
	.string "Swimming in the ORANGE\n"
	.string "ISLANDS is the best!$"

OrangeIslands_Route_17_Text_OrangeClaraDefeat::
	.string "Oh, I lost!$"

OrangeIslands_Route_17_Text_OrangeClaraPostBattle::
	.string "The reefs are so pretty\n"
	.string "from out here.$"

OrangeIslands_Route_17_Text_OrangeLisaIntro::
	.string "Hey! You splashed me!\n"
	.string "That means a battle!$"

OrangeIslands_Route_17_Text_OrangeLisaDefeat::
	.string "Fine, you win.$"

OrangeIslands_Route_17_Text_OrangeLisaPostBattle::
	.string "Watch out for TENTACOOL\n"
	.string "near the reefs.$"

