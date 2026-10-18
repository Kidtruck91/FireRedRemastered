OrangeIslands_Route_3_MapScripts::
	.byte 0

OrangeIslands_Route_3_EventScript_Passage::
	msgbox OrangeIslands_Route_3_Text_Passage, MSGBOX_SIGN
	end

OrangeIslands_Route_3_EventScript_Islet::
	msgbox OrangeIslands_Route_3_Text_Islet, MSGBOX_SIGN
	end

@ ---- Orange Islands trainers (orange_route_trainers.py) ----
OrangeIslands_Route_3_EventScript_OrangeAllen::
	trainerbattle_single TRAINER_ORANGE_SWIMMER_M_ALLEN, OrangeIslands_Route_3_Text_OrangeAllenIntro, OrangeIslands_Route_3_Text_OrangeAllenDefeat
	msgbox OrangeIslands_Route_3_Text_OrangeAllenPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Route_3_EventScript_OrangeJared::
	trainerbattle_single TRAINER_ORANGE_SWIMMER_M_JARED, OrangeIslands_Route_3_Text_OrangeJaredIntro, OrangeIslands_Route_3_Text_OrangeJaredDefeat
	msgbox OrangeIslands_Route_3_Text_OrangeJaredPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Route_3_EventScript_OrangeScott::
	trainerbattle_single TRAINER_ORANGE_TUBER_SCOTT, OrangeIslands_Route_3_Text_OrangeScottIntro, OrangeIslands_Route_3_Text_OrangeScottDefeat
	msgbox OrangeIslands_Route_3_Text_OrangeScottPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Route_3_EventScript_OrangeMila::
	trainerbattle_single TRAINER_ORANGE_SWIMMER_F_MILA, OrangeIslands_Route_3_Text_OrangeMilaIntro, OrangeIslands_Route_3_Text_OrangeMilaDefeat
	msgbox OrangeIslands_Route_3_Text_OrangeMilaPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Route_3_EventScript_OrangeTony::
	trainerbattle_single TRAINER_ORANGE_SWIMMER_M_TONY, OrangeIslands_Route_3_Text_OrangeTonyIntro, OrangeIslands_Route_3_Text_OrangeTonyDefeat
	msgbox OrangeIslands_Route_3_Text_OrangeTonyPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Route_3_Text_OrangeAllenIntro::
	.string "The water's perfect today!\n"
	.string "Let's battle out here!$"

OrangeIslands_Route_3_Text_OrangeAllenDefeat::
	.string "I'm sinking!$"

OrangeIslands_Route_3_Text_OrangeAllenPostBattle::
	.string "I swim between the islands\n"
	.string "every morning.$"

OrangeIslands_Route_3_Text_OrangeJaredIntro::
	.string "You're surfing? Then you\n"
	.string "can't escape me!$"

OrangeIslands_Route_3_Text_OrangeJaredDefeat::
	.string "Glub... You win.$"

OrangeIslands_Route_3_Text_OrangeJaredPostBattle::
	.string "The currents here are strong.\n"
	.string "Watch where they push you.$"

OrangeIslands_Route_3_Text_OrangeScottIntro::
	.string "Splash splash!\n"
	.string "Battle me!$"

OrangeIslands_Route_3_Text_OrangeScottDefeat::
	.string "My float popped!$"

OrangeIslands_Route_3_Text_OrangeScottPostBattle::
	.string "My big sister swims way\n"
	.string "out past the rocks.$"

OrangeIslands_Route_3_Text_OrangeMilaIntro::
	.string "Hey! You splashed me!\n"
	.string "That means a battle!$"

OrangeIslands_Route_3_Text_OrangeMilaDefeat::
	.string "Fine, you win.$"

OrangeIslands_Route_3_Text_OrangeMilaPostBattle::
	.string "Watch out for TENTACOOL\n"
	.string "near the reefs.$"

OrangeIslands_Route_3_Text_OrangeTonyIntro::
	.string "The water's perfect today!\n"
	.string "Let's battle out here!$"

OrangeIslands_Route_3_Text_OrangeTonyDefeat::
	.string "I'm sinking!$"

OrangeIslands_Route_3_Text_OrangeTonyPostBattle::
	.string "I swim between the islands\n"
	.string "every morning.$"

