GoldenrodGym_MapScripts::
	.byte 0

@ Leader: first win gives the PLAIN BADGE (FLAG_BADGE_PLAIN, also set in CB2_EndTrainerBattle);
@ afterwards the leader offers rematches.
GoldenrodGym_EventScript_Leader_Whitney::
	lock
	faceplayer
	goto_if_set FLAG_BADGE_PLAIN, GoldenrodGym_EventScript_LeaderAfter
	goto_if_defeated TRAINER_LEADER_WHITNEYA, GoldenrodGym_EventScript_LeaderWon
	goto_if_defeated TRAINER_LEADER_WHITNEYB, GoldenrodGym_EventScript_LeaderWon
	goto_if_defeated TRAINER_LEADER_WHITNEYC, GoldenrodGym_EventScript_LeaderWon
	msgbox GoldenrodGym_Text_Intro
	random 3
	goto_if_eq VAR_RESULT, 1, GoldenrodGym_Script_Whitney_TeamOption2
	goto_if_eq VAR_RESULT, 2, GoldenrodGym_Script_Whitney_TeamOption3
GoldenrodGym_Script_Whitney_TeamOption1::
	trainerbattle_no_intro TRAINER_LEADER_WHITNEYA, GoldenrodGym_Text_Deafeated
	goto GoldenrodGym_EventScript_LeaderWon
	end
GoldenrodGym_Script_Whitney_TeamOption2::
	trainerbattle_no_intro TRAINER_LEADER_WHITNEYB, GoldenrodGym_Text_Deafeated
	goto GoldenrodGym_EventScript_LeaderWon
	end
GoldenrodGym_Script_Whitney_TeamOption3::
	trainerbattle_no_intro TRAINER_LEADER_WHITNEYC, GoldenrodGym_Text_Deafeated
	goto GoldenrodGym_EventScript_LeaderWon
	end

GoldenrodGym_EventScript_LeaderWon::
	setflag FLAG_BADGE_PLAIN
	message GoldenrodGym_Text_ReceivedBadge
	waitmessage
	playfanfare MUS_OBTAIN_BADGE
	waitfanfare
	msgbox GoldenrodGym_Text_Whitney_postbattle
	release
	end

@ Rematch (after the Champion, FLAG_DEFEATED_CHAMP): any number of times, one of three teams
GoldenrodGym_EventScript_LeaderAfter::
	goto_if_unset FLAG_DEFEATED_CHAMP, GoldenrodGym_EventScript_LeaderPostBattle
	msgbox GoldenrodGym_Text_LeaderRematchAsk, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, GoldenrodGym_Script_NoRematch
	msgbox GoldenrodGym_Text_RebattleIntro
	random 3
	goto_if_eq VAR_RESULT, 1, GoldenrodGym_Script_Whitney_RematchTeamOption2
	goto_if_eq VAR_RESULT, 2, GoldenrodGym_Script_Whitney_RematchTeamOption3
GoldenrodGym_Script_Whitney_RematchTeamOption1::
	cleartrainerflag TRAINER_LEADER_WHITNEYREMATCHA
	trainerbattle_no_intro TRAINER_LEADER_WHITNEYREMATCHA, GoldenrodGym_Text_RebattleDeafeated
	goto GoldenrodGym_EventScript_RematchWon
	end
GoldenrodGym_Script_Whitney_RematchTeamOption2::
	cleartrainerflag TRAINER_LEADER_WHITNEYREMATCHB
	trainerbattle_no_intro TRAINER_LEADER_WHITNEYREMATCHB, GoldenrodGym_Text_RebattleDeafeated
	goto GoldenrodGym_EventScript_RematchWon
	end
GoldenrodGym_Script_Whitney_RematchTeamOption3::
	cleartrainerflag TRAINER_LEADER_WHITNEYREMATCHC
	trainerbattle_no_intro TRAINER_LEADER_WHITNEYREMATCHC, GoldenrodGym_Text_RebattleDeafeated
	goto GoldenrodGym_EventScript_RematchWon
	end

GoldenrodGym_EventScript_LeaderPostBattle::
	msgbox GoldenrodGym_Text_Whitney_postbattle
	release
	end

GoldenrodGym_EventScript_RematchWon::
	msgbox GoldenrodGym_Text_Whitney_postrematch
	release
	end

GoldenrodGym_Script_NoRematch::
	msgbox GoldenrodGym_Text_LeaderNoRematch
	release
	end

@ Gym helper: explains rematches
GoldenrodGym_Script_Rematch::
	lock
	faceplayer
	goto_if_set FLAG_DEFEATED_CHAMP, GoldenrodGym_Script_HelperAfter
	goto_if_set FLAG_BADGE_PLAIN, GoldenrodGym_Script_HelperBadge
	msgbox GoldenrodGym_Text_HelperBefore
	release
	end
GoldenrodGym_Script_HelperBadge::
	msgbox GoldenrodGym_Text_HelperBadge
	release
	end
GoldenrodGym_Script_HelperAfter::
	msgbox GoldenrodGym_Text_HelperAfter
	release
	end

GoldenrodGym_Text_ReceivedBadge::
	.string "{PLAYER} received the\n"
	.string "PLAIN BADGE from WHITNEY!$"

GoldenrodGym_Text_LeaderRematchAsk::
	.string "You again? Fine, I'll get you\n"
	.string "back this time! Rematch?$"

GoldenrodGym_Text_LeaderNoRematch::
	.string "Come back any time.$"

GoldenrodGym_Text_HelperBefore::
	.string "Hey, champ! Beat WHITNEY and the\n"
	.string "PLAIN BADGE is yours!$"

GoldenrodGym_Text_HelperBadge::
	.string "Nice BADGE, champ! Once you've\n"
	.string "beaten the CHAMPION, WHITNEY\l"
	.string "takes on rematches.$"

GoldenrodGym_Text_HelperAfter::
	.string "Hey, champ! WHITNEY takes on\n"
	.string "rematches any time. Just ask!$"
