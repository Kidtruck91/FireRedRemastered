#ifndef LEVEL_CAP_H
#define LEVEL_CAP_H

#include "global.h"

// Function prototypes
void SetLevelCap(u8 cap);          // Set the level cap manually
u8 GetLevelCap(void);              // Retrieve the current level cap
void SetLevelCapTo(void);          // Set the level cap based on flags or progression
void SPECIAL_GetSpecialVarLevelCap(void);  // Store the current level cap in gSpecialVar_0x8000

#endif // LEVEL_CAP_H
