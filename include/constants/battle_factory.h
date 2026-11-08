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

// What the script asks the challenge to do, through VAR_0x8004.
#define FACTORY_FUNC_INIT             0
#define FACTORY_FUNC_GET_DATA         1
#define FACTORY_FUNC_SET_DATA         2
#define FACTORY_FUNC_SAVE             3
#define FACTORY_FUNC_SELECT_RENTALS   4
#define FACTORY_FUNC_SWAP             5
#define FACTORY_FUNC_SET_OPPONENT     6
#define FACTORY_FUNC_SET_PARTY        7
#define FACTORY_FUNC_RESET_HELD_ITEMS 8

// FACTORY_FUNC_GET_DATA / SET_DATA selectors, through VAR_0x8005.
#define FACTORY_DATA_WIN_STREAK       0
#define FACTORY_DATA_BEST_STREAK      1
#define FACTORY_DATA_RENTS_COUNT      2
#define FACTORY_DATA_BATTLE_NUM       3

#endif // GUARD_CONSTANTS_BATTLE_FACTORY_H
