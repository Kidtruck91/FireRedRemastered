// Optional bonus Pokemon for the wandering overworld spawns (src/overworld_mons.c).
//
// Every map with wild land encounters already spawns Pokemon from its own encounter table.
// A map listed here can ALSO spawn these, OW_MON_BONUS_CHANCE % of the time (20% by default):
// rarer or out-of-place Pokemon you only meet in the overworld.
//
// Add a list:  static const struct OverworldBonusMon sBonus_MyRoute[] = { {SPECIES_X, minLv, maxLv}, ... };
// and a line:  { MAP_MY_ROUTE, sBonus_MyRoute, ARRAY_COUNT(sBonus_MyRoute) },

static const struct OverworldBonusMon sBonus_Route1[] =
{
    { SPECIES_SENTRET,  3, 5 },
    { SPECIES_HOOTHOOT, 3, 5 },
};

static const struct OverworldBonusList sOverworldBonusLists[] =
{
    { MAP_ROUTE1, sBonus_Route1, ARRAY_COUNT(sBonus_Route1) },
};
