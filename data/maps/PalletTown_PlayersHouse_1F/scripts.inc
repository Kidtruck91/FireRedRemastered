.equ LOCALID_MOM, 1

PalletTown_PlayersHouse_1F_MapScripts::
	map_script MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE, PalletTown_PlayersHouse_1F_OnWarp
	call_if_unset FLAG_SYS_B_DASH, PalletTown_PlayersHouse_1F_EventScript_GiveRunningShoes
	map_script MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE, PalletTown_PlayersHouse_1F_OnWarp2
	call_if_set FLAG_NUZLOCKE_FIN, PalletTown_PlayersHouse_1F_EventScript_EndNuzlocke
	
	.byte 0

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

PalletTown_PlayersHouse_1F_OnWarp2::
    end


PalletTown_PlayersHouse_1F_EventScript_EndNuzlocke::
    lockall
    applymovement LOCALID_MOM, PalletTown_PlayersHouse_1F_Movement_MomApproachPlayer2
    waitmovement LOCALID_MOM
	textcolor NPC_TEXT_COLOR_FEMALE
    msgbox PlayersHouse_1F_Text_NuzlockeIsOver, MSGBOX_NPC
    clearflag FLAG_NUZLOCKE
    applymovement LOCALID_MOM, PalletTown_PlayersHouse_1F_Movement_MomReturnHome2
    waitmovement 0
    releaseall
    end

PalletTown_PlayersHouse_1F_Movement_MomApproachPlayer2::
    walk_right
	walk_up
	walk_up
    face_right
    step_end

PalletTown_PlayersHouse_1F_Movement_MomReturnHome2::
    walk_left
    walk_down
    walk_down
    face_left
    step_end


PalletTown_PlayersHouse_1F_OnWarp::
    PalletTown_PlayersHouse_1F_OnWarp::
    checkflag FLAG_WHITEOUT
    call_if_set FLAG_WHITEOUT, EventScript_AfterWhiteOutMomHeal
    clearflag FLAG_WHITEOUT
    end


PalletTown_PlayersHouse_1F_EventScript_GiveRunningShoes::
    lockall
    applymovement LOCALID_MOM, PalletTown_PlayersHouse_1F_Movement_MomApproachPlayer
    waitmovement LOCALID_MOM
	textcolor NPC_TEXT_COLOR_FEMALE
    msgbox Text_MomGiveShoes, MSGBOX_NPC
    setflag FLAG_SYS_B_DASH
	textcolor NPC_TEXT_COLOR_NEUTRAL
    msgbox Text_YouReceivedRunningShoes, MSGBOX_SIGN
    applymovement LOCALID_MOM, PalletTown_PlayersHouse_1F_Movement_MomReturnHome
    waitmovement 0
    releaseall
    end

PalletTown_PlayersHouse_1F_Movement_MomApproachPlayer::
    walk_down
	walk_down
	walk_left
    walk_left
    walk_left
	walk_left
    walk_down
    step_end

PalletTown_PlayersHouse_1F_Movement_MomReturnHome::
    walk_up
    walk_right
    walk_right
    walk_right
	walk_right
	walk_up
	walk_up
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
