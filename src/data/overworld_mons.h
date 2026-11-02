// Optional bonus Pokemon for the wandering overworld spawns (src/overworld_mons.c).
//
// Every map with wild land encounters already spawns Pokemon from its own encounter table.
// A map listed here can ALSO spawn these, OW_MON_BONUS_CHANCE % of the time (20% by default):
// rarer or out-of-place Pokemon you only meet in the overworld.
//
// Add a list:  static const struct OverworldBonusMon sBonus_MyRoute[] = { {SPECIES_X, minLv, maxLv}, ... };
// and a line:  { MAP_MY_ROUTE, sBonus_MyRoute, ARRAY_COUNT(sBonus_MyRoute) },

//static const struct OverworldBonusMon sBonus_Route1[] =
//{
//    { SPECIES_SENTRET,  3, 5 },
//    { SPECIES_HOOTHOOT, 3, 5 },
//};

//static const struct OverworldBonusList sOverworldBonusLists[] =
//{
 //   { MAP_ROUTE1, sBonus_Route1, ARRAY_COUNT(sBonus_Route1) },
//};

// Keep this array even when it has no real lists: the last line is an empty placeholder
// (count 0) that is never used, so the file compiles with every bonus list commented out.
static const struct OverworldBonusList sOverworldBonusLists[] =
{
    // { MAP_ROUTE1, sBonus_Route1, ARRAY_COUNT(sBonus_Route1) },
    { 0xFFFF, NULL, 0 }, // placeholder, leave last
};

// Maps where wild Pokemon are ONLY ever the visible wandering ones.
//
// Two things change on a map listed here, and they are two halves of the same idea:
//
//   - a spawn may stand on any walkable tile, not only an encounter tile. Beach sand
//     carries no encounter attribute at all (TILE_ENCOUNTER_NONE), so without this a
//     beach spawns nothing on land however good its encounter table is.
//
//   - stepping does not roll an encounter, anywhere on the map. Beaches are not bare
//     sand: Valencia Beach has 53 tiles of encounter grass in it, Mikan Beach 64,
//     Mandarin South 97. Left alone those few tiles would still throw blind battles,
//     which is the thing this is meant to do away with. Fishing and surfing encounters
//     are untouched -- only the land-step and Sweet Scent rolls stop.
//
// So a listed map's wild Pokemon are all walked into on purpose, and never sprung.
// The map still needs land encounters in src/data/wild_encounters.json for anything to
// appear: that table is where the species and levels come from. Of the beaches only
// Valencia and Mikan have one today, so the rest are listed here but stay empty until
// they get a table of their own.
static const u16 sOverworldOnlyMaps[] =
{
    MAP_ORANGE_ISLANDS_VALENCIA_BEACH,
    MAP_ORANGE_ISLANDS_MIKAN_BEACH,
    MAP_ORANGE_ISLANDS_SUNBURST_BEACH,
    MAP_ORANGE_ISLANDS_NAVEL_BEACH,
    MAP_ORANGE_ISLANDS_MORO_BEACH,
    MAP_ORANGE_ISLANDS_KINNOW_BEACH,
    MAP_ORANGE_ISLANDS_MANDARIN_SOUTH_BEACH,
    MAP_ORANGE_ISLANDS_TROVITA_BEACH,
    MAP_ORANGE_ISLANDS_KUMQUAT_BEACH,
    MAP_ORANGE_ISLANDS_PINKAN_BEACH,
    MAP_ORANGE_ISLANDS_PUMMELO_BEACH,
};
