OrangeIslands_Mandarin_South_MapScripts::
	.byte 0

OrangeIslands_Mandarin_South_EventScript_Beach::
	msgbox OrangeIslands_Mandarin_South_Text_Beach, MSGBOX_SIGN
	end

OrangeIslands_Mandarin_South_EventScript_Town::
	msgbox OrangeIslands_Mandarin_South_Text_Town, MSGBOX_SIGN
	end

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Route 6
OrangeIslands_Mandarin_South_EventScript_SeaEdgeToRoute6::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8005, 68
	goto_if_lt OrangeIslands_Mandarin_South_EventScript_SeaEdgeToRoute6_Lo
	compare VAR_0x8005, 75
	goto_if_gt OrangeIslands_Mandarin_South_EventScript_SeaEdgeToRoute6_Hi
OrangeIslands_Mandarin_South_EventScript_SeaEdgeToRoute6_Go:
	subvar VAR_0x8005, 10
	setvar VAR_0x8004, 1
	warp MAP_ORANGE_ISLANDS_ROUTE_6, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Mandarin_South_EventScript_SeaEdgeToRoute6_Lo:
	setvar VAR_0x8005, 68
	goto OrangeIslands_Mandarin_South_EventScript_SeaEdgeToRoute6_Go
OrangeIslands_Mandarin_South_EventScript_SeaEdgeToRoute6_Hi:
	setvar VAR_0x8005, 75
	goto OrangeIslands_Mandarin_South_EventScript_SeaEdgeToRoute6_Go

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Route 8
OrangeIslands_Mandarin_South_EventScript_SeaEdgeToRoute8::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8004, 1
	goto_if_lt OrangeIslands_Mandarin_South_EventScript_SeaEdgeToRoute8_Lo
	compare VAR_0x8004, 94
	goto_if_gt OrangeIslands_Mandarin_South_EventScript_SeaEdgeToRoute8_Hi
OrangeIslands_Mandarin_South_EventScript_SeaEdgeToRoute8_Go:
	addvar VAR_0x8004, 36
	setvar VAR_0x8005, 1
	warp MAP_ORANGE_ISLANDS_ROUTE_8, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Mandarin_South_EventScript_SeaEdgeToRoute8_Lo:
	setvar VAR_0x8004, 1
	goto OrangeIslands_Mandarin_South_EventScript_SeaEdgeToRoute8_Go
OrangeIslands_Mandarin_South_EventScript_SeaEdgeToRoute8_Hi:
	setvar VAR_0x8004, 94
	goto OrangeIslands_Mandarin_South_EventScript_SeaEdgeToRoute8_Go

@ ---- Orange Islands trainers (orange_route_trainers.py) ----
OrangeIslands_Mandarin_South_EventScript_OrangeLuna::
	trainerbattle_single TRAINER_ORANGE_PICNICKER_LUNA, OrangeIslands_Mandarin_South_Text_OrangeLunaIntro, OrangeIslands_Mandarin_South_Text_OrangeLunaDefeat
	msgbox OrangeIslands_Mandarin_South_Text_OrangeLunaPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Mandarin_South_EventScript_OrangeRoss::
	trainerbattle_single TRAINER_ORANGE_BIRD_KEEPER_ROSS, OrangeIslands_Mandarin_South_Text_OrangeRossIntro, OrangeIslands_Mandarin_South_Text_OrangeRossDefeat
	msgbox OrangeIslands_Mandarin_South_Text_OrangeRossPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Mandarin_South_EventScript_OrangeHugo::
	trainerbattle_single TRAINER_ORANGE_YOUNGSTER_HUGO, OrangeIslands_Mandarin_South_Text_OrangeHugoIntro, OrangeIslands_Mandarin_South_Text_OrangeHugoDefeat
	msgbox OrangeIslands_Mandarin_South_Text_OrangeHugoPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Mandarin_South_EventScript_OrangeDale::
	trainerbattle_single TRAINER_ORANGE_HIKER_DALE, OrangeIslands_Mandarin_South_Text_OrangeDaleIntro, OrangeIslands_Mandarin_South_Text_OrangeDaleDefeat
	msgbox OrangeIslands_Mandarin_South_Text_OrangeDalePostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Mandarin_South_EventScript_OrangeHank::
	trainerbattle_single TRAINER_ORANGE_HIKER_HANK, OrangeIslands_Mandarin_South_Text_OrangeHankIntro, OrangeIslands_Mandarin_South_Text_OrangeHankDefeat
	msgbox OrangeIslands_Mandarin_South_Text_OrangeHankPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Mandarin_South_EventScript_OrangeDamon::
	trainerbattle_single TRAINER_ORANGE_SWIMMER_M_DAMON, OrangeIslands_Mandarin_South_Text_OrangeDamonIntro, OrangeIslands_Mandarin_South_Text_OrangeDamonDefeat
	msgbox OrangeIslands_Mandarin_South_Text_OrangeDamonPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Mandarin_South_EventScript_OrangeLaila::
	trainerbattle_single TRAINER_ORANGE_SWIMMER_F_LAILA, OrangeIslands_Mandarin_South_Text_OrangeLailaIntro, OrangeIslands_Mandarin_South_Text_OrangeLailaDefeat
	msgbox OrangeIslands_Mandarin_South_Text_OrangeLailaPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Mandarin_South_Text_OrangeLunaIntro::
	.string "Let's have a fun battle!$"

OrangeIslands_Mandarin_South_Text_OrangeLunaDefeat::
	.string "That was fun anyway!$"

OrangeIslands_Mandarin_South_Text_OrangeLunaPostBattle::
	.string "I'm visiting every island\n"
	.string "on my holiday!$"

OrangeIslands_Mandarin_South_Text_OrangeRossIntro::
	.string "My birds fly between the\n"
	.string "islands every day!$"

OrangeIslands_Mandarin_South_Text_OrangeRossDefeat::
	.string "Grounded!$"

OrangeIslands_Mandarin_South_Text_OrangeRossPostBattle::
	.string "Sea birds are tougher than\n"
	.string "they look.$"

OrangeIslands_Mandarin_South_Text_OrangeHugoIntro::
	.string "Hey! You're not from\n"
	.string "around here, are you?$"

OrangeIslands_Mandarin_South_Text_OrangeHugoDefeat::
	.string "Aww, man!$"

OrangeIslands_Mandarin_South_Text_OrangeHugoPostBattle::
	.string "I want to beat the ORANGE\n"
	.string "CREW someday!$"

OrangeIslands_Mandarin_South_Text_OrangeDaleIntro::
	.string "These island mountains are\n"
	.string "steep! Let's battle!$"

OrangeIslands_Mandarin_South_Text_OrangeDaleDefeat::
	.string "I'm beat!$"

OrangeIslands_Mandarin_South_Text_OrangeDalePostBattle::
	.string "Volcanic rock makes these\n"
	.string "islands. Hot stuff!$"

OrangeIslands_Mandarin_South_Text_OrangeHankIntro::
	.string "Hey, kid! Test your team\n"
	.string "against my rocks!$"

OrangeIslands_Mandarin_South_Text_OrangeHankDefeat::
	.string "Crumbled!$"

OrangeIslands_Mandarin_South_Text_OrangeHankPostBattle::
	.string "Good boots are a hiker's\n"
	.string "best friend.$"

OrangeIslands_Mandarin_South_Text_OrangeDamonIntro::
	.string "The water's perfect today!\n"
	.string "Let's battle out here!$"

OrangeIslands_Mandarin_South_Text_OrangeDamonDefeat::
	.string "I'm sinking!$"

OrangeIslands_Mandarin_South_Text_OrangeDamonPostBattle::
	.string "I swim between the islands\n"
	.string "every morning.$"

OrangeIslands_Mandarin_South_Text_OrangeLailaIntro::
	.string "Swimming in the ORANGE\n"
	.string "ISLANDS is the best!$"

OrangeIslands_Mandarin_South_Text_OrangeLailaDefeat::
	.string "Oh, I lost!$"

OrangeIslands_Mandarin_South_Text_OrangeLailaPostBattle::
	.string "The reefs are so pretty\n"
	.string "from out here.$"

