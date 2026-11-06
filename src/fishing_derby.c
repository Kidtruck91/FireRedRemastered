// The Mandarin Fishing Derby.
//
// A contest in the spirit of the Bug Catching Contest, but without a contest
// mode: the player fishes in their own time and brings the judge at the foot
// of the Mandarin South Beach jetty one POKeMON to be weighed.
//
// What makes it a fishing contest rather than "show me your best mon" is the
// met location. A POKeMON records the MAPSEC it was caught in, and Mandarin
// South and its beach share MAPSEC_MANDARIN_SOUTH, so an entry only counts if
// it was hooked (or surfed up) in these waters. Nothing from the box, nothing
// traded, nothing carried over from Kanto.
//
// The score is the level plus a twentieth of the base stat total. That ranks a
// GYARADOS (540 -> +27) well above the MAGIKARP it came from (200 -> +10)
// without anyone maintaining a table of species, and it keeps levelling a
// modest catch worthwhile.
//
// Script use:
//   special ChoosePartyMon   /  waitstate      @ VAR_0x8004 = slot, 255 cancelled
//   callnative FishingDerby_Score
//     VAR_RESULT  0 rejected, else the prize band 1 (consolation) to 5 (grand)
//     VAR_0x8005  the score
//     VAR_0x8006  why it was rejected: 1 empty slot, 2 an egg, 3 caught
//                 elsewhere, 4 not a WATER type
//     STR_VAR_1   its nickname

#include "global.h"
#include "event_data.h"
#include "pokemon.h"
#include "string_util.h"
#include "constants/region_map_sections.h"
#include "constants/pokemon.h"

#define DERBY_MAPSEC        MAPSEC_MANDARIN_SOUTH
#define DERBY_BAND_GRAND    95
#define DERBY_BAND_FIRST    70
#define DERBY_BAND_SECOND   50
#define DERBY_BAND_THIRD    30

enum {
    DERBY_OK,
    DERBY_NO_MON,
    DERBY_EGG,
    DERBY_WRONG_PLACE,
    DERBY_NOT_WATER,
};

static u16 BaseStatTotal(u16 species)
{
    const struct SpeciesInfo * info = &gSpeciesInfo[species];

    return info->baseHP + info->baseAttack + info->baseDefense
         + info->baseSpeed + info->baseSpAttack + info->baseSpDefense;
}

void FishingDerby_Score(void)
{
    struct Pokemon * mon;
    u16 species;
    u32 score;
    u8 nickname[POKEMON_NAME_LENGTH + 1];

    gSpecialVar_Result = 0;
    gSpecialVar_0x8005 = 0;
    gSpecialVar_0x8006 = DERBY_NO_MON;

    if (gSpecialVar_0x8004 >= CalculatePlayerPartyCount())
        return;

    mon = &gPlayerParty[gSpecialVar_0x8004];
    GetMonData(mon, MON_DATA_NICKNAME, nickname);
    StringCopy_Nickname(gStringVar1, nickname);

    if (GetMonData(mon, MON_DATA_IS_EGG))
    {
        gSpecialVar_0x8006 = DERBY_EGG;
        return;
    }
    if (GetMonData(mon, MON_DATA_MET_LOCATION) != DERBY_MAPSEC)
    {
        gSpecialVar_0x8006 = DERBY_WRONG_PLACE;
        return;
    }

    species = GetMonData(mon, MON_DATA_SPECIES);
    if (gSpeciesInfo[species].types[0] != TYPE_WATER
     && gSpeciesInfo[species].types[1] != TYPE_WATER)
    {
        gSpecialVar_0x8006 = DERBY_NOT_WATER;
        return;
    }

    score = GetMonData(mon, MON_DATA_LEVEL) + BaseStatTotal(species) / 20;
    if (score > 999)
        score = 999;

    gSpecialVar_0x8006 = DERBY_OK;
    gSpecialVar_0x8005 = score;
    if (score >= DERBY_BAND_GRAND)
        gSpecialVar_Result = 5;
    else if (score >= DERBY_BAND_FIRST)
        gSpecialVar_Result = 4;
    else if (score >= DERBY_BAND_SECOND)
        gSpecialVar_Result = 3;
    else if (score >= DERBY_BAND_THIRD)
        gSpecialVar_Result = 2;
    else
        gSpecialVar_Result = 1;
}
