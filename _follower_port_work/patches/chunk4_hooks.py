# Chunk 4: hook the follower into the rest of FireRed
import sys, subprocess
# Start from the committed versions so the script can be re-run
subprocess.run(['git','checkout','--','src/field_player_avatar.c','src/field_control_avatar.c','src/event_object_movement.c',
    'src/overworld.c','src/field_fadetransition.c','src/bike.c','src/new_game.c','src/script_movement.c','src/quest_log_objects.c'], check=True)
def patch(path, pairs):
    s=open(path).read(); crlf='\r\n' in s; s=s.replace('\r\n','\n')
    for old,new in pairs:
        n=s.count(old)
        if n!=1: sys.exit(f'{path}: expected 1 match, got {n} for:\n{old}')
        s=s.replace(old,new)
    if crlf: s=s.replace('\n','\r\n')
    open(path,'w').write(s)
def add_include(path, after):
    patch(path, [(after, after+'#include "follow_me.h"\n')])

# --- field_player_avatar.c
add_include('src/field_player_avatar.c', '#include "global.h"\n')
patch('src/field_player_avatar.c', [
("""        if (collision == COLLISION_LEDGE_JUMP)
        {
            PlayerJumpLedge(direction);
        }
        else if (collision == COLLISION_DIRECTIONAL_STAIR_WARP)""",
"""        if (collision == COLLISION_LEDGE_JUMP)
        {
            // The follower is in its ball: jump the ledge once it comes out
            if (gSaveBlock2Ptr->follower.inProgress && GetFollowerObjectId() < OBJECT_EVENTS_COUNT
             && gObjectEvents[GetFollowerObjectId()].invisible == TRUE)
                gSaveBlock2Ptr->follower.delayedState = MOVEMENT_ACTION_JUMP_2_DOWN;
            FollowerPokeballSparkle();
            PlayerJumpLedge(direction);
        }
        else if (collision == COLLISION_DIRECTIONAL_STAIR_WARP)"""),
("""        if (PlayerIsMovingOnRockStairs(direction))
            PlayerRunSlow(direction);
        else
            PlayerRun(direction);""",
"""        FollowerPokeballSparkle();
        if (PlayerIsMovingOnRockStairs(direction))
            PlayerRunSlow(direction);
        else
            PlayerRun(direction);"""),
("""        if (PlayerIsMovingOnRockStairs(direction))
            PlayerWalkSlow(direction);
        else
            PlayerWalkNormal(direction);""",
"""        FollowerPokeballSparkle();
        if (PlayerIsMovingOnRockStairs(direction))
            PlayerWalkSlow(direction);
        else
            PlayerWalkNormal(direction);"""),
("""    gPlayerAvatar.gender = gender;
    SetPlayerAvatarStateMask(PLAYER_AVATAR_FLAG_CONTROLLABLE | PLAYER_AVATAR_FLAG_ON_FOOT);
}""",
"""    gPlayerAvatar.gender = gender;
    SetPlayerAvatarStateMask(PLAYER_AVATAR_FLAG_CONTROLLABLE | PLAYER_AVATAR_FLAG_ON_FOOT);
    CreateFollowerAvatar();
}"""),
("""void StartPlayerAvatarSummonMonForFieldMoveAnim(void)
{""",
"""void StartPlayerAvatarSummonMonForFieldMoveAnim(void)
{
    FollowerIntoPokeball(); // the follower goes back into its ball when a field move is used
"""),
])

# --- field_control_avatar.c
add_include('src/field_control_avatar.c', '#include "global.h"\n')
patch('src/field_control_avatar.c', [
("""        if (forcedMove == FALSE && tileTransitionState == T_TILE_CENTER)
            input->checkStandardWildEncounter = TRUE;""",
"""        if (forcedMove == FALSE && tileTransitionState == T_TILE_CENTER && !gPlayerAvatar.preventStep)
            input->checkStandardWildEncounter = TRUE;"""),
("""        if (TryStartStepBasedScript(&position, metatileBehavior, playerDirection) == TRUE)
        {
            gFieldInputRecord.tookStep = TRUE;
            return TRUE;
        }
    }
""",
"""        if (TryStartStepBasedScript(&position, metatileBehavior, playerDirection) == TRUE)
        {
            gFieldInputRecord.tookStep = TRUE;
            return TRUE;
        }
    }
    else
    {
        // Keep the follower's idle animation going (unless it's about to pop out of its ball)
        FollowMe_IdleUpdate();
    }
"""),
("""    if (InUnionRoom() == TRUE && !ObjectEventCheckHeldMovementStatus(&gObjectEvents[objectEventId]))
        return NULL;
""",
"""    if (InUnionRoom() == TRUE && !ObjectEventCheckHeldMovementStatus(&gObjectEvents[objectEventId]))
        return NULL;

    // The follower can't be talked to while it's in its ball
    if (FollowMe_IsFollowerObjectEventId(objectEventId) && gObjectEvents[objectEventId].invisible)
        return NULL;
"""),
])

# --- event_object_movement.c: follower script, bounce, quest log guard
patch('src/event_object_movement.c', [
("""const u8 *GetObjectEventScriptPointerByObjectEventId(u8 objectEventId)
{
    return""",
"""const u8 *GetObjectEventScriptPointerByObjectEventId(u8 objectEventId)
{
    if (FollowMe_IsFollowerObjectEventId(objectEventId))
        return GetFollowerScriptPointer();
    return"""),
("""    DoGroundEffects_OnBeginStep(objectEvent, sprite);
    DoGroundEffects_OnFinishStep(objectEvent, sprite);
    UpdateObjectEventSpriteAnimPause(objectEvent, sprite);
    UpdateObjectEventVisibility(objectEvent, sprite);
    ObjectEventUpdateSubpriority(objectEvent, sprite);
}

void UpdateQuestLogObjectEventCurrentMovement""",
"""    DoGroundEffects_OnBeginStep(objectEvent, sprite);
    DoGroundEffects_OnFinishStep(objectEvent, sprite);
    UpdateObjectEventSpriteAnimPause(objectEvent, sprite);
    UpdateObjectEventVisibility(objectEvent, sprite);
    ObjectEventUpdateSubpriority(objectEvent, sprite);
    FollowMe_UpdateBounce(objectEvent, sprite);
}

void UpdateQuestLogObjectEventCurrentMovement"""),
("""static bool8 IsFollowerObjectEvent(struct ObjectEvent *objectEvent)
{
    return gSaveBlock2Ptr->follower.inProgress && objectEvent == &gObjectEvents[gSaveBlock2Ptr->follower.objId];""",
"""static bool8 IsFollowerObjectEvent(struct ObjectEvent *objectEvent)
{
    return gSaveBlock2Ptr->follower.inProgress && !QL_IS_PLAYBACK_STATE
        && objectEvent == &gObjectEvents[gSaveBlock2Ptr->follower.objId];"""),
])

# --- overworld.c
add_include('src/overworld.c', '#include "global.h"\n')
patch('src/overworld.c', [
("""static void Overworld_ResetStateAfterWhitingOut(void)
{
    ResetInitialPlayerAvatarState();""",
"""static void Overworld_ResetStateAfterWhitingOut(void)
{
    FollowMe_TryRemoveFollowerOnWhiteOut();
    ResetInitialPlayerAvatarState();"""),
("""        QuestLog_AdvancePlayhead_();
    }
    else
    {
        SetFieldVBlankCallback();
        SetMainCallback1(CB1_Overworld);
        SetMainCallback2(CB2_Overworld);
    }
}""",
"""        QuestLog_AdvancePlayhead_();
    }
    else
    {
        SetFieldVBlankCallback();
        SetMainCallback1(CB1_Overworld);
        SetMainCallback2(CB2_Overworld);
        // Place the follower after a scripted warp (setfollowerwarppos)
        FollowMe_TrySetCoordsFromWarp();
    }
}"""),
("""    OverworldBasic();
    if (fading)
        SetFieldVBlankCallback();
}""",
"""    OverworldBasic();
    if (fading)
        SetFieldVBlankCallback();
    FollowMe_FixSpriteOffset();
}"""),
("""    ResetInitialPlayerAvatarState();
    TrySpawnObjectEvents(0, 0);
    TryRunOnWarpIntoMapScript();
}""",
"""    ResetInitialPlayerAvatarState();
    TrySpawnObjectEvents(0, 0);
    TryRunOnWarpIntoMapScript();
    FollowMe_HandleSprite();
}"""),
("""    SpawnObjectEventsOnReturnToField(0, 0);
    RunOnReturnToFieldMapScript();""",
"""    SpawnObjectEventsOnReturnToField(0, 0);
    // The lead Pokemon may have changed (party menu, PC, battle...)
    UpdateFollowerPokemonGraphic();
    RunOnReturnToFieldMapScript();"""),
])

# --- field_fadetransition.c (door warps / exits)
add_include('src/field_fadetransition.c', '#include "global.h"\n')
patch('src/field_fadetransition.c', [
("""    switch (task->data[0])
    {
    case 0:
        FreezeObjectEvents();
        PlayerGetDestCoords(xp, yp);
        PlaySE(GetDoorSoundEffect(*xp, *yp - 1));""",
"""    switch (task->data[0])
    {
    case 0:
        FollowMe_DoorWarp_Start();
        FreezeObjectEvents();
        PlayerGetDestCoords(xp, yp);
        PlaySE(GetDoorSoundEffect(*xp, *yp - 1));"""),
("""            ObjectEventSetHeldMovement(&gObjectEvents[GetObjectEventIdByLocalIdAndMap(OBJ_EVENT_ID_PLAYER, 0, 0)], MOVEMENT_ACTION_WALK_NORMAL_UP);
            task->data[0] = 2;
        }
        break;
    case 2:
        if (walkrun_is_standing_still())
        {
            task->data[1] = FieldAnimateDoorClose(*xp, *yp - 1);""",
"""            ObjectEventSetHeldMovement(&gObjectEvents[GetObjectEventIdByLocalIdAndMap(OBJ_EVENT_ID_PLAYER, 0, 0)], MOVEMENT_ACTION_WALK_NORMAL_UP);
            FollowMe_DoorWarp_PlayerWalkedIn();
            task->data[0] = 2;
        }
        break;
    case 2:
        if (walkrun_is_standing_still())
        {
            if (FollowMe_DoorWarp_ShouldCloseDoor())
                task->data[1] = FieldAnimateDoorClose(*xp, *yp - 1);
            else
                task->data[1] = -1; // don't close the door on the follower"""),
("""    case 4:
        TryFadeOutOldMapMusic();
        WarpFadeOutScreen();
        PlayRainStoppingSoundEffect();
        task->data[0] = 0;
        task->func = Task_Teleport2Warp;
        break;
    case 5:""",
"""    case 4:
        FollowMe_DoorWarp_FollowerEnter();
        TryFadeOutOldMapMusic();
        WarpFadeOutScreen();
        PlayRainStoppingSoundEffect();
        task->data[0] = 0;
        task->func = Task_Teleport2Warp;
        break;
    case 5:"""),
("""    case 5:
        SetPlayerVisibility(0);
        FreezeObjectEvents();
        DoOutwardBarnDoorWipe();""",
"""    case 5:
        SetPlayerVisibility(0);
        FollowMe_DoorExit();
        FreezeObjectEvents();
        DoOutwardBarnDoorWipe();"""),
("""    switch (task->data[0])
    {
    case 0:
        SetPlayerVisibility(0);
        FreezeObjectEvents();
        PlayerGetDestCoords(x, y);
        task->data[0] = 1;
        break;
    case 1:
        if (FieldFadeTransitionBackgroundEffectIsFinished())
        {
            SetPlayerVisibility(TRUE);
            ObjectEventSetHeldMovement(&gObjectEvents[GetObjectEventIdByLocalIdAndMap(OBJ_EVENT_ID_PLAYER, 0, 0)], GetWalkNormalMovementAction(GetPlayerFacingDirection()));""",
"""    switch (task->data[0])
    {
    case 0:
        SetPlayerVisibility(0);
        FollowMe_DoorExit();
        FreezeObjectEvents();
        PlayerGetDestCoords(x, y);
        task->data[0] = 1;
        break;
    case 1:
        if (FieldFadeTransitionBackgroundEffectIsFinished())
        {
            SetPlayerVisibility(TRUE);
            ObjectEventSetHeldMovement(&gObjectEvents[GetObjectEventIdByLocalIdAndMap(OBJ_EVENT_ID_PLAYER, 0, 0)], GetWalkNormalMovementAction(GetPlayerFacingDirection()));"""),
])

# --- bike.c: the follower goes into its ball when the player gets on the bike
add_include('src/bike.c', '#include "global.h"\n')
patch('src/bike.c', [
("""    else
    {
        SetPlayerAvatarTransitionFlags(flags);
        if (Overworld_MusicCanOverrideMapMusic(MUS_CYCLING))""",
"""    else
    {
        FollowerIntoPokeball();
        SetPlayerAvatarTransitionFlags(flags);
        if (Overworld_MusicCanOverrideMapMusic(MUS_CYCLING))"""),
])

# --- new_game.c
patch('src/new_game.c', [
("""    gSaveBlock2Ptr->unkFlag2 = FALSE;
    InitPlayerTrainerId();""",
"""    gSaveBlock2Ptr->unkFlag2 = FALSE;
    memset(&gSaveBlock2Ptr->follower, 0, sizeof(gSaveBlock2Ptr->follower));
    InitPlayerTrainerId();"""),
])

# --- script_movement.c: a script moving the follower cancels its door/warp state
patch('src/script_movement.c', [
("""    if (!FuncIsActiveTask(ScriptMovement_MoveObjects))
        ScriptMovement_StartMoveObjects(50);
    return ScriptMovement_TryAddNewMovement""",
"""    if (!FuncIsActiveTask(ScriptMovement_MoveObjects))
        ScriptMovement_StartMoveObjects(50);
    if (gSaveBlock2Ptr->follower.inProgress && objEventId == gSaveBlock2Ptr->follower.objId)
    {
        gSaveBlock2Ptr->follower.comeOutDoorStairs = 0;
        gSaveBlock2Ptr->follower.warpEnd = 0;
    }
    return ScriptMovement_TryAddNewMovement"""),
])

# --- quest log: don't record the follower
add_include('src/quest_log_objects.c', '#include "global.h"\n')
patch('src/quest_log_objects.c', [
("""        questLog->unk_008[i].active = gObjectEvents[i].active;""",
"""        questLog->unk_008[i].active = gObjectEvents[i].active && !FollowMe_IsFollowerObjectEventId(i);"""),
])
print('chunk4 hooks ok')
