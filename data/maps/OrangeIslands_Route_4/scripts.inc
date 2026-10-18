OrangeIslands_Route_4_MapScripts::
	.byte 0

OrangeIslands_Route_4_EventScript_West::
	msgbox OrangeIslands_Route_4_Text_West, MSGBOX_SIGN
	end

OrangeIslands_Route_4_EventScript_East::
	msgbox OrangeIslands_Route_4_Text_East, MSGBOX_SIGN
	end

OrangeIslands_Route_4_EventScript_Sandbar::
	msgbox OrangeIslands_Route_4_Text_Sandbar, MSGBOX_SIGN
	end

@ ---- Orange Islands trainers (orange_route_trainers.py) ----
OrangeIslands_Route_4_EventScript_OrangeBrent::
	trainerbattle_single TRAINER_ORANGE_HIKER_BRENT, OrangeIslands_Route_4_Text_OrangeBrentIntro, OrangeIslands_Route_4_Text_OrangeBrentDefeat
	msgbox OrangeIslands_Route_4_Text_OrangeBrentPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Route_4_EventScript_OrangeGregg::
	trainerbattle_single TRAINER_ORANGE_FISHERMAN_GREGG, OrangeIslands_Route_4_Text_OrangeGreggIntro, OrangeIslands_Route_4_Text_OrangeGreggDefeat
	msgbox OrangeIslands_Route_4_Text_OrangeGreggPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Route_4_EventScript_OrangeMaria::
	trainerbattle_single TRAINER_ORANGE_SWIMMER_F_MARIA, OrangeIslands_Route_4_Text_OrangeMariaIntro, OrangeIslands_Route_4_Text_OrangeMariaDefeat
	msgbox OrangeIslands_Route_4_Text_OrangeMariaPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Route_4_EventScript_OrangeRocco::
	trainerbattle_single TRAINER_ORANGE_SWIMMER_M_ROCCO, OrangeIslands_Route_4_Text_OrangeRoccoIntro, OrangeIslands_Route_4_Text_OrangeRoccoDefeat
	msgbox OrangeIslands_Route_4_Text_OrangeRoccoPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Route_4_EventScript_OrangeFelix::
	trainerbattle_single TRAINER_ORANGE_SWIMMER_M_FELIX, OrangeIslands_Route_4_Text_OrangeFelixIntro, OrangeIslands_Route_4_Text_OrangeFelixDefeat
	msgbox OrangeIslands_Route_4_Text_OrangeFelixPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Route_4_Text_OrangeBrentIntro::
	.string "These island mountains are\n"
	.string "steep! Let's battle!$"

OrangeIslands_Route_4_Text_OrangeBrentDefeat::
	.string "I'm beat!$"

OrangeIslands_Route_4_Text_OrangeBrentPostBattle::
	.string "Volcanic rock makes these\n"
	.string "islands. Hot stuff!$"

OrangeIslands_Route_4_Text_OrangeGreggIntro::
	.string "I've been waiting all day\n"
	.string "for a bite. You'll do!$"

OrangeIslands_Route_4_Text_OrangeGreggDefeat::
	.string "Snapped my line!$"

OrangeIslands_Route_4_Text_OrangeGreggPostBattle::
	.string "Rumor says a huge GYARADOS\n"
	.string "lives near these islands.$"

OrangeIslands_Route_4_Text_OrangeMariaIntro::
	.string "Swimming in the ORANGE\n"
	.string "ISLANDS is the best!$"

OrangeIslands_Route_4_Text_OrangeMariaDefeat::
	.string "Oh, I lost!$"

OrangeIslands_Route_4_Text_OrangeMariaPostBattle::
	.string "The reefs are so pretty\n"
	.string "from out here.$"

OrangeIslands_Route_4_Text_OrangeRoccoIntro::
	.string "You're surfing? Then you\n"
	.string "can't escape me!$"

OrangeIslands_Route_4_Text_OrangeRoccoDefeat::
	.string "Glub... You win.$"

OrangeIslands_Route_4_Text_OrangeRoccoPostBattle::
	.string "The currents here are strong.\n"
	.string "Watch where they push you.$"

OrangeIslands_Route_4_Text_OrangeFelixIntro::
	.string "You're surfing? Then you\n"
	.string "can't escape me!$"

OrangeIslands_Route_4_Text_OrangeFelixDefeat::
	.string "Glub... You win.$"

OrangeIslands_Route_4_Text_OrangeFelixPostBattle::
	.string "The currents here are strong.\n"
	.string "Watch where they push you.$"

