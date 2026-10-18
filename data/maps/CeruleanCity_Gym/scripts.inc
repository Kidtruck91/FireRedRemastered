CeruleanCity_Gym_MapScripts::
	.byte 0

CeruleanCity_Gym_EventScript_Misty::
	famechecker FAMECHECKER_MISTY, FCPICKSTATE_COLORED, UpdatePickStateFromSpecialVar8005
	goto_if_set FLAG_DEFEATED_CHAMP, CeruleanCity_Gym_EventScript_LeaderRematchAsk
CeruleanCity_Gym_EventScript_LeaderBattle::
	trainerbattle_single TRAINER_LEADER_MISTY, CeruleanCity_Gym_Text_MistyIntro, CeruleanCity_Gym_Text_MistyDefeat, CeruleanCity_Gym_EventScript_MistyDefeated, NO_MUSIC
	goto_if_unset FLAG_GOT_TM03_FROM_MISTY, CeruleanCity_Gym_EventScript_GiveTM03
	msgbox CeruleanCity_Gym_Text_ExplainTM03
	release
	end

CeruleanCity_Gym_EventScript_MistyDefeated::
	famechecker FAMECHECKER_MISTY, 1
	setflag FLAG_DEFEATED_MISTY
	setflag FLAG_BADGE02_GET
	setrespawn SPAWN_CERULEAN_CITY
	set_gym_trainers 2
	goto CeruleanCity_Gym_EventScript_GiveTM03
	end

CeruleanCity_Gym_EventScript_GiveTM03::
	msgbox CeruleanCity_Gym_Text_ExplainCascadeBadge
	checkitemspace ITEM_TM03
	goto_if_eq VAR_RESULT, FALSE, CeruleanCity_Gym_EventScript_NoRoomForTM03
	giveitem_msg CeruleanCity_Gym_Text_ReceivedTM03FromMisty, ITEM_TM03
	setflag FLAG_GOT_TM03_FROM_MISTY
	msgbox CeruleanCity_Gym_Text_ExplainTM03
	release
	end

CeruleanCity_Gym_EventScript_NoRoomForTM03::
	msgbox CeruleanCity_Gym_Text_BetterMakeRoomForThis
	release
	end

CeruleanCity_Gym_EventScript_Diana::
	trainerbattle_single TRAINER_PICNICKER_DIANA, CeruleanCity_Gym_Text_DianaIntro, CeruleanCity_Gym_Text_DianaDefeat
	msgbox CeruleanCity_Gym_Text_DianaPostBattle, MSGBOX_AUTOCLOSE
	end

CeruleanCity_Gym_EventScript_Luis::
	trainerbattle_single TRAINER_SWIMMER_MALE_LUIS, CeruleanCity_Gym_Text_LuisIntro, CeruleanCity_Gym_Text_LuisDefeat
	famechecker FAMECHECKER_MISTY, 2
	msgbox CeruleanCity_Gym_Text_LuisPostBattle, MSGBOX_AUTOCLOSE
	end

CeruleanCity_Gym_EventScript_GymGuy::
	lock
	faceplayer
	goto_if_set FLAG_DEFEATED_MISTY, CeruleanCity_Gym_EventScript_GymGuyPostVictory
	msgbox CeruleanCity_Gym_Text_GymGuyAdvice
	release
	end

CeruleanCity_Gym_EventScript_GymGuyPostVictory::
	msgbox CeruleanCity_Gym_Text_WeMakePrettyGoodTeam
	release
	end

CeruleanCity_Gym_EventScript_GymStatue::
	lockall
	goto_if_set FLAG_BADGE02_GET, CeruleanCity_Gym_EventScript_GymStatuePostVictory
	msgbox CeruleanCity_Gym_Text_GymStatue
	releaseall
	end

CeruleanCity_Gym_EventScript_GymStatuePostVictory::
	msgbox CeruleanCity_Gym_Text_GymStatuePlayerWon
	releaseall
	end

@ Rematch (after the Champion): any number of times, one of two teams
CeruleanCity_Gym_EventScript_LeaderRematchAsk::
	lock
	faceplayer
	msgbox CeruleanCity_Gym_Text_LeaderRematchAsk, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, CeruleanCity_Gym_EventScript_LeaderBattle
	msgbox CeruleanCity_Gym_Text_RebattleIntro
	random 2
	goto_if_eq VAR_RESULT, 1, CeruleanCity_Gym_Script_Misty_RematchTeamOption2
CeruleanCity_Gym_Script_Misty_RematchTeamOption1::
	cleartrainerflag TRAINER_LEADER_MISTYREMATCHA
	trainerbattle_no_intro TRAINER_LEADER_MISTYREMATCHA, CeruleanCity_Gym_Text_RebattleDeafeated
	goto CeruleanCity_Gym_EventScript_RematchWon
	end
CeruleanCity_Gym_Script_Misty_RematchTeamOption2::
	cleartrainerflag TRAINER_LEADER_MISTYREMATCHB
	trainerbattle_no_intro TRAINER_LEADER_MISTYREMATCHB, CeruleanCity_Gym_Text_RebattleDeafeated
	goto CeruleanCity_Gym_EventScript_RematchWon
	end

CeruleanCity_Gym_EventScript_RematchWon::
	msgbox CeruleanCity_Gym_Text_Misty_postrematch
	release
	end

@ Rematch NPC: explains when rematches open up
CeruleanCity_Gym_Text_RematchCheckUnlocked::
	lock
	faceplayer
	goto_if_set FLAG_DEFEATED_CHAMP, CeruleanCity_Gym_Script_RematchHelperAfter
	msgbox CeruleanCity_Gym_Text_RematchNotUnlocked
	release
	end
CeruleanCity_Gym_Script_RematchHelperAfter::
	msgbox CeruleanCity_Gym_Text_RematchHelperAfter
	release
	end

CeruleanCity_Gym_Text_LeaderRematchAsk::
	.string "Hey! I've been training hard.\n"
	.string "Rematch? You'd better say yes!$"

CeruleanCity_Gym_Text_RematchHelperAfter::
	.string "Hey, champ! MISTY takes on\n"
	.string "rematches now. Just ask!$"
