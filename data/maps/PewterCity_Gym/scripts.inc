PewterCity_Gym_MapScripts::
	.byte 0
	
PewterCity_Gym_EventScript_Brock::
	famechecker FAMECHECKER_BROCK, FCPICKSTATE_COLORED, UpdatePickStateFromSpecialVar8005
	goto_if_set FLAG_DEFEATED_CHAMP, PewterCity_Gym_EventScript_LeaderRematchAsk
PewterCity_Gym_EventScript_LeaderBattle::
	trainerbattle_single TRAINER_LEADER_BROCK, PewterCity_Gym_Text_BrockIntro, PewterCity_Gym_Text_BrockDefeat, PewterCity_Gym_EventScript_DefeatedBrock, NO_MUSIC
	goto_if_unset FLAG_GOT_TM39_FROM_BROCK, PewterCity_Gym_EventScript_GiveTM39
	msgbox PewterCity_Gym_Text_BrockPostBattle
	release
	end

PewterCity_Gym_EventScript_DefeatedBrock::
	famechecker FAMECHECKER_BROCK, 1
	setflag FLAG_DEFEATED_BROCK
	setflag FLAG_BADGE01_GET
	setvar VAR_MAP_SCENE_PEWTER_CITY, 1
	setflag FLAG_HIDE_PEWTER_CITY_GYM_GUIDE
	clearflag FLAG_HIDE_PEWTER_CITY_RUNNING_SHOES_GUY
	set_gym_trainers 1
	goto PewterCity_Gym_EventScript_GiveTM39
	end

PewterCity_Gym_EventScript_GiveTM39::
	msgbox PewterCity_Gym_Text_TakeThisWithYou
	checkitemspace ITEM_TM39
	goto_if_eq VAR_RESULT, FALSE, PewterCity_Gym_EventScript_NoRoomForTM39
	giveitem_msg PewterCity_Gym_Text_ReceivedTM39FromBrock, ITEM_TM39
	setflag FLAG_GOT_TM39_FROM_BROCK
	msgbox PewterCity_Gym_Text_ExplainTM39
	release
	end

PewterCity_Gym_EventScript_NoRoomForTM39::
	msgbox PewterCity_Gym_Text_DontHaveRoomForThis
	release
	end

PewterCity_Gym_EventScript_Liam::
	trainerbattle_single TRAINER_CAMPER_LIAM, PewterCity_Gym_Text_LiamIntro, PewterCity_Gym_Text_LiamDefeat
	msgbox PewterCity_Gym_Text_LiamPostBattle, MSGBOX_AUTOCLOSE
	end

PewterCity_Gym_EventScript_GymGuy::
	lock
	faceplayer
	goto_if_set FLAG_DEFEATED_BROCK, PewterCity_Gym_EventScript_GymGuyPostVictory
	msgbox PewterCity_Gym_Text_LetMeTakeYouToTheTop, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, YES, PewterCity_Gym_EventScript_GymGuyTakeMeToTop
	goto_if_eq VAR_RESULT, NO, PewterCity_Gym_EventScript_GymGuyDontTakeMeToTop
	end

PewterCity_Gym_EventScript_GymGuyPostVictory::
	msgbox PewterCity_Gym_Text_YoureChampMaterial
	release
	end

PewterCity_Gym_EventScript_GymGuyTakeMeToTop::
	msgbox PewterCity_Gym_Text_LetsGetHappening
	goto PewterCity_Gym_EventScript_GymGuyAdvice
	end

PewterCity_Gym_EventScript_GymGuyDontTakeMeToTop::
	msgbox PewterCity_Gym_Text_ItsFreeLetsGetHappening
	goto PewterCity_Gym_EventScript_GymGuyAdvice
	end

PewterCity_Gym_EventScript_GymGuyAdvice::
	msgbox PewterCity_Gym_Text_TryDifferentPartyOrders
	release
	end

PewterCity_Gym_EventScript_GymStatue::
	lockall
	goto_if_set FLAG_BADGE01_GET, PewterCity_Gym_EventScript_GymStatuePostVictory
	msgbox PewterCity_Gym_Text_GymStatue
	releaseall
	end

PewterCity_Gym_EventScript_GymStatuePostVictory::
	msgbox PewterCity_Gym_Text_GymStatuePlayerWon
	releaseall
	end

@ Rematch (after the Champion): any number of times, one of two teams
PewterCity_Gym_EventScript_LeaderRematchAsk::
	lock
	faceplayer
	msgbox PewterCity_Gym_Text_LeaderRematchAsk, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, PewterCity_Gym_EventScript_LeaderBattle
	msgbox PewterCity_Gym_Text_RebattleIntro
	random 2
	goto_if_eq VAR_RESULT, 1, PewterCity_Gym_Script_Brock_RematchTeamOption2
PewterCity_Gym_Script_Brock_RematchTeamOption1::
	cleartrainerflag TRAINER_LEADER_BROCKREMATCHA
	trainerbattle_no_intro TRAINER_LEADER_BROCKREMATCHA, PewterCity_Gym_Text_RebattleDeafeated
	goto PewterCity_Gym_EventScript_RematchWon
	end
PewterCity_Gym_Script_Brock_RematchTeamOption2::
	cleartrainerflag TRAINER_LEADER_BROCKREMATCHB
	trainerbattle_no_intro TRAINER_LEADER_BROCKREMATCHB, PewterCity_Gym_Text_RebattleDeafeated
	goto PewterCity_Gym_EventScript_RematchWon
	end

PewterCity_Gym_EventScript_RematchWon::
	msgbox PewterCity_Gym_Text_Brock_postrematch
	release
	end

@ Rematch NPC: explains when rematches open up
PewterCity_Gym_Text_RematchCheckUnlocked::
	lock
	faceplayer
	goto_if_set FLAG_DEFEATED_CHAMP, PewterCity_Gym_Script_RematchHelperAfter
	msgbox PewterCity_Gym_Text_RematchNotUnlocked
	release
	end
PewterCity_Gym_Script_RematchHelperAfter::
	msgbox PewterCity_Gym_Text_RematchHelperAfter
	release
	end

PewterCity_Gym_Text_LeaderRematchAsk::
	.string "My rock-hard willpower hasn't\n"
	.string "budged. Up for a rematch?$"

PewterCity_Gym_Text_RematchHelperAfter::
	.string "Hey, champ! BROCK takes on\n"
	.string "rematches now. Just ask!$"
