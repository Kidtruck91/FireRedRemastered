OrangeIslands_Mikan_MapScripts::
	.byte 0

OrangeIslands_Mikan_EventScript_Gym::
	msgbox OrangeIslands_Mikan_Text_Gym, MSGBOX_SIGN
	end

OrangeIslands_Mikan_EventScript_Beach::
	msgbox OrangeIslands_Mikan_Text_Beach, MSGBOX_SIGN
	end

OrangeIslands_Mikan_EventScript_Town::
	msgbox OrangeIslands_Mikan_Text_Town, MSGBOX_SIGN
	end

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Route 5
OrangeIslands_Mikan_EventScript_SeaEdgeToRoute5::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8004, 8
	goto_if_lt OrangeIslands_Mikan_EventScript_SeaEdgeToRoute5_Lo
	compare VAR_0x8004, 51
	goto_if_gt OrangeIslands_Mikan_EventScript_SeaEdgeToRoute5_Hi
OrangeIslands_Mikan_EventScript_SeaEdgeToRoute5_Go:
	subvar VAR_0x8004, 8
	setvar VAR_0x8005, 122
	warp MAP_ORANGE_ISLANDS_ROUTE_5, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Mikan_EventScript_SeaEdgeToRoute5_Lo:
	setvar VAR_0x8004, 8
	goto OrangeIslands_Mikan_EventScript_SeaEdgeToRoute5_Go
OrangeIslands_Mikan_EventScript_SeaEdgeToRoute5_Hi:
	setvar VAR_0x8004, 51
	goto OrangeIslands_Mikan_EventScript_SeaEdgeToRoute5_Go

@ ---- Orange Islands trainers (orange_route_trainers.py) ----
OrangeIslands_Mikan_EventScript_OrangeRicky::
	trainerbattle_single TRAINER_ORANGE_CAMPER_RICKY, OrangeIslands_Mikan_Text_OrangeRickyIntro, OrangeIslands_Mikan_Text_OrangeRickyDefeat
	msgbox OrangeIslands_Mikan_Text_OrangeRickyPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Mikan_EventScript_OrangeTyler::
	trainerbattle_single TRAINER_ORANGE_BUG_CATCHER_TYLER, OrangeIslands_Mikan_Text_OrangeTylerIntro, OrangeIslands_Mikan_Text_OrangeTylerDefeat
	msgbox OrangeIslands_Mikan_Text_OrangeTylerPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Mikan_EventScript_OrangeCarla::
	trainerbattle_single TRAINER_ORANGE_PICNICKER_CARLA, OrangeIslands_Mikan_Text_OrangeCarlaIntro, OrangeIslands_Mikan_Text_OrangeCarlaDefeat
	msgbox OrangeIslands_Mikan_Text_OrangeCarlaPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Mikan_EventScript_OrangeFinn::
	trainerbattle_single TRAINER_ORANGE_HIKER_FINN, OrangeIslands_Mikan_Text_OrangeFinnIntro, OrangeIslands_Mikan_Text_OrangeFinnDefeat
	msgbox OrangeIslands_Mikan_Text_OrangeFinnPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Mikan_EventScript_OrangeRyan::
	trainerbattle_single TRAINER_ORANGE_TUBER_RYAN, OrangeIslands_Mikan_Text_OrangeRyanIntro, OrangeIslands_Mikan_Text_OrangeRyanDefeat
	msgbox OrangeIslands_Mikan_Text_OrangeRyanPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Mikan_Text_OrangeRickyIntro::
	.string "You look tough. Let's see\n"
	.string "how tough!$"

OrangeIslands_Mikan_Text_OrangeRickyDefeat::
	.string "You win!$"

OrangeIslands_Mikan_Text_OrangeRickyPostBattle::
	.string "I'm hiking around every\n"
	.string "island this summer.$"

OrangeIslands_Mikan_Text_OrangeTylerIntro::
	.string "The bugs on these islands\n"
	.string "are huge! Look!$"

OrangeIslands_Mikan_Text_OrangeTylerDefeat::
	.string "Squashed!$"

OrangeIslands_Mikan_Text_OrangeTylerPostBattle::
	.string "Tropical bugs are the\n"
	.string "coolest bugs!$"

OrangeIslands_Mikan_Text_OrangeCarlaIntro::
	.string "Let's have a fun battle!$"

OrangeIslands_Mikan_Text_OrangeCarlaDefeat::
	.string "That was fun anyway!$"

OrangeIslands_Mikan_Text_OrangeCarlaPostBattle::
	.string "I'm visiting every island\n"
	.string "on my holiday!$"

OrangeIslands_Mikan_Text_OrangeFinnIntro::
	.string "Hah! I climbed this whole\n"
	.string "island! Now, you!$"

OrangeIslands_Mikan_Text_OrangeFinnDefeat::
	.string "Rockslide!$"

OrangeIslands_Mikan_Text_OrangeFinnPostBattle::
	.string "I found some strange\n"
	.string "crystals in a cave here.$"

OrangeIslands_Mikan_Text_OrangeRyanIntro::
	.string "Splash splash!\n"
	.string "Battle me!$"

OrangeIslands_Mikan_Text_OrangeRyanDefeat::
	.string "My float popped!$"

OrangeIslands_Mikan_Text_OrangeRyanPostBattle::
	.string "My big sister swims way\n"
	.string "out past the rocks.$"

