OrangeIslands_Trovita_Falls_MapScripts::
	.byte 0

OrangeIslands_Trovita_Falls_EventScript_WalkToTrovita::
	warp MAP_ORANGE_ISLANDS_TROVITA, 33, 13
	waitstate
	end

@ ---- Orange Islands trainers (orange_route_trainers.py) ----
OrangeIslands_Trovita_Falls_EventScript_OrangeMilo::
	trainerbattle_single TRAINER_ORANGE_HIKER_MILO, OrangeIslands_Trovita_Falls_Text_OrangeMiloIntro, OrangeIslands_Trovita_Falls_Text_OrangeMiloDefeat
	msgbox OrangeIslands_Trovita_Falls_Text_OrangeMiloPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Trovita_Falls_Text_OrangeMiloIntro::
	.string "These island mountains are\n"
	.string "steep! Let's battle!$"

OrangeIslands_Trovita_Falls_Text_OrangeMiloDefeat::
	.string "I'm beat!$"

OrangeIslands_Trovita_Falls_Text_OrangeMiloPostBattle::
	.string "Volcanic rock makes these\n"
	.string "islands. Hot stuff!$"

