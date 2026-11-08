OrangeIslands_Kinnow_BattleBoat_Deck_MapScripts::
	.byte 0

@ --- built by docs/orange-islands/build_battle_boat.py ---
OrangeIslands_Kinnow_BattleBoat_Deck_EventScript_Purser::
	msgbox OrangeIslands_Kinnow_BattleBoat_Deck_Text_Purser, MSGBOX_NPC
	end

OrangeIslands_Kinnow_BattleBoat_Deck_EventScript_Lookout::
	msgbox OrangeIslands_Kinnow_BattleBoat_Deck_Text_Lookout, MSGBOX_NPC
	end

OrangeIslands_Kinnow_BattleBoat_Deck_EventScript_Brac::
	trainerbattle_single TRAINER_ORANGE_SAILOR_BRAC, OrangeIslands_Kinnow_BattleBoat_Deck_Text_BracIntro, OrangeIslands_Kinnow_BattleBoat_Deck_Text_BracDefeat
	msgbox OrangeIslands_Kinnow_BattleBoat_Deck_Text_BracAfter, MSGBOX_AUTOCLOSE
	end
@ --- end build_battle_boat.py ---
