OrangeIslands_Navel_Beach_MapScripts::
	.byte 0

OrangeIslands_Navel_Beach_EventScript_Town::
	msgbox OrangeIslands_Navel_Beach_Text_Town, MSGBOX_SIGN
	end

OrangeIslands_Navel_Beach_EventScript_Harbor::
	msgbox OrangeIslands_Navel_Beach_Text_Harbor, MSGBOX_SIGN
	end

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Route 11
OrangeIslands_Navel_Beach_EventScript_SeaEdgeToRoute11::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8005, 1
	goto_if_lt OrangeIslands_Navel_Beach_EventScript_SeaEdgeToRoute11_Lo
	compare VAR_0x8005, 36
	goto_if_gt OrangeIslands_Navel_Beach_EventScript_SeaEdgeToRoute11_Hi
OrangeIslands_Navel_Beach_EventScript_SeaEdgeToRoute11_Go:
	addvar VAR_0x8005, 42
	setvar VAR_0x8004, 78
	warp MAP_ORANGE_ISLANDS_ROUTE_11, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Navel_Beach_EventScript_SeaEdgeToRoute11_Lo:
	setvar VAR_0x8005, 1
	goto OrangeIslands_Navel_Beach_EventScript_SeaEdgeToRoute11_Go
OrangeIslands_Navel_Beach_EventScript_SeaEdgeToRoute11_Hi:
	setvar VAR_0x8005, 36
	goto OrangeIslands_Navel_Beach_EventScript_SeaEdgeToRoute11_Go

@ ---- Orange Islands trainers (orange_route_trainers.py) ----
OrangeIslands_Navel_Beach_EventScript_OrangeMason::
	trainerbattle_single TRAINER_ORANGE_SWIMMER_M_MASON, OrangeIslands_Navel_Beach_Text_OrangeMasonIntro, OrangeIslands_Navel_Beach_Text_OrangeMasonDefeat
	msgbox OrangeIslands_Navel_Beach_Text_OrangeMasonPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Navel_Beach_EventScript_OrangeFrank::
	trainerbattle_single TRAINER_ORANGE_FISHERMAN_FRANK, OrangeIslands_Navel_Beach_Text_OrangeFrankIntro, OrangeIslands_Navel_Beach_Text_OrangeFrankDefeat
	msgbox OrangeIslands_Navel_Beach_Text_OrangeFrankPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Navel_Beach_EventScript_OrangeBeth::
	trainerbattle_single TRAINER_ORANGE_SWIMMER_F_BETH, OrangeIslands_Navel_Beach_Text_OrangeBethIntro, OrangeIslands_Navel_Beach_Text_OrangeBethDefeat
	msgbox OrangeIslands_Navel_Beach_Text_OrangeBethPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Navel_Beach_EventScript_OrangeWes::
	trainerbattle_single TRAINER_ORANGE_TUBER_WES, OrangeIslands_Navel_Beach_Text_OrangeWesIntro, OrangeIslands_Navel_Beach_Text_OrangeWesDefeat
	msgbox OrangeIslands_Navel_Beach_Text_OrangeWesPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Navel_Beach_EventScript_OrangeRae::
	trainerbattle_single TRAINER_ORANGE_SWIMMER_F_RAE, OrangeIslands_Navel_Beach_Text_OrangeRaeIntro, OrangeIslands_Navel_Beach_Text_OrangeRaeDefeat
	msgbox OrangeIslands_Navel_Beach_Text_OrangeRaePostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Navel_Beach_EventScript_OrangeSelma::
	trainerbattle_single TRAINER_ORANGE_SWIMMER_F_SELMA, OrangeIslands_Navel_Beach_Text_OrangeSelmaIntro, OrangeIslands_Navel_Beach_Text_OrangeSelmaDefeat
	msgbox OrangeIslands_Navel_Beach_Text_OrangeSelmaPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Navel_Beach_Text_OrangeMasonIntro::
	.string "The water's perfect today!\n"
	.string "Let's battle out here!$"

OrangeIslands_Navel_Beach_Text_OrangeMasonDefeat::
	.string "I'm sinking!$"

OrangeIslands_Navel_Beach_Text_OrangeMasonPostBattle::
	.string "I swim between the islands\n"
	.string "every morning.$"

OrangeIslands_Navel_Beach_Text_OrangeFrankIntro::
	.string "Look at this catch!\n"
	.string "Want to see it in battle?$"

OrangeIslands_Navel_Beach_Text_OrangeFrankDefeat::
	.string "It got away...$"

OrangeIslands_Navel_Beach_Text_OrangeFrankPostBattle::
	.string "Fishing is like battling.\n"
	.string "Timing is everything.$"

OrangeIslands_Navel_Beach_Text_OrangeBethIntro::
	.string "My POKéMON and I swim\n"
	.string "together every day!$"

OrangeIslands_Navel_Beach_Text_OrangeBethDefeat::
	.string "We'll swim harder!$"

OrangeIslands_Navel_Beach_Text_OrangeBethPostBattle::
	.string "Each island has its own\n"
	.string "kind of water. Isn't it neat?$"

OrangeIslands_Navel_Beach_Text_OrangeWesIntro::
	.string "I can't swim yet, but my\n"
	.string "POKéMON can!$"

OrangeIslands_Navel_Beach_Text_OrangeWesDefeat::
	.string "Waaah!$"

OrangeIslands_Navel_Beach_Text_OrangeWesPostBattle::
	.string "When I grow up, I'll swim\n"
	.string "to every island!$"

OrangeIslands_Navel_Beach_Text_OrangeRaeIntro::
	.string "My POKéMON and I swim\n"
	.string "together every day!$"

OrangeIslands_Navel_Beach_Text_OrangeRaeDefeat::
	.string "We'll swim harder!$"

OrangeIslands_Navel_Beach_Text_OrangeRaePostBattle::
	.string "Each island has its own\n"
	.string "kind of water. Isn't it neat?$"

OrangeIslands_Navel_Beach_Text_OrangeSelmaIntro::
	.string "My POKéMON and I swim\n"
	.string "together every day!$"

OrangeIslands_Navel_Beach_Text_OrangeSelmaDefeat::
	.string "We'll swim harder!$"

OrangeIslands_Navel_Beach_Text_OrangeSelmaPostBattle::
	.string "Each island has its own\n"
	.string "kind of water. Isn't it neat?$"

