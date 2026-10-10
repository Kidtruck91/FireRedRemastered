#ifndef LEVEL_CAP_H
#define LEVEL_CAP_H

#include "global.h"

void SetLevelCap(u8 cap);                                   // Saves a level cap value
u8 GetLevelCap(void);                                       // Current level cap (MAX_LEVEL = no cap)
void SetLevelCapTo(void);                                   // Recalculates the cap from progress and saves it
void GetSpecialVarLevelCap(void);                           // Puts the current cap in VAR_TEMP_0
bool8 IsMonAtLevelCap(struct Pokemon *mon);
u32 CapExpGainAtLevelCap(struct Pokemon *mon, u32 expGain);

#endif // LEVEL_CAP_H
