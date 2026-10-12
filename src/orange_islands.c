// Orange Islands challenge: the player's party is put away in the PC for the
// trip and the PC is locked (FLAG_ORANGE_CHALLENGE), so the Orange Islands are
// played with a team caught there. On the way home the two teams swap back.
//
// The slots the home team was stored in are kept in VAR_ORANGE_STASH_0..5
// (box << 8 | position, 0xFFFF = empty).
//
// Script use:
//   callnative OrangeChallenge_Begin   @ VAR_RESULT: TRUE ok, FALSE no PC space
//   callnative OrangeChallenge_End     @ VAR_RESULT: TRUE ok, FALSE no PC space
//   callnative OrangeChallenge_CountFreeSlots  @ VAR_RESULT = free PC slots
#include "global.h"
#include "event_data.h"
#include "pokemon.h"
#include "pokemon_storage_system.h"
#include "constants/flags.h"
#include "constants/vars.h"

#define STASH_EMPTY 0xFFFF

static u16 GetStashVar(u8 i)
{
    return VarGet(VAR_ORANGE_STASH_0 + i);
}

static void SetStashVar(u8 i, u16 value)
{
    VarSet(VAR_ORANGE_STASH_0 + i, value);
}

static bool8 IsBoxSlotFree(u8 box, u8 pos)
{
    return GetBoxMonData(GetBoxedMonPtr(box, pos), MON_DATA_SPECIES, NULL) == SPECIES_NONE;
}

static u16 CountFreeBoxSlots(void)
{
    u16 count = 0;
    u8 box, pos;

    for (box = 0; box < TOTAL_BOXES_COUNT; box++)
        for (pos = 0; pos < IN_BOX_COUNT; pos++)
            if (IsBoxSlotFree(box, pos))
                count++;
    return count;
}

// Stores `mon` in a free PC slot, searching from the last box backwards so the
// stored team ends up out of the way. Returns box << 8 | pos, or STASH_EMPTY.
static u16 StoreMonAtEndOfPC(struct Pokemon *mon)
{
    s8 box;
    s8 pos;

    for (box = TOTAL_BOXES_COUNT - 1; box >= 0; box--)
    {
        for (pos = IN_BOX_COUNT - 1; pos >= 0; pos--)
        {
            if (IsBoxSlotFree(box, pos))
            {
                MonRestorePP(mon);
                CopyMon(GetBoxedMonPtr(box, pos), &mon->box, sizeof(mon->box));
                return (box << 8) | pos;
            }
        }
    }
    return STASH_EMPTY;
}

void OrangeChallenge_CountFreeSlots(void)
{
    gSpecialVar_Result = CountFreeBoxSlots();
}

void OrangeChallenge_Begin(void)
{
    u8 i;
    u8 partyCount = CalculatePlayerPartyCount();

    gSpecialVar_Result = FALSE;
    if (FlagGet(FLAG_ORANGE_CHALLENGE) || CountFreeBoxSlots() < partyCount)
        return;

    for (i = 0; i < PARTY_SIZE; i++)
    {
        if (i < partyCount)
            SetStashVar(i, StoreMonAtEndOfPC(&gPlayerParty[i]));
        else
            SetStashVar(i, STASH_EMPTY);
        ZeroMonData(&gPlayerParty[i]);
    }
    gPlayerPartyCount = 0;
    FlagSet(FLAG_ORANGE_CHALLENGE);
    gSpecialVar_Result = TRUE;
}

void OrangeChallenge_End(void)
{
    u8 i, homeCount = 0;
    u8 orangeCount = CalculatePlayerPartyCount();

    gSpecialVar_Result = FALSE;
    if (!FlagGet(FLAG_ORANGE_CHALLENGE))
        return;

    for (i = 0; i < PARTY_SIZE; i++)
        if (GetStashVar(i) != STASH_EMPTY)
            homeCount++;

    // The home team's slots free up before the Orange team goes in.
    if (CountFreeBoxSlots() + homeCount < orangeCount)
        return;

    // gEnemyParty is free outside battle: use it to hold the home team.
    for (i = 0; i < PARTY_SIZE; i++)
    {
        u16 slot = GetStashVar(i);

        ZeroMonData(&gEnemyParty[i]);
        if (slot != STASH_EMPTY)
        {
            struct BoxPokemon *boxMon = GetBoxedMonPtr(slot >> 8, slot & 0xFF);

            BoxMonToMon(boxMon, &gEnemyParty[i]);
            ZeroBoxMonData(boxMon);
        }
    }

    for (i = 0; i < orangeCount; i++)
        StoreMonAtEndOfPC(&gPlayerParty[i]);

    for (i = 0; i < PARTY_SIZE; i++)
    {
        gPlayerParty[i] = gEnemyParty[i];
        ZeroMonData(&gEnemyParty[i]);
        SetStashVar(i, STASH_EMPTY);
    }
    CompactPartySlots();
    CalculatePlayerPartyCount();
    FlagClear(FLAG_ORANGE_CHALLENGE);
    gSpecialVar_Result = TRUE;
}
