CianwoodGym_MapScripts::
	.byte 0

@ Leader: first win gives the STORM BADGE (FLAG_BADGE_STORM, also set in CB2_EndTrainerBattle);
@ afterwards the leader offers rematches.
CianwoodGym_EventScript_Leader_Chuck::
	lock
	faceplayer
	goto_if_set FLAG_BADGE_STORM, CianwoodGym_EventScript_LeaderAfter
	goto_if_defeated TRAINER_LEADER_CHUCKA, CianwoodGym_EventScript_LeaderWon
	goto_if_defeated TRAINER_LEADER_CHUCKB, CianwoodGym_EventScript_LeaderWon
	goto_if_defeated TRAINER_LEADER_CHUCKC, CianwoodGym_EventScript_LeaderWon
	msgbox CianwoodGym_Text_Intro
	random 3
	goto_if_eq VAR_RESULT, 1, CianwoodGym_Script_Chuck_TeamOption2
	goto_if_eq VAR_RESULT, 2, CianwoodGym_Script_Chuck_TeamOption3
CianwoodGym_Script_Chuck_TeamOption1::
	trainerbattle_no_intro TRAINER_LEADER_CHUCKA, CianwoodGym_Text_Deafeated
	goto CianwoodGym_EventScript_LeaderWon
	end
CianwoodGym_Script_Chuck_TeamOption2::
	trainerbattle_no_intro TRAINER_LEADER_CHUCKB, CianwoodGym_Text_Deafeated
	goto CianwoodGym_EventScript_LeaderWon
	end
CianwoodGym_Script_Chuck_TeamOption3::
	trainerbattle_no_intro TRAINER_LEADER_CHUCKC, CianwoodGym_Text_Deafeated
	goto CianwoodGym_EventScript_LeaderWon
	end

CianwoodGym_EventScript_LeaderWon::
	setflag FLAG_BADGE_STORM
	message CianwoodGym_Text_ReceivedBadge
	waitmessage
	playfanfare MUS_OBTAIN_BADGE
	waitfanfare
	msgbox CianwoodGym_Text_Chuck_postbattle
	release
	end

@ Rematch (after the Champion, FLAG_DEFEATED_CHAMP): any number of times, one of three teams
CianwoodGym_EventScript_LeaderAfter::
	goto_if_unset FLAG_DEFEATED_CHAMP, CianwoodGym_EventScript_LeaderPostBattle
	msgbox CianwoodGym_Text_LeaderRematchAsk, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, CianwoodGym_Script_NoRematch
	msgbox CianwoodGym_Text_RebattleIntro
	random 3
	goto_if_eq VAR_RESULT, 1, CianwoodGym_Script_Chuck_RematchTeamOption2
	goto_if_eq VAR_RESULT, 2, CianwoodGym_Script_Chuck_RematchTeamOption3
CianwoodGym_Script_Chuck_RematchTeamOption1::
	cleartrainerflag TRAINER_LEADER_CHUCKREMATCHA
	trainerbattle_no_intro TRAINER_LEADER_CHUCKREMATCHA, CianwoodGym_Text_RebattleDeafeated
	goto CianwoodGym_EventScript_RematchWon
	end
CianwoodGym_Script_Chuck_RematchTeamOption2::
	cleartrainerflag TRAINER_LEADER_CHUCKREMATCHB
	trainerbattle_no_intro TRAINER_LEADER_CHUCKREMATCHB, CianwoodGym_Text_RebattleDeafeated
	goto CianwoodGym_EventScript_RematchWon
	end
CianwoodGym_Script_Chuck_RematchTeamOption3::
	cleartrainerflag TRAINER_LEADER_CHUCKREMATCHC
	trainerbattle_no_intro TRAINER_LEADER_CHUCKREMATCHC, CianwoodGym_Text_RebattleDeafeated
	goto CianwoodGym_EventScript_RematchWon
	end

CianwoodGym_EventScript_LeaderPostBattle::
	msgbox CianwoodGym_Text_Chuck_postbattle
	release
	end

CianwoodGym_EventScript_RematchWon::
	msgbox CianwoodGym_Text_Chuck_postrematch
	release
	end

CianwoodGym_Script_NoRematch::
	msgbox CianwoodGym_Text_LeaderNoRematch
	release
	end

@ Gym helper: explains rematches
CianwoodGym_Script_Rematch::
	lock
	faceplayer
	goto_if_set FLAG_DEFEATED_CHAMP, CianwoodGym_Script_HelperAfter
	goto_if_set FLAG_BADGE_STORM, CianwoodGym_Script_HelperBadge
	msgbox CianwoodGym_Text_HelperBefore
	release
	end
CianwoodGym_Script_HelperBadge::
	msgbox CianwoodGym_Text_HelperBadge
	release
	end
CianwoodGym_Script_HelperAfter::
	msgbox CianwoodGym_Text_HelperAfter
	release
	end

CianwoodGym_Text_ReceivedBadge::
	.string "{PLAYER} received the\n"
	.string "STORM BADGE from CHUCK!$"

CianwoodGym_Text_LeaderRematchAsk::
	.string "Wahahah! My fists are itching!\n"
	.string "How about a rematch?$"

CianwoodGym_Text_LeaderNoRematch::
	.string "Come back any time.$"

CianwoodGym_Text_HelperBefore::
	.string "Hey, champ! Beat CHUCK and the\n"
	.string "STORM BADGE is yours!$"

CianwoodGym_Text_HelperBadge::
	.string "Nice BADGE, champ! Once you've\n"
	.string "beaten the CHAMPION, CHUCK\l"
	.string "takes on rematches.$"

CianwoodGym_Text_HelperAfter::
	.string "Hey, champ! CHUCK takes on\n"
	.string "rematches any time. Just ask!$"
