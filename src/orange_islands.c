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
#include "battle_main.h"
#include "load_save.h"
#include "main.h"
#include "overworld.h"
#include "party_menu.h"
#include "script_pokemon_util.h"
#include "string_util.h"
#include "strings.h"
#include "constants/party_menu.h"
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

// ---------------------------------------------------------------------------
// Gym rules
//
// Type triad (Rudy): the player brings exactly 3 Pokemon, one of each type in
// VAR_0x8005/6/7 (e.g. TYPE_FIRE, TYPE_WATER, TYPE_GRASS).
//   setvar VAR_0x8005, TYPE_FIRE ... callnative OrangeGym_ChooseTypeTriad
//   waitstate   @ VAR_RESULT TRUE = party is now those 3, FALSE = cancelled
//   trainerbattle_single ...   @ the full party comes back when the battle ends
//
// No items (Danny): setflag FLAG_NO_BATTLE_ITEMS before the gauntlet and
// clearflag it afterwards; it is also cleared if the player loses.
// ---------------------------------------------------------------------------

static void CB2_ReturnFromChooseTypeTriad(void)
{
    u8 i;
    u16 packed = 0;

    if (gSelectedOrderFromParty[0] == 0)
    {
        gSpecialVar_Result = FALSE;
    }
    else
    {
        for (i = 0; i < 3; i++)
            packed |= gSelectedOrderFromParty[i] << (i * 3);
        VarSet(VAR_ORANGE_TRIAD_ORDER, packed);
        SavePlayerParty();
        ReducePlayerPartyToThree();
        FlagSet(FLAG_ORANGE_PARTY_REDUCED);
        gSpecialVar_Result = TRUE;
    }
    SetMainCallback2(CB2_ReturnToFieldContinueScriptPlayMapMusic);
}

void OrangeGym_ChooseTypeTriad(void)
{
    u8 *str;
    u8 i;

    gTypeTriadTypes[0] = gSpecialVar_0x8005;
    gTypeTriadTypes[1] = gSpecialVar_0x8006;
    gTypeTriadTypes[2] = gSpecialVar_0x8007;

    // "Choose one FIRE, one WATER and one GRASS POKéMON."
    str = StringCopy(gTypeTriadMessage, gText_OrangeTriad_ChooseOne);
    for (i = 0; i < 3; i++)
    {
        str = StringCopy(str, gTypeNames[gTypeTriadTypes[i]]);
        if (i == 0)
            str = StringCopy(str, gText_OrangeTriad_CommaOne);
        else if (i == 1)
            str = StringCopy(str, gText_OrangeTriad_AndOne);
    }
    StringCopy(str, gText_OrangeTriad_Pokemon);

    gMain.savedCallback = CB2_ReturnFromChooseTypeTriad;
    InitChooseMonsForBattle(CHOOSE_MONS_FOR_TYPE_TRIAD);
}

// Puts the 3 battlers back in their original slots and restores the rest.
void OrangeGym_RestoreParty(void)
{
    u8 i;
    u16 packed;

    if (!FlagGet(FLAG_ORANGE_PARTY_REDUCED))
        return;
    packed = VarGet(VAR_ORANGE_TRIAD_ORDER);
    for (i = 0; i < 3; i++)
        gEnemyParty[i] = gPlayerParty[i];
    LoadPlayerParty();
    for (i = 0; i < 3; i++)
    {
        u8 slot = (packed >> (i * 3)) & 7;

        if (slot != 0)
            gPlayerParty[slot - 1] = gEnemyParty[i];
        ZeroMonData(&gEnemyParty[i]);
    }
    CalculatePlayerPartyCount();
    FlagClear(FLAG_ORANGE_PARTY_REDUCED);
}

// Called when any trainer battle ends, before a whiteout.
void OrangeGym_OnTrainerBattleEnd(bool8 playerLost)
{
    OrangeGym_RestoreParty();
    if (playerLost)
        FlagClear(FLAG_NO_BATTLE_ITEMS);
}
