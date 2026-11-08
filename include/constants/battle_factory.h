#ifndef GUARD_CONSTANTS_BATTLE_FACTORY_H
#define GUARD_CONSTANTS_BATTLE_FACTORY_H

// The BATTLE FACTORY, ported from Emerald for the battle boat moored at the
// end of KINNOW's pier.
//
// Emerald's factory runs at two level modes, Lv 50 and Open Level, and indexes
// most of its save state by which one you picked. The boat runs at Lv 50 only,
// so that dimension is gone throughout: no lvlMode field, no [2] on the
// streaks, and FACTORY_MON_LEVEL below instead of a lookup.

#define FACTORY_PARTY_SIZE            3   // how many you take into a battle
#define FACTORY_RENTAL_COUNT          6   // how many you are offered to pick from
#define FACTORY_STAGES_PER_CHALLENGE  7   // battles in one run before it pays out
#define FACTORY_MON_LEVEL            50

// challengeStatus
#define FACTORY_CHALLENGE_NONE        0
#define FACTORY_CHALLENGE_RUNNING     1
#define FACTORY_CHALLENGE_PAUSED      2

// What the script asks the challenge to do, through VAR_0x8004. Emerald's
// names and numbering, kept so the ported dispatch table reads unchanged.
#define BATTLE_FACTORY_FUNC_INIT                    0
#define BATTLE_FACTORY_FUNC_GET_DATA                1
#define BATTLE_FACTORY_FUNC_SET_DATA                2
#define BATTLE_FACTORY_FUNC_SAVE                    3
#define BATTLE_FACTORY_FUNC_NULL                    4
#define BATTLE_FACTORY_FUNC_NULL2                   5
#define BATTLE_FACTORY_FUNC_SELECT_RENT_MONS        6
#define BATTLE_FACTORY_FUNC_SWAP_RENT_MONS          7
#define BATTLE_FACTORY_FUNC_SET_SWAPPED             8
#define BATTLE_FACTORY_FUNC_SET_OPPONENT_MONS       9
#define BATTLE_FACTORY_FUNC_SET_PARTIES             10
#define BATTLE_FACTORY_FUNC_SET_OPPONENT_GFX        11
#define BATTLE_FACTORY_FUNC_GENERATE_OPPONENT_MONS  12
#define BATTLE_FACTORY_FUNC_GENERATE_RENTAL_MONS    13
#define BATTLE_FACTORY_FUNC_GET_OPPONENT_MON_TYPE   14
#define BATTLE_FACTORY_FUNC_GET_OPPONENT_STYLE      15
#define BATTLE_FACTORY_FUNC_RESET_HELD_ITEMS        16

// FACTORY_FUNC_GET_DATA / SET_DATA selectors, through VAR_0x8005.
#define FACTORY_DATA_WIN_STREAK         1
#define FACTORY_DATA_WIN_STREAK_ACTIVE  2
#define FACTORY_DATA_WIN_STREAK_SWAPS   3


// How the game describes an opponent's team to you before you fight it, worked
// out from the moves they carry. Straight from Emerald.
#define FACTORY_STYLE_NONE           0
#define FACTORY_STYLE_PREPARATION    1
#define FACTORY_STYLE_SLOW_STEADY    2
#define FACTORY_STYLE_ENDURANCE      3
#define FACTORY_STYLE_HIGH_RISK      4
#define FACTORY_STYLE_WEAKENING      5
#define FACTORY_STYLE_UNPREDICTABLE  6
#define FACTORY_STYLE_WEATHER        7
#define FACTORY_NUM_STYLES           8

// Rentals past this index are the strong ones Emerald held back for Open
// Level. The boat is Lv 50, so they stay out of the opponent pool.
#define FACTORY_MONS_HIGH_TIER       849

#endif // GUARD_CONSTANTS_BATTLE_FACTORY_H
