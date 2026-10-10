
// ---------------------------------------------------------------------------
// Follower Pokemon movement actions (ported from FollowerEm)
// ---------------------------------------------------------------------------

// Idle: keeps the bouncing "face" animation going and re-centers side-facing sprites.
static bool8 MovementAction_FollowingPokemon_Step0(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    switch (sprite->animNum % 4)
    {
    case 0:
    case 1:
        sprite->x2 = 0;
        break;
    case 2:
        sprite->x2 = 8;
        break;
    case 3:
        sprite->x2 = -8;
        break;
    }
    if (sprite->data[7] != 1)
    {
        sprite->data[7] = 1;
        sprite->animPaused = FALSE;
        if (sprite->animNum > 3)
            sprite->animNum = sprite->animNum % 4;
        SeekSpriteAnim(sprite, ++sprite->animCmdIndex);
    }

    return ArePlayerFieldControlsLocked();
}

// Turn the follower in place. (FollowerEm respawned the whole object here; resetting the sprite's
// animation/affine state is enough and avoids the object moving to a different slot.)
static bool8 FollowingPokemon_Face(struct ObjectEvent *objectEvent, struct Sprite *sprite, u8 direction, u8 animNum, s16 x2)
{
    SetObjectEventDirection(objectEvent, direction);
    if (sprite->oam.affineMode != ST_OAM_AFFINE_OFF)
    {
        FreeSpriteOamMatrix(sprite);
        sprite->oam.affineMode = ST_OAM_AFFINE_OFF;
    }
    sprite->data[7] = 0;
    sprite->animPaused = FALSE;
    StartSpriteAnim(sprite, animNum);
    sprite->x2 = x2;
    sprite->y2 = 0;
    MoveObjectEventToMapCoords(objectEvent, objectEvent->currentCoords.x, objectEvent->currentCoords.y);
    sprite->data[2]++;
    return FALSE;
}

static bool8 MovementAction_FollowingPokemon_FaceSouth_Step0(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    return FollowingPokemon_Face(objectEvent, sprite, DIR_SOUTH, 0, 0);
}

static bool8 MovementAction_FollowingPokemon_FaceNorth_Step0(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    return FollowingPokemon_Face(objectEvent, sprite, DIR_NORTH, 1, 0);
}

static bool8 MovementAction_FollowingPokemon_FaceWest_Step0(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    return FollowingPokemon_Face(objectEvent, sprite, DIR_WEST, 2, 8);
}

static bool8 MovementAction_FollowingPokemon_FaceEast_Step0(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    return FollowingPokemon_Face(objectEvent, sprite, DIR_EAST, 3, -8);
}

// Pokemon with a separate east-facing frame don't need the mirrored affine animation
static bool8 FollowerUsesMirroredEast(struct ObjectEvent *objectEvent)
{
    return GetObjectEventGraphicsInfo(objectEvent->graphicsId)->anims != gObjectEventImageAnimTable_FollowingPokemon_UniqueEast
        && objectEvent->graphicsId != OBJ_EVENT_GFX_FOLLOWER(SPECIES_KRABBY);
}

// Shrink into the Poke Ball
static bool8 MovementAction_FollowingPokemon_Shrink_Step0(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    s16 x = objectEvent->currentCoords.x;
    s16 y = objectEvent->currentCoords.y;
    u8 spriteId;
    bool8 playerPriority;

    sprite->data[7] = 1;
    sprite->oam.affineMode = ST_OAM_AFFINE_NORMAL;
    InitSpriteAffineAnim(sprite);

    if (objectEvent->facingDirection == DIR_EAST && FollowerUsesMirroredEast(objectEvent))
        StartSpriteAffineAnim(sprite, 2);
    else
        StartSpriteAffineAnim(sprite, 0);

    if (y == gObjectEvents[gPlayerAvatar.objectEventId].currentCoords.y) // east or west of the player
    {
        SetSpritePosToOffsetMapCoords(&x, &y, 8, 8);
        playerPriority = FALSE;
    }
    else if (y < gObjectEvents[gPlayerAvatar.objectEventId].currentCoords.y) // north of the player
    {
        SetSpritePosToOffsetMapCoords(&x, &y, 8, 4);
        playerPriority = TRUE;
    }
    else // south of the player
    {
        SetSpritePosToOffsetMapCoords(&x, &y, 8, 0);
        playerPriority = FALSE;
    }

    spriteId = CreateFollowerPokeballSprite(x, y, 2, NULL);
    if (spriteId != MAX_SPRITES)
    {
        gSprites[spriteId].coordOffsetEnabled = TRUE;
        gSprites[spriteId].oam.priority = 2;
    }

    if (playerPriority)
    {
        gSprites[gPlayerAvatar.spriteId].oam.priority = 1;
        gSprites[gPlayerAvatar.spriteId].subpriority = 1;
        gObjectEvents[gPlayerAvatar.objectEventId].fixedPriority = TRUE;
    }

    sprite->oam.priority = 1;
    sprite->subpriority = 1;

    objectEvent->hasShadow = FALSE;
    sprite->data[2]++;
    return FALSE;
}

static void FollowerShrinkSetY2(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    // Adjust the height of the 64x64 sprites differently
    if (IsBigSprite(objectEvent->graphicsId))
    {
        if (sprite->data[7] < 3)
            sprite->y2 = sprite->data[7] * 2 + 1;
        else if (sprite->data[7] < 5)
            sprite->y2 = sprite->data[7] * 2 + 2;
        else if (sprite->data[7] < 6)
            sprite->y2 = sprite->data[7] * 2 + 3;
        else if (sprite->data[7] < 8)
            sprite->y2 = sprite->data[7] * 2 + 4;
        else if (sprite->data[7] < 9)
            sprite->y2 = sprite->data[7] * 2 + 5;
        else
            sprite->y2 = sprite->data[7] * 2 + 6;
    }
    else
    {
        if (sprite->data[7] >= 10)
            sprite->y2 = 10;
        else
            sprite->y2 = sprite->data[7];
    }
}

static bool8 MovementAction_FollowingPokemon_Shrink_Step1(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    struct ObjectEvent *player = &gObjectEvents[gPlayerAvatar.objectEventId];

    sprite->data[7]++;

    if (objectEvent->currentCoords.x > player->currentCoords.x) // east side
    {
        switch (objectEvent->facingDirection)
        {
        case DIR_EAST:
            if (sprite->data[7] < 8)
                sprite->x2 += sprite->data[7] - 1;
            else
                sprite->x2 += 7;
            break;
        case DIR_WEST:
            if (sprite->data[7] <= 5)
            {
                if (sprite->data[7] > 3)
                    sprite->x2 += 1 - sprite->data[7];
                else
                    sprite->x2 -= sprite->data[7];
            }
            else if (sprite->data[7] == 6)
                sprite->x2 += -5;
            else
                sprite->x2 += 1 - sprite->data[7];
            break;
        }
        FollowerShrinkSetY2(objectEvent, sprite);
    }
    else if (objectEvent->currentCoords.x < player->currentCoords.x) // west side
    {
        switch (objectEvent->facingDirection)
        {
        case DIR_EAST:
            if (sprite->data[7] <= 5)
            {
                if (sprite->data[7] > 3)
                    sprite->x2 -= 2 - sprite->data[7];
                else
                    sprite->x2 += sprite->data[7] - 1;
            }
            else if (sprite->data[7] == 6)
                sprite->x2 -= -4;
            else
                sprite->x2 -= 2 - sprite->data[7];
            break;
        case DIR_WEST:
            if (sprite->data[7] < 9)
                sprite->x2 -= sprite->data[7] - 1;
            else
                sprite->x2 -= 8;
            break;
        }
        FollowerShrinkSetY2(objectEvent, sprite);
    }
    else if (objectEvent->currentCoords.y > player->currentCoords.y) // south side
    {
        switch (objectEvent->facingDirection)
        {
        case DIR_EAST:
            if (sprite->data[7] < 7)
                sprite->x2 += sprite->data[7] - 1;
            else
                sprite->x2 += 6;
            break;
        case DIR_WEST:
            if (sprite->data[7] < 8)
                sprite->x2 -= sprite->data[7] - 1;
            else
                sprite->x2 -= 7;
            break;
        }

        if (IsBigSprite(objectEvent->graphicsId))
        {
            if (sprite->data[7] < 3)
                sprite->y2 = sprite->data[7] + 3;
            else if (sprite->data[7] < 5)
                sprite->y2 = sprite->data[7] + 4;
            else if (sprite->data[7] < 6)
                sprite->y2 = sprite->data[7] + 5;
            else if (sprite->data[7] < 8)
                sprite->y2 = sprite->data[7] + 6;
            else if (sprite->data[7] < 9)
                sprite->y2 = sprite->data[7] + 7;
            else
                sprite->y2 = sprite->data[7] + 8;
        }
        else
        {
            sprite->y2 = 1;
        }
    }
    else // north side
    {
        switch (objectEvent->facingDirection)
        {
        case DIR_EAST:
            if (sprite->data[7] < 7)
                sprite->x2 += sprite->data[7] - 1;
            else
                sprite->x2 += 6;
            break;
        case DIR_WEST:
            if (sprite->data[7] < 8)
                sprite->x2 -= sprite->data[7] - 1;
            else
                sprite->x2 -= 7;
            break;
        }

        if (IsBigSprite(objectEvent->graphicsId))
        {
            if (sprite->data[7] < 5)
                sprite->y2 = sprite->data[7] * 2 + 1;
            else if (sprite->data[7] < 8)
                sprite->y2 = sprite->data[7] * 2 + 2;
            else
                sprite->y2 = sprite->data[7] * 2 + 3;
        }
        else
        {
            if (sprite->data[7] < 6)
                sprite->y2 = sprite->data[7];
            else
                sprite->y2 = 6;
        }
    }

    if (sprite->data[7] >= 11)
    {
        FreeSpriteOamMatrix(sprite);
        sprite->oam.affineMode = ST_OAM_AFFINE_OFF;
        player->fixedPriority = FALSE;
        gPlayerAvatar.preventStep = FALSE;
        objectEvent->invisible = TRUE;
        sprite->data[2]++;
    }

    objectEvent->hasShadow = FALSE;
    return FALSE;
}

// Grow back out of the Poke Ball
static bool8 MovementAction_FollowingPokemon_Grow_Step0(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    u8 playerDirection = gObjectEvents[gPlayerAvatar.objectEventId].facingDirection;

    sprite->data[7] = 11;
    sprite->oam.affineMode = ST_OAM_AFFINE_NORMAL;
    InitSpriteAffineAnim(sprite);

    if (playerDirection == DIR_EAST && FollowerUsesMirroredEast(objectEvent))
        StartSpriteAffineAnim(sprite, 3);
    else
        StartSpriteAffineAnim(sprite, 1);

    switch (playerDirection)
    {
    case DIR_SOUTH:
        sprite->y2 = 6;
        sprite->x2 = 0;
        break;
    case DIR_NORTH:
        sprite->y2 = 1;
        sprite->x2 = 0;
        break;
    case DIR_EAST:
        sprite->y2 = 10;
        sprite->x2 += 9;
        break;
    case DIR_WEST:
        sprite->y2 = 10;
        sprite->x2 -= 9;
        break;
    }
    sprite->data[2]++;
    return FALSE;
}

static bool8 MovementAction_FollowingPokemon_Grow_Step1(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    u8 playerDirection = gObjectEvents[gPlayerAvatar.objectEventId].facingDirection;

    sprite->data[7]--;

    if (sprite->data[7] == 10)
        objectEvent->invisible = FALSE;

    switch (playerDirection)
    {
    case DIR_SOUTH:
        if (sprite->data[7] < 6)
            sprite->y2 = sprite->data[7];
        else
            sprite->y2 = 6;
        break;
    case DIR_NORTH:
        sprite->y2 = 1;
        break;
    case DIR_WEST:
        if (sprite->data[7] <= 5)
        {
            if (sprite->data[7] > 3)
                sprite->x2 += 1 - sprite->data[7];
            else
                sprite->x2 -= sprite->data[7];
        }
        else if (sprite->data[7] == 6)
            sprite->x2 += -5;
        else
            sprite->x2 += 1 - sprite->data[7];

        if (sprite->data[7] >= 10)
            sprite->y2 = 10;
        else
            sprite->y2 = sprite->data[7];
        break;
    case DIR_EAST:
        if (sprite->data[7] <= 5)
        {
            if (sprite->data[7] > 3)
                sprite->x2 -= 2 - sprite->data[7];
            else
                sprite->x2 += sprite->data[7] - 1;
        }
        else if (sprite->data[7] == 6)
            sprite->x2 -= -4;
        else
            sprite->x2 -= 2 - sprite->data[7];

        if (sprite->data[7] >= 10)
            sprite->y2 = 10;
        else
            sprite->y2 = sprite->data[7];
        break;
    }

    // Adjust the height of the 64x64 sprites differently
    if (IsBigSprite(objectEvent->graphicsId))
    {
        if (sprite->data[7] > 9)
            sprite->y2 -= (11 - sprite->data[7]) + 2;
        else if (sprite->data[7] > 7)
            sprite->y2 -= (11 - sprite->data[7]) + 3;
        else if (sprite->data[7] > 6)
            sprite->y2 -= (11 - sprite->data[7]) + 4;
        else if (sprite->data[7] > 4)
            sprite->y2 -= (11 - sprite->data[7]) + 5;
        else if (sprite->data[7] > 3)
            sprite->y2 -= (11 - sprite->data[7]) + 6;
        else
            sprite->y2 -= (11 - sprite->data[7]) + 7;
    }

    if (sprite->data[7] <= 1)
    {
        // Set y2 of the 64x64 sprites back to 0, and compensate with y.
        if (IsBigSprite(objectEvent->graphicsId))
        {
            sprite->y -= 16;
            sprite->y2 = 0;
        }

        FreeSpriteOamMatrix(sprite);
        sprite->oam.affineMode = ST_OAM_AFFINE_OFF;

        // Make sure water ripples at the feet of the follower if it appears on top of shallow water.
        if (MetatileBehavior_IsShallowFlowingWater(MapGridGetMetatileBehaviorAt(objectEvent->currentCoords.x, objectEvent->currentCoords.y)))
        {
            objectEvent->inShallowFlowingWater = TRUE;
            StartFieldEffectForObjectEvent(FLDEFF_FEET_IN_FLOWING_WATER, objectEvent);
        }
        gPlayerAvatar.preventStep = FALSE;

        // Account for warps in scripts where the follower needs to pop out of a Poke Ball afterwards
        if (gSaveBlock2Ptr->follower.createSurfBlob == 2)
        {
            sprite->data[7] = 0;
            gSaveBlock2Ptr->follower.createSurfBlob = 0;
            switch (playerDirection)
            {
            case DIR_SOUTH:
                ObjectEventForceSetHeldMovement(objectEvent, MOVEMENT_ACTION_FOLLOWING_POKEMON_FACESOUTH);
                break;
            case DIR_NORTH:
                ObjectEventForceSetHeldMovement(objectEvent, MOVEMENT_ACTION_FOLLOWING_POKEMON_FACENORTH);
                break;
            case DIR_WEST:
                ObjectEventForceSetHeldMovement(objectEvent, MOVEMENT_ACTION_FOLLOWING_POKEMON_FACEWEST);
                break;
            case DIR_EAST:
                ObjectEventForceSetHeldMovement(objectEvent, MOVEMENT_ACTION_FOLLOWING_POKEMON_FACEEAST);
                break;
            }
        }

        sprite->data[2]++;
    }

    return FALSE;
}

// Turns a follower Pokemon to face the player
bool8 ObjectEventPokemonFacePlayer(struct ObjectEvent *follower, struct ObjectEvent *player)
{
    if (follower->currentCoords.x == player->currentCoords.x)
    {
        if (follower->currentCoords.y > player->currentCoords.y) // follower is south of the player
        {
            gSprites[follower->spriteId].animNum = 5;
            follower->facingDirection = DIR_NORTH;
        }
        else
        {
            gSprites[follower->spriteId].animNum = 4;
            follower->facingDirection = DIR_SOUTH;
        }
    }
    else
    {
        if (follower->currentCoords.x > player->currentCoords.x) // follower is east of the player
        {
            gSprites[follower->spriteId].animNum = 6;
            follower->facingDirection = DIR_WEST;
        }
        else
        {
            gSprites[follower->spriteId].animNum = 7;
            follower->facingDirection = DIR_EAST;
        }
    }
    return ObjectEventSetHeldMovement(follower, MOVEMENT_ACTION_FOLLOWING_POKEMON);
}

bool8 PlayerIsUnderWaterfall(struct ObjectEvent *objectEvent)
{
    s16 x = objectEvent->currentCoords.x;
    s16 y = objectEvent->currentCoords.y;

    MoveCoordsInDirection(DIR_NORTH, &x, &y, 0, 1);
    return MetatileBehavior_IsWaterfall(MapGridGetMetatileBehaviorAt(x, y));
}
