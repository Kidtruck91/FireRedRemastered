AzaleaGym_MapScripts::
	.byte 0

AzaleaGym_EventScript_Leader_Bugsy::
	goto_if_set FLAG_REMATCH_BUGSY, AzaleaGym_EventScript_LeaderRematch
	random 3
	switch VAR_RESULT
	case 0, AzaleaGym_Script_Bugsy_TeamOption1
	case 1, AzaleaGym_Script_Bugsy_TeamOption2
	case 2, AzaleaGym_Script_Bugsy_TeamOption3
	msgbox AzaleaGym_Text_Bugsy_postbattle, MSGBOX_AUTOCLOSE
	end
AzaleaGym_Script_Bugsy_TeamOption1::
 trainerbattle_single TRAINER_LEADER_BUGSYA, AzaleaGym_Text_Intro,AzaleaGym_Text_Deafeated
AzaleaGym_Script_Bugsy_TeamOption2::
 trainerbattle_single TRAINER_LEADER_BUGSYB, AzaleaGym_Text_Intro, AzaleaGym_Text_Deafeated
AzaleaGym_Script_Bugsy_TeamOption3::
 trainerbattle_single TRAINER_LEADER_BUGSYC, AzaleaGym_Text_Intro, AzaleaGym_Text_Deafeated

AzaleaGym_EventScript_LeaderRematch::
 random 3
 switch VAR_RESULT
 case 0, AzaleaGym_Script_Bugsy_RematchTeamOption1
 case 1, AzaleaGym_Script_Bugsy_RematchTeamOption2
 case 2, AzaleaGym_Script_Bugsy_RematchTeamOption3
 msgbox AzaleaGym_Text_Bugsy_postrematch, MSGBOX_AUTOCLOSE
 clearflag FLAG_REMATCH_BUGSY
 end
AzaleaGym_Script_Bugsy_RematchTeamOption1::
 trainerbattle_single TRAINER_LEADER_BUGSYREMATCHA, AzaleaGym_Text_RebattleIntro, AzaleaGym_Text_RebattleDeafeated
AzaleaGym_Script_Bugsy_RematchTeamOption2::
 trainerbattle_single TRAINER_LEADER_BUGSYREMATCHB, AzaleaGym_Text_RebattleIntro, AzaleaGym_Text_RebattleDeafeated
AzaleaGym_Script_Bugsy_RematchTeamOption3::
 trainerbattle_single TRAINER_LEADER_BUGSYREMATCHC,AzaleaGym_Text_RebattleIntro, AzaleaGym_Text_RebattleDeafeated

AzaleaGym_Script_Rematch::
	lock
	faceplayer
	msgbox AzaleaGym_Text_RematchQuestion, MSGBOX_YESNO
	compare VAR_RESULT, 0
	goto_if_eq AzaleaGym_Script_NoRematch
	setflag FLAG_REMATCH_BUGSY
	msgbox AzaleaGym_Text_RematchGo, MSGBOX_AUTOCLOSE
	end
AzaleaGym_Script_NoRematch::
	msgbox AzaleaGym_Text_NoRematch, MSGBOX_AUTOCLOSE
	release
	end
