.equ LOCALID_MOM, 1

PalletTown_PlayersHouse_1F_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, PalletTown_PlayersHouse_1F_OnTransition
	.byte 0

@ Arm Mom's one-time cutscenes. Temp vars reset on every map load, so these only fire
@ while the condition still holds:
@   VAR_TEMP_1 = 1 -> no Running Shoes yet: stepping onto row 7 (the way to the door) triggers Mom
@   VAR_TEMP_2 = 1 -> a Nuzlocke run just ended: stepping off the stairs triggers Mom
@ (Mom's heal after a whiteout is started by field_screen_effect.c, not here.)
PalletTown_PlayersHouse_1F_OnTransition::
	clearflag FLAG_WHITEOUT
	setvar VAR_TEMP_1, 0
	setvar VAR_TEMP_2, 0
	call_if_unset FLAG_SYS_B_DASH, PalletTown_PlayersHouse_1F_EventScript_ArmRunningShoes
	call_if_set FLAG_NUZLOCKE_FIN, PalletTown_PlayersHouse_1F_EventScript_ArmEndNuzlocke
	end

PalletTown_PlayersHouse_1F_EventScript_ArmRunningShoes::
	setvar VAR_TEMP_1, 1
	return

PalletTown_PlayersHouse_1F_EventScript_ArmEndNuzlocke::
	setvar VAR_TEMP_2, 1
	return

PalletTown_PlayersHouse_1F_EventScript_Mom::
	lock
	faceplayer
	goto_if_set FLAG_BEAT_RIVAL_IN_OAKS_LAB, PalletTown_PlayersHouse_1F_EventScript_MomHeal
	checkplayergender
	call_if_eq VAR_RESULT, MALE, PalletTown_PlayersHouse_1F_EventScript_MomOakLookingForYouMale
	call_if_eq VAR_RESULT, FEMALE, PalletTown_PlayersHouse_1F_EventScript_MomOakLookingForYouFemale
	closemessage
	applymovement LOCALID_MOM, Common_Movement_FaceOriginalDirection
	waitmovement 0
	release
	end


@ Player has just stepped off the stairs onto (9,2). Mom comes to stand just below them.
PalletTown_PlayersHouse_1F_EventScript_EndNuzlocke::
	lockall
	setvar VAR_TEMP_2, 0
	applymovement LOCALID_MOM, PalletTown_PlayersHouse_1F_Movement_MomApproachStairs
	waitmovement 0
	applymovement OBJ_EVENT_ID_PLAYER, Common_Movement_FaceDown
	waitmovement 0
	textcolor NPC_TEXT_COLOR_FEMALE
	msgbox PlayersHouse_1F_Text_NuzlockeIsOver, MSGBOX_NPC
	clearflag FLAG_NUZLOCKE
	clearflag FLAG_NUZLOCKE_FIN
	applymovement LOCALID_MOM, PalletTown_PlayersHouse_1F_Movement_MomReturnFromStairs
	waitmovement 0
	releaseall
	end

PalletTown_PlayersHouse_1F_Movement_MomApproachStairs:
	walk_right
	walk_up
	face_up
	step_end

PalletTown_PlayersHouse_1F_Movement_MomReturnFromStairs:
	walk_down
	walk_left
	face_left
	step_end

@ First time heading for the door without Running Shoes: Mom walks over and hands them over.
@ Triggered from any tile on row 7 (every path to the doormat crosses it).
PalletTown_PlayersHouse_1F_EventScript_GiveRunningShoes::
	lockall
	setvar VAR_TEMP_1, 0
	getplayerxy VAR_0x8004, VAR_0x8005
	call_if_eq VAR_0x8004, 2, PalletTown_PlayersHouse_1F_EventScript_MomApproach2
	call_if_eq VAR_0x8004, 3, PalletTown_PlayersHouse_1F_EventScript_MomApproach3
	call_if_eq VAR_0x8004, 4, PalletTown_PlayersHouse_1F_EventScript_MomApproach4
	call_if_eq VAR_0x8004, 5, PalletTown_PlayersHouse_1F_EventScript_MomApproach5
	call_if_eq VAR_0x8004, 6, PalletTown_PlayersHouse_1F_EventScript_MomApproach6
	call_if_eq VAR_0x8004, 7, PalletTown_PlayersHouse_1F_EventScript_MomApproach7
	call_if_eq VAR_0x8004, 8, PalletTown_PlayersHouse_1F_EventScript_MomApproach8
	call_if_eq VAR_0x8004, 9, PalletTown_PlayersHouse_1F_EventScript_MomApproach9
	call_if_eq VAR_0x8004, 10, PalletTown_PlayersHouse_1F_EventScript_MomApproach10
	call_if_eq VAR_0x8004, 11, PalletTown_PlayersHouse_1F_EventScript_MomApproach11
	textcolor NPC_TEXT_COLOR_FEMALE
	msgbox Text_MomGiveShoes, MSGBOX_NPC
	setflag FLAG_SYS_B_DASH
	textcolor NPC_TEXT_COLOR_NEUTRAL
	msgbox Text_YouReceivedRunningShoes, MSGBOX_SIGN
	closemessage
	call_if_eq VAR_0x8004, 2, PalletTown_PlayersHouse_1F_EventScript_MomReturn2
	call_if_eq VAR_0x8004, 3, PalletTown_PlayersHouse_1F_EventScript_MomReturn3
	call_if_eq VAR_0x8004, 4, PalletTown_PlayersHouse_1F_EventScript_MomReturn4
	call_if_eq VAR_0x8004, 5, PalletTown_PlayersHouse_1F_EventScript_MomReturn5
	call_if_eq VAR_0x8004, 6, PalletTown_PlayersHouse_1F_EventScript_MomReturn6
	call_if_eq VAR_0x8004, 7, PalletTown_PlayersHouse_1F_EventScript_MomReturn7
	call_if_eq VAR_0x8004, 8, PalletTown_PlayersHouse_1F_EventScript_MomReturn8
	call_if_eq VAR_0x8004, 9, PalletTown_PlayersHouse_1F_EventScript_MomReturn9
	call_if_eq VAR_0x8004, 10, PalletTown_PlayersHouse_1F_EventScript_MomReturn10
	call_if_eq VAR_0x8004, 11, PalletTown_PlayersHouse_1F_EventScript_MomReturn11
	releaseall
	end

PalletTown_PlayersHouse_1F_EventScript_MomApproach2::
	applymovement LOCALID_MOM, PalletTown_PlayersHouse_1F_Movement_MomApproach2
	waitmovement 0
	applymovement OBJ_EVENT_ID_PLAYER, PalletTown_PlayersHouse_1F_Movement_PlayerFaceRight
	waitmovement 0
	return

PalletTown_PlayersHouse_1F_EventScript_MomReturn2::
	applymovement LOCALID_MOM, PalletTown_PlayersHouse_1F_Movement_MomReturn2
	waitmovement 0
	return

PalletTown_PlayersHouse_1F_Movement_MomApproach2:
	walk_down
	walk_down
	walk_down
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	face_left
	step_end

PalletTown_PlayersHouse_1F_Movement_MomReturn2:
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_up
	walk_up
	walk_up
	face_left
	step_end

PalletTown_PlayersHouse_1F_EventScript_MomApproach3::
	applymovement LOCALID_MOM, PalletTown_PlayersHouse_1F_Movement_MomApproach3
	waitmovement 0
	applymovement OBJ_EVENT_ID_PLAYER, PalletTown_PlayersHouse_1F_Movement_PlayerFaceRight
	waitmovement 0
	return

PalletTown_PlayersHouse_1F_EventScript_MomReturn3::
	applymovement LOCALID_MOM, PalletTown_PlayersHouse_1F_Movement_MomReturn3
	waitmovement 0
	return

PalletTown_PlayersHouse_1F_Movement_MomApproach3:
	walk_down
	walk_down
	walk_down
	walk_left
	walk_left
	walk_left
	walk_left
	face_left
	step_end

PalletTown_PlayersHouse_1F_Movement_MomReturn3:
	walk_right
	walk_right
	walk_right
	walk_right
	walk_up
	walk_up
	walk_up
	face_left
	step_end

PalletTown_PlayersHouse_1F_EventScript_MomApproach4::
	applymovement LOCALID_MOM, PalletTown_PlayersHouse_1F_Movement_MomApproach4
	waitmovement 0
	applymovement OBJ_EVENT_ID_PLAYER, PalletTown_PlayersHouse_1F_Movement_PlayerFaceRight
	waitmovement 0
	return

PalletTown_PlayersHouse_1F_EventScript_MomReturn4::
	applymovement LOCALID_MOM, PalletTown_PlayersHouse_1F_Movement_MomReturn4
	waitmovement 0
	return

PalletTown_PlayersHouse_1F_Movement_MomApproach4:
	walk_down
	walk_down
	walk_down
	walk_left
	walk_left
	walk_left
	face_left
	step_end

PalletTown_PlayersHouse_1F_Movement_MomReturn4:
	walk_right
	walk_right
	walk_right
	walk_up
	walk_up
	walk_up
	face_left
	step_end

PalletTown_PlayersHouse_1F_EventScript_MomApproach5::
	applymovement LOCALID_MOM, PalletTown_PlayersHouse_1F_Movement_MomApproach5
	waitmovement 0
	applymovement OBJ_EVENT_ID_PLAYER, PalletTown_PlayersHouse_1F_Movement_PlayerFaceRight
	waitmovement 0
	return

PalletTown_PlayersHouse_1F_EventScript_MomReturn5::
	applymovement LOCALID_MOM, PalletTown_PlayersHouse_1F_Movement_MomReturn5
	waitmovement 0
	return

PalletTown_PlayersHouse_1F_Movement_MomApproach5:
	walk_down
	walk_down
	walk_down
	walk_left
	walk_left
	face_left
	step_end

PalletTown_PlayersHouse_1F_Movement_MomReturn5:
	walk_right
	walk_right
	walk_up
	walk_up
	walk_up
	face_left
	step_end

PalletTown_PlayersHouse_1F_EventScript_MomApproach6::
	applymovement LOCALID_MOM, PalletTown_PlayersHouse_1F_Movement_MomApproach6
	waitmovement 0
	applymovement OBJ_EVENT_ID_PLAYER, PalletTown_PlayersHouse_1F_Movement_PlayerFaceRight
	waitmovement 0
	return

PalletTown_PlayersHouse_1F_EventScript_MomReturn6::
	applymovement LOCALID_MOM, PalletTown_PlayersHouse_1F_Movement_MomReturn6
	waitmovement 0
	return

PalletTown_PlayersHouse_1F_Movement_MomApproach6:
	walk_down
	walk_down
	walk_down
	walk_left
	face_left
	step_end

PalletTown_PlayersHouse_1F_Movement_MomReturn6:
	walk_right
	walk_up
	walk_up
	walk_up
	face_left
	step_end

PalletTown_PlayersHouse_1F_EventScript_MomApproach7::
	applymovement LOCALID_MOM, PalletTown_PlayersHouse_1F_Movement_MomApproach7
	waitmovement 0
	applymovement OBJ_EVENT_ID_PLAYER, PalletTown_PlayersHouse_1F_Movement_PlayerFaceRight
	waitmovement 0
	return

PalletTown_PlayersHouse_1F_EventScript_MomReturn7::
	applymovement LOCALID_MOM, PalletTown_PlayersHouse_1F_Movement_MomReturn7
	waitmovement 0
	return

PalletTown_PlayersHouse_1F_Movement_MomApproach7:
	walk_down
	walk_down
	walk_down
	face_left
	step_end

PalletTown_PlayersHouse_1F_Movement_MomReturn7:
	walk_up
	walk_up
	walk_up
	face_left
	step_end

PalletTown_PlayersHouse_1F_EventScript_MomApproach8::
	applymovement LOCALID_MOM, PalletTown_PlayersHouse_1F_Movement_MomApproach8
	waitmovement 0
	applymovement OBJ_EVENT_ID_PLAYER, PalletTown_PlayersHouse_1F_Movement_PlayerFaceRight
	waitmovement 0
	return

PalletTown_PlayersHouse_1F_EventScript_MomReturn8::
	applymovement LOCALID_MOM, PalletTown_PlayersHouse_1F_Movement_MomReturn8
	waitmovement 0
	return

PalletTown_PlayersHouse_1F_Movement_MomApproach8:
	walk_right
	walk_down
	walk_down
	walk_down
	face_left
	step_end

PalletTown_PlayersHouse_1F_Movement_MomReturn8:
	walk_up
	walk_up
	walk_up
	walk_left
	face_left
	step_end

PalletTown_PlayersHouse_1F_EventScript_MomApproach9::
	applymovement LOCALID_MOM, PalletTown_PlayersHouse_1F_Movement_MomApproach9
	waitmovement 0
	applymovement OBJ_EVENT_ID_PLAYER, PalletTown_PlayersHouse_1F_Movement_PlayerFaceLeft
	waitmovement 0
	return

PalletTown_PlayersHouse_1F_EventScript_MomReturn9::
	applymovement LOCALID_MOM, PalletTown_PlayersHouse_1F_Movement_MomReturn9
	waitmovement 0
	return

PalletTown_PlayersHouse_1F_Movement_MomApproach9:
	walk_down
	walk_down
	walk_down
	face_right
	step_end

PalletTown_PlayersHouse_1F_Movement_MomReturn9:
	walk_up
	walk_up
	walk_up
	face_left
	step_end

PalletTown_PlayersHouse_1F_EventScript_MomApproach10::
	applymovement LOCALID_MOM, PalletTown_PlayersHouse_1F_Movement_MomApproach10
	waitmovement 0
	applymovement OBJ_EVENT_ID_PLAYER, PalletTown_PlayersHouse_1F_Movement_PlayerFaceLeft
	waitmovement 0
	return

PalletTown_PlayersHouse_1F_EventScript_MomReturn10::
	applymovement LOCALID_MOM, PalletTown_PlayersHouse_1F_Movement_MomReturn10
	waitmovement 0
	return

PalletTown_PlayersHouse_1F_Movement_MomApproach10:
	walk_right
	walk_down
	walk_down
	walk_down
	face_right
	step_end

PalletTown_PlayersHouse_1F_Movement_MomReturn10:
	walk_up
	walk_up
	walk_up
	walk_left
	face_left
	step_end

PalletTown_PlayersHouse_1F_EventScript_MomApproach11::
	applymovement LOCALID_MOM, PalletTown_PlayersHouse_1F_Movement_MomApproach11
	waitmovement 0
	applymovement OBJ_EVENT_ID_PLAYER, PalletTown_PlayersHouse_1F_Movement_PlayerFaceLeft
	waitmovement 0
	return

PalletTown_PlayersHouse_1F_EventScript_MomReturn11::
	applymovement LOCALID_MOM, PalletTown_PlayersHouse_1F_Movement_MomReturn11
	waitmovement 0
	return

PalletTown_PlayersHouse_1F_Movement_MomApproach11:
	walk_right
	walk_right
	walk_down
	walk_down
	walk_down
	face_right
	step_end

PalletTown_PlayersHouse_1F_Movement_MomReturn11:
	walk_up
	walk_up
	walk_up
	walk_left
	walk_left
	face_left
	step_end

PalletTown_PlayersHouse_1F_Movement_PlayerFaceRight:
	face_right
	step_end

PalletTown_PlayersHouse_1F_Movement_PlayerFaceLeft:
	face_left
	step_end

PalletTown_PlayersHouse_1F_EventScript_MomOakLookingForYouMale::
	msgbox PalletTown_PlayersHouse_1F_Text_AllBoysLeaveOakLookingForYou
	return

PalletTown_PlayersHouse_1F_EventScript_MomOakLookingForYouFemale::
	msgbox PalletTown_PlayersHouse_1F_Text_AllGirlsLeaveOakLookingForYou
	return

PalletTown_PlayersHouse_1F_EventScript_MomHeal::
	msgbox PalletTown_PlayersHouse_1F_Text_YouShouldTakeQuickRest
	closemessage
	call EventScript_OutOfCenterPartyHeal
	msgbox PalletTown_PlayersHouse_1F_Text_LookingGreatTakeCare
	release
	end

@ Displays special text if interacted with from side or back (which are normally inaccessible)
PalletTown_PlayersHouse_1F_EventScript_TV::
	lockall
	goto_if_eq VAR_FACING, DIR_NORTH, PalletTown_PlayersHouse_1F_EventScript_TVScreen
	msgbox PalletTown_PlayersHouse_1F_Text_OopsWrongSide
	releaseall
	end

PalletTown_PlayersHouse_1F_EventScript_TVScreen::
	checkplayergender
	call_if_eq VAR_RESULT, MALE, PalletTown_PlayersHouse_1F_EventScript_TVScreenMale
	call_if_eq VAR_RESULT, FEMALE, PalletTown_PlayersHouse_1F_EventScript_TVScreenFemale
	releaseall
	end

PalletTown_PlayersHouse_1F_EventScript_TVScreenMale::
	msgbox PalletTown_PlayersHouse_1F_Text_MovieOnTVFourBoysOnRailroad
	return

PalletTown_PlayersHouse_1F_EventScript_TVScreenFemale::
	msgbox PalletTown_PlayersHouse_1F_Text_MovieOnTVGirlOnBrickRoad
	return
