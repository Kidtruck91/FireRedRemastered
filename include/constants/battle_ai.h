#ifndef GUARD_CONSTANTS_BATTLE_AI_H
#define GUARD_CONSTANTS_BATTLE_AI_H

// battlers
#define AI_TARGET 0
#define AI_USER 1
#define AI_TARGET_PARTNER 2
#define AI_USER_PARTNER 3

// get_type command
#define AI_TYPE1_TARGET 0
#define AI_TYPE1_USER 1
#define AI_TYPE2_TARGET 2
#define AI_TYPE2_USER 3
#define AI_TYPE_MOVE 4

// type effectiveness
#define AI_EFFECTIVENESS_x4     160
#define AI_EFFECTIVENESS_x2     80
#define AI_EFFECTIVENESS_x1     40
#define AI_EFFECTIVENESS_x0_5   20
#define AI_EFFECTIVENESS_x0_25  10
#define AI_EFFECTIVENESS_x0     0

// ai weather
#define AI_WEATHER_SUN       0
#define AI_WEATHER_RAIN      1
#define AI_WEATHER_SANDSTORM 2
#define AI_WEATHER_HAIL      3

// get_how_powerful_move_is
#define MOVE_POWER_DISCOURAGED  0
#define MOVE_NOT_MOST_POWERFUL  1
#define MOVE_MOST_POWERFUL      2

// script's table id to bit
#define AI_SCRIPT_CHECK_BAD_MOVE (1 << 0)
#define AI_SCRIPT_CHECK_VIABILITY (1 << 1)
#define AI_SCRIPT_TRY_TO_FAINT (1 << 2)
#define AI_SCRIPT_SETUP_FIRST_TURN (1 << 3)
#define AI_SCRIPT_RISKY (1 << 4)
#define AI_SCRIPT_PREFER_STRONGEST_MOVE (1 << 5)
#define AI_SCRIPT_PREFER_BATON_PASS (1 << 6)
#define AI_SCRIPT_DOUBLE_BATTLE (1 << 7)
#define AI_SCRIPT_HP_AWARE (1 << 8)
#define AI_SCRIPT_UNKNOWN (1 << 9)
#define AI_SCRIPT_ADVANCED (1 << 10) // C-side AI in src/battle_ai_advanced.c (no script)

// Trainer AI levels. Put one of these in a trainer's .aiFlags in src/data/trainers.h.
//   SIMPLE   - vanilla route-trainer AI: only avoids moves that obviously fail.
//   NORMAL   - gym-leader AI: also goes for KOs, picks sensible moves, and plays around its HP.
//   ADVANCED - NORMAL plus src/battle_ai_advanced.c: remembers every move your Pokemon use,
//              uses real damage calcs to take sure KOs, sets up when it's safe, reads
//              switches, and switches its own Pokemon out of bad matchups.
// You can still OR in extra script flags, e.g. AI_LEVEL_NORMAL | AI_SCRIPT_SETUP_FIRST_TURN.
#define AI_LEVEL_SIMPLE   (AI_SCRIPT_CHECK_BAD_MOVE)
#define AI_LEVEL_NORMAL   (AI_SCRIPT_CHECK_BAD_MOVE | AI_SCRIPT_CHECK_VIABILITY | AI_SCRIPT_TRY_TO_FAINT | AI_SCRIPT_HP_AWARE)
#define AI_LEVEL_ADVANCED (AI_LEVEL_NORMAL | AI_SCRIPT_ADVANCED)
// 10 - 28 are not used
#define AI_SCRIPT_ROAMING (1 << 29)
#define AI_SCRIPT_SAFARI (1 << 30)
#define AI_SCRIPT_FIRST_BATTLE (1 << 31)

#endif // GUARD_CONSTANTS_BATTLE_AI_H
