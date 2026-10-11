// Visible wild Pokemon on routes.
//
// While you walk around a map that has wild Pokemon on land (grass, caves...), a Pokemon from
// that map's land encounter table - or from its optional bonus list in
// src/data/overworld_mons.h - now and then appears on a nearby encounter tile and wanders around.
// Walk into it or press A to battle it. These are extra: normal random encounters still happen.
//
// Spawned Pokemon:
//   - use the follower overworld sprites (one per species)
//   - have much better shiny odds (OW_MON_SHINY_ODDS). A shiny one is drawn in its shiny colours,
//     appears with a burst of sparkles and the shiny sound, and twinkles while it wanders
//   - also appear on water (from the map's surf table) - mostly while you're surfing - and in caves
//   - often know an egg move (OW_MON_EGG_MOVE_CHANCE)
//   - count as the route's encounter in Nuzlocke mode, like any wild battle
//   - aren't saved: leaving the map (or loading a save) clears them
//
// Tuning is in the block below.

#include "global.h"
#include "event_data.h"
#include "field_effect.h"
#include "constants/field_effects.h"
#include "event_object_movement.h"
#include "event_scripts.h"
#include "field_player_avatar.h"
#include "fieldmap.h"
#include "follow_me.h"
#include "metatile_behavior.h"
#include "overworld_mons.h"
#include "pokemon.h"
#include "quest_log.h"
#include "random.h"
#include "safari_zone.h"
#include "script.h"
#include "sound.h"
#include "sprite.h"
#include "wild_encounter.h"
#include "constants/event_object_movement.h"
#include "constants/event_objects.h"
#include "constants/maps.h"
#include "constants/moves.h"
#include "constants/songs.h"
#include "constants/species.h"
#include "constants/trainer_types.h"

// ---------------------------------------------------------------------------------------------
// Tuning
#define OW_MONS_ENABLED         TRUE
#define OW_MON_MAX              2    // at most this many on the map at once
#define OW_MON_STEP_INTERVAL    6    // every this many steps, roll to spawn / despawn
#define OW_MON_SPAWN_CHANCE     60   // % chance to spawn one when below the max
#define OW_MON_DESPAWN_CHANCE   20   // % chance the farthest one leaves when at the max
#define OW_MON_DESPAWN_DISTANCE 12   // tiles: anything farther than this leaves
#define OW_MON_SHINY_ODDS       4096 // 1 in this many (normal wild odds stay 1/8192)
#define OW_MON_EGG_MOVE_CHANCE  50   // % chance to know one egg move
#define OW_MON_BONUS_CHANCE     20   // % chance to pick from the map's bonus list, if it has one
#define OW_MON_WANDER_RANGE     2    // how far (tiles) a spawn wanders from where it appeared
#define OW_MON_WATER_CHANCE     25   // % of spawns on water when you're on land (if the map has a surf table)
#define OW_MON_WATER_CHANCE_SURFING 85 // % of spawns on water while you're surfing
#define OW_MON_SPARKLE_FRAMES   90   // a shiny twinkles this often (frames) while on screen
// ---------------------------------------------------------------------------------------------

#define OW_MON_LOCALID_BASE 0xF0 // local ids 0xF0..0xF0+OW_MON_MAX-1 (player 0xFF, follower 0xFE)

struct OverworldBonusMon
{
    u16 species;
    u8 minLevel;
    u8 maxLevel;
};

struct OverworldBonusList
{
    u16 map; // MAP_xxx
    const struct OverworldBonusMon *mons;
    u8 count;
};

#include "data/overworld_mons.h"

struct OverworldMonSlot
{
    bool8 active;
    u8 mapGroup;
    u8 mapNum;
    u8 level;
    u16 species;
    u16 eggMove;
    u32 personality;
    bool8 shiny;
    u8 sparkleTimer;
    u8 burstPending; // frames left to show the "just appeared" sparkle burst
};

static struct OverworldMonSlot sSlots[OW_MON_MAX];
static u8 sStepCounter;

extern const u8 EventScript_OverworldMon[];

u16 GetRandomEggMoveForSpecies(u16 species); // daycare.c
bool8 GetLandEncounterForOverworldMon(u16 *species, u8 *level); // wild_encounter.c
bool8 GetWaterEncounterForOverworldMon(u16 *species, u8 *level); // wild_encounter.c

enum { HABITAT_LAND, HABITAT_WATER };

// ---------------------------------------------------------------------------------------------

bool8 OverworldMon_IsOverworldMonLocalId(u8 localId)
{
    return localId >= OW_MON_LOCALID_BASE && localId < OW_MON_LOCALID_BASE + OW_MON_MAX;
}

bool8 OverworldMon_IsOverworldMonObjectEventId(u8 objectEventId)
{
    return objectEventId < OBJECT_EVENTS_COUNT && gObjectEvents[objectEventId].active
        && !gObjectEvents[objectEventId].isPlayer
        && OverworldMon_IsOverworldMonLocalId(gObjectEvents[objectEventId].localId);
}

static bool8 IsSlotOnThisMap(u8 slot)
{
    return sSlots[slot].active
        && sSlots[slot].mapGroup == gSaveBlock1Ptr->location.mapGroup
        && sSlots[slot].mapNum == gSaveBlock1Ptr->location.mapNum;
}

static u8 FindObjectForSlot(u8 slot)
{
    u8 i;

    for (i = 0; i < OBJECT_EVENTS_COUNT; i++)
    {
        if (gObjectEvents[i].active && !gObjectEvents[i].isPlayer
         && gObjectEvents[i].localId == OW_MON_LOCALID_BASE + slot
         && gObjectEvents[i].mapGroup == gSaveBlock1Ptr->location.mapGroup
         && gObjectEvents[i].mapNum == gSaveBlock1Ptr->location.mapNum)
            return i;
    }
    return OBJECT_EVENTS_COUNT;
}

static void DespawnSlot(u8 slot)
{
    u8 objId = FindObjectForSlot(slot);

    if (objId < OBJECT_EVENTS_COUNT)
        RemoveObjectEvent(&gObjectEvents[objId]);
    sSlots[slot].active = FALSE;
}

// Drops slots whose map is gone / whose object vanished, and objects left over without a slot
// (e.g. restored from a save).
static void CleanUpSlots(void)
{
    u8 i;

    for (i = 0; i < OW_MON_MAX; i++)
    {
        if (sSlots[i].active && (!IsSlotOnThisMap(i) || FindObjectForSlot(i) == OBJECT_EVENTS_COUNT))
            sSlots[i].active = FALSE;
    }
    for (i = 0; i < OBJECT_EVENTS_COUNT; i++)
    {
        if (OverworldMon_IsOverworldMonObjectEventId(i))
        {
            u8 slot = gObjectEvents[i].localId - OW_MON_LOCALID_BASE;
            if (!IsSlotOnThisMap(slot))
                RemoveObjectEvent(&gObjectEvents[i]);
        }
    }
}

static const struct OverworldBonusList *GetBonusListForMap(void)
{
    u16 map = (gSaveBlock1Ptr->location.mapGroup << 8) | gSaveBlock1Ptr->location.mapNum;
    u16 i;

    for (i = 0; i < ARRAY_COUNT(sOverworldBonusLists); i++)
    {
        if (sOverworldBonusLists[i].map == map && sOverworldBonusLists[i].count != 0)
            return &sOverworldBonusLists[i];
    }
    return NULL;
}

static bool8 ChooseSpecies(u8 habitat, u16 *species, u8 *level)
{
    const struct OverworldBonusList *bonus = GetBonusListForMap();

    if (habitat == HABITAT_WATER)
        return GetWaterEncounterForOverworldMon(species, level);
    if (bonus != NULL && (Random() % 100) < OW_MON_BONUS_CHANCE)
    {
        const struct OverworldBonusMon *mon = &bonus->mons[Random() % bonus->count];
        u8 lo = mon->minLevel, hi = mon->maxLevel;
        if (hi < lo)
            hi = lo;
        *species = mon->species;
        *level = lo + Random() % (hi - lo + 1);
        return TRUE;
    }
    return GetLandEncounterForOverworldMon(species, level);
}

// A free tile with wild encounters of the right kind, not too close to the player and on screen.
static bool8 FindSpawnTile(u8 habitat, s16 *outX, s16 *outY)
{
    struct ObjectEvent *player = &gObjectEvents[gPlayerAvatar.objectEventId];
    u8 tries;

    for (tries = 0; tries < 16; tries++)
    {
        s16 dx = (Random() % 13) - 6;   // -6..6
        s16 dy = (Random() % 9) - 4;    // -4..4
        s16 x = player->currentCoords.x + dx;
        s16 y = player->currentCoords.y + dy;
        s16 dist = (dx < 0 ? -dx : dx) + (dy < 0 ? -dy : dy);

        if (dist < 3)
            continue;
        if (MapGridGetMetatileAttributeAt(x, y, METATILE_ATTRIBUTE_ENCOUNTER_TYPE)
            != (habitat == HABITAT_WATER ? TILE_ENCOUNTER_WATER : TILE_ENCOUNTER_LAND))
            continue;
        if (MapGridGetCollisionAt(x, y))
            continue;
        // Land spawns stay on the player's level; water spawns sit at the water's own level
        if (habitat == HABITAT_LAND && MapGridGetElevationAt(x, y) != player->currentElevation && MapGridGetElevationAt(x, y) != 0)
            continue;
        if (GetObjectEventIdByXY(x, y) != OBJECT_EVENTS_COUNT)
            continue;
        *outX = x;
        *outY = y;
        return TRUE;
    }
    return FALSE;
}

// Personality that makes the Pokemon shiny for this player's Trainer ID.
static u32 MakeShinyPersonality(void)
{
    u16 tid = gSaveBlock2Ptr->playerTrainerId[0] | (gSaveBlock2Ptr->playerTrainerId[1] << 8);
    u16 sid = gSaveBlock2Ptr->playerTrainerId[2] | (gSaveBlock2Ptr->playerTrainerId[3] << 8);
    u16 hi = Random();
    u16 lo = (tid ^ sid ^ hi) ^ (Random() & 7);

    return ((u32)hi << 16) | lo;
}

static bool8 HasPaletteRoomFor(u16 species, bool8 shiny)
{
    u8 i;

    if (IndexOfSpritePaletteTag(FOLLOWER_PAL_TAG(species)) != 0xFF
     || IndexOfSpritePaletteTag(FOLLOWER_SHINY_PAL_TAG(species)) != 0xFF)
        return TRUE; // this species' palette is already loaded
    // Only the slots after the ones reserved for map objects can be loaded at runtime (4 on maps
    // with reflective water, 10 elsewhere). A shiny is recoloured in place, so one free slot is
    // enough either way.
    for (i = gReservedSpritePaletteCount; i < 16; i++)
    {
        if (GetSpritePaletteTagByPaletteNum(i) == TAG_NONE)
            return TRUE;
    }
    return FALSE;
}

static void TrySpawn(void)
{
    struct ObjectEventTemplate template;
    u16 species;
    u8 level, slot, objId, habitat, elevation;
    s16 x, y;
    bool8 shiny;

    for (slot = 0; slot < OW_MON_MAX; slot++)
    {
        if (!sSlots[slot].active)
            break;
    }
    if (slot == OW_MON_MAX)
        return;

    // Land or water: mostly water while surfing, sometimes water from the shore
    habitat = HABITAT_LAND;
    if ((Random() % 100) < (TestPlayerAvatarFlags(PLAYER_AVATAR_FLAG_SURFING) ? OW_MON_WATER_CHANCE_SURFING : OW_MON_WATER_CHANCE))
        habitat = HABITAT_WATER;
    if (!FindSpawnTile(habitat, &x, &y))
    {
        habitat = (habitat == HABITAT_WATER) ? HABITAT_LAND : HABITAT_WATER;
        if (!FindSpawnTile(habitat, &x, &y))
            return;
    }
    if (!ChooseSpecies(habitat, &species, &level) || species == SPECIES_NONE || species >= FOLLOWER_GFX_COUNT)
        return;
    shiny = (Random() % OW_MON_SHINY_ODDS) == 0;
    if (!HasPaletteRoomFor(species, shiny))
        return;
    elevation = (habitat == HABITAT_WATER) ? MapGridGetElevationAt(x, y)
                                           : gObjectEvents[gPlayerAvatar.objectEventId].currentElevation;

    memset(&template, 0, sizeof(template));
    template.localId = OW_MON_LOCALID_BASE + slot;
    template.kind = OBJ_KIND_NORMAL;
    template.graphicsId = OBJ_EVENT_GFX_FOLLOWER(species);
    template.x = x - MAP_OFFSET;
    template.y = y - MAP_OFFSET;
    template.objUnion.normal.elevation = elevation;
    template.objUnion.normal.movementType = MOVEMENT_TYPE_WANDER_AROUND;
    template.objUnion.normal.movementRangeX = OW_MON_WANDER_RANGE;
    template.objUnion.normal.movementRangeY = OW_MON_WANDER_RANGE;
    template.objUnion.normal.trainerType = TRAINER_TYPE_NONE;
    template.script = EventScript_OverworldMon;

    objId = SpawnSpecialObjectEvent(&template);
    if (objId >= OBJECT_EVENTS_COUNT)
        return;

    sSlots[slot].active = TRUE;
    sSlots[slot].mapGroup = gSaveBlock1Ptr->location.mapGroup;
    sSlots[slot].mapNum = gSaveBlock1Ptr->location.mapNum;
    sSlots[slot].species = species;
    sSlots[slot].level = level;
    sSlots[slot].personality = shiny ? MakeShinyPersonality() : ((u32)Random() << 16 | Random());
    sSlots[slot].eggMove = (Random() % 100) < OW_MON_EGG_MOVE_CHANCE ? GetRandomEggMoveForSpecies(species) : MOVE_NONE;
    sSlots[slot].shiny = shiny;
    sSlots[slot].sparkleTimer = OW_MON_SPARKLE_FRAMES;
    if (shiny)
    {
        ObjectEventSetShinyPalette(&gObjectEvents[objId], TRUE);
        sSlots[slot].burstPending = 60; // sparkle burst as soon as a palette slot allows (see below)
        PlaySE(SE_SHINY);
    }
}

// Every frame, for each spawned object (event_object_movement.c): keeps a shiny in its shiny
// colours (e.g. after returning from a menu or another battle) and makes it twinkle.
void OverworldMon_UpdateObject(struct ObjectEvent *objectEvent)
{
    u8 slot = objectEvent->localId - OW_MON_LOCALID_BASE;

    if (slot >= OW_MON_MAX || !IsSlotOnThisMap(slot) || !sSlots[slot].shiny)
        return;
    ObjectEventSetShinyPalette(objectEvent, TRUE);

    // The sparkle sprites need their own palette, and the overworld only has 4 free palette
    // slots (weather, grass effects, the follower...), so wait for one for up to a second.
    // Sparkle burst when it appears (retried for up to a second if no palette is available)
    if (sSlots[slot].burstPending)
    {
        if (CreateShinySparklesAtObject(objectEvent - gObjectEvents, FALSE))
            sSlots[slot].burstPending = 0;
        else
            sSlots[slot].burstPending--;
    }

    // ...and a twinkle every so often while it's on screen
    if (--sSlots[slot].sparkleTimer == 0)
    {
        sSlots[slot].sparkleTimer = OW_MON_SPARKLE_FRAMES;
        if (!objectEvent->offScreen && !objectEvent->invisible)
            CreateShinySparklesAtObject(objectEvent - gObjectEvents, TRUE);
    }
}

static void TryDespawnFarthest(bool8 forceFar)
{
    struct ObjectEvent *player = &gObjectEvents[gPlayerAvatar.objectEventId];
    u8 slot, best = OW_MON_MAX;
    s16 bestDist = -1;

    for (slot = 0; slot < OW_MON_MAX; slot++)
    {
        u8 objId;
        s16 dx, dy, dist;

        if (!sSlots[slot].active)
            continue;
        objId = FindObjectForSlot(slot);
        if (objId == OBJECT_EVENTS_COUNT)
            continue;
        dx = gObjectEvents[objId].currentCoords.x - player->currentCoords.x;
        dy = gObjectEvents[objId].currentCoords.y - player->currentCoords.y;
        dist = (dx < 0 ? -dx : dx) + (dy < 0 ? -dy : dy);
        if (forceFar && dist > OW_MON_DESPAWN_DISTANCE)
            DespawnSlot(slot);
        else if (dist > bestDist)
        {
            bestDist = dist;
            best = slot;
        }
    }
    if (!forceFar && best != OW_MON_MAX && bestDist >= 4) // never vanish right next to the player
        DespawnSlot(best);
}

// Called on every player step (field_control_avatar.c)
// Called when walking onto a connected map, before the palettes are reshuffled
void OverworldMons_CleanUp(void)
{
    CleanUpSlots();
}

void OverworldMons_OnStep(void)
{
    u8 i, active;

    if (!OW_MONS_ENABLED || QL_IS_PLAYBACK_STATE || GetSafariZoneFlag())
        return;

    CleanUpSlots();
    TryDespawnFarthest(TRUE);

    if (++sStepCounter < OW_MON_STEP_INTERVAL)
        return;
    sStepCounter = 0;

    for (active = 0, i = 0; i < OW_MON_MAX; i++)
    {
        if (sSlots[i].active)
            active++;
    }
    if (active < OW_MON_MAX)
    {
        if ((Random() % 100) < OW_MON_SPAWN_CHANCE)
            TrySpawn();
    }
    else if ((Random() % 100) < OW_MON_DESPAWN_CHANCE)
    {
        TryDespawnFarthest(FALSE);
    }
}

// Walking into a spawn starts the battle (field_player_avatar.c)
bool8 OverworldMon_TryWalkInto(u8 direction)
{
    struct ObjectEvent *player = &gObjectEvents[gPlayerAvatar.objectEventId];
    s16 x = player->currentCoords.x;
    s16 y = player->currentCoords.y;
    u8 objId;

    MoveCoords(direction, &x, &y);
    objId = GetObjectEventIdByXY(x, y);
    if (!OverworldMon_IsOverworldMonObjectEventId(objId) || ArePlayerFieldControlsLocked())
        return FALSE;

    PlayerFaceDirection(direction);
    gSpecialVar_LastTalked = gObjectEvents[objId].localId;
    gSelectedObjectEvent = objId;
    ScriptContext_SetupScript(EventScript_OverworldMon);
    return TRUE;
}

// callnative from EventScript_OverworldMon: builds the wild Pokemon in gEnemyParty[0].
// VAR_RESULT = FALSE if this spawn is no longer valid, VAR_0x8004 = species (for the cry).
void OverworldMon_PrepareBattle(void)
{
    u8 slot = gSpecialVar_LastTalked - OW_MON_LOCALID_BASE;
    struct Pokemon *mon = &gEnemyParty[0];

    gSpecialVar_Result = FALSE;
    if (!OverworldMon_IsOverworldMonLocalId(gSpecialVar_LastTalked) || !IsSlotOnThisMap(slot))
        return;

    ZeroEnemyPartyMons();
    CreateMon(mon, sSlots[slot].species, sSlots[slot].level, USE_RANDOM_IVS, TRUE,
              sSlots[slot].personality, OT_ID_PLAYER_ID, 0);
    if (sSlots[slot].eggMove != MOVE_NONE)
    {
        if (GiveMoveToMon(mon, sSlots[slot].eggMove) == MON_HAS_MAX_MOVES)
            SetMonMoveSlot(mon, sSlots[slot].eggMove, Random() % MAX_MON_MOVES);
    }
    gSpecialVar_0x8004 = sSlots[slot].species;
    gSpecialVar_Result = TRUE;
}

// callnative: the spawn leaves the overworld as the battle starts
void OverworldMon_RemoveTalked(void)
{
    u8 slot = gSpecialVar_LastTalked - OW_MON_LOCALID_BASE;

    if (OverworldMon_IsOverworldMonLocalId(gSpecialVar_LastTalked))
        DespawnSlot(slot);
}
