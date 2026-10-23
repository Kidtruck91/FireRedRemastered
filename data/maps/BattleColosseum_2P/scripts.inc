@ This map is the link battle colosseum and doubles as the Training Room, which the
@ Pokemon Center attendant warps you into. The Training Room battle is driven by the
@ coordinate event on the seat at (3,5), so there is no map script table here: the
@ entries that used to be in it were malformed (a bare `call` assembles as another
@ ON_WARP_INTO_MAP_TABLE entry) and would have been read during real link battles too.

BattleColosseum_2P_MapScripts::
	.byte 0

@ The seat. Asks first, so stepping on it by accident -- or changing your mind -- does
@ not commit you to the battle; the exits at the bottom of the room work either way.
BattleColosseum_2P_EventScript_NurseJoyBattle::
	lockall
	msgbox gText_NurseJoy_Text_Ready, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, BattleColosseum_2P_EventScript_NurseJoyDeclined
	special HealPlayerParty
	special SavePlayerParty
	cleartrainerflag TRAINER_NURSE_JOY
	trainerbattle_single TRAINER_NURSE_JOY, gText_NurseJoy_Text_BattleIntro, gText_NurseJoy_Text_BattleDefeat
	releaseall
	end

BattleColosseum_2P_EventScript_NurseJoyDeclined::
	msgbox gText_NurseJoy_Text_Maybe
	releaseall
	end
