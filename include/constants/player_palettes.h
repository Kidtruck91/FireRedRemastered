#ifndef PLAYER_PALETTES_H
#define PLAYER_PALETTES_H

enum
{
   PLAYER_PALETTE_MALE, 
   PLAYER_PALETTE_FEMALE, 
   PLAYER_PALETTE_GRASS_STARTER,
   PLAYER_PALETTE_WATER_STARTER, 
   PLAYER_PALETTE_FIRE_STARTER, 
   PLAYER_PALETTES_COUNT 
// Add other custom palettes as needed
};
u8 GetPlayerPaletteId(void);
void SetPlayerPaletteId(u8 playerPaletteId);
void SetCustomPlayerPaletteId(u8 playerPaletteId);
void RevertPlayerPaletteToGenderDefault(void);

#endif // PLAYER_PALETTES_H