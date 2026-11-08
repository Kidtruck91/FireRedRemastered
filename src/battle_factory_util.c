// The handful of frontier helpers the BATTLE FACTORY leans on.
//
// In Emerald these live in battle_tower.c and pokemon.c among the rest of the
// frontier. FireRed has its own battle_tower.c doing an unrelated job, so they
// are gathered here instead and nothing of FireRed's is touched.

#include "global.h"
#include "battle_factory.h"
#include "event_data.h"
#include "pokemon.h"
#include "random.h"
#include "constants/battle_factory.h"
#include "constants/battle_frontier_trainers.h"
#include "constants/pokemon.h"
#include "constants/event_objects.h"
#include "constants/trainers.h"

static const u16 sFrontierTrainerIdRanges[][2] =
{
    {FRONTIER_TRAINER_BRADY,   FRONTIER_TRAINER_JILL},   //   0 -  99
    {FRONTIER_TRAINER_TREVIN,  FRONTIER_TRAINER_CHLOE},  //  80 - 119
    {FRONTIER_TRAINER_ERIK,    FRONTIER_TRAINER_SOFIA},  // 100 - 139
    {FRONTIER_TRAINER_NORTON,  FRONTIER_TRAINER_JAZLYN}, // 120 - 159
    {FRONTIER_TRAINER_BRADEN,  FRONTIER_TRAINER_ALISON}, // 140 - 179
    {FRONTIER_TRAINER_ZACHERY, FRONTIER_TRAINER_LAMAR},  // 160 - 199
    {FRONTIER_TRAINER_HANK,    FRONTIER_TRAINER_TESS},   // 180 - 219
    {FRONTIER_TRAINER_JAXON,   FRONTIER_TRAINER_GRETEL}, // 200 - 299
};
static const u16 sFrontierTrainerIdRangesHard[][2] =
{
    {FRONTIER_TRAINER_ERIK,    FRONTIER_TRAINER_CHLOE},  // 100 - 119
    {FRONTIER_TRAINER_NORTON,  FRONTIER_TRAINER_SOFIA},  // 120 - 139
    {FRONTIER_TRAINER_BRADEN,  FRONTIER_TRAINER_JAZLYN}, // 140 - 159
    {FRONTIER_TRAINER_ZACHERY, FRONTIER_TRAINER_ALISON}, // 160 - 179
    {FRONTIER_TRAINER_HANK,    FRONTIER_TRAINER_LAMAR},  // 180 - 199
    {FRONTIER_TRAINER_JAXON,   FRONTIER_TRAINER_TESS},   // 200 - 219
    {FRONTIER_TRAINER_LEON,    FRONTIER_TRAINER_RAUL},   // 220 - 239
    {FRONTIER_TRAINER_JAXON,   FRONTIER_TRAINER_GRETEL}, // 200 - 299
};

// Which trainers a given depth of run may draw from. The last battle of each
// challenge jumps to a harder table, which is the difficulty curve.
u16 GetRandomScaledFrontierTrainerId(u8 challengeNum, u8 battleNum)
{
    u16 trainerId;

    if (challengeNum <= 7)
    {
        if (battleNum == FACTORY_STAGES_PER_CHALLENGE - 1)
        {
            // The last battle in each challenge has a jump in difficulty, pulls from a table with higher ranges
            trainerId = (sFrontierTrainerIdRangesHard[challengeNum][1] - sFrontierTrainerIdRangesHard[challengeNum][0]) + 1;
            trainerId = sFrontierTrainerIdRangesHard[challengeNum][0] + (Random() % trainerId);
        }
        else
        {
            trainerId = (sFrontierTrainerIdRanges[challengeNum][1] - sFrontierTrainerIdRanges[challengeNum][0]) + 1;
            trainerId = sFrontierTrainerIdRanges[challengeNum][0] + (Random() % trainerId);
        }
    }
    else
    {
        // After challenge 7, trainer IDs always come from the last, hardest range, which is the same for both trainer ID tables
        trainerId = (sFrontierTrainerIdRanges[7][1] - sFrontierTrainerIdRanges[7][0]) + 1;
        trainerId = sFrontierTrainerIdRanges[7][0] + (Random() % trainerId);
    }

    return trainerId;
}

// Emerald builds a rental with a chosen nature and an even EV spread over the
// stats named in a bitmask. FireRed's pokemon.c has no equivalent, so it comes
// across as written.
void CreateMonWithEVSpreadNatureOTID(struct Pokemon *mon, u16 species, u8 level,
                                     u8 nature, u8 fixedIV, u8 evSpread, u32 otId)
{
    s32 i;
    s32 statCount = 0;
    u8 evsBits;
    u16 evAmount;

    // i is reused as the personality value
    do
    {
        i = Random32();
        // FireRed keeps GetNatureFromPersonality static inside pokemon.c, and
        // it is only this one line, so it is spelled out rather than made
        // public for one caller.
    } while (nature != (u8)(i % NUM_NATURES));

    CreateMon(mon, species, level, fixedIV, TRUE, i, OT_ID_PRESET, otId);
    evsBits = evSpread;
    for (i = 0; i < NUM_STATS; i++)
    {
        if (evsBits & 1)
            statCount++;
        evsBits >>= 1;
    }

    evAmount = MAX_TOTAL_EVS / statCount;
    evsBits = 1;
    for (i = 0; i < NUM_STATS; i++)
    {
        if (evSpread & evsBits)
            SetMonData(mon, MON_DATA_HP_EV + i, &evAmount);
        evsBits <<= 1;
    }

    CalculateMonStats(mon);
}

// The boat has one facility at one level, so this is a constant where Emerald
// had to look it up.
u8 SetFacilityPtrsGetLevel(void)
{
    gFacilityTrainerMons = gBattleFrontierMons;
    return FACTORY_MON_LEVEL;
}

static const u8 sTowerMaleFacilityClasses[30] =
{
    FACILITY_CLASS_RUIN_MANIAC,
    FACILITY_CLASS_TUBER,
    FACILITY_CLASS_COOLTRAINER_M,
    FACILITY_CLASS_RICH_BOY,
    FACILITY_CLASS_POKEMANIAC,
    FACILITY_CLASS_SWIMMER_M,
    FACILITY_CLASS_BLACK_BELT,
    FACILITY_CLASS_GUITARIST,
    FACILITY_CLASS_KINDLER,
    FACILITY_CLASS_CAMPER,
    FACILITY_CLASS_BUG_MANIAC,
    FACILITY_CLASS_PSYCHIC_M,
    FACILITY_CLASS_GENTLEMAN,
    FACILITY_CLASS_SCHOOL_KID_M,
    FACILITY_CLASS_POKEFAN_M,
    FACILITY_CLASS_EXPERT_M,
    FACILITY_CLASS_YOUNGSTER,
    FACILITY_CLASS_FISHERMAN,
    FACILITY_CLASS_CYCLING_TRIATHLETE_M,
    FACILITY_CLASS_RUNNING_TRIATHLETE_M,
    FACILITY_CLASS_SWIMMING_TRIATHLETE_M,
    FACILITY_CLASS_DRAGON_TAMER,
    FACILITY_CLASS_BIRD_KEEPER,
    FACILITY_CLASS_NINJA_BOY,
    FACILITY_CLASS_SAILOR,
    FACILITY_CLASS_COLLECTOR,
    FACILITY_CLASS_CAMPER,
    FACILITY_CLASS_PKMN_RANGER_M,
    FACILITY_CLASS_BUG_CATCHER,
    FACILITY_CLASS_HIKER
};
// Twenty-two of Emerald's overworld sprites have no FireRed counterpart, so
// each takes the nearest one it does have -- a POKEFAN_M shows as a GENTLEMAN,
// a HEX_MANIAC as a CHANNELER, the triathletes as bikers and campers. Only the
// sprite changes; the trainer keeps Emerald's class and name.
static const u8 sTowerMaleTrainerGfxIds[30] =
{
    OBJ_EVENT_GFX_HIKER,
    OBJ_EVENT_GFX_TUBER_M_LAND,
    OBJ_EVENT_GFX_MAN,
    OBJ_EVENT_GFX_BOY,
    OBJ_EVENT_GFX_SUPER_NERD,
    OBJ_EVENT_GFX_CAMPER,
    OBJ_EVENT_GFX_BLACKBELT,
    OBJ_EVENT_GFX_BALDING_MAN,
    OBJ_EVENT_GFX_BALDING_MAN,
    OBJ_EVENT_GFX_CAMPER,
    OBJ_EVENT_GFX_SUPER_NERD,
    OBJ_EVENT_GFX_SUPER_NERD,
    OBJ_EVENT_GFX_GENTLEMAN,
    OBJ_EVENT_GFX_YOUNGSTER,
    OBJ_EVENT_GFX_GENTLEMAN,
    OBJ_EVENT_GFX_OLD_MAN_1,
    OBJ_EVENT_GFX_YOUNGSTER,
    OBJ_EVENT_GFX_FISHER,
    OBJ_EVENT_GFX_BIKER,
    OBJ_EVENT_GFX_CAMPER,
    OBJ_EVENT_GFX_CAMPER,
    OBJ_EVENT_GFX_MAN,
    OBJ_EVENT_GFX_BALDING_MAN,
    OBJ_EVENT_GFX_LITTLE_BOY,
    OBJ_EVENT_GFX_SAILOR,
    OBJ_EVENT_GFX_SUPER_NERD,
    OBJ_EVENT_GFX_FAT_MAN,
    OBJ_EVENT_GFX_CAMPER,
    OBJ_EVENT_GFX_BUG_CATCHER,
    OBJ_EVENT_GFX_HIKER
};
static const u8 sTowerFemaleFacilityClasses[20] =
{
    FACILITY_CLASS_AROMA_LADY,
    FACILITY_CLASS_TUBER,
    FACILITY_CLASS_COOLTRAINER_F,
    FACILITY_CLASS_HEX_MANIAC,
    FACILITY_CLASS_LADY,
    FACILITY_CLASS_BEAUTY,
    FACILITY_CLASS_PSYCHIC_F,
    FACILITY_CLASS_SCHOOL_KID_F,
    FACILITY_CLASS_POKEFAN_F,
    FACILITY_CLASS_EXPERT_F,
    FACILITY_CLASS_CYCLING_TRIATHLETE_F,
    FACILITY_CLASS_RUNNING_TRIATHLETE_F,
    FACILITY_CLASS_SWIMMING_TRIATHLETE_F,
    FACILITY_CLASS_BATTLE_GIRL,
    FACILITY_CLASS_PARASOL_LADY,
    FACILITY_CLASS_SWIMMER_F,
    FACILITY_CLASS_PICNICKER,
    FACILITY_CLASS_PICNICKER,
    FACILITY_CLASS_PKMN_RANGER_F,
    FACILITY_CLASS_LASS
};
static const u8 sTowerFemaleTrainerGfxIds[20] =
{
    OBJ_EVENT_GFX_WOMAN_2,
    OBJ_EVENT_GFX_TUBER_F,
    OBJ_EVENT_GFX_WOMAN_2,
    OBJ_EVENT_GFX_CHANNELER,
    OBJ_EVENT_GFX_WOMAN_2,
    OBJ_EVENT_GFX_BEAUTY,
    OBJ_EVENT_GFX_LASS,
    OBJ_EVENT_GFX_LASS,
    OBJ_EVENT_GFX_WOMAN_3,
    OBJ_EVENT_GFX_OLD_WOMAN,
    OBJ_EVENT_GFX_BATTLE_GIRL,
    OBJ_EVENT_GFX_PICNICKER,
    OBJ_EVENT_GFX_PICNICKER,
    OBJ_EVENT_GFX_LASS,
    OBJ_EVENT_GFX_WOMAN_2,
    OBJ_EVENT_GFX_PICNICKER,
    OBJ_EVENT_GFX_PICNICKER,
    OBJ_EVENT_GFX_WOMAN_2,
    OBJ_EVENT_GFX_PICNICKER,
    OBJ_EVENT_GFX_LASS
};

// Which overworld sprite an opponent shows as, from their facility class.
// Emerald searches a male table then a female one; the same two tables come
// across, with the four classes FireRed does not split by gender folded onto
// the ones it has.
void SetBattleFacilityTrainerGfxId(u16 trainerId, u8 tempVarId)
{
    u32 i;
    u8 facilityClass;
    u8 gfxId = OBJ_EVENT_GFX_BOY;

    SetFacilityPtrsGetLevel();
    if (trainerId >= FRONTIER_TRAINERS_COUNT)
        return;
    facilityClass = gFacilityTrainers[trainerId].facilityClass;

    for (i = 0; i < ARRAY_COUNT(sTowerMaleFacilityClasses); i++)
    {
        if (sTowerMaleFacilityClasses[i] == facilityClass)
        {
            gfxId = sTowerMaleTrainerGfxIds[i];
            break;
        }
    }
    if (i == ARRAY_COUNT(sTowerMaleFacilityClasses))
    {
        for (i = 0; i < ARRAY_COUNT(sTowerFemaleFacilityClasses); i++)
        {
            if (sTowerFemaleFacilityClasses[i] == facilityClass)
            {
                gfxId = sTowerFemaleTrainerGfxIds[i];
                break;
            }
        }
    }

    switch (tempVarId)
    {
    case 1:
        VarSet(VAR_OBJ_GFX_ID_1, gfxId);
        break;
    case 0:
    default:
        VarSet(VAR_OBJ_GFX_ID_0, gfxId);
        break;
    }
}
