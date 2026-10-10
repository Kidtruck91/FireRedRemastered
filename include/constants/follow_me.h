#ifndef GUARD_CONSTANTS_FOLLOW_ME_H
#define GUARD_CONSTANTS_FOLLOW_ME_H

#define FOLLOWER_FLAG_HAS_RUNNING_FRAMES    0x1
#define FOLLOWER_FLAG_CAN_BIKE              0x2
#define FOLLOWER_FLAG_CAN_LEAVE_ROUTE       0x4     // teleport, dig, fly, etc
#define FOLLOWER_FLAG_CAN_SURF              0x8
#define FOLLOWER_FLAG_CAN_WATERFALL         0x10
#define FOLLOWER_FLAG_CAN_DIVE              0x20
#define FOLLOWER_FLAG_CAN_ROCK_CLIMB        0x40    // need rock climb implemented
#define FOLLOWER_FLAG_CLEAR_ON_WHITE_OUT    0x80
#define FOLLOWER_FLAG_CUSTOM_FOLLOW_SCRIPT  0x100   // follower has custom script while following (use loadword prior to setfollower)

#define FOLLOWER_FLAG_ALL_WATER             FOLLOWER_FLAG_CAN_SURF | FOLLOWER_FLAG_CAN_WATERFALL | FOLLOWER_FLAG_CAN_DIVE
#define FOLLOWER_FLAG_ALL_LAND              FOLLOWER_FLAG_HAS_RUNNING_FRAMES | FOLLOWER_FLAG_CAN_BIKE | FOLLOWER_FLAG_CAN_LEAVE_ROUTE
#define FOLLOWER_FLAG_ALL                   FOLLOWER_FLAG_ALL_WATER | FOLLOWER_FLAG_ALL_LAND | FOLLOWER_FLAG_CLEAR_ON_WHITE_OUT


// Follower Pokemon dialogue: conditions for the follower_message macro
// (see data/text/follower_dialogue.inc). The value's meaning depends on the condition.
#define FOLLOWER_COND_NONE          0   // always matches (generic lines)
#define FOLLOWER_COND_SPECIES       1   // value: SPECIES_*
#define FOLLOWER_COND_TYPE          2   // value: TYPE_* (either of the Pokemon's types)
#define FOLLOWER_COND_EGG_GROUP     3   // value: EGG_GROUP_*
#define FOLLOWER_COND_MAP           4   // value: MAP_* (one exact map)
#define FOLLOWER_COND_MAPSEC        5   // value: MAPSEC_* (an area, e.g. every floor of a cave)
#define FOLLOWER_COND_MAP_TYPE      6   // value: MAP_TYPE_* (MAP_TYPE_UNDERGROUND = caves, MAP_TYPE_INDOOR, ...)
#define FOLLOWER_COND_TERRAIN       7   // value: FOLLOWER_TERRAIN_* (what the Pokemon is standing on / next to)
#define FOLLOWER_COND_WEATHER       8   // value: WEATHER_*
#define FOLLOWER_COND_FRIENDSHIP    9   // value: minimum friendship (0-255; 255 = max)
#define FOLLOWER_COND_LOW_HP        10  // value: ignored. HP is at or below 1/4
#define FOLLOWER_COND_STATUS        11  // value: ignored. The Pokemon has a status problem (poison, burn...)
#define FOLLOWER_COND_FLAG          12  // value: FLAG_* that must be set (story progress, badges...)
#define FOLLOWER_COND_NOT_FLAG      13  // value: FLAG_* that must NOT be set

#define FOLLOWER_TERRAIN_SAND       0
#define FOLLOWER_TERRAIN_TALL_GRASS 1
#define FOLLOWER_TERRAIN_WATER      2   // next to water (surfable tiles around the Pokemon)
#define FOLLOWER_TERRAIN_ICE        3

#endif // GUARD_CONSTANTS_FOLLOW_ME_H
