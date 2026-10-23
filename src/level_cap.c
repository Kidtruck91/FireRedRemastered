#include "global.h"
#include "event_data.h"
#include "level_cap.h"
#include "pokemon.h"
#include "constants/vars.h"
#include "constants/flags.h"
#include "constants/opponents.h"

// Kanto level caps only apply with the LEVEL CAP option on (Oak's intro turns it on
// together with Nuzlocke mode; either can be toggled on its own in Options). The cap is worked out from the badges
// every time it is asked for, so it goes up automatically after each gym.
// To change the caps, edit the table in CalculateLevelCap.

// Orange Islands challenge (after the Sevii Islands): caps always apply, whatever the
// LEVEL CAP option says. Cap by how many Orange leaders are beaten (any order).
static const u8 sOrangeLevelCaps[] = {20, 28, 38, 44, 50};
static const u16 sOrangeLeaders[] = {
    TRAINER_LEADER_CISSY, TRAINER_LEADER_DANNY, TRAINER_LEADER_RUDY, TRAINER_LEADER_LUANA,
};

static u8 CalculateLevelCap(void)
{
    if (FlagGet(FLAG_ORANGE_CHALLENGE) && !FlagGet(TRAINER_FLAGS_START + TRAINER_CHAMPION_DRAKE))
    {
        u8 i, beaten = 0;
        for (i = 0; i < ARRAY_COUNT(sOrangeLeaders); i++)
            if (FlagGet(TRAINER_FLAGS_START + sOrangeLeaders[i]))
                beaten++;
        return sOrangeLevelCaps[beaten];
    }

    if (!FlagGet(FLAG_LEVEL_CAP))
        return MAX_LEVEL; // no cap

    if (FlagGet(FLAG_DEFEATED_CHAMP))
        return MAX_LEVEL;
    if (FlagGet(FLAG_DEFEATED_LEADER_GIOVANNI))
        return 63;
    if (FlagGet(FLAG_DEFEATED_BLAINE))
        return 50;
    if (FlagGet(FLAG_DEFEATED_SABRINA))
        return 47;
    if (FlagGet(FLAG_DEFEATED_KOGA))
        return 43;
    if (FlagGet(FLAG_DEFEATED_ERIKA))
        return 43;
    if (FlagGet(FLAG_DEFEATED_LT_SURGE))
        return 29;
    if (FlagGet(FLAG_DEFEATED_MISTY))
        return 24;
    if (FlagGet(FLAG_DEFEATED_BROCK))
        return 21;
    return 14; // starting cap
}

// Stores the current level cap in VAR_TEMP_0 (used by the Pallet Town sign script)
void GetSpecialVarLevelCap(void)
{
    VarSet(VAR_TEMP_0, GetLevelCap());
}

// Kept for compatibility with existing scripts/code. The saved value is just a copy of the
// calculated cap; GetLevelCap always recalculates it.
void SetLevelCap(u8 cap)
{
    gSaveBlock1Ptr->levelCap = cap;
}

// Recalculates the level cap from the current progress and saves it
void SetLevelCapTo(void)
{
    SetLevelCap(CalculateLevelCap());
}

u8 GetLevelCap(void)
{
    u8 cap = CalculateLevelCap();

    gSaveBlock1Ptr->levelCap = cap;
    return cap;
}

// TRUE if this Pokemon is at (or above) the current level cap and can't gain levels
bool8 IsMonAtLevelCap(struct Pokemon *mon)
{
    return GetMonData(mon, MON_DATA_LEVEL, NULL) >= GetLevelCap();
}

// Limits an amount of EXP so the Pokemon can't go past the current level cap
u32 CapExpGainAtLevelCap(struct Pokemon *mon, u32 expGain)
{
    u8 cap = GetLevelCap();
    u16 species;
    u32 currentExp, capExp;

    if (cap >= MAX_LEVEL)
        return expGain;

    species = GetMonData(mon, MON_DATA_SPECIES, NULL);
    currentExp = GetMonData(mon, MON_DATA_EXP, NULL);
    capExp = gExperienceTables[gSpeciesInfo[species].growthRate][cap];

    if (currentExp >= capExp)
        return 0;
    if (currentExp + expGain > capExp)
        return capExp - currentExp;
    return expGain;
}
