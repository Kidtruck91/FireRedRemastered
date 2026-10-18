// Orange Islands challenge (FLAG_ORANGE_CHALLENGE). The trip has its own team and its own PC.
// In RAM there is only ever one of each: the Kanto PC + home team at home, the Orange PC
// (ORANGE_BOXES_COUNT boxes) + Orange team on the trip. The pair not in use is kept in its own
// flash sectors (src/save.c, SaveStoredPC). Flash writes only ever store what is in RAM, and the
// main save only changes when the player saves, so a reset before saving can't lose anything.
//
// Script use:
//   callnative OrangeChallenge_Begin   @ VAR_RESULT: TRUE ok, FALSE flash error (nothing changed)
//   callnative OrangeChallenge_End     @ VAR_RESULT: TRUE ok, FALSE flash error (nothing changed)
#include "global.h"
#include "event_data.h"
#include "follow_me.h"
#include "pokemon.h"
#include "pokemon_storage_system.h"
#include "battle_main.h"
#include "load_save.h"
#include "main.h"
#include "overworld.h"
#include "party_menu.h"
#include "save.h"
#include "script_pokemon_util.h"
#include "string_util.h"
#include "strings.h"
#include "constants/party_menu.h"
#include "constants/flags.h"
#include "constants/vars.h"

// The Orange PC: currentBox + its boxes (the start of struct PokemonStorage), its box names and
// wallpapers, and the Orange team while it waits at home.
#define ORANGE_PC_HEAD_BYTES (offsetof(struct PokemonStorage, boxes) + sizeof(struct BoxPokemon) * IN_BOX_COUNT * ORANGE_BOXES_COUNT)

static const u8 sText_Orange[] = _("ORANGE ");

u8 GetActiveBoxCount(void)
{
    return FlagGet(FLAG_ORANGE_CHALLENGE) ? ORANGE_BOXES_COUNT : TOTAL_BOXES_COUNT;
}

static void GetKantoParts(struct StoredPCPart *parts)
{
    parts[0].data = gPokemonStoragePtr;
    parts[0].size = sizeof(struct PokemonStorage);
    parts[1].data = gPlayerParty;
    parts[1].size = sizeof(gPlayerParty);
}

static void GetOrangeParts(struct StoredPCPart *parts)
{
    parts[0].data = gPokemonStoragePtr;
    parts[0].size = ORANGE_PC_HEAD_BYTES;
    parts[1].data = gPokemonStoragePtr->boxNames;
    parts[1].size = sizeof(gPokemonStoragePtr->boxNames[0]) * ORANGE_BOXES_COUNT;
    parts[2].data = gPokemonStoragePtr->boxWallpapers;
    parts[2].size = ORANGE_BOXES_COUNT;
    parts[3].data = gPlayerParty;
    parts[3].size = sizeof(gPlayerParty);
}

static void AfterPartyChange(void)
{
    CompactPartySlots();
    CalculatePlayerPartyCount();
    if (gPlayerPartyCount == 0)
        DestroyFollower();
    else
        UpdateFollowerPokemonGraphic();
}

void OrangeChallenge_Begin(void)
{
    struct StoredPCPart parts[4];
    u8 box, pos, i;

    gSpecialVar_Result = FALSE;
    if (FlagGet(FLAG_ORANGE_CHALLENGE))
        return;
    GetKantoParts(parts);
    if (!SaveStoredPC(STORED_PC_KANTO, parts, 2))
        return;

    // Kanto PC + home team are safe in flash: bring in the Orange PC + Orange team.
    GetOrangeParts(parts);
    if (!FlagGet(FLAG_ORANGE_PC_SAVED) || !LoadStoredPC(STORED_PC_ORANGE, parts, 4))
    {
        // First trip: a fresh Orange PC and no team (Prof. Ivy gives a starter).
        gPokemonStoragePtr->currentBox = 0;
        for (box = 0; box < ORANGE_BOXES_COUNT; box++)
        {
            u8 *name = StringCopy(gPokemonStoragePtr->boxNames[box], sText_Orange);
            ConvertIntToDecimalStringN(name, box + 1, STR_CONV_MODE_LEFT_ALIGN, 1);
            gPokemonStoragePtr->boxWallpapers[box] = WALLPAPER_BEACH;
            for (pos = 0; pos < IN_BOX_COUNT; pos++)
                ZeroBoxMonData(&gPokemonStoragePtr->boxes[box][pos]);
        }
        for (i = 0; i < PARTY_SIZE; i++)
            ZeroMonData(&gPlayerParty[i]);
    }
    for (box = ORANGE_BOXES_COUNT; box < TOTAL_BOXES_COUNT; box++)
        for (pos = 0; pos < IN_BOX_COUNT; pos++)
            ZeroBoxMonData(&gPokemonStoragePtr->boxes[box][pos]);
    if (gPokemonStoragePtr->currentBox >= ORANGE_BOXES_COUNT)
        gPokemonStoragePtr->currentBox = 0;

    FlagSet(FLAG_ORANGE_CHALLENGE);
    AfterPartyChange();
    gSpecialVar_Result = TRUE;
}

void OrangeChallenge_End(void)
{
    struct StoredPCPart parts[4];

    gSpecialVar_Result = FALSE;
    if (!FlagGet(FLAG_ORANGE_CHALLENGE))
        return;
    GetOrangeParts(parts);
    if (!SaveStoredPC(STORED_PC_ORANGE, parts, 4))
        return;
    FlagSet(FLAG_ORANGE_PC_SAVED);

    // Only now replace RAM; a damaged Kanto copy leaves everything as it was.
    GetKantoParts(parts);
    if (!LoadStoredPC(STORED_PC_KANTO, parts, 2))
        return;

    FlagClear(FLAG_ORANGE_CHALLENGE);
    AfterPartyChange();
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
//
// Rain arena (Cissy): overworld rain turns into permanent rain in battle, so
//   setweather WEATHER_RAIN / doweather   before trainerbattle_single, and
//   resetweather / doweather              in the post-battle script.
// A loss whites out to a Pokemon Center, which resets the weather by itself.
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
