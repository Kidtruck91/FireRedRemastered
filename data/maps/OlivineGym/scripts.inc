OlivineGym_MapScripts::
	.byte 0

@ Leader: first win gives the MINERAL BADGE (FLAG_BADGE_MINERAL, also set in CB2_EndTrainerBattle);
@ afterwards the leader offers rematches.
OlivineGym_EventScript_Leader_Jasmine::
	lock
	faceplayer
	goto_if_set FLAG_BADGE_MINERAL, OlivineGym_EventScript_LeaderAfter
	goto_if_defeated TRAINER_LEADER_JASMINEA, OlivineGym_EventScript_LeaderWon
	goto_if_defeated TRAINER_LEADER_JASMINEB, OlivineGym_EventScript_LeaderWon
	goto_if_defeated TRAINER_LEADER_JASMINEC, OlivineGym_EventScript_LeaderWon
	msgbox OlivineGym_Text_Intro
	random 3
	goto_if_eq VAR_RESULT, 1, OlivineGym_Script_Jasmine_TeamOption2
	goto_if_eq VAR_RESULT, 2, OlivineGym_Script_Jasmine_TeamOption3
OlivineGym_Script_Jasmine_TeamOption1::
	trainerbattle_no_intro TRAINER_LEADER_JASMINEA, OlivineGym_Text_Deafeated
	goto OlivineGym_EventScript_LeaderWon
	end
OlivineGym_Script_Jasmine_TeamOption2::
	trainerbattle_no_intro TRAINER_LEADER_JASMINEB, OlivineGym_Text_Deafeated
	goto OlivineGym_EventScript_LeaderWon
	end
OlivineGym_Script_Jasmine_TeamOption3::
	trainerbattle_no_intro TRAINER_LEADER_JASMINEC, OlivineGym_Text_Deafeated
	goto OlivineGym_EventScript_LeaderWon
	end

OlivineGym_EventScript_LeaderWon::
	setflag FLAG_BADGE_MINERAL
	message OlivineGym_Text_ReceivedBadge
	waitmessage
	playfanfare MUS_OBTAIN_BADGE
	waitfanfare
	msgbox OlivineGym_Text_Jasmine_postbattle
	release
	end

@ Rematch (after the Champion, FLAG_DEFEATED_CHAMP): any number of times, one of three teams
OlivineGym_EventScript_LeaderAfter::
	goto_if_unset FLAG_DEFEATED_CHAMP, OlivineGym_EventScript_LeaderPostBattle
	msgbox OlivineGym_Text_LeaderRematchAsk, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, OlivineGym_Script_NoRematch
	msgbox OlivineGym_Text_RebattleIntro
	random 3
	goto_if_eq VAR_RESULT, 1, OlivineGym_Script_Jasmine_RematchTeamOption2
	goto_if_eq VAR_RESULT, 2, OlivineGym_Script_Jasmine_RematchTeamOption3
OlivineGym_Script_Jasmine_RematchTeamOption1::
	cleartrainerflag TRAINER_LEADER_JASMINEREMATCHA
	trainerbattle_no_intro TRAINER_LEADER_JASMINEREMATCHA, OlivineGym_Text_RebattleDeafeated
	goto OlivineGym_EventScript_RematchWon
	end
OlivineGym_Script_Jasmine_RematchTeamOption2::
	cleartrainerflag TRAINER_LEADER_JASMINEREMATCHB
	trainerbattle_no_intro TRAINER_LEADER_JASMINEREMATCHB, OlivineGym_Text_RebattleDeafeated
	goto OlivineGym_EventScript_RematchWon
	end
OlivineGym_Script_Jasmine_RematchTeamOption3::
	cleartrainerflag TRAINER_LEADER_JASMINEREMATCHC
	trainerbattle_no_intro TRAINER_LEADER_JASMINEREMATCHC, OlivineGym_Text_RebattleDeafeated
	goto OlivineGym_EventScript_RematchWon
	end

OlivineGym_EventScript_LeaderPostBattle::
	msgbox OlivineGym_Text_Jasmine_postbattle
	release
	end

OlivineGym_EventScript_RematchWon::
	msgbox OlivineGym_Text_Jasmine_postrematch
	release
	end

OlivineGym_Script_NoRematch::
	msgbox OlivineGym_Text_LeaderNoRematch
	release
	end

@ Gym helper: explains rematches
OlivineGym_Script_Rematch::
	lock
	faceplayer
	goto_if_set FLAG_DEFEATED_CHAMP, OlivineGym_Script_HelperAfter
	goto_if_set FLAG_BADGE_MINERAL, OlivineGym_Script_HelperBadge
	msgbox OlivineGym_Text_HelperBefore
	release
	end
OlivineGym_Script_HelperBadge::
	msgbox OlivineGym_Text_HelperBadge
	release
	end
OlivineGym_Script_HelperAfter::
	msgbox OlivineGym_Text_HelperAfter
	release
	end

OlivineGym_Text_ReceivedBadge::
	.string "{PLAYER} received the\n"
	.string "MINERAL BADGE from JASMINE!$"

OlivineGym_Text_LeaderRematchAsk::
	.string "Um... if you have time... would\n"
	.string "you battle me again?$"

OlivineGym_Text_LeaderNoRematch::
	.string "Come back any time.$"

OlivineGym_Text_HelperBefore::
	.string "Hey, champ! Beat JASMINE and the\n"
	.string "MINERAL BADGE is yours!$"

OlivineGym_Text_HelperBadge::
	.string "Nice BADGE, champ! Once you've\n"
	.string "beaten the CHAMPION, JASMINE\l"
	.string "takes on rematches.$"

OlivineGym_Text_HelperAfter::
	.string "Hey, champ! JASMINE takes on\n"
	.string "rematches any time. Just ask!$"
