.equ LOCALID_RUDY, 1

OrangeIslands_Trovita_Gym_MapScripts::
	map_script MAP_SCRIPT_ON_LOAD, OrangeIslands_Trovita_Gym_OnLoad
	.byte 0

@ The hedges are shut in the layout; ON_LOAD parts the ones already answered.
OrangeIslands_Trovita_Gym_OnLoad::
	call_if_set FLAG_TROVITA_GYM_GATE_1, OrangeIslands_Trovita_Gym_OnLoadOpenGate1
	call_if_set FLAG_TROVITA_GYM_GATE_2, OrangeIslands_Trovita_Gym_OnLoadOpenGate2
	call_if_set FLAG_TROVITA_GYM_GATE_3, OrangeIslands_Trovita_Gym_OnLoadOpenGate3
	call_if_set FLAG_TROVITA_GYM_GATE_4, OrangeIslands_Trovita_Gym_OnLoadOpenGate4
	call_if_set FLAG_TROVITA_GYM_GATE_5, OrangeIslands_Trovita_Gym_OnLoadOpenGate5
	call_if_set FLAG_TROVITA_GYM_GATE_6, OrangeIslands_Trovita_Gym_OnLoadOpenGate6
	end

OrangeIslands_Trovita_Gym_OnLoadOpenGate1::
	call OrangeIslands_Trovita_Gym_EventScript_OpenGate1
	return

OrangeIslands_Trovita_Gym_OnLoadOpenGate2::
	call OrangeIslands_Trovita_Gym_EventScript_OpenGate2
	return

OrangeIslands_Trovita_Gym_OnLoadOpenGate3::
	call OrangeIslands_Trovita_Gym_EventScript_OpenGate3
	return

OrangeIslands_Trovita_Gym_OnLoadOpenGate4::
	call OrangeIslands_Trovita_Gym_EventScript_OpenGate4
	return

OrangeIslands_Trovita_Gym_OnLoadOpenGate5::
	call OrangeIslands_Trovita_Gym_EventScript_OpenGate5
	return

OrangeIslands_Trovita_Gym_OnLoadOpenGate6::
	call OrangeIslands_Trovita_Gym_EventScript_OpenGate6
	return

OrangeIslands_Trovita_Gym_EventScript_OpenGate1::
	setmetatile  3, 22, METATILE_CeladonGym_Floor, 0
	setmetatile  4, 22, METATILE_CeladonGym_Floor, 0
	return

OrangeIslands_Trovita_Gym_EventScript_OpenGate2::
	setmetatile 10, 19, METATILE_CeladonGym_Floor, 0
	setmetatile 11, 19, METATILE_CeladonGym_Floor, 0
	return

OrangeIslands_Trovita_Gym_EventScript_OpenGate3::
	setmetatile  3, 16, METATILE_CeladonGym_Floor, 0
	setmetatile  4, 16, METATILE_CeladonGym_Floor, 0
	return

OrangeIslands_Trovita_Gym_EventScript_OpenGate4::
	setmetatile 10, 13, METATILE_CeladonGym_Floor, 0
	setmetatile 11, 13, METATILE_CeladonGym_Floor, 0
	return

OrangeIslands_Trovita_Gym_EventScript_OpenGate5::
	setmetatile  3, 10, METATILE_CeladonGym_Floor, 0
	setmetatile  4, 10, METATILE_CeladonGym_Floor, 0
	return

OrangeIslands_Trovita_Gym_EventScript_OpenGate6::
	setmetatile  7,  7, METATILE_CeladonGym_Floor, 0
	setmetatile  8,  7, METATILE_CeladonGym_Floor, 0
	return

@ Each panel keeps two questions and picks one, so a second run through
@ the gym is not the same six answers. Every index below is what the ROM
@ data gives -- see docs/orange-islands/trovita-quiz.md.
OrangeIslands_Trovita_Gym_EventScript_Panel1::
	lockall
	goto_if_set FLAG_TROVITA_GYM_GATE_1, OrangeIslands_Trovita_Gym_EventScript_GateAlreadyOpen
	random 2
	goto_if_eq VAR_RESULT, 0, OrangeIslands_Trovita_Gym_EventScript_Ask1A
	goto OrangeIslands_Trovita_Gym_EventScript_Ask1B

OrangeIslands_Trovita_Gym_EventScript_Ask1A::
	message OrangeIslands_Trovita_Gym_Text_Q1A
	waitmessage
	multichoice 0, 0, MULTICHOICE_TROVITA_QUIZ_1A, FALSE
	goto_if_eq VAR_RESULT, SCR_MENU_CANCEL, OrangeIslands_Trovita_Gym_EventScript_QuizCancelled
	goto_if_eq VAR_RESULT, 2, OrangeIslands_Trovita_Gym_EventScript_Gate1Correct
	goto OrangeIslands_Trovita_Gym_EventScript_Gate1Wrong

OrangeIslands_Trovita_Gym_EventScript_Ask1B::
	message OrangeIslands_Trovita_Gym_Text_Q1B
	waitmessage
	multichoice 0, 0, MULTICHOICE_TROVITA_QUIZ_1B, FALSE
	goto_if_eq VAR_RESULT, SCR_MENU_CANCEL, OrangeIslands_Trovita_Gym_EventScript_QuizCancelled
	goto_if_eq VAR_RESULT, 1, OrangeIslands_Trovita_Gym_EventScript_Gate1Correct
	goto OrangeIslands_Trovita_Gym_EventScript_Gate1Wrong

OrangeIslands_Trovita_Gym_EventScript_Gate1Correct::
	call OrangeIslands_Trovita_Gym_EventScript_CorrectAnswer
	call OrangeIslands_Trovita_Gym_EventScript_Gate1Opens
	releaseall
	end

OrangeIslands_Trovita_Gym_EventScript_Gate1Opens::
	playse SE_UNLOCK
	waitse
	call OrangeIslands_Trovita_Gym_EventScript_OpenGate1
	special DrawWholeMapView
	setflag FLAG_TROVITA_GYM_GATE_1
	return

OrangeIslands_Trovita_Gym_EventScript_Gate1Wrong::
	call OrangeIslands_Trovita_Gym_EventScript_WrongAnswer
	goto_if_not_defeated TRAINER_COOLTRAINER_TROVITA_LIME, OrangeIslands_Trovita_Gym_EventScript_BattleLime
	releaseall
	end

@ Beat the student and the hedge opens anyway -- know it or earn it.
OrangeIslands_Trovita_Gym_EventScript_BattleLime::
	msgbox OrangeIslands_Trovita_Gym_Text_LimeStepsIn
	trainerbattle_no_intro TRAINER_COOLTRAINER_TROVITA_LIME, OrangeIslands_Trovita_Gym_Text_LimeDefeat
	goto_if_set FLAG_TROVITA_GYM_GATE_1, OrangeIslands_Trovita_Gym_EventScript_GateAlreadyOpen
	call OrangeIslands_Trovita_Gym_EventScript_Gate1Opens
	releaseall
	end

OrangeIslands_Trovita_Gym_EventScript_Panel2::
	lockall
	goto_if_set FLAG_TROVITA_GYM_GATE_2, OrangeIslands_Trovita_Gym_EventScript_GateAlreadyOpen
	random 2
	goto_if_eq VAR_RESULT, 0, OrangeIslands_Trovita_Gym_EventScript_Ask2A
	goto OrangeIslands_Trovita_Gym_EventScript_Ask2B

OrangeIslands_Trovita_Gym_EventScript_Ask2A::
	message OrangeIslands_Trovita_Gym_Text_Q2A
	waitmessage
	multichoice 0, 0, MULTICHOICE_TROVITA_QUIZ_2A, FALSE
	goto_if_eq VAR_RESULT, SCR_MENU_CANCEL, OrangeIslands_Trovita_Gym_EventScript_QuizCancelled
	goto_if_eq VAR_RESULT, 3, OrangeIslands_Trovita_Gym_EventScript_Gate2Correct
	goto OrangeIslands_Trovita_Gym_EventScript_Gate2Wrong

OrangeIslands_Trovita_Gym_EventScript_Ask2B::
	message OrangeIslands_Trovita_Gym_Text_Q2B
	waitmessage
	multichoice 0, 0, MULTICHOICE_TROVITA_QUIZ_2B, FALSE
	goto_if_eq VAR_RESULT, SCR_MENU_CANCEL, OrangeIslands_Trovita_Gym_EventScript_QuizCancelled
	goto_if_eq VAR_RESULT, 0, OrangeIslands_Trovita_Gym_EventScript_Gate2Correct
	goto OrangeIslands_Trovita_Gym_EventScript_Gate2Wrong

OrangeIslands_Trovita_Gym_EventScript_Gate2Correct::
	call OrangeIslands_Trovita_Gym_EventScript_CorrectAnswer
	call OrangeIslands_Trovita_Gym_EventScript_Gate2Opens
	releaseall
	end

OrangeIslands_Trovita_Gym_EventScript_Gate2Opens::
	playse SE_UNLOCK
	waitse
	call OrangeIslands_Trovita_Gym_EventScript_OpenGate2
	special DrawWholeMapView
	setflag FLAG_TROVITA_GYM_GATE_2
	return

OrangeIslands_Trovita_Gym_EventScript_Gate2Wrong::
	call OrangeIslands_Trovita_Gym_EventScript_WrongAnswer
	goto_if_not_defeated TRAINER_COOLTRAINER_TROVITA_ROWAN, OrangeIslands_Trovita_Gym_EventScript_BattleRowan
	releaseall
	end

@ Beat the student and the hedge opens anyway -- know it or earn it.
OrangeIslands_Trovita_Gym_EventScript_BattleRowan::
	msgbox OrangeIslands_Trovita_Gym_Text_RowanStepsIn
	trainerbattle_no_intro TRAINER_COOLTRAINER_TROVITA_ROWAN, OrangeIslands_Trovita_Gym_Text_RowanDefeat
	goto_if_set FLAG_TROVITA_GYM_GATE_2, OrangeIslands_Trovita_Gym_EventScript_GateAlreadyOpen
	call OrangeIslands_Trovita_Gym_EventScript_Gate2Opens
	releaseall
	end

OrangeIslands_Trovita_Gym_EventScript_Panel3::
	lockall
	goto_if_set FLAG_TROVITA_GYM_GATE_3, OrangeIslands_Trovita_Gym_EventScript_GateAlreadyOpen
	random 2
	goto_if_eq VAR_RESULT, 0, OrangeIslands_Trovita_Gym_EventScript_Ask3A
	goto OrangeIslands_Trovita_Gym_EventScript_Ask3B

OrangeIslands_Trovita_Gym_EventScript_Ask3A::
	message OrangeIslands_Trovita_Gym_Text_Q3A
	waitmessage
	multichoice 0, 0, MULTICHOICE_TROVITA_QUIZ_3A, FALSE
	goto_if_eq VAR_RESULT, SCR_MENU_CANCEL, OrangeIslands_Trovita_Gym_EventScript_QuizCancelled
	goto_if_eq VAR_RESULT, 3, OrangeIslands_Trovita_Gym_EventScript_Gate3Correct
	goto OrangeIslands_Trovita_Gym_EventScript_Gate3Wrong

OrangeIslands_Trovita_Gym_EventScript_Ask3B::
	message OrangeIslands_Trovita_Gym_Text_Q3B
	waitmessage
	multichoice 0, 0, MULTICHOICE_TROVITA_QUIZ_3B, FALSE
	goto_if_eq VAR_RESULT, SCR_MENU_CANCEL, OrangeIslands_Trovita_Gym_EventScript_QuizCancelled
	goto_if_eq VAR_RESULT, 0, OrangeIslands_Trovita_Gym_EventScript_Gate3Correct
	goto OrangeIslands_Trovita_Gym_EventScript_Gate3Wrong

OrangeIslands_Trovita_Gym_EventScript_Gate3Correct::
	call OrangeIslands_Trovita_Gym_EventScript_CorrectAnswer
	call OrangeIslands_Trovita_Gym_EventScript_Gate3Opens
	releaseall
	end

OrangeIslands_Trovita_Gym_EventScript_Gate3Opens::
	playse SE_UNLOCK
	waitse
	call OrangeIslands_Trovita_Gym_EventScript_OpenGate3
	special DrawWholeMapView
	setflag FLAG_TROVITA_GYM_GATE_3
	return

OrangeIslands_Trovita_Gym_EventScript_Gate3Wrong::
	call OrangeIslands_Trovita_Gym_EventScript_WrongAnswer
	goto_if_not_defeated TRAINER_COOLTRAINER_TROVITA_YUZU, OrangeIslands_Trovita_Gym_EventScript_BattleYuzu
	releaseall
	end

@ Beat the student and the hedge opens anyway -- know it or earn it.
OrangeIslands_Trovita_Gym_EventScript_BattleYuzu::
	msgbox OrangeIslands_Trovita_Gym_Text_YuzuStepsIn
	trainerbattle_no_intro TRAINER_COOLTRAINER_TROVITA_YUZU, OrangeIslands_Trovita_Gym_Text_YuzuDefeat
	goto_if_set FLAG_TROVITA_GYM_GATE_3, OrangeIslands_Trovita_Gym_EventScript_GateAlreadyOpen
	call OrangeIslands_Trovita_Gym_EventScript_Gate3Opens
	releaseall
	end

OrangeIslands_Trovita_Gym_EventScript_Panel4::
	lockall
	goto_if_set FLAG_TROVITA_GYM_GATE_4, OrangeIslands_Trovita_Gym_EventScript_GateAlreadyOpen
	random 2
	goto_if_eq VAR_RESULT, 0, OrangeIslands_Trovita_Gym_EventScript_Ask4A
	goto OrangeIslands_Trovita_Gym_EventScript_Ask4B

OrangeIslands_Trovita_Gym_EventScript_Ask4A::
	message OrangeIslands_Trovita_Gym_Text_Q4A
	waitmessage
	multichoice 0, 0, MULTICHOICE_TROVITA_QUIZ_4A, FALSE
	goto_if_eq VAR_RESULT, SCR_MENU_CANCEL, OrangeIslands_Trovita_Gym_EventScript_QuizCancelled
	goto_if_eq VAR_RESULT, 1, OrangeIslands_Trovita_Gym_EventScript_Gate4Correct
	goto OrangeIslands_Trovita_Gym_EventScript_Gate4Wrong

OrangeIslands_Trovita_Gym_EventScript_Ask4B::
	message OrangeIslands_Trovita_Gym_Text_Q4B
	waitmessage
	multichoice 0, 0, MULTICHOICE_TROVITA_QUIZ_4B, FALSE
	goto_if_eq VAR_RESULT, SCR_MENU_CANCEL, OrangeIslands_Trovita_Gym_EventScript_QuizCancelled
	goto_if_eq VAR_RESULT, 0, OrangeIslands_Trovita_Gym_EventScript_Gate4Correct
	goto OrangeIslands_Trovita_Gym_EventScript_Gate4Wrong

OrangeIslands_Trovita_Gym_EventScript_Gate4Correct::
	call OrangeIslands_Trovita_Gym_EventScript_CorrectAnswer
	call OrangeIslands_Trovita_Gym_EventScript_Gate4Opens
	releaseall
	end

OrangeIslands_Trovita_Gym_EventScript_Gate4Opens::
	playse SE_UNLOCK
	waitse
	call OrangeIslands_Trovita_Gym_EventScript_OpenGate4
	special DrawWholeMapView
	setflag FLAG_TROVITA_GYM_GATE_4
	return

OrangeIslands_Trovita_Gym_EventScript_Gate4Wrong::
	call OrangeIslands_Trovita_Gym_EventScript_WrongAnswer
	goto_if_not_defeated TRAINER_COOLTRAINER_TROVITA_CEDRO, OrangeIslands_Trovita_Gym_EventScript_BattleCedro
	releaseall
	end

@ Beat the student and the hedge opens anyway -- know it or earn it.
OrangeIslands_Trovita_Gym_EventScript_BattleCedro::
	msgbox OrangeIslands_Trovita_Gym_Text_CedroStepsIn
	trainerbattle_no_intro TRAINER_COOLTRAINER_TROVITA_CEDRO, OrangeIslands_Trovita_Gym_Text_CedroDefeat
	goto_if_set FLAG_TROVITA_GYM_GATE_4, OrangeIslands_Trovita_Gym_EventScript_GateAlreadyOpen
	call OrangeIslands_Trovita_Gym_EventScript_Gate4Opens
	releaseall
	end

OrangeIslands_Trovita_Gym_EventScript_Panel5::
	lockall
	goto_if_set FLAG_TROVITA_GYM_GATE_5, OrangeIslands_Trovita_Gym_EventScript_GateAlreadyOpen
	random 2
	goto_if_eq VAR_RESULT, 0, OrangeIslands_Trovita_Gym_EventScript_Ask5A
	goto OrangeIslands_Trovita_Gym_EventScript_Ask5B

OrangeIslands_Trovita_Gym_EventScript_Ask5A::
	message OrangeIslands_Trovita_Gym_Text_Q5A
	waitmessage
	multichoice 0, 0, MULTICHOICE_TROVITA_QUIZ_5A, FALSE
	goto_if_eq VAR_RESULT, SCR_MENU_CANCEL, OrangeIslands_Trovita_Gym_EventScript_QuizCancelled
	goto_if_eq VAR_RESULT, 1, OrangeIslands_Trovita_Gym_EventScript_Gate5Correct
	goto OrangeIslands_Trovita_Gym_EventScript_Gate5Wrong

OrangeIslands_Trovita_Gym_EventScript_Ask5B::
	message OrangeIslands_Trovita_Gym_Text_Q5B
	waitmessage
	multichoice 0, 0, MULTICHOICE_TROVITA_QUIZ_5B, FALSE
	goto_if_eq VAR_RESULT, SCR_MENU_CANCEL, OrangeIslands_Trovita_Gym_EventScript_QuizCancelled
	goto_if_eq VAR_RESULT, 2, OrangeIslands_Trovita_Gym_EventScript_Gate5Correct
	goto OrangeIslands_Trovita_Gym_EventScript_Gate5Wrong

OrangeIslands_Trovita_Gym_EventScript_Gate5Correct::
	call OrangeIslands_Trovita_Gym_EventScript_CorrectAnswer
	call OrangeIslands_Trovita_Gym_EventScript_Gate5Opens
	releaseall
	end

OrangeIslands_Trovita_Gym_EventScript_Gate5Opens::
	playse SE_UNLOCK
	waitse
	call OrangeIslands_Trovita_Gym_EventScript_OpenGate5
	special DrawWholeMapView
	setflag FLAG_TROVITA_GYM_GATE_5
	return

OrangeIslands_Trovita_Gym_EventScript_Gate5Wrong::
	call OrangeIslands_Trovita_Gym_EventScript_WrongAnswer
	releaseall
	end

OrangeIslands_Trovita_Gym_EventScript_Panel6::
	lockall
	goto_if_set FLAG_TROVITA_GYM_GATE_6, OrangeIslands_Trovita_Gym_EventScript_GateAlreadyOpen
	random 2
	goto_if_eq VAR_RESULT, 0, OrangeIslands_Trovita_Gym_EventScript_Ask6A
	goto OrangeIslands_Trovita_Gym_EventScript_Ask6B

OrangeIslands_Trovita_Gym_EventScript_Ask6A::
	message OrangeIslands_Trovita_Gym_Text_Q6A
	waitmessage
	multichoice 0, 0, MULTICHOICE_TROVITA_QUIZ_6A, FALSE
	goto_if_eq VAR_RESULT, SCR_MENU_CANCEL, OrangeIslands_Trovita_Gym_EventScript_QuizCancelled
	goto_if_eq VAR_RESULT, 2, OrangeIslands_Trovita_Gym_EventScript_Gate6Correct
	goto OrangeIslands_Trovita_Gym_EventScript_Gate6Wrong

OrangeIslands_Trovita_Gym_EventScript_Ask6B::
	message OrangeIslands_Trovita_Gym_Text_Q6B
	waitmessage
	multichoice 0, 0, MULTICHOICE_TROVITA_QUIZ_6B, FALSE
	goto_if_eq VAR_RESULT, SCR_MENU_CANCEL, OrangeIslands_Trovita_Gym_EventScript_QuizCancelled
	goto_if_eq VAR_RESULT, 2, OrangeIslands_Trovita_Gym_EventScript_Gate6Correct
	goto OrangeIslands_Trovita_Gym_EventScript_Gate6Wrong

OrangeIslands_Trovita_Gym_EventScript_Gate6Correct::
	call OrangeIslands_Trovita_Gym_EventScript_CorrectAnswer
	call OrangeIslands_Trovita_Gym_EventScript_Gate6Opens
	releaseall
	end

OrangeIslands_Trovita_Gym_EventScript_Gate6Opens::
	playse SE_UNLOCK
	waitse
	call OrangeIslands_Trovita_Gym_EventScript_OpenGate6
	special DrawWholeMapView
	setflag FLAG_TROVITA_GYM_GATE_6
	return

OrangeIslands_Trovita_Gym_EventScript_Gate6Wrong::
	call OrangeIslands_Trovita_Gym_EventScript_WrongAnswer
	releaseall
	end

OrangeIslands_Trovita_Gym_EventScript_CorrectAnswer::
	playfanfare MUS_LEVEL_UP
	waitfanfare
	msgbox OrangeIslands_Trovita_Gym_Text_Correct
	return

OrangeIslands_Trovita_Gym_EventScript_WrongAnswer::
	playse SE_BOO
	waitse
	msgbox OrangeIslands_Trovita_Gym_Text_Wrong
	closemessage
	return

OrangeIslands_Trovita_Gym_EventScript_GateAlreadyOpen::
	releaseall
	end

OrangeIslands_Trovita_Gym_EventScript_QuizCancelled::
	closemessage
	releaseall
	end

OrangeIslands_Trovita_Gym_EventScript_Yuzu::
	trainerbattle_single TRAINER_COOLTRAINER_TROVITA_YUZU, OrangeIslands_Trovita_Gym_Text_YuzuIntro, OrangeIslands_Trovita_Gym_Text_YuzuDefeat
	msgbox OrangeIslands_Trovita_Gym_Text_YuzuPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Trovita_Gym_EventScript_Cedro::
	trainerbattle_single TRAINER_COOLTRAINER_TROVITA_CEDRO, OrangeIslands_Trovita_Gym_Text_CedroIntro, OrangeIslands_Trovita_Gym_Text_CedroDefeat
	msgbox OrangeIslands_Trovita_Gym_Text_CedroPostBattle, MSGBOX_AUTOCLOSE
	end

@ The player brings three POKéMON that share no type; Rudy builds a team to counter them.
OrangeIslands_Trovita_Gym_EventScript_Rudy::
	lock
	faceplayer
	goto_if_defeated TRAINER_LEADER_RUDY, OrangeIslands_Trovita_Gym_EventScript_RudyAfter
	msgbox OrangeIslands_Trovita_Gym_Text_RudyChallenge, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, OrangeIslands_Trovita_Gym_EventScript_RudyDeclined
	msgbox OrangeIslands_Trovita_Gym_Text_ChooseThree
	closemessage
	callnative OrangeGym_ChooseDistinctTypes
	waitstate
	goto_if_eq VAR_RESULT, FALSE, OrangeIslands_Trovita_Gym_EventScript_RudyDeclined
	msgbox OrangeIslands_Trovita_Gym_Text_RudyIntro
	trainerbattle_no_intro TRAINER_LEADER_RUDY, OrangeIslands_Trovita_Gym_Text_RudyDefeat
	setflag FLAG_BADGE_SEA_RUBY
	message OrangeIslands_Trovita_Gym_Text_ReceivedBadge
	waitmessage
	playfanfare MUS_OBTAIN_BADGE
	waitfanfare
	msgbox OrangeIslands_Trovita_Gym_Text_ExplainBadge
	goto OrangeIslands_Trovita_Gym_EventScript_GiveTM10
	end

OrangeIslands_Trovita_Gym_EventScript_RudyDeclined::
	msgbox OrangeIslands_Trovita_Gym_Text_RudyDeclined
	release
	end

OrangeIslands_Trovita_Gym_EventScript_RudyAfter::
	goto_if_unset FLAG_GOT_TM10_FROM_RUDY, OrangeIslands_Trovita_Gym_EventScript_GiveTM10
	msgbox OrangeIslands_Trovita_Gym_Text_RudyPostBattle
	release
	end

OrangeIslands_Trovita_Gym_EventScript_GiveTM10::
	checkitemspace ITEM_TM10
	goto_if_eq VAR_RESULT, FALSE, OrangeIslands_Trovita_Gym_EventScript_NoRoom
	giveitem_msg OrangeIslands_Trovita_Gym_Text_ReceivedTM10, ITEM_TM10
	setflag FLAG_GOT_TM10_FROM_RUDY
	msgbox OrangeIslands_Trovita_Gym_Text_ExplainTM10
	release
	end

OrangeIslands_Trovita_Gym_EventScript_NoRoom::
	msgbox OrangeIslands_Trovita_Gym_Text_NoRoom
	release
	end

OrangeIslands_Trovita_Gym_EventScript_Lime::
	trainerbattle_single TRAINER_COOLTRAINER_TROVITA_LIME, OrangeIslands_Trovita_Gym_Text_LimeIntro, OrangeIslands_Trovita_Gym_Text_LimeDefeat
	msgbox OrangeIslands_Trovita_Gym_Text_LimePostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Trovita_Gym_EventScript_Rowan::
	trainerbattle_single TRAINER_COOLTRAINER_TROVITA_ROWAN, OrangeIslands_Trovita_Gym_Text_RowanIntro, OrangeIslands_Trovita_Gym_Text_RowanDefeat
	msgbox OrangeIslands_Trovita_Gym_Text_RowanPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Trovita_Gym_EventScript_GymGuy::
	lock
	faceplayer
	goto_if_set FLAG_BADGE_SEA_RUBY, OrangeIslands_Trovita_Gym_EventScript_GymGuyAfter
	msgbox OrangeIslands_Trovita_Gym_Text_GymGuyAdvice
	release
	end

OrangeIslands_Trovita_Gym_EventScript_GymGuyAfter::
	msgbox OrangeIslands_Trovita_Gym_Text_GymGuyAfter
	release
	end

OrangeIslands_Trovita_Gym_EventScript_GymStatue::
	lockall
	goto_if_set FLAG_BADGE_SEA_RUBY, OrangeIslands_Trovita_Gym_EventScript_GymStatueAfter
	msgbox OrangeIslands_Trovita_Gym_Text_GymStatue
	releaseall
	end

OrangeIslands_Trovita_Gym_EventScript_GymStatueAfter::
	msgbox OrangeIslands_Trovita_Gym_Text_GymStatueAfter
	releaseall
	end

OrangeIslands_Trovita_Gym_Text_RudyChallenge::
	.string "Welcome to TROVITA. I'm RUDY.\p"
	.string "A battle is a dance. I watch my\n"
	.string "partner, then I make my move.\p"
	.string "Bring me three POKéMON, no two\n"
	.string "sharing a type. I'll see what\l"
	.string "you've got, then pick my team.\p"
	.string "Shall we?$"

OrangeIslands_Trovita_Gym_Text_ChooseThree::
	.string "Pick three POKéMON. No two may\n"
	.string "share a type.$"

OrangeIslands_Trovita_Gym_Text_RudyDeclined::
	.string "Come back when you have three\n"
	.string "POKéMON that share no type.$"

OrangeIslands_Trovita_Gym_Text_RudyIntro::
	.string "I see… Yes, I know just who to\n"
	.string "send against each of those.\p"
	.string "Let's dance!$"

OrangeIslands_Trovita_Gym_Text_RudyDefeat::
	.string "You stayed a step ahead of me.\n"
	.string "You've earned this.$"

OrangeIslands_Trovita_Gym_Text_ReceivedBadge::
	.string "{PLAYER} received the\n"
	.string "SEA RUBY BADGE from RUDY!$"

OrangeIslands_Trovita_Gym_Text_ExplainBadge::
	.string "The SEA RUBY BADGE. The ORANGE\n"
	.string "CREW will raise your level cap.\p"
	.string "Please, take this as well.$"

OrangeIslands_Trovita_Gym_Text_ReceivedTM10::
	.string "{PLAYER} received TM10\n"
	.string "from RUDY.$"

OrangeIslands_Trovita_Gym_Text_ExplainTM10::
	.string "TM10 is HIDDEN POWER. Its type\n"
	.string "is different for every POKéMON.\p"
	.string "Good for surprising someone who\n"
	.string "thinks they've read you!$"

OrangeIslands_Trovita_Gym_Text_NoRoom::
	.string "Your BAG is full. Come back\n"
	.string "once you've made room.$"

OrangeIslands_Trovita_Gym_Text_RudyPostBattle::
	.string "The last gym is on KUMQUAT\n"
	.string "ISLAND. Good luck!$"

OrangeIslands_Trovita_Gym_Text_LimeIntro::
	.string "FIRE, WATER, GRASS! Can you\n"
	.string "handle all three?$"

OrangeIslands_Trovita_Gym_Text_LimeDefeat::
	.string "My triad fell apart!$"

OrangeIslands_Trovita_Gym_Text_LimePostBattle::
	.string "RUDY only lets you use three\n"
	.string "POKéMON. Pick them well!$"

OrangeIslands_Trovita_Gym_Text_RowanIntro::
	.string "Show me your balance!$"

OrangeIslands_Trovita_Gym_Text_RowanDefeat::
	.string "Out of balance...$"

OrangeIslands_Trovita_Gym_Text_RowanPostBattle::
	.string "RUDY picks his team after he\n"
	.string "sees yours. Watch out for moves\l"
	.string "that cover his weak spots!$"

OrangeIslands_Trovita_Gym_Text_GymGuyAdvice::
	.string "Yo! Six hedges between you and\n"
	.string "RUDY, and a panel on every one.\p"
	.string "They don't just ask about types.\n"
	.string "Which move leaves a burn. Which\l"
	.string "ability makes a move pointless.\l"
	.string "Which BERRY undoes it.\p"
	.string "And each panel knows two\n"
	.string "questions. It won't always pick\l"
	.string "the one you got last time!\p"
	.string "Miss one and the student in that\n"
	.string "room takes it up with you. Beat\l"
	.string "them and the hedge opens anyway.\p"
	.string "Then RUDY's own rule: three\n"
	.string "POKéMON, and no two can share a\l"
	.string "type.\p"
	.string "Here's the catch. RUDY looks at\n"
	.string "your three and picks a POKéMON\l"
	.string "to beat each one!\p"
	.string "So bring three that cover each\n"
	.string "other. If he counters one, the\l"
	.string "next should beat his counter!$"

OrangeIslands_Trovita_Gym_Text_GymGuyAfter::
	.string "RUDY read your team and you\n"
	.string "still won! Nicely done!$"

OrangeIslands_Trovita_Gym_Text_GymStatue::
	.string "TROVITA ISLAND POKéMON GYM\n"
	.string "LEADER: RUDY\p"
	.string "WINNING TRAINERS:\n"
	.string "{RIVAL}$"

OrangeIslands_Trovita_Gym_Text_GymStatueAfter::
	.string "TROVITA ISLAND POKéMON GYM\n"
	.string "LEADER: RUDY\p"
	.string "WINNING TRAINERS:\n"
	.string "{RIVAL}, {PLAYER}$"

@ ---- quiz ----
OrangeIslands_Trovita_Gym_Text_Q1A::
	.string "RUDY'S PANEL:\p"
	.string "A WATER and FLYING POKéMON.\nWhich type hits it hardest?$"

OrangeIslands_Trovita_Gym_Text_Q1B::
	.string "RUDY'S PANEL:\p"
	.string "A BUG and STEEL POKéMON.\nWhich type hits it hardest?$"

OrangeIslands_Trovita_Gym_Text_Q2A::
	.string "RUDY'S PANEL:\p"
	.string "A GRASS and DRAGON POKéMON.\nWhich type hits it hardest?$"

OrangeIslands_Trovita_Gym_Text_Q2B::
	.string "RUDY'S PANEL:\p"
	.string "A WATER and GROUND POKéMON.\nWhich type hits it hardest?$"

OrangeIslands_Trovita_Gym_Text_Q3A::
	.string "RUDY'S PANEL:\p"
	.string "A NORMAL and FLYING POKéMON.\nWhich type does nothing at all\lto it?$"

OrangeIslands_Trovita_Gym_Text_Q3B::
	.string "RUDY'S PANEL:\p"
	.string "A ROCK and GROUND POKéMON.\nWhich type does it shrug off\lhardest?$"

OrangeIslands_Trovita_Gym_Text_Q4A::
	.string "RUDY'S PANEL:\p"
	.string "Which of these can leave a\nPOKéMON PARALYZED?$"

OrangeIslands_Trovita_Gym_Text_Q4B::
	.string "RUDY'S PANEL:\p"
	.string "Which of these can leave a\nPOKéMON BURNED?$"

OrangeIslands_Trovita_Gym_Text_Q5A::
	.string "RUDY'S PANEL:\p"
	.string "A POKéMON with LIMBER.\nWhich move is wasted on it?$"

OrangeIslands_Trovita_Gym_Text_Q5B::
	.string "RUDY'S PANEL:\p"
	.string "A POKéMON with WATER VEIL.\nWhich move is wasted on it?$"

OrangeIslands_Trovita_Gym_Text_Q6A::
	.string "RUDY'S PANEL:\p"
	.string "Which ability makes THUNDER WAVE\npointless?$"

OrangeIslands_Trovita_Gym_Text_Q6B::
	.string "RUDY'S PANEL:\p"
	.string "A POKéMON is PARALYZED.\nWhich BERRY would cure it?$"

OrangeIslands_Trovita_Gym_Text_Correct::
	.string "Right! The hedge is opening!$"

OrangeIslands_Trovita_Gym_Text_Wrong::
	.string "Wrong. Read the whole POKéMON,\nnot just the first thing you saw.$"

OrangeIslands_Trovita_Gym_Text_LimeStepsIn::
	.string "LIME: Wrong! And a wrong answer in\nthis gym gets you me.$"

OrangeIslands_Trovita_Gym_Text_RowanStepsIn::
	.string "ROWAN: Not even close. Let's see if\nyou battle better than you think.$"

OrangeIslands_Trovita_Gym_Text_YuzuStepsIn::
	.string "YUZU: Hah! RUDY said someone would\nguess that one.$"

OrangeIslands_Trovita_Gym_Text_YuzuIntro::
	.string "YUZU: Every POKéMON I carry has two\ntypes. Work out why that matters.$"

OrangeIslands_Trovita_Gym_Text_YuzuDefeat::
	.string "You're better in a battle than at\nthe panel. Go on.$"

OrangeIslands_Trovita_Gym_Text_YuzuPostBattle::
	.string "Two types, twice the holes.$"

OrangeIslands_Trovita_Gym_Text_CedroStepsIn::
	.string "CEDRO: That's the answer people give\nwhen they only read half of it.$"

OrangeIslands_Trovita_Gym_Text_CedroIntro::
	.string "CEDRO: Read the whole thing. Type,\nability, item. That's the lesson.$"

OrangeIslands_Trovita_Gym_Text_CedroDefeat::
	.string "Beaten fair. The hedge is yours.$"

OrangeIslands_Trovita_Gym_Text_CedroPostBattle::
	.string "I read mine wrong that time.$"

