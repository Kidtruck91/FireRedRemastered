# Chunk 2: src/event_object_movement.c and include/event_object_movement.h
import re,sys
sys.path.insert(0,'_follower_port_work/patches')
def patch(path, pairs):
    s=open(path).read(); crlf='\r\n' in s; s=s.replace('\r\n','\n')
    for old,new in pairs:
        n=s.count(old)
        if n!=1: raise SystemExit(f'{path}: expected 1 match, got {n} for:\n{old}')
        s=s.replace(old,new)
    if crlf: s=s.replace('\n','\r\n')
    open(path,'w').write(s)
P='src/event_object_movement.c'
pairs=[]
# includes
pairs.append(('#include "field_player_avatar.h"\n#include "fieldmap.h"\n',
 '#include "field_player_avatar.h"\n#include "field_weather.h"\n#include "fieldmap.h"\n#include "follow_me.h"\n'))
pairs.append(('#include "constants/event_objects.h"\n', '#include "constants/event_objects.h"\n#include "constants/field_effects.h"\n#include "constants/songs.h"\n'))
# prototypes
pairs.append(('static void DoTracksGroundEffect_BikeTireTracks(struct ObjectEvent *, struct Sprite *, u8);\n',
 'static void DoTracksGroundEffect_BikeTireTracks(struct ObjectEvent *, struct Sprite *, u8);\nstatic void DoTracksGroundEffect_SlitherTracks(struct ObjectEvent *, struct Sprite *, u8);\nstatic void DoTracksGroundEffect_FootprintsBug(struct ObjectEvent *, struct Sprite *, u8);\nstatic void DoTracksGroundEffect_FootprintsSpot(struct ObjectEvent *, struct Sprite *, u8);\n'))
pairs.append(('static const struct ObjectEventTemplate *GetObjectEventTemplateByLocalIdAndMap(u8, u8, u8);\n',
 'static u8 GetObjectEventPaletteNum(const struct ObjectEventGraphicsInfo *graphicsInfo);\nstatic void FreeDynamicObjectEventPaletteIfUnused(u16 paletteTag);\n'))
pairs.append(('static void MovementType_WanderAroundSlower(struct Sprite *);\n',
 'static void MovementType_WanderAroundSlower(struct Sprite *);\nstatic void MovementType_FollowingPokemon(struct Sprite *);\n'))
pairs.append(('    [MOVEMENT_TYPE_WANDER_AROUND_SLOWER]                  = MovementType_WanderAroundSlower,\n',
 '    [MOVEMENT_TYPE_WANDER_AROUND_SLOWER]                  = MovementType_WanderAroundSlower,\n    [MOVEMENT_TYPE_FOLLOWING_POKEMON]                     = MovementType_FollowingPokemon,\n'))
pairs.append(('    [MOVEMENT_TYPE_WANDER_AROUND_SLOWER] = TRUE,\n', '    [MOVEMENT_TYPE_WANDER_AROUND_SLOWER] = TRUE,\n    [MOVEMENT_TYPE_FOLLOWING_POKEMON] = FALSE,\n'))
pairs.append(('    [MOVEMENT_TYPE_WANDER_AROUND_SLOWER] = DIR_SOUTH,\n', '    [MOVEMENT_TYPE_WANDER_AROUND_SLOWER] = DIR_SOUTH,\n    [MOVEMENT_TYPE_FOLLOWING_POKEMON] = DIR_SOUTH,\n'))
# follower data
pairs.append(('#include "data/object_events/object_event_graphics_info.h"\n',
 '#include "data/object_events/object_event_graphics_info.h"\n#include "data/object_events/follower_anims.h"\n#include "data/object_events/follower_graphics.h"\n'))
# reflection palette map: dynamic slots map to themselves
pairs.append(('    [PALSLOT_NPC_SPECIAL_REFLECTION] = PALSLOT_NPC_SPECIAL_REFLECTION\n',
 '    [PALSLOT_NPC_SPECIAL_REFLECTION] = PALSLOT_NPC_SPECIAL_REFLECTION,\n    // Dynamically allocated palettes (follower Pokemon) reflect with their own palette\n    [12] = 12,\n    [13] = 13,\n    [14] = 14,\n    [15] = 15,\n'))
# follower local id
pairs.append(('''u8 GetObjectEventIdByLocalIdAndMap(u8 localId, u8 mapNum, u8 mapGroupId)
{
    if (localId < OBJ_EVENT_ID_PLAYER)''','''u8 GetObjectEventIdByLocalIdAndMap(u8 localId, u8 mapNum, u8 mapGroupId)
{
    if (localId == OBJ_EVENT_ID_FOLLOWER)
        return GetFollowerObjectId();
    if (localId < OBJ_EVENT_ID_PLAYER)'''))
# RemoveObjectEvent public + free dynamic palettes
pairs.append(('static void RemoveObjectEvent(struct ObjectEvent *objectEvent)\n', 'void RemoveObjectEvent(struct ObjectEvent *objectEvent)\n'))
pairs.append(('''static void RemoveObjectEventInternal(struct ObjectEvent *objectEvent)
{
    struct SpriteFrameImage image;
    image.size = GetObjectEventGraphicsInfo(objectEvent->graphicsId)->size;
    gSprites[objectEvent->spriteId].images = &image;
    DestroySprite(&gSprites[objectEvent->spriteId]);
}''','''static void RemoveObjectEventInternal(struct ObjectEvent *objectEvent)
{
    struct SpriteFrameImage image;
    const struct ObjectEventGraphicsInfo *graphicsInfo = GetObjectEventGraphicsInfo(objectEvent->graphicsId);
    image.size = graphicsInfo->size;
    gSprites[objectEvent->spriteId].images = &image;
    DestroySprite(&gSprites[objectEvent->spriteId]);
    if (graphicsInfo->paletteSlot == PALSLOT_DYNAMIC)
        FreeDynamicObjectEventPaletteIfUnused(graphicsInfo->paletteTag);
}'''))
# TrySetupObjectEventSprite palette
pairs.append(('''    sprite->y += 16 + sprite->centerToCornerVecY;
    sprite->oam.paletteNum = graphicsInfo->paletteSlot;
    sprite->coordOffsetEnabled = TRUE;
    sprite->data[0] = objectEventId;
    objectEvent->spriteId = spriteId;
    objectEvent->inanimate = graphicsInfo->inanimate;''','''    sprite->y += 16 + sprite->centerToCornerVecY;
    sprite->oam.paletteNum = GetObjectEventPaletteNum(graphicsInfo);
    sprite->coordOffsetEnabled = TRUE;
    sprite->data[0] = objectEventId;
    objectEvent->spriteId = spriteId;
    objectEvent->inanimate = graphicsInfo->inanimate;'''))
pairs.append(('static u8 TrySpawnObjectEventTemplate(const struct ObjectEventTemplate *objectEventTemplate, u8 mapNum, u8 mapGroup, s16 cameraX, s16 cameraY)\n',
 'u8 TrySpawnObjectEventTemplate(const struct ObjectEventTemplate *objectEventTemplate, u8 mapNum, u8 mapGroup, s16 cameraX, s16 cameraY)\n'))
pairs.append(('int SpawnSpecialObjectEventParameterized(u8 graphicsId,', 'int SpawnSpecialObjectEventParameterized(u16 graphicsId,'))
pairs.append(('u8 CreateVirtualObject(u8 graphicsId,', 'u8 CreateVirtualObject(u16 graphicsId,'))
pairs.append(('''        sprite->y += sprite->centerToCornerVecY;
        sprite->oam.paletteNum = graphicsInfo->paletteSlot;
        sprite->coordOffsetEnabled = TRUE;
        sprite->sVirtualObjId = virtualObjId;''','''        sprite->y += sprite->centerToCornerVecY;
        sprite->oam.paletteNum = GetObjectEventPaletteNum(graphicsInfo);
        sprite->coordOffsetEnabled = TRUE;
        sprite->sVirtualObjId = virtualObjId;'''))
pairs.append(('u8 CreateFameCheckerObject(u8 graphicsId,', 'u8 CreateFameCheckerObject(u16 graphicsId,'))
pairs.append(('''        sprite->oam.paletteNum = graphicsInfo->paletteSlot;
        sprite->data[0] = localId;''','''        sprite->oam.paletteNum = GetObjectEventPaletteNum(graphicsInfo);
        sprite->data[0] = localId;'''))
# outside view: never despawn follower
pairs.append(('''            if (objectEvent->active && !objectEvent->isPlayer)
                RemoveObjectEventIfOutsideView(objectEvent);''','''            if (objectEvent->active && !objectEvent->isPlayer && i != GetFollowerObjectId())
                RemoveObjectEventIfOutsideView(objectEvent);'''))
# SpawnObjectEventOnReturnToField
pairs.append(('''    if (graphicsInfo->paletteSlot >= PALSLOT_NPC_SPECIAL)
        LoadSpecialObjectReflectionPalette(graphicsInfo->paletteTag, graphicsInfo->paletteSlot);''','''    if (graphicsInfo->paletteSlot >= PALSLOT_NPC_SPECIAL && graphicsInfo->paletteSlot != PALSLOT_DYNAMIC)
        LoadSpecialObjectReflectionPalette(graphicsInfo->paletteTag, graphicsInfo->paletteSlot);'''))
pairs.append(('''            SetSubspriteTables(sprite, subspriteTables);

        sprite->oam.paletteNum = graphicsInfo->paletteSlot;
        sprite->coordOffsetEnabled = TRUE;''','''            SetSubspriteTables(sprite, subspriteTables);

        sprite->oam.paletteNum = GetObjectEventPaletteNum(graphicsInfo);
        sprite->coordOffsetEnabled = TRUE;'''))
# ObjectEventSetGraphicsId
pairs.append(('''void ObjectEventSetGraphicsId(struct ObjectEvent *objectEvent, u8 graphicsId)
{
    const struct ObjectEventGraphicsInfo *graphicsInfo;
    struct Sprite *sprite;
    u8 var;
    u8 var3;

    graphicsInfo = GetObjectEventGraphicsInfo(graphicsId);''','''void ObjectEventSetGraphicsId(struct ObjectEvent *objectEvent, u16 graphicsId)
{
    const struct ObjectEventGraphicsInfo *graphicsInfo;
    const struct ObjectEventGraphicsInfo *oldGraphicsInfo;
    struct Sprite *sprite;
    u8 var;
    u8 var3;

    oldGraphicsInfo = GetObjectEventGraphicsInfo(objectEvent->graphicsId);
    graphicsInfo = GetObjectEventGraphicsInfo(graphicsId);'''))
pairs.append(('''    sprite->subspriteTables = graphicsInfo->subspriteTables;
    sprite->oam.paletteNum = graphicsInfo->paletteSlot;
    if (!sprite->usingSheet)''','''    sprite->subspriteTables = graphicsInfo->subspriteTables;
    sprite->oam.paletteNum = GetObjectEventPaletteNum(graphicsInfo);
    if (oldGraphicsInfo->paletteSlot == PALSLOT_DYNAMIC && oldGraphicsInfo->paletteTag != graphicsInfo->paletteTag)
        FreeDynamicObjectEventPaletteIfUnused(oldGraphicsInfo->paletteTag);
    if (!sprite->usingSheet)'''))
pairs.append(('void ObjectEventSetGraphicsIdByLocalIdAndMap(u8 localId, u8 mapNum, u8 mapGroup, u8 graphicsId)', 'void ObjectEventSetGraphicsIdByLocalIdAndMap(u8 localId, u8 mapNum, u8 mapGroup, u16 graphicsId)'))
pairs.append(('''const struct ObjectEventGraphicsInfo *GetObjectEventGraphicsInfo(u8 graphicsId)
{
    if (graphicsId >= OBJ_EVENT_GFX_VARS)
        graphicsId = VarGetObjectEventGraphicsId(graphicsId - OBJ_EVENT_GFX_VARS);
    
    if (graphicsId >= NUM_OBJ_EVENT_GFX)
        graphicsId = OBJ_EVENT_GFX_LITTLE_BOY;
    
    return gObjectEventGraphicsInfoPointers[graphicsId];
}''','''const struct ObjectEventGraphicsInfo *GetObjectEventGraphicsInfo(u16 graphicsId)
{
    // Follower Pokemon graphics live in their own table above the 8-bit range
    if (graphicsId >= OBJ_EVENT_GFX_FOLLOWERS_START)
    {
        if (graphicsId < OBJ_EVENT_GFX_FOLLOWERS_END)
            return gFollowerGraphicsInfoPointers[graphicsId - OBJ_EVENT_GFX_FOLLOWERS_START];
        graphicsId = OBJ_EVENT_GFX_LITTLE_BOY;
    }

    if (graphicsId >= OBJ_EVENT_GFX_VARS)
        graphicsId = VarGetObjectEventGraphicsId(graphicsId - OBJ_EVENT_GFX_VARS);

    if (graphicsId >= OBJ_EVENT_GFX_FOLLOWERS_START)
    {
        if (graphicsId < OBJ_EVENT_GFX_FOLLOWERS_END)
            return gFollowerGraphicsInfoPointers[graphicsId - OBJ_EVENT_GFX_FOLLOWERS_START];
        graphicsId = OBJ_EVENT_GFX_LITTLE_BOY;
    }

    if (graphicsId >= NUM_OBJ_EVENT_GFX)
        graphicsId = OBJ_EVENT_GFX_LITTLE_BOY;

    return gObjectEventGraphicsInfoPointers[graphicsId];
}'''))
pairs.append(('''    if (objectEvent->graphicsId >= OBJ_EVENT_GFX_VARS)
    {
        objectEvent->graphicsId = VarGetObjectEventGraphicsId(objectEvent->graphicsId - OBJ_EVENT_GFX_VARS);
    }''','''    if (objectEvent->graphicsId >= OBJ_EVENT_GFX_VARS && objectEvent->graphicsId < OBJ_EVENT_GFX_FOLLOWERS_START)
    {
        objectEvent->graphicsId = VarGetObjectEventGraphicsId(objectEvent->graphicsId - OBJ_EVENT_GFX_VARS);
    }'''))
# dynamic palettes: LoadObjectEventPalette + helpers
pairs.append(('''static void LoadObjectEventPalette(u16 paletteTag)
{
    u16 i = FindObjectEventPaletteIndexByTag(paletteTag);
''','''// Follower Pokemon palettes are not in sObjectEventSpritePalettes; they are loaded on demand
// into the free (non-reserved) sprite palette slots, like field effect palettes.
static bool8 IsDynamicObjectEventPaletteTag(u16 paletteTag)
{
    return paletteTag >= FOLLOWER_PAL_TAG_BASE && paletteTag < FOLLOWER_PAL_TAG_BASE + FOLLOWER_GFX_COUNT;
}

static u8 LoadDynamicObjectEventPalette(u16 paletteTag)
{
    struct SpritePalette spritePalette;
    u8 paletteNum = IndexOfSpritePaletteTag(paletteTag);

    if (paletteNum != 0xFF)
        return paletteNum;
    if (!IsDynamicObjectEventPaletteTag(paletteTag))
        return 0xFF;

    spritePalette.data = sFollowerPalettes[paletteTag - FOLLOWER_PAL_TAG_BASE];
    spritePalette.tag = paletteTag;
    paletteNum = LoadSpritePalette(&spritePalette);
    if (paletteNum != 0xFF)
    {
        UpdateSpritePaletteWithWeather(paletteNum);
        ApplyGlobalFieldPaletteTint(paletteNum);
    }
    return paletteNum;
}

static void FreeDynamicObjectEventPaletteIfUnused(u16 paletteTag)
{
    u8 i;
    u8 paletteNum = IndexOfSpritePaletteTag(paletteTag);

    if (paletteNum == 0xFF || !IsDynamicObjectEventPaletteTag(paletteTag))
        return;
    for (i = 0; i < MAX_SPRITES; i++)
    {
        if (gSprites[i].inUse && gSprites[i].oam.paletteNum == paletteNum)
            return;
    }
    FreeSpritePaletteByTag(paletteTag);
}

// Returns the OAM palette number an object event sprite should use
static u8 GetObjectEventPaletteNum(const struct ObjectEventGraphicsInfo *graphicsInfo)
{
    if (graphicsInfo->paletteSlot == PALSLOT_DYNAMIC)
    {
        u8 paletteNum = LoadDynamicObjectEventPalette(graphicsInfo->paletteTag);
        if (paletteNum == 0xFF)
            paletteNum = PALSLOT_NPC_SPECIAL; // out of palette slots: wrong colors, but no crash
        return paletteNum;
    }
    return graphicsInfo->paletteSlot;
}

static void LoadObjectEventPalette(u16 paletteTag)
{
    u16 i;

    if (IsDynamicObjectEventPaletteTag(paletteTag))
    {
        LoadDynamicObjectEventPalette(paletteTag);
        return;
    }
    i = FindObjectEventPaletteIndexByTag(paletteTag);
'''))
pairs.append(('static const struct ObjectEventTemplate *GetObjectEventTemplateByLocalIdAndMap(u8 localId, u8 mapNum, u8 mapGroup)\n',
 'const struct ObjectEventTemplate *GetObjectEventTemplateByLocalIdAndMap(u8 localId, u8 mapNum, u8 mapGroup)\n'))
# collisions: player can walk into the follower
pairs.append(('''        curObject = &gObjectEvents[i];
        if (curObject->active && curObject != objectEvent)
        {''','''        curObject = &gObjectEvents[i];
        if (curObject->active && curObject != objectEvent && !FollowMe_IsCollisionExempt(curObject, objectEvent))
        {'''))
# FollowMe hook
pairs.append(('''    objectEvent->movementActionId = movementActionId;
    objectEvent->heldMovementActive = TRUE;
    objectEvent->heldMovementFinished = FALSE;
    gSprites[objectEvent->spriteId].data[2] = 0;
    return FALSE;
}''','''    objectEvent->movementActionId = movementActionId;
    objectEvent->heldMovementActive = TRUE;
    objectEvent->heldMovementFinished = FALSE;
    gSprites[objectEvent->spriteId].data[2] = 0;
    FollowMe(objectEvent, movementActionId, FALSE);
    return FALSE;
}'''))
# keep the follower's idle animation running between steps
pairs.append(('''bool8 UpdateMovementNormal(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    if (NpcTakeStep(sprite))
    {
        ShiftStillObjectEventCoords(objectEvent);
        objectEvent->triggerGroundEffectsOnStop = TRUE;
        sprite->animPaused = TRUE;''','''bool8 UpdateMovementNormal(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    if (NpcTakeStep(sprite))
    {
        ShiftStillObjectEventCoords(objectEvent);
        objectEvent->triggerGroundEffectsOnStop = TRUE;
        if (!IsFollowerObjectEvent(objectEvent))
            sprite->animPaused = TRUE;'''))
pairs.append(('''        ShiftStillObjectEventCoords(objectEvent);
        objectEvent->triggerGroundEffectsOnStop = TRUE;
        objectEvent->landingJump = TRUE;
        sprite->animPaused = TRUE;''','''        ShiftStillObjectEventCoords(objectEvent);
        objectEvent->triggerGroundEffectsOnStop = TRUE;
        objectEvent->landingJump = TRUE;
        if (!IsFollowerObjectEvent(objectEvent))
            sprite->animPaused = TRUE;'''))
# elevation: follower directly above the player takes the player's elevation
pairs.append(('''    if (curElevation == 15 || prevElevation == 15)
        return;

    objEvent->currentElevation = curElevation;''','''    if (curElevation == 15 || prevElevation == 15)
        return;

    if (IsFollowerObjectEvent(objEvent)
     && objEvent->currentCoords.y == gObjectEvents[gPlayerAvatar.objectEventId].currentCoords.y - 1)
        curElevation = gObjectEvents[gPlayerAvatar.objectEventId].currentElevation;

    objEvent->currentElevation = curElevation;'''))
# tracks
pairs.append(('''    DoTracksGroundEffect_None,
    DoTracksGroundEffect_Footprints,
    DoTracksGroundEffect_BikeTireTracks,
};''','''    [TRACKS_NONE]      = DoTracksGroundEffect_None,
    [TRACKS_FOOT]      = DoTracksGroundEffect_Footprints,
    [TRACKS_BIKE_TIRE] = DoTracksGroundEffect_BikeTireTracks,
    [TRACKS_SLITHER]   = DoTracksGroundEffect_SlitherTracks,
    [TRACKS_BUG]       = DoTracksGroundEffect_FootprintsBug,
    [TRACKS_SPOT]      = DoTracksGroundEffect_FootprintsSpot,
};'''))
pairs.append(('''void GroundEffect_Ripple(struct ObjectEvent *objEvent, struct Sprite *sprite)
{''','''static void DoTracksGroundEffect_FootprintsSpot(struct ObjectEvent *objEvent, struct Sprite *sprite, u8 a)
{
    gFieldEffectArguments[0] = objEvent->previousCoords.x;
    gFieldEffectArguments[1] = objEvent->previousCoords.y;
    gFieldEffectArguments[2] = 149;
    gFieldEffectArguments[3] = 2;
    gFieldEffectArguments[4] = objEvent->facingDirection;
    FieldEffectStart(FLDEFF_TRACKS_SPOT);
}

static void DoTracksGroundEffect_FootprintsBug(struct ObjectEvent *objEvent, struct Sprite *sprite, u8 a)
{
    gFieldEffectArguments[0] = objEvent->previousCoords.x;
    gFieldEffectArguments[1] = objEvent->previousCoords.y;
    gFieldEffectArguments[2] = 149;
    gFieldEffectArguments[3] = 2;
    gFieldEffectArguments[4] = objEvent->facingDirection;
    FieldEffectStart(FLDEFF_TRACKS_BUG);
}

static void DoTracksGroundEffect_SlitherTracks(struct ObjectEvent *objEvent, struct Sprite *sprite, u8 a)
{
    // Same shape transitions as the bike tire tracks
    static const u8 slitherTracks_Transitions[4][4] = {
        1, 2, 7, 8,
        1, 2, 6, 5,
        5, 8, 3, 4,
        6, 7, 3, 4,
    };

    if (objEvent->currentCoords.x != objEvent->previousCoords.x || objEvent->currentCoords.y != objEvent->previousCoords.y)
    {
        gFieldEffectArguments[0] = objEvent->previousCoords.x;
        gFieldEffectArguments[1] = objEvent->previousCoords.y;
        gFieldEffectArguments[2] = 149;
        gFieldEffectArguments[3] = 2;
        gFieldEffectArguments[4] =
            slitherTracks_Transitions[objEvent->previousMovementDirection][objEvent->facingDirection - 5];
        FieldEffectStart(FLDEFF_TRACKS_SLITHER);
    }
}

void GroundEffect_Ripple(struct ObjectEvent *objEvent, struct Sprite *sprite)
{'''))
# no ground effects for a hidden follower
pairs.append(('''    if (objEvent->localId == OBJ_EVENT_ID_CAMERA && objEvent->invisible)
        return;
''','''    if (objEvent->localId == OBJ_EVENT_ID_CAMERA && objEvent->invisible)
        return;

    if (IsFollowerObjectEvent(objEvent) && objEvent->invisible)
        return;
'''))
pairs.append(('''        const struct ObjectEventGraphicsInfo *info = GetObjectEventGraphicsInfo(direction);
        u16 baseBlock = sprite->oam.tileNum;
        sprite->oam = *info->oam;
        sprite->oam.tileNum = baseBlock;
        sprite->oam.paletteNum = info->paletteSlot;''','''        const struct ObjectEventGraphicsInfo *info = GetObjectEventGraphicsInfo(direction);
        u16 baseBlock = sprite->oam.tileNum;
        sprite->oam = *info->oam;
        sprite->oam.tileNum = baseBlock;
        sprite->oam.paletteNum = GetObjectEventPaletteNum(info);'''))
# ministep helpers for follower ledge jumps
pairs.append(('''bool8 NpcTakeStep(struct Sprite *sprite)
{''','''u16 GetMiniStepCount(u8 speed)
{
    return (u16)sStepTimes[speed];
}

void RunMiniStep(struct Sprite *sprite, u8 speed, u8 currentFrame)
{
    sNpcStepFuncTables[speed][currentFrame](sprite, sprite->tDirection);
}

bool8 NpcTakeStep(struct Sprite *sprite)
{'''))
patch(P,pairs)
print('pass1 ok')
