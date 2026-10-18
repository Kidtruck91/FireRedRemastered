AzaleaGym_MapScripts::
	.byte 0

@ Leader: first win gives the HIVE BADGE (FLAG_BADGE_HIVE, also set in CB2_EndTrainerBattle);
@ afterwards the leader offers rematches.
AzaleaGym_EventScript_Leader_Bugsy::
	lock
	faceplayer
	goto_if_set FLAG_BADGE_HIVE, AzaleaGym_EventScript_LeaderAfter
	goto_if_defeated TRAINER_LEADER_BUGSYA, AzaleaGym_EventScript_LeaderWon
	goto_if_defeated TRAINER_LEADER_BUGSYB, AzaleaGym_EventScript_LeaderWon
	goto_if_defeated TRAINER_LEADER_BUGSYC, AzaleaGym_EventScript_LeaderWon
	msgbox AzaleaGym_Text_Intro
	random 3
	goto_if_eq VAR_RESULT, 1, AzaleaGym_Script_Bugsy_TeamOption2
	goto_if_eq VAR_RESULT, 2, AzaleaGym_Script_Bugsy_TeamOption3
AzaleaGym_Script_Bugsy_TeamOption1::
	trainerbattle_no_intro TRAINER_LEADER_BUGSYA, AzaleaGym_Text_Deafeated
	goto AzaleaGym_EventScript_LeaderWon
	end
AzaleaGym_Script_Bugsy_TeamOption2::
	trainerbattle_no_intro TRAINER_LEADER_BUGSYB, AzaleaGym_Text_Deafeated
	goto AzaleaGym_EventScript_LeaderWon
	end
AzaleaGym_Script_Bugsy_TeamOption3::
	trainerbattle_no_intro TRAINER_LEADER_BUGSYC, AzaleaGym_Text_Deafeated
	goto AzaleaGym_EventScript_LeaderWon
	end

AzaleaGym_EventScript_LeaderWon::
	setflag FLAG_BADGE_HIVE
	message AzaleaGym_Text_ReceivedBadge
	waitmessage
	playfanfare MUS_OBTAIN_BADGE
	waitfanfare
	msgbox AzaleaGym_Text_Bugsy_postbattle
	release
	end

@ Rematch: any number of times, one of three teams
AzaleaGym_EventScript_LeaderAfter::
	msgbox AzaleaGym_Text_LeaderRematchAsk, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, AzaleaGym_Script_NoRematch
	msgbox AzaleaGym_Text_RebattleIntro
	random 3
	goto_if_eq VAR_RESULT, 1, AzaleaGym_Script_Bugsy_RematchTeamOption2
	goto_if_eq VAR_RESULT, 2, AzaleaGym_Script_Bugsy_RematchTeamOption3
AzaleaGym_Script_Bugsy_RematchTeamOption1::
	cleartrainerflag TRAINER_LEADER_BUGSYREMATCHA
	trainerbattle_no_intro TRAINER_LEADER_BUGSYREMATCHA, AzaleaGym_Text_RebattleDeafeated
	goto AzaleaGym_EventScript_RematchWon
	end
AzaleaGym_Script_Bugsy_RematchTeamOption2::
	cleartrainerflag TRAINER_LEADER_BUGSYREMATCHB
	trainerbattle_no_intro TRAINER_LEADER_BUGSYREMATCHB, AzaleaGym_Text_RebattleDeafeated
	goto AzaleaGym_EventScript_RematchWon
	end
AzaleaGym_Script_Bugsy_RematchTeamOption3::
	cleartrainerflag TRAINER_LEADER_BUGSYREMATCHC
	trainerbattle_no_intro TRAINER_LEADER_BUGSYREMATCHC, AzaleaGym_Text_RebattleDeafeated
	goto AzaleaGym_EventScript_RematchWon
	end

AzaleaGym_EventScript_RematchWon::
	msgbox AzaleaGym_Text_Bugsy_postrematch
	release
	end

AzaleaGym_Script_NoRematch::
	msgbox AzaleaGym_Text_LeaderNoRematch
	release
	end

@ Gym helper: explains rematches
AzaleaGym_Script_Rematch::
	lock
	faceplayer
	goto_if_set FLAG_BADGE_HIVE, AzaleaGym_Script_HelperAfter
	msgbox AzaleaGym_Text_HelperBefore
	release
	end
AzaleaGym_Script_HelperAfter::
	msgbox AzaleaGym_Text_HelperAfter
	release
	end

AzaleaGym_Text_ReceivedBadge::
	.string "{PLAYER} received the\n"
	.string "HIVE BADGE from BUGSY!$"

AzaleaGym_Text_LeaderRematchAsk::
	.string "My bugs have been studying hard.\n"
	.string "Want to see what they've learned\l"
	.string "in a rematch?$"

AzaleaGym_Text_LeaderNoRematch::
	.string "Come back any time.$"

AzaleaGym_Text_HelperBefore::
	.string "Hey, champ! Beat BUGSY and the\n"
	.string "HIVE BADGE is yours!$"

AzaleaGym_Text_HelperAfter::
	.string "Hey, champ! BUGSY takes on\n"
	.string "rematches any time. Just ask!$"
