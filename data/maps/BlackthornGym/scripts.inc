BlackthornGym_MapScripts::
	.byte 0

@ Leader: first win gives the RISING BADGE (FLAG_BADGE_RISING, also set in CB2_EndTrainerBattle);
@ afterwards the leader offers rematches.
BlackthornGym_EventScript_Leader_Clair::
	lock
	faceplayer
	goto_if_set FLAG_BADGE_RISING, BlackthornGym_EventScript_LeaderAfter
	goto_if_defeated TRAINER_LEADER_CLAIRA, BlackthornGym_EventScript_LeaderWon
	goto_if_defeated TRAINER_LEADER_CLAIRB, BlackthornGym_EventScript_LeaderWon
	goto_if_defeated TRAINER_LEADER_CLAIRC, BlackthornGym_EventScript_LeaderWon
	msgbox BlackthornGym_Text_Intro
	random 3
	goto_if_eq VAR_RESULT, 1, BlackthornGym_Script_Clair_TeamOption2
	goto_if_eq VAR_RESULT, 2, BlackthornGym_Script_Clair_TeamOption3
BlackthornGym_Script_Clair_TeamOption1::
	trainerbattle_no_intro TRAINER_LEADER_CLAIRA, BlackthornGym_Text_Deafeated
	goto BlackthornGym_EventScript_LeaderWon
	end
BlackthornGym_Script_Clair_TeamOption2::
	trainerbattle_no_intro TRAINER_LEADER_CLAIRB, BlackthornGym_Text_Deafeated
	goto BlackthornGym_EventScript_LeaderWon
	end
BlackthornGym_Script_Clair_TeamOption3::
	trainerbattle_no_intro TRAINER_LEADER_CLAIRC, BlackthornGym_Text_Deafeated
	goto BlackthornGym_EventScript_LeaderWon
	end

BlackthornGym_EventScript_LeaderWon::
	setflag FLAG_BADGE_RISING
	message BlackthornGym_Text_ReceivedBadge
	waitmessage
	playfanfare MUS_OBTAIN_BADGE
	waitfanfare
	msgbox BlackthornGym_Text_Clair_postbattle
	release
	end

@ Rematch: any number of times, one of three teams
BlackthornGym_EventScript_LeaderAfter::
	msgbox BlackthornGym_Text_LeaderRematchAsk, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, BlackthornGym_Script_NoRematch
	msgbox BlackthornGym_Text_RebattleIntro
	random 3
	goto_if_eq VAR_RESULT, 1, BlackthornGym_Script_Clair_RematchTeamOption2
	goto_if_eq VAR_RESULT, 2, BlackthornGym_Script_Clair_RematchTeamOption3
BlackthornGym_Script_Clair_RematchTeamOption1::
	cleartrainerflag TRAINER_LEADER_CLAIRREMATCHA
	trainerbattle_no_intro TRAINER_LEADER_CLAIRREMATCHA, BlackthornGym_Text_RebattleDeafeated
	goto BlackthornGym_EventScript_RematchWon
	end
BlackthornGym_Script_Clair_RematchTeamOption2::
	cleartrainerflag TRAINER_LEADER_CLAIRREMATCHB
	trainerbattle_no_intro TRAINER_LEADER_CLAIRREMATCHB, BlackthornGym_Text_RebattleDeafeated
	goto BlackthornGym_EventScript_RematchWon
	end
BlackthornGym_Script_Clair_RematchTeamOption3::
	cleartrainerflag TRAINER_LEADER_CLAIRREMATCHC
	trainerbattle_no_intro TRAINER_LEADER_CLAIRREMATCHC, BlackthornGym_Text_RebattleDeafeated
	goto BlackthornGym_EventScript_RematchWon
	end

BlackthornGym_EventScript_RematchWon::
	msgbox BlackthornGym_Text_Clair_postrematch
	release
	end

BlackthornGym_Script_NoRematch::
	msgbox BlackthornGym_Text_LeaderNoRematch
	release
	end

@ Gym helper: explains rematches
BlackthornGym_Script_Rematch::
	lock
	faceplayer
	goto_if_set FLAG_BADGE_RISING, BlackthornGym_Script_HelperAfter
	msgbox BlackthornGym_Text_HelperBefore
	release
	end
BlackthornGym_Script_HelperAfter::
	msgbox BlackthornGym_Text_HelperAfter
	release
	end

BlackthornGym_Text_ReceivedBadge::
	.string "{PLAYER} received the\n"
	.string "RISING BADGE from CLAIR!$"

BlackthornGym_Text_LeaderRematchAsk::
	.string "Don't think I've forgotten that\n"
	.string "loss. Rematch. Now?$"

BlackthornGym_Text_LeaderNoRematch::
	.string "Come back any time.$"

BlackthornGym_Text_HelperBefore::
	.string "Hey, champ! Beat CLAIR and the\n"
	.string "RISING BADGE is yours!$"

BlackthornGym_Text_HelperAfter::
	.string "Hey, champ! CLAIR takes on\n"
	.string "rematches any time. Just ask!$"
