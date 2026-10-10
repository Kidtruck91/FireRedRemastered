#ifndef GUARD_FOLLOW_ME_H
#define GUARD_FOLLOW_ME_H

#include "constants/follow_me.h"

// Follower Pokemon / "follow me" system, ported from the FollowerEm (pokeemerald) project.

enum
{
    FOLLOWER_SPRITE_INDEX_NORMAL,
    FOLLOWER_SPRITE_INDEX_MACH_BIKE,
    FOLLOWER_SPRITE_INDEX_ACRO_BIKE,
    FOLLOWER_SPRITE_INDEX_SURF,
    FOLLOWER_SPRITE_INDEX_UNDERWATER,
};

#define MOVEMENT_INVALID 0xFE

// Palette tags used by follower Pokemon sprites (one per species, loaded on demand).
#define FOLLOWER_PAL_TAG_BASE       0x2000
#define FOLLOWER_PAL_TAG(species)   (FOLLOWER_PAL_TAG_BASE + (species))

// follow_me.c
u8 GetFollowerObjectId(void);
u8 GetFollowerLocalId(void);
const u8 *GetFollowerScriptPointer(void);
void HideFollower(void);
void ShowFollower(void);
void FollowMe_SetIndicatorToComeOutDoor(void);
void FollowMe_SetIndicatorToRecreateSurfBlob(void);
void FollowMe_TryRemoveFollowerOnWhiteOut(void);
void FollowMe(struct ObjectEvent *npc, u8 state, bool8 ignoreScriptActive);
void FollowMe_Ledges(struct ObjectEvent *npc, struct Sprite *sprite, u16 *ledgeFramesTbl);
bool8 FollowMe_IsCollisionExempt(struct ObjectEvent *obstacle, struct ObjectEvent *collider);
void FollowMe_FollowerToWater(void);
void FollowMe_BindToSurbBlobOnReloadScreen(void);
void PrepareFollowerDismountSurf(void);
void FollowMe_HandleBike(void);
void FollowMe_HandleSprite(void);
void FollowMe_WarpSetEnd(void);
void CreateFollowerAvatar(void);
void SetFollowerSprite(u8 spriteIndex);
bool8 FollowerCanBike(void);
bool8 CheckFollowerFlag(u16 flag);
void StairsMoveFollower(void);
void EscalatorMoveFollower(u8 movementType);
void EscalatorMoveFollowerFinish(void);
void FollowerPositionFix(u8 offset);
void FollowerTrainerSightingPositionFix(void);
void FollowerIntoPlayer(void);
bool8 PlayerHasFollower(void);
bool8 IsPlayerOnFoot(void);
bool8 FollowerComingThroughDoor(void);
void SetUpFollowerSprite(u8 localId, u16 flags);
void DestroyFollower(void);
void PlayerFaceFollowerSprite(void);
void CheckPlayerHasFollower(void);
void SetFollowerWarpPosition(void);
void Task_DoDoorWarp(u8 taskId);
void FollowMe_HideFollowerForSurf(void);
const struct ObjectEventTemplate *GetFollowerObjectEventTemplate(void);
u8 CreateFollowerPokeballSprite(s16 x, s16 y, u8 subpriority, SpriteCallback callback);
void DestroyFollowerPokeballSprite(struct Sprite *sprite);
bool8 IsBigSprite(u16 graphicsId);

// Follower Pokemon (the lead party Pokemon walking behind the player)
bool8 FollowerPokemon_IsEnabled(void);
void FollowerPokemon_TryStart(void);
void FollowerPokemon_Enable(void);
void FollowerPokemon_Disable(void);
u16 GetFollowerPokemonGraphicsId(void);
u16 GetFollowerPokemonSpecies(void);
void FollowerPokemon_BufferLeadMonInfo(void);

#endif // GUARD_FOLLOW_ME_H
