#include "global.h"
#include "event_data.h"
#include "constants/player_palettes.h"

void SetPlayerPaletteId(u8 playerPaletteId)
{
    gPlayerPaletteId = playerPaletteId;
}
void SetCustomPlayerPaletteId(u8 playerPaletteId)
{
    gPlayerPaletteId = playerPaletteId;
}
u8 GetPlayerPaletteId(void)
{
    return gPlayerPaletteId;  // Return the current value of gPlayerPaletteId
}

void RevertPlayerPaletteToGenderDefault(void)
{
    if (gSaveBlock2Ptr->playerGender == MALE)
        SetPlayerPaletteId(PLAYER_PALETTE_MALE);
    else
        SetPlayerPaletteId(PLAYER_PALETTE_FEMALE);
}