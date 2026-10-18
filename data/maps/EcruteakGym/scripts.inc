EcruteakGym_MapScripts::
	.byte 0

@ Leader: first win gives the FOG BADGE (FLAG_BADGE_FOG, also set in CB2_EndTrainerBattle);
@ afterwards the leader offers rematches.
EcruteakGym_EventScript_Leader_Morty::
	lock
	faceplayer
	goto_if_set FLAG_BADGE_FOG, EcruteakGym_EventScript_LeaderAfter
	goto_if_defeated TRAINER_LEADER_MORTYA, EcruteakGym_EventScript_LeaderWon
	goto_if_defeated TRAINER_LEADER_MORTYB, EcruteakGym_EventScript_LeaderWon
	goto_if_defeated TRAINER_LEADER_MORTYC, EcruteakGym_EventScript_LeaderWon
	msgbox EcruteakGym_Text_Intro
	random 3
	goto_if_eq VAR_RESULT, 1, EcruteakGym_Script_Morty_TeamOption2
	goto_if_eq VAR_RESULT, 2, EcruteakGym_Script_Morty_TeamOption3
EcruteakGym_Script_Morty_TeamOption1::
	trainerbattle_no_intro TRAINER_LEADER_MORTYA, EcruteakGym_Text_Deafeated
	goto EcruteakGym_EventScript_LeaderWon
	end
EcruteakGym_Script_Morty_TeamOption2::
	trainerbattle_no_intro TRAINER_LEADER_MORTYB, EcruteakGym_Text_Deafeated
	goto EcruteakGym_EventScript_LeaderWon
	end
EcruteakGym_Script_Morty_TeamOption3::
	trainerbattle_no_intro TRAINER_LEADER_MORTYC, EcruteakGym_Text_Deafeated
	goto EcruteakGym_EventScript_LeaderWon
	end

EcruteakGym_EventScript_LeaderWon::
	setflag FLAG_BADGE_FOG
	message EcruteakGym_Text_ReceivedBadge
	waitmessage
	playfanfare MUS_OBTAIN_BADGE
	waitfanfare
	msgbox EcruteakGym_Text_Morty_postbattle
	release
	end

@ Rematch (after the Champion, FLAG_DEFEATED_CHAMP): any number of times, one of three teams
EcruteakGym_EventScript_LeaderAfter::
	goto_if_unset FLAG_DEFEATED_CHAMP, EcruteakGym_EventScript_LeaderPostBattle
	msgbox EcruteakGym_Text_LeaderRematchAsk, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, EcruteakGym_Script_NoRematch
	msgbox EcruteakGym_Text_RebattleIntro
	random 3
	goto_if_eq VAR_RESULT, 1, EcruteakGym_Script_Morty_RematchTeamOption2
	goto_if_eq VAR_RESULT, 2, EcruteakGym_Script_Morty_RematchTeamOption3
EcruteakGym_Script_Morty_RematchTeamOption1::
	cleartrainerflag TRAINER_LEADER_MORTYREMATCHA
	trainerbattle_no_intro TRAINER_LEADER_MORTYREMATCHA, EcruteakGym_Text_RebattleDeafeated
	goto EcruteakGym_EventScript_RematchWon
	end
EcruteakGym_Script_Morty_RematchTeamOption2::
	cleartrainerflag TRAINER_LEADER_MORTYREMATCHB
	trainerbattle_no_intro TRAINER_LEADER_MORTYREMATCHB, EcruteakGym_Text_RebattleDeafeated
	goto EcruteakGym_EventScript_RematchWon
	end
EcruteakGym_Script_Morty_RematchTeamOption3::
	cleartrainerflag TRAINER_LEADER_MORTYREMATCHC
	trainerbattle_no_intro TRAINER_LEADER_MORTYREMATCHC, EcruteakGym_Text_RebattleDeafeated
	goto EcruteakGym_EventScript_RematchWon
	end

EcruteakGym_EventScript_LeaderPostBattle::
	msgbox EcruteakGym_Text_Morty_postbattle
	release
	end

EcruteakGym_EventScript_RematchWon::
	msgbox EcruteakGym_Text_Morty_postrematch
	release
	end

EcruteakGym_Script_NoRematch::
	msgbox EcruteakGym_Text_LeaderNoRematch
	release
	end

@ Gym helper: explains rematches
EcruteakGym_Script_Rematch::
	lock
	faceplayer
	goto_if_set FLAG_DEFEATED_CHAMP, EcruteakGym_Script_HelperAfter
	goto_if_set FLAG_BADGE_FOG, EcruteakGym_Script_HelperBadge
	msgbox EcruteakGym_Text_HelperBefore
	release
	end
EcruteakGym_Script_HelperBadge::
	msgbox EcruteakGym_Text_HelperBadge
	release
	end
EcruteakGym_Script_HelperAfter::
	msgbox EcruteakGym_Text_HelperAfter
	release
	end

EcruteakGym_Text_ReceivedBadge::
	.string "{PLAYER} received the\n"
	.string "FOG BADGE from MORTY!$"

EcruteakGym_Text_LeaderRematchAsk::
	.string "The mist has shown me more of\n"
	.string "your path. Shall we battle\l"
	.string "again?$"

EcruteakGym_Text_LeaderNoRematch::
	.string "Come back any time.$"

EcruteakGym_Text_HelperBefore::
	.string "Hey, champ! Beat MORTY and the\n"
	.string "FOG BADGE is yours!$"

EcruteakGym_Text_HelperBadge::
	.string "Nice BADGE, champ! Once you've\n"
	.string "beaten the CHAMPION, MORTY\l"
	.string "takes on rematches.$"

EcruteakGym_Text_HelperAfter::
	.string "Hey, champ! MORTY takes on\n"
	.string "rematches any time. Just ask!$"
