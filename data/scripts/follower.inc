@ Follower Pokemon (see src/follow_me.c)

@ Talking to the follower Pokemon. What it says comes from data/text/follower_dialogue.inc
EventScript_FollowerPokemon::
	lock
	callnative FollowerPokemon_BufferLeadMonInfo
	pokemonfaceplayer
	playmoncry VAR_0x8004, CRY_MODE_NORMAL
	message gStringVar4
	waitmessage
	waitbuttonpress
	waitmoncry
	release
	end

@ Used by scripts to line the follower up with the player (FollowerEm)
FollowerPositionFixScript::
	setvar VAR_0x8000, 0
	callnative FollowerTrainerSightingPositionFix
	goto_if_eq VAR_RESULT, 0xFFFF, FollowerPositionFixScriptReturn
	call FollowerWalkCall
	setvar VAR_0x8000, 1
	callnative FollowerTrainerSightingPositionFix
FollowerPositionFixScriptReturn:
	return

FollowerIntoPlayerScript::
	setvar VAR_0x8000, 0
	callnative FollowerIntoPlayer
	goto_if_eq VAR_RESULT, 0xFFFF, FollowerPositionFixScriptReturn
	setvar VAR_0x8005, OBJ_EVENT_ID_FOLLOWER
	call FollowerWalkCall
	setvar VAR_0x8000, 1
	callnative FollowerIntoPlayer
	return

FollowerWalkCall:
	call_if_eq VAR_RESULT, 0, FollowerWalkDownCall
	call_if_eq VAR_RESULT, 1, FollowerWalkUpCall
	call_if_eq VAR_RESULT, 2, FollowerWalkLeftCall
	call_if_eq VAR_RESULT, 3, FollowerWalkRightCall
	waitmovement VAR_0x8005
	return

FollowerWalkDownCall:
	applymovement VAR_0x8005, FollowerMovement_WalkDown
	return

FollowerWalkUpCall:
	applymovement VAR_0x8005, FollowerMovement_WalkUp
	return

FollowerWalkLeftCall:
	applymovement VAR_0x8005, FollowerMovement_WalkLeft
	return

FollowerWalkRightCall:
	applymovement VAR_0x8005, FollowerMovement_WalkRight
	return

FollowerMovement_WalkDown:
	walk_down
	step_end

FollowerMovement_WalkUp:
	walk_up
	step_end

FollowerMovement_WalkLeft:
	walk_left
	step_end

FollowerMovement_WalkRight:
	walk_right
	step_end
