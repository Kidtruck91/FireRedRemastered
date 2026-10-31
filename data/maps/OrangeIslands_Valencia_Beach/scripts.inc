OrangeIslands_Valencia_Beach_MapScripts::
	.byte 0

OrangeIslands_Valencia_Beach_EventScript_Town::
	msgbox OrangeIslands_Valencia_Beach_Text_Town, MSGBOX_SIGN
	end

OrangeIslands_Valencia_Beach_EventScript_Harbor::
	msgbox OrangeIslands_Valencia_Beach_Text_Harbor, MSGBOX_SIGN
	end

OrangeIslands_Valencia_Beach_EventScript_EdgeToValencia_33::
	warp MAP_ORANGE_ISLANDS_VALENCIA, 65, 49
	waitstate
	end

OrangeIslands_Valencia_Beach_EventScript_EdgeToValencia_34::
	warp MAP_ORANGE_ISLANDS_VALENCIA, 65, 50
	waitstate
	end

OrangeIslands_Valencia_Beach_EventScript_EdgeToValencia_35::
	warp MAP_ORANGE_ISLANDS_VALENCIA, 65, 51
	waitstate
	end

@ ---- Orange Islands trainers (orange_route_trainers.py) ----
OrangeIslands_Valencia_Beach_EventScript_OrangeKent::
	trainerbattle_single TRAINER_ORANGE_FISHERMAN_KENT, OrangeIslands_Valencia_Beach_Text_OrangeKentIntro, OrangeIslands_Valencia_Beach_Text_OrangeKentDefeat
	msgbox OrangeIslands_Valencia_Beach_Text_OrangeKentPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Valencia_Beach_EventScript_OrangeMax::
	trainerbattle_single TRAINER_ORANGE_TUBER_MAX, OrangeIslands_Valencia_Beach_Text_OrangeMaxIntro, OrangeIslands_Valencia_Beach_Text_OrangeMaxDefeat
	msgbox OrangeIslands_Valencia_Beach_Text_OrangeMaxPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Valencia_Beach_EventScript_OrangeBlake::
	trainerbattle_single TRAINER_ORANGE_FISHERMAN_BLAKE, OrangeIslands_Valencia_Beach_Text_OrangeBlakeIntro, OrangeIslands_Valencia_Beach_Text_OrangeBlakeDefeat
	msgbox OrangeIslands_Valencia_Beach_Text_OrangeBlakePostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Valencia_Beach_EventScript_OrangeKara::
	trainerbattle_single TRAINER_ORANGE_SWIMMER_F_KARA, OrangeIslands_Valencia_Beach_Text_OrangeKaraIntro, OrangeIslands_Valencia_Beach_Text_OrangeKaraDefeat
	msgbox OrangeIslands_Valencia_Beach_Text_OrangeKaraPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Valencia_Beach_EventScript_OrangeBilly::
	trainerbattle_single TRAINER_ORANGE_TUBER_BILLY, OrangeIslands_Valencia_Beach_Text_OrangeBillyIntro, OrangeIslands_Valencia_Beach_Text_OrangeBillyDefeat
	msgbox OrangeIslands_Valencia_Beach_Text_OrangeBillyPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Valencia_Beach_Text_OrangeKentIntro::
	.string "Shh! You'll scare the fish!\p"
	.string "Well, now that they're gone,\n"
	.string "let's battle!$"

OrangeIslands_Valencia_Beach_Text_OrangeKentDefeat::
	.string "Reeled in!$"

OrangeIslands_Valencia_Beach_Text_OrangeKentPostBattle::
	.string "The big ones bite at dawn.\n"
	.string "Patience is everything.$"

OrangeIslands_Valencia_Beach_Text_OrangeMaxIntro::
	.string "Look at my float!\n"
	.string "Wanna battle?$"

OrangeIslands_Valencia_Beach_Text_OrangeMaxDefeat::
	.string "No fair!$"

OrangeIslands_Valencia_Beach_Text_OrangeMaxPostBattle::
	.string "Sand castles are fun too!$"

OrangeIslands_Valencia_Beach_Text_OrangeBlakeIntro::
	.string "Look at this catch!\n"
	.string "Want to see it in battle?$"

OrangeIslands_Valencia_Beach_Text_OrangeBlakeDefeat::
	.string "It got away...$"

OrangeIslands_Valencia_Beach_Text_OrangeBlakePostBattle::
	.string "Fishing is like battling.\n"
	.string "Timing is everything.$"

OrangeIslands_Valencia_Beach_Text_OrangeKaraIntro::
	.string "Hey! You splashed me!\n"
	.string "That means a battle!$"

OrangeIslands_Valencia_Beach_Text_OrangeKaraDefeat::
	.string "Fine, you win.$"

OrangeIslands_Valencia_Beach_Text_OrangeKaraPostBattle::
	.string "Watch out for TENTACOOL\n"
	.string "near the reefs.$"

OrangeIslands_Valencia_Beach_Text_OrangeBillyIntro::
	.string "I can't swim yet, but my\n"
	.string "POKéMON can!$"

OrangeIslands_Valencia_Beach_Text_OrangeBillyDefeat::
	.string "Waaah!$"

OrangeIslands_Valencia_Beach_Text_OrangeBillyPostBattle::
	.string "When I grow up, I'll swim\n"
	.string "to every island!$"

