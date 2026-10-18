OrangeIslands_Trovita_MapScripts::
	.byte 0

OrangeIslands_Trovita_EventScript_Beach::
	msgbox OrangeIslands_Trovita_Text_Beach, MSGBOX_SIGN
	end

OrangeIslands_Trovita_EventScript_Town::
	msgbox OrangeIslands_Trovita_Text_Town, MSGBOX_SIGN
	end

OrangeIslands_Trovita_EventScript_WalkToTrovitaFalls::
	warp MAP_ORANGE_ISLANDS_TROVITA_FALLS, 22, 53
	waitstate
	end

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Route 14
OrangeIslands_Trovita_EventScript_SeaEdgeToRoute14::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8005, 41
	goto_if_lt OrangeIslands_Trovita_EventScript_SeaEdgeToRoute14_Lo
	compare VAR_0x8005, 54
	goto_if_gt OrangeIslands_Trovita_EventScript_SeaEdgeToRoute14_Hi
OrangeIslands_Trovita_EventScript_SeaEdgeToRoute14_Go:
	subvar VAR_0x8005, 40
	setvar VAR_0x8004, 86
	warp MAP_ORANGE_ISLANDS_ROUTE_14, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Trovita_EventScript_SeaEdgeToRoute14_Lo:
	setvar VAR_0x8005, 41
	goto OrangeIslands_Trovita_EventScript_SeaEdgeToRoute14_Go
OrangeIslands_Trovita_EventScript_SeaEdgeToRoute14_Hi:
	setvar VAR_0x8005, 54
	goto OrangeIslands_Trovita_EventScript_SeaEdgeToRoute14_Go

@ ---- Orange Islands trainers (orange_route_trainers.py) ----
OrangeIslands_Trovita_EventScript_OrangeSam::
	trainerbattle_single TRAINER_ORANGE_FISHERMAN_SAM, OrangeIslands_Trovita_Text_OrangeSamIntro, OrangeIslands_Trovita_Text_OrangeSamDefeat
	msgbox OrangeIslands_Trovita_Text_OrangeSamPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Trovita_EventScript_OrangeAnna::
	trainerbattle_single TRAINER_ORANGE_PSYCHIC_ANNA, OrangeIslands_Trovita_Text_OrangeAnnaIntro, OrangeIslands_Trovita_Text_OrangeAnnaDefeat
	msgbox OrangeIslands_Trovita_Text_OrangeAnnaPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Trovita_EventScript_OrangeSusie::
	trainerbattle_single TRAINER_ORANGE_COOLTRAINER_SUSIE, OrangeIslands_Trovita_Text_OrangeSusieIntro, OrangeIslands_Trovita_Text_OrangeSusieDefeat
	msgbox OrangeIslands_Trovita_Text_OrangeSusiePostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Trovita_EventScript_OrangeJesse::
	trainerbattle_single TRAINER_ORANGE_BUG_CATCHER_JESSE, OrangeIslands_Trovita_Text_OrangeJesseIntro, OrangeIslands_Trovita_Text_OrangeJesseDefeat
	msgbox OrangeIslands_Trovita_Text_OrangeJessePostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Trovita_EventScript_OrangeGwen::
	trainerbattle_single TRAINER_ORANGE_SWIMMER_F_GWEN, OrangeIslands_Trovita_Text_OrangeGwenIntro, OrangeIslands_Trovita_Text_OrangeGwenDefeat
	msgbox OrangeIslands_Trovita_Text_OrangeGwenPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Trovita_Text_OrangeSamIntro::
	.string "Shh! You'll scare the fish!\p"
	.string "Well, now that they're gone,\n"
	.string "let's battle!$"

OrangeIslands_Trovita_Text_OrangeSamDefeat::
	.string "Reeled in!$"

OrangeIslands_Trovita_Text_OrangeSamPostBattle::
	.string "The big ones bite at dawn.\n"
	.string "Patience is everything.$"

OrangeIslands_Trovita_Text_OrangeAnnaIntro::
	.string "Your future holds a battle.\n"
	.string "With me. Right now.$"

OrangeIslands_Trovita_Text_OrangeAnnaDefeat::
	.string "Unforeseen!$"

OrangeIslands_Trovita_Text_OrangeAnnaPostBattle::
	.string "The ORANGE CREW... I see\n"
	.string "great challenges ahead.$"

OrangeIslands_Trovita_Text_OrangeSusieIntro::
	.string "Let's see if you're worth\n"
	.string "the ORANGE LEAGUE.$"

OrangeIslands_Trovita_Text_OrangeSusieDefeat::
	.string "You are. Good luck.$"

OrangeIslands_Trovita_Text_OrangeSusiePostBattle::
	.string "DRAKE is said to be the\n"
	.string "strongest of them all.$"

OrangeIslands_Trovita_Text_OrangeJesseIntro::
	.string "The bugs on these islands\n"
	.string "are huge! Look!$"

OrangeIslands_Trovita_Text_OrangeJesseDefeat::
	.string "Squashed!$"

OrangeIslands_Trovita_Text_OrangeJessePostBattle::
	.string "Tropical bugs are the\n"
	.string "coolest bugs!$"

OrangeIslands_Trovita_Text_OrangeGwenIntro::
	.string "My POKéMON and I swim\n"
	.string "together every day!$"

OrangeIslands_Trovita_Text_OrangeGwenDefeat::
	.string "We'll swim harder!$"

OrangeIslands_Trovita_Text_OrangeGwenPostBattle::
	.string "Each island has its own\n"
	.string "kind of water. Isn't it neat?$"

