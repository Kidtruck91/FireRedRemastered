// The settings the Options Closet cycles (data/maps/PalletTown_PlayersHouse_OptionsCloset).
//
// A furnishing in the closet puts its option index in VAR_0x8004 and then
// callnatives BufferGameOption to read the setting, or CycleGameOption to
// advance it. Both leave the setting's name in STR_VAR_1 and its current value
// in STR_VAR_2, so one pair of texts serves every option and adding another
// setting is one row of sOptions[] plus a bg_event in the map.
//
// Nuzlocke mode, the level cap, the physical/special split and EXP All are the
// same flags the Options menu sets, so the two ways of changing them agree.
#include "global.h"
#include "event_data.h"
#include "string_util.h"
#include "constants/flags.h"
#include "constants/game_options.h"
#include "constants/vars.h"

static const u8 sText_Nuzlocke[] = _("NUZLOCKE MODE");
static const u8 sText_MugshotPalette[] = _("BATTLE INTRO");
static const u8 sText_LevelCap[] = _("LEVEL CAP");
static const u8 sText_PhysSpecSplit[] = _("PHYS/SPEC SPLIT");
static const u8 sText_ExpAll[] = _("EXP ALL");

static const u8 sText_Off[] = _("OFF");
static const u8 sText_On[] = _("ON");

static const u8 sText_PalAuto[] = _("STARTER COLOUR");
static const u8 sText_PalGender[] = _("DEFAULT");
static const u8 sText_PalGrass[] = _("ALWAYS GRASS");
static const u8 sText_PalWater[] = _("ALWAYS WATER");
static const u8 sText_PalFire[] = _("ALWAYS FIRE");

static const u8 *const sOffOn[] = {sText_Off, sText_On};

static const u8 *const sMugshotPaletteNames[MUGSHOT_PALETTE_OPTION_COUNT] = {
    [MUGSHOT_PALETTE_OPTION_AUTO]   = sText_PalAuto,
    [MUGSHOT_PALETTE_OPTION_GENDER] = sText_PalGender,
    [MUGSHOT_PALETTE_OPTION_GRASS]  = sText_PalGrass,
    [MUGSHOT_PALETTE_OPTION_WATER]  = sText_PalWater,
    [MUGSHOT_PALETTE_OPTION_FIRE]   = sText_PalFire,
};

// A setting is stored either as a flag (`flag`, two values) or as a var
// (`var`, `count` values). Exactly one of flag/var is set on each row.
struct GameOption
{
    const u8 *name;
    const u8 *const *valueNames;
    u16 flag;
    u16 var;
    u8 count;
};

static const struct GameOption sOptions[GAME_OPTION_COUNT] = {
    [GAME_OPTION_NUZLOCKE] = {
        .name = sText_Nuzlocke, .valueNames = sOffOn,
        .flag = FLAG_NUZLOCKE, .count = 2,
    },
    [GAME_OPTION_MUGSHOT_PALETTE] = {
        .name = sText_MugshotPalette, .valueNames = sMugshotPaletteNames,
        .var = VAR_OPTION_MUGSHOT_PALETTE, .count = MUGSHOT_PALETTE_OPTION_COUNT,
    },
    [GAME_OPTION_LEVEL_CAP] = {
        .name = sText_LevelCap, .valueNames = sOffOn,
        .flag = FLAG_LEVEL_CAP, .count = 2,
    },
    [GAME_OPTION_PHYS_SPEC_SPLIT] = {
        .name = sText_PhysSpecSplit, .valueNames = sOffOn,
        .flag = FLAG_PHYS_SPEC_SPLIT, .count = 2,
    },
    [GAME_OPTION_EXP_ALL] = {
        .name = sText_ExpAll, .valueNames = sOffOn,
        .flag = FLAG_EXP_ALL, .count = 2,
    },
};

static const struct GameOption *CurrentOption(void)
{
    u16 i = VarGet(VAR_0x8004);

    if (i >= GAME_OPTION_COUNT)
        i = 0;
    return &sOptions[i];
}

static u8 GetOptionValue(const struct GameOption *opt)
{
    u8 value;

    if (opt->flag != 0)
        return FlagGet(opt->flag) ? 1 : 0;

    value = VarGet(opt->var);
    if (value >= opt->count)
        value = 0;
    return value;
}

static void SetOptionValue(const struct GameOption *opt, u8 value)
{
    if (opt->flag != 0)
    {
        if (value)
            FlagSet(opt->flag);
        else
            FlagClear(opt->flag);

        // The Options menu keeps this in step with the flag and the battle
        // code reads it, so changing the flag from the closet has to as well.
        if (opt->flag == FLAG_NUZLOCKE)
            gSaveBlock2Ptr->enableNuzlocke = value ? TRUE : FALSE;
        return;
    }
    VarSet(opt->var, value);
}

static void BufferOptionStrings(const struct GameOption *opt)
{
    StringCopy(gStringVar1, opt->name);
    StringCopy(gStringVar2, opt->valueNames[GetOptionValue(opt)]);
}

// callnative: fills STR_VAR_1 with the setting's name, STR_VAR_2 with its value
void BufferGameOption(void)
{
    BufferOptionStrings(CurrentOption());
}

// callnative: advances the setting one step, wrapping, then rebuffers both
void CycleGameOption(void)
{
    const struct GameOption *opt = CurrentOption();
    u8 value = GetOptionValue(opt) + 1;

    if (value >= opt->count)
        value = 0;
    SetOptionValue(opt, value);
    BufferOptionStrings(opt);
}
