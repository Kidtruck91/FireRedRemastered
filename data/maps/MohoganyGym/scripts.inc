MohoganyGym_MapScripts::
	.byte 0

@ Leader: first win gives the GLACIER BADGE (FLAG_BADGE_GLACIER, also set in CB2_EndTrainerBattle);
@ afterwards the leader offers rematches.
MohoganyGym_EventScript_Leader_Pryce::
	lock
	faceplayer
	goto_if_set FLAG_BADGE_GLACIER, MohoganyGym_EventScript_LeaderAfter
	goto_if_defeated TRAINER_LEADER_PRYCEA, MohoganyGym_EventScript_LeaderWon
	goto_if_defeated TRAINER_LEADER_PRYCEB, MohoganyGym_EventScript_LeaderWon
	goto_if_defeated TRAINER_LEADER_PRYCEC, MohoganyGym_EventScript_LeaderWon
	msgbox MohoganyGym_Text_Intro
	random 3
	goto_if_eq VAR_RESULT, 1, MohoganyGym_Script_Pryce_TeamOption2
	goto_if_eq VAR_RESULT, 2, MohoganyGym_Script_Pryce_TeamOption3
MohoganyGym_Script_Pryce_TeamOption1::
	trainerbattle_no_intro TRAINER_LEADER_PRYCEA, MohoganyGym_Text_Deafeated
	goto MohoganyGym_EventScript_LeaderWon
	end
MohoganyGym_Script_Pryce_TeamOption2::
	trainerbattle_no_intro TRAINER_LEADER_PRYCEB, MohoganyGym_Text_Deafeated
	goto MohoganyGym_EventScript_LeaderWon
	end
MohoganyGym_Script_Pryce_TeamOption3::
	trainerbattle_no_intro TRAINER_LEADER_PRYCEC, MohoganyGym_Text_Deafeated
	goto MohoganyGym_EventScript_LeaderWon
	end

MohoganyGym_EventScript_LeaderWon::
	setflag FLAG_BADGE_GLACIER
	message MohoganyGym_Text_ReceivedBadge
	waitmessage
	playfanfare MUS_OBTAIN_BADGE
	waitfanfare
	msgbox MohoganyGym_Text_Pryce_postbattle
	release
	end

@ Rematch (after the Champion, FLAG_DEFEATED_CHAMP): any number of times, one of three teams
MohoganyGym_EventScript_LeaderAfter::
	goto_if_unset FLAG_DEFEATED_CHAMP, MohoganyGym_EventScript_LeaderPostBattle
	msgbox MohoganyGym_Text_LeaderRematchAsk, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, MohoganyGym_Script_NoRematch
	msgbox MohoganyGym_Text_RebattleIntro
	random 3
	goto_if_eq VAR_RESULT, 1, MohoganyGym_Script_Pryce_RematchTeamOption2
	goto_if_eq VAR_RESULT, 2, MohoganyGym_Script_Pryce_RematchTeamOption3
MohoganyGym_Script_Pryce_RematchTeamOption1::
	cleartrainerflag TRAINER_LEADER_PRYCEREMATCHA
	trainerbattle_no_intro TRAINER_LEADER_PRYCEREMATCHA, MohoganyGym_Text_RebattleDeafeated
	goto MohoganyGym_EventScript_RematchWon
	end
MohoganyGym_Script_Pryce_RematchTeamOption2::
	cleartrainerflag TRAINER_LEADER_PRYCEREMATCHB
	trainerbattle_no_intro TRAINER_LEADER_PRYCEREMATCHB, MohoganyGym_Text_RebattleDeafeated
	goto MohoganyGym_EventScript_RematchWon
	end
MohoganyGym_Script_Pryce_RematchTeamOption3::
	cleartrainerflag TRAINER_LEADER_PRYCEREMATCHC
	trainerbattle_no_intro TRAINER_LEADER_PRYCEREMATCHC, MohoganyGym_Text_RebattleDeafeated
	goto MohoganyGym_EventScript_RematchWon
	end

MohoganyGym_EventScript_LeaderPostBattle::
	msgbox MohoganyGym_Text_Pryce_postbattle
	release
	end

MohoganyGym_EventScript_RematchWon::
	msgbox MohoganyGym_Text_Pryce_postrematch
	release
	end

MohoganyGym_Script_NoRematch::
	msgbox MohoganyGym_Text_LeaderNoRematch
	release
	end

@ Gym helper: explains rematches
MohoganyGym_Script_Rematch::
	lock
	faceplayer
	goto_if_set FLAG_DEFEATED_CHAMP, MohoganyGym_Script_HelperAfter
	goto_if_set FLAG_BADGE_GLACIER, MohoganyGym_Script_HelperBadge
	msgbox MohoganyGym_Text_HelperBefore
	release
	end
MohoganyGym_Script_HelperBadge::
	msgbox MohoganyGym_Text_HelperBadge
	release
	end
MohoganyGym_Script_HelperAfter::
	msgbox MohoganyGym_Text_HelperAfter
	release
	end

MohoganyGym_Text_ReceivedBadge::
	.string "{PLAYER} received the\n"
	.string "GLACIER BADGE from PRYCE!$"

MohoganyGym_Text_LeaderRematchAsk::
	.string "Winter always returns. Will you\n"
	.string "face my rematch team?$"

MohoganyGym_Text_LeaderNoRematch::
	.string "Come back any time.$"

MohoganyGym_Text_HelperBefore::
	.string "Hey, champ! Beat PRYCE and the\n"
	.string "GLACIER BADGE is yours!$"

MohoganyGym_Text_HelperBadge::
	.string "Nice BADGE, champ! Once you've\n"
	.string "beaten the CHAMPION, PRYCE\l"
	.string "takes on rematches.$"

MohoganyGym_Text_HelperAfter::
	.string "Hey, champ! PRYCE takes on\n"
	.string "rematches any time. Just ask!$"
