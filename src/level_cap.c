#include "global.h"
#include "event_data.h"
#include "level_cap.h"
#include "constants/vars.h"
#include "constants/flags.h"

// Retrieves the current level cap and stores it in the special variable
void GetSpecialVarLevelCap(void) {
    VarSet(VAR_TEMP_0, gSaveBlock1Ptr->levelCap);
}

// Sets the level cap and saves it in gSaveBlock1Ptr
void SetLevelCap(u8 cap)
{
    gSaveBlock1Ptr->levelCap = cap; // Update the save block
}

// Sets the level cap based on game progression (e.g., badges or flags)
void SetLevelCapTo(void)
{
    u8 cap = 10; // Default cap (non-Nuzlocke)

    if (FlagGet(FLAG_NUZLOCKE)) { // If Nuzlocke mode is enabled
        cap = 14; // Starting level cap for Nuzlocke

        if (FlagGet(FLAG_DEFEATED_BROCK)) {
            cap = 21;
        }
        if (FlagGet(FLAG_DEFEATED_MISTY)) {
            cap = 24;
        }
        if (FlagGet(FLAG_DEFEATED_LT_SURGE)) {
            cap = 29;
        }
        if (FlagGet(FLAG_DEFEATED_ERIKA)) {
            cap = 43;
        }
        if (FlagGet(FLAG_DEFEATED_KOGA)) {
            cap = 43;
        }
        if (FlagGet(FLAG_DEFEATED_SABRINA)) {
            cap = 47;
        }
        if (FlagGet(FLAG_DEFEATED_BLAINE)) {
            cap = 50;
        }
        if (FlagGet(FLAG_DEFEATED_LEADER_GIOVANNI)) {
            cap = 63;
        }
        if (FlagGet(FLAG_DEFEATED_CHAMP)) {
            cap = 100; // Max cap for Nuzlocke after Champion
        }
    }

    SetLevelCap(cap); // Update the level cap in SaveBlock1
}

// Retrieves the current level cap from SaveBlock1
u8 GetLevelCap(void)
{
    return gSaveBlock1Ptr->levelCap;
}
