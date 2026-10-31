#ifndef GUARD_CONSTANTS_GAME_OPTIONS_H
#define GUARD_CONSTANTS_GAME_OPTIONS_H

// The settings the Options Closet cycles. Most of them already had storage of
// their own before the closet existed -- Nuzlocke mode, the level cap, the
// physical/special split and EXP All are flags the Options menu also sets, and
// the closet is a second way to reach the same switches, not a second copy of
// them. Only the mugshot palette is new.

// Which object in the closet maps to which setting. The map scripts put one of
// these in VAR_0x8004 before calling BufferGameOption / CycleGameOption, so
// these values and the sOptions[] table in src/game_options.c have to agree.
#define GAME_OPTION_NUZLOCKE            0
#define GAME_OPTION_MUGSHOT_PALETTE     1
#define GAME_OPTION_LEVEL_CAP           2
#define GAME_OPTION_PHYS_SPEC_SPLIT     3
#define GAME_OPTION_EXP_ALL             4
#define GAME_OPTION_COUNT               5

// VAR_OPTION_MUGSHOT_PALETTE. AUTO is the default: the starter's colour in
// rival battles, the gender default everywhere else.
#define MUGSHOT_PALETTE_OPTION_AUTO     0
#define MUGSHOT_PALETTE_OPTION_GENDER   1
#define MUGSHOT_PALETTE_OPTION_GRASS    2
#define MUGSHOT_PALETTE_OPTION_WATER    3
#define MUGSHOT_PALETTE_OPTION_FIRE     4
#define MUGSHOT_PALETTE_OPTION_COUNT    5

#endif // GUARD_CONSTANTS_GAME_OPTIONS_H
