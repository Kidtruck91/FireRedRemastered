#include "global.h"
#include "event_data.h"
#include "battle.h"
#include "constants/game_options.h"
#include "constants/opponents.h"
#include "constants/player_palettes.h"
#include "constants/trainers.h"
#include "constants/vars.h"

void SetPlayerPaletteId(u8 playerPaletteId)
{
    gPlayerPaletteId = playerPaletteId;
}

// Kept for scripts that want to force a palette for one particular battle:
// set VAR_0x8015 and callnative this immediately before the battle. Nothing in
// the game uses it now -- rival battles are detected from the trainer instead,
// in SetMugshotPlayerPalette below -- but it stays as the manual override.
void SetCustomPlayerPaletteId(void)
{
    gPlayerPaletteId = VarGet(VAR_0x8015);
}

u8 GetPlayerPaletteId(void)
{
    return gPlayerPaletteId;
}

void RevertPlayerPaletteToGenderDefault(void)
{
    if (gSaveBlock2Ptr->playerGender == MALE)
        SetPlayerPaletteId(PLAYER_PALETTE_MALE);
    else
        SetPlayerPaletteId(PLAYER_PALETTE_FEMALE);
}

// VAR_STARTER_MON is written by `copyvar VAR_STARTER_MON, PLAYER_STARTER_NUM`
// when the starter is accepted in Oak's lab, so it is already set before the
// first rival battle and is saved from then on.
u8 GetStarterPlayerPaletteId(void)
{
    switch (VarGet(VAR_STARTER_MON))
    {
    case 0: // Bulbasaur
        return PLAYER_PALETTE_GRASS_STARTER;
    case 1: // Squirtle
        return PLAYER_PALETTE_WATER_STARTER;
    case 2: // Charmander
        return PLAYER_PALETTE_FIRE_STARTER;
    }
    // No starter yet, which only happens if a mugshot battle somehow runs
    // before the lab scene.
    if (gSaveBlock2Ptr->playerGender == MALE)
        return PLAYER_PALETTE_MALE;
    return PLAYER_PALETTE_FEMALE;
}

// The rival battles are exactly the two rival trainer classes plus Terry's two
// championship fights. Drake shares TRAINER_CLASS_CHAMPION with Terry, so the
// class alone is not enough to tell them apart.
static bool8 IsRivalTrainer(u16 opponentId)
{
    switch (gTrainers[opponentId].trainerClass)
    {
    case TRAINER_CLASS_RIVAL_EARLY:
    case TRAINER_CLASS_RIVAL_LATE:
        return TRUE;
    case TRAINER_CLASS_CHAMPION:
        return opponentId != TRAINER_CHAMPION_DRAKE
            && opponentId != TRAINER_CHAMPION_DRAKE_REMATCH;
    }
    return FALSE;
}

// Picks the player's half of the mugshot, called once per mugshot battle as the
// transition is chosen. The gender-default branch is what keeps the starter
// colour to rival battles only: without it a palette set for an earlier battle
// would still be in gPlayerPaletteId and would bleed into the next gym leader.
void SetMugshotPlayerPalette(u16 opponentId)
{
    switch (VarGet(VAR_OPTION_MUGSHOT_PALETTE))
    {
    case MUGSHOT_PALETTE_OPTION_GENDER:
        RevertPlayerPaletteToGenderDefault();
        return;
    case MUGSHOT_PALETTE_OPTION_GRASS:
        SetPlayerPaletteId(PLAYER_PALETTE_GRASS_STARTER);
        return;
    case MUGSHOT_PALETTE_OPTION_WATER:
        SetPlayerPaletteId(PLAYER_PALETTE_WATER_STARTER);
        return;
    case MUGSHOT_PALETTE_OPTION_FIRE:
        SetPlayerPaletteId(PLAYER_PALETTE_FIRE_STARTER);
        return;
    }

    if (IsRivalTrainer(opponentId))
        SetPlayerPaletteId(GetStarterPlayerPaletteId());
    else
        RevertPlayerPaletteToGenderDefault();
}
