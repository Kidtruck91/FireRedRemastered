#include "global.h"
#include "battle.h"
#include "event_data.h"
#include "nuzlocke.h"
#include "overworld.h"
#include "pokemon.h"
#include "pokemon_storage_system.h"
#include "script_pokemon_util.h"
#include "constants/battle.h"
#include "constants/flags.h"
#include "constants/items.h"
#include "constants/region_map_sections.h"

/*
    Nuzlocke mode (on when FLAG_NUZLOCKE is set, from Oak's intro or the Options menu).

    Fainted = dead
      After every battle the player didn't lose, any party Pokemon at 0 HP is marked dead
      (one of the unused ribbon bits, so it stays marked in the PC and the Day Care too).
      Dead Pokemon stay in the party but can't be healed (Pokemon Center, Mom, healing items,
      PC withdraw...). The only way back is a Revive or Revival Herb (a deliberate rule bend);
      Max Revive and Sacred Ash don't work on fainted Pokemon in Nuzlocke mode.
      Pokemon that faint from poison outside battle are marked dead too.

    One encounter per area
      The first wild battle in each area (map section, e.g. a whole route or every floor of a
      cave) uses up that area. Throwing a Poke Ball in a later battle there shows a reminder
      with Yes / No, so the rule can be bypassed on purpose.

    Losing (whiting out) ends the run: the whiteout script shows the "failed" message, turns
    Nuzlocke mode off and calls Nuzlocke_EndRun, which brings everyone back.
*/

#define NUZLOCKE_DEAD_RIBBON_BIT 1 // bit in MON_DATA_UNUSED_RIBBONS

bool8 Nuzlocke_IsActive(void)
{
    return FlagGet(FLAG_NUZLOCKE);
}

// ---------------------------------------------------------------------------
// Fainted = dead
// ---------------------------------------------------------------------------

bool8 Nuzlocke_IsBoxMonDead(struct BoxPokemon *boxMon)
{
    if (!GetBoxMonData(boxMon, MON_DATA_SANITY_HAS_SPECIES, NULL))
        return FALSE;
    return (GetBoxMonData(boxMon, MON_DATA_UNUSED_RIBBONS, NULL) & NUZLOCKE_DEAD_RIBBON_BIT) != 0;
}

bool8 Nuzlocke_IsMonDead(struct Pokemon *mon)
{
    return Nuzlocke_IsBoxMonDead(&mon->box);
}

void Nuzlocke_SetMonDead(struct Pokemon *mon)
{
    u8 ribbons = GetMonData(mon, MON_DATA_UNUSED_RIBBONS, NULL) | NUZLOCKE_DEAD_RIBBON_BIT;
    u16 hp = 0;

    SetMonData(mon, MON_DATA_UNUSED_RIBBONS, &ribbons);
    SetMonData(mon, MON_DATA_HP, &hp);
}

static void ClearBoxMonDead(struct BoxPokemon *boxMon)
{
    u8 ribbons;

    if (!GetBoxMonData(boxMon, MON_DATA_SANITY_HAS_SPECIES, NULL))
        return;
    ribbons = GetBoxMonData(boxMon, MON_DATA_UNUSED_RIBBONS, NULL) & ~NUZLOCKE_DEAD_RIBBON_BIT;
    SetBoxMonData(boxMon, MON_DATA_UNUSED_RIBBONS, &ribbons);
}

void Nuzlocke_MarkFaintedPartyMonsDead(void)
{
    u8 i;

    if (!Nuzlocke_IsActive())
        return;

    for (i = 0; i < PARTY_SIZE; i++)
    {
        struct Pokemon *mon = &gPlayerParty[i];
        if (GetMonData(mon, MON_DATA_SANITY_HAS_SPECIES, NULL)
         && !GetMonData(mon, MON_DATA_IS_EGG, NULL)
         && GetMonData(mon, MON_DATA_HP, NULL) == 0)
            Nuzlocke_SetMonDead(mon);
    }
}

// The only items that can bring back a fainted / dead Pokemon in Nuzlocke mode.
// Using one is the player's deliberate choice to bend the rules. Max Revive and Sacred Ash
// are left out on purpose; add them here to allow them too.
static bool8 IsNuzlockeReviveItem(u16 item)
{
    switch (item)
    {
    case ITEM_REVIVE:
    case ITEM_REVIVAL_HERB:
        return TRUE;
    }
    return FALSE;
}

static bool8 IsAnyReviveItem(u16 item)
{
    switch (item)
    {
    case ITEM_REVIVE:
    case ITEM_MAX_REVIVE:
    case ITEM_REVIVAL_HERB:
    case ITEM_SACRED_ASH:
        return TRUE;
    }
    return FALSE;
}

// TRUE if the item must not work on this Pokemon:
// - dead Pokemon: only Revive / Revival Herb work on them
// - Nuzlocke mode: other revive items (Max Revive, Sacred Ash) never work on fainted Pokemon
bool8 Nuzlocke_ItemBlocked(struct Pokemon *mon, u16 item)
{
    if (Nuzlocke_IsMonDead(mon))
        return !IsNuzlockeReviveItem(item);
    if (Nuzlocke_IsActive() && IsAnyReviveItem(item) && !IsNuzlockeReviveItem(item)
     && GetMonData(mon, MON_DATA_HP, NULL) == 0)
        return TRUE;
    return FALSE;
}

// Called after an item was used successfully: a revived Pokemon is no longer dead
void Nuzlocke_OnItemUsed(struct Pokemon *mon, u16 item)
{
    if (IsNuzlockeReviveItem(item) && GetMonData(mon, MON_DATA_HP, NULL) != 0)
        ClearBoxMonDead(&mon->box);
}

// ---------------------------------------------------------------------------
// One encounter per area
// ---------------------------------------------------------------------------

static bool8 IsAreaUsed(u8 mapsec)
{
    return (gSaveBlock2Ptr->nuzlockeAreas[mapsec / 8] >> (mapsec % 8)) & 1;
}

static void SetAreaUsed(u8 mapsec)
{
    gSaveBlock2Ptr->nuzlockeAreas[mapsec / 8] |= 1 << (mapsec % 8);
}

void Nuzlocke_ResetAreas(void)
{
    memset(gSaveBlock2Ptr->nuzlockeAreas, 0, sizeof(gSaveBlock2Ptr->nuzlockeAreas));
}

bool8 Nuzlocke_IsCurrentAreaUsed(void)
{
    return IsAreaUsed(gMapHeader.regionMapSectionId);
}

// Shown when throwing a Poke Ball in a wild battle in an area whose encounter was already used
bool8 Nuzlocke_ShouldWarnBeforeCatch(void)
{
    if (!Nuzlocke_IsActive())
        return FALSE;
    if (gBattleTypeFlags & (BATTLE_TYPE_TRAINER | BATTLE_TYPE_SAFARI | BATTLE_TYPE_OLD_MAN_TUTORIAL | BATTLE_TYPE_POKEDUDE))
        return FALSE;
    return Nuzlocke_IsCurrentAreaUsed();
}

// Called once at the end of every battle (battle_main.c, HandleEndTurn_FinishBattle)
void Nuzlocke_OnBattleEnd(void)
{
    if (!Nuzlocke_IsActive())
        return;
    if (gBattleTypeFlags & (BATTLE_TYPE_LINK | BATTLE_TYPE_POKEDUDE | BATTLE_TYPE_OLD_MAN_TUTORIAL | BATTLE_TYPE_FIRST_BATTLE))
        return;

    // Losing ends the run instead (see Nuzlocke_EndRun)
    if (gBattleOutcome != B_OUTCOME_LOST && gBattleOutcome != B_OUTCOME_DREW)
        Nuzlocke_MarkFaintedPartyMonsDead();

    // The first wild battle in an area uses up its encounter (ghost battles without the Silph Scope don't count)
    if (!(gBattleTypeFlags & (BATTLE_TYPE_TRAINER | BATTLE_TYPE_GHOST)))
        SetAreaUsed(gMapHeader.regionMapSectionId);
}

// ---------------------------------------------------------------------------
// Losing the run
// ---------------------------------------------------------------------------

void Nuzlocke_EndRun(void)
{
    u16 box, slot;
    u8 i;

    for (i = 0; i < PARTY_SIZE; i++)
        ClearBoxMonDead(&gPlayerParty[i].box);
    for (box = 0; box < TOTAL_BOXES_COUNT; box++)
    {
        for (slot = 0; slot < IN_BOX_COUNT; slot++)
            ClearBoxMonDead(GetBoxedMonPtr(box, slot));
    }
    HealPlayerParty();
}
