VioletGym_MapScripts::
	.byte 0

@ Leader: first win gives the ZEPHYR BADGE (FLAG_BADGE_ZEPHYR, also set in CB2_EndTrainerBattle);
@ afterwards the leader offers rematches.
VioletGym_EventScript_Leader_Walker::
	lock
	faceplayer
	goto_if_set FLAG_BADGE_ZEPHYR, VioletGym_EventScript_LeaderAfter
	goto_if_defeated TRAINER_LEADER_WALKERA, VioletGym_EventScript_LeaderWon
	goto_if_defeated TRAINER_LEADER_WALKERB, VioletGym_EventScript_LeaderWon
	goto_if_defeated TRAINER_LEADER_WALKERC, VioletGym_EventScript_LeaderWon
	msgbox VioletGym_Text_Intro
	random 3
	goto_if_eq VAR_RESULT, 1, VioletGym_Script_Walker_TeamOption2
	goto_if_eq VAR_RESULT, 2, VioletGym_Script_Walker_TeamOption3
VioletGym_Script_Walker_TeamOption1::
	trainerbattle_no_intro TRAINER_LEADER_WALKERA, VioletGym_Text_Deafeated
	goto VioletGym_EventScript_LeaderWon
	end
VioletGym_Script_Walker_TeamOption2::
	trainerbattle_no_intro TRAINER_LEADER_WALKERB, VioletGym_Text_Deafeated
	goto VioletGym_EventScript_LeaderWon
	end
VioletGym_Script_Walker_TeamOption3::
	trainerbattle_no_intro TRAINER_LEADER_WALKERC, VioletGym_Text_Deafeated
	goto VioletGym_EventScript_LeaderWon
	end

VioletGym_EventScript_LeaderWon::
	setflag FLAG_BADGE_ZEPHYR
	message VioletGym_Text_ReceivedBadge
	waitmessage
	playfanfare MUS_OBTAIN_BADGE
	waitfanfare
	msgbox VioletGym_Text_Walker_postbattle
	release
	end

@ Rematch (after the Champion, FLAG_DEFEATED_CHAMP): any number of times, one of three teams
VioletGym_EventScript_LeaderAfter::
	goto_if_unset FLAG_DEFEATED_CHAMP, VioletGym_EventScript_LeaderPostBattle
	msgbox VioletGym_Text_LeaderRematchAsk, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, VioletGym_Script_NoRematch
	msgbox VioletGym_Text_RebattleIntro
	random 3
	goto_if_eq VAR_RESULT, 1, VioletGym_Script_Walker_RematchTeamOption2
	goto_if_eq VAR_RESULT, 2, VioletGym_Script_Walker_RematchTeamOption3
VioletGym_Script_Walker_RematchTeamOption1::
	cleartrainerflag TRAINER_LEADER_WALKERREMATCHA
	trainerbattle_no_intro TRAINER_LEADER_WALKERREMATCHA, VioletGym_Text_RebattleDeafeated
	goto VioletGym_EventScript_RematchWon
	end
VioletGym_Script_Walker_RematchTeamOption2::
	cleartrainerflag TRAINER_LEADER_WALKERREMATCHB
	trainerbattle_no_intro TRAINER_LEADER_WALKERREMATCHB, VioletGym_Text_RebattleDeafeated
	goto VioletGym_EventScript_RematchWon
	end
VioletGym_Script_Walker_RematchTeamOption3::
	cleartrainerflag TRAINER_LEADER_WALKERREMATCHC
	trainerbattle_no_intro TRAINER_LEADER_WALKERREMATCHC, VioletGym_Text_RebattleDeafeated
	goto VioletGym_EventScript_RematchWon
	end

VioletGym_EventScript_LeaderPostBattle::
	msgbox VioletGym_Text_Walker_postbattle
	release
	end

VioletGym_EventScript_RematchWon::
	msgbox VioletGym_Text_Walker_postrematch
	release
	end

VioletGym_Script_NoRematch::
	msgbox VioletGym_Text_LeaderNoRematch
	release
	end

@ Gym helper: explains rematches
VioletGym_Script_Rematch::
	lock
	faceplayer
	goto_if_set FLAG_DEFEATED_CHAMP, VioletGym_Script_HelperAfter
	goto_if_set FLAG_BADGE_ZEPHYR, VioletGym_Script_HelperBadge
	msgbox VioletGym_Text_HelperBefore
	release
	end
VioletGym_Script_HelperBadge::
	msgbox VioletGym_Text_HelperBadge
	release
	end
VioletGym_Script_HelperAfter::
	msgbox VioletGym_Text_HelperAfter
	release
	end

VioletGym_Text_ReceivedBadge::
	.string "{PLAYER} received the\n"
	.string "ZEPHYR BADGE from WALKER!$"

VioletGym_Text_LeaderRematchAsk::
	.string "Back for another round, high\n"
	.string "flyer? I'm always up for a\l"
	.string "rematch!$"

VioletGym_Text_LeaderNoRematch::
	.string "Come back any time.$"

VioletGym_Text_HelperBefore::
	.string "Hey, champ! Beat WALKER and the\n"
	.string "ZEPHYR BADGE is yours!$"

VioletGym_Text_HelperBadge::
	.string "Nice BADGE, champ! Once you've\n"
	.string "beaten the CHAMPION, WALKER\l"
	.string "takes on rematches.$"

VioletGym_Text_HelperAfter::
	.string "Hey, champ! WALKER takes on\n"
	.string "rematches any time. Just ask!$"
