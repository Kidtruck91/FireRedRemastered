// A new game that starts on Valencia with every badge, every HM, a team that
// can use them, and a bag full of balls and candy. Build it with:
//
//     make ORANGE_TEST=1          -> pokefirered_orangetest.gba
//
// Add BOAT=1 to start at the arena desk on KINNOW's battle boat instead,
// which is the whole point of the FACTORY port and twenty tiles of jetty plus
// three decks away from anywhere else:
//
//     make ORANGE_TEST=1 BOAT=1
//
// The switch sets -DORANGE_TEST_START and changes BUILD_NAME, so the test
// build never shares object files with the real one and a plain `make` is
// byte-for-byte unaffected. Everything below compiles to nothing without it.
#include "global.h"
#include "event_data.h"
#include "item.h"
#include "money.h"
#include "overworld.h"
#include "pokemon.h"
#include "pokedex.h"
#include "orange_islands.h"
#include "constants/flags.h"
#include "constants/heal_locations.h"
#include "constants/items.h"
#include "constants/maps.h"
#include "constants/moves.h"
#include "constants/species.h"
#include "constants/vars.h"

#ifdef ORANGE_TEST_START

#define TEST_LEVEL 50

struct TestMon
{
    u16 species;
    u16 moves[MAX_MON_MOVES];
};

// Between them these know all eight HM moves. The party menu offers a field
// move when the Pokemon knows it, so the moves are written straight into the
// slots rather than relying on what each species can legitimately learn.
static const struct TestMon sTestParty[] =
{
    { SPECIES_LAPRAS,   {MOVE_SURF,         MOVE_DIVE,        MOVE_WATERFALL,     MOVE_ICE_BEAM} },
    { SPECIES_PIDGEOT,  {MOVE_FLY,          MOVE_CUT,         MOVE_WING_ATTACK,   MOVE_QUICK_ATTACK} },
    { SPECIES_GOLEM,    {MOVE_STRENGTH,     MOVE_ROCK_SMASH,  MOVE_FLASH,         MOVE_EARTHQUAKE} },
    { SPECIES_ARCANINE, {MOVE_FLAMETHROWER, MOVE_BITE,        MOVE_EXTREME_SPEED, MOVE_ROAR} },
    { SPECIES_ALAKAZAM, {MOVE_PSYCHIC,      MOVE_SHADOW_BALL, MOVE_CALM_MIND,     MOVE_RECOVER} },
    { SPECIES_SNORLAX,  {MOVE_BODY_SLAM,    MOVE_EARTHQUAKE,  MOVE_REST,          MOVE_SHADOW_BALL} },
};

static const struct { u16 item; u16 count; } sTestBag[] =
{
    // the ferry sailor runs `checkitem ITEM_SUNKIST_TICKET`, so the flag on
    // its own is not enough -- the ticket has to be in the bag
    {ITEM_SUNKIST_TICKET, 1},
    {ITEM_TOWN_MAP, 1},
    {ITEM_OLD_ROD, 1}, {ITEM_GOOD_ROD, 1}, {ITEM_SUPER_ROD, 1},
    {ITEM_HM01, 1}, {ITEM_HM02, 1}, {ITEM_HM03, 1}, {ITEM_HM04, 1},
    {ITEM_HM05, 1}, {ITEM_HM06, 1}, {ITEM_HM07, 1}, {ITEM_HM08, 1},
    {ITEM_POKE_BALL, 50}, {ITEM_GREAT_BALL, 40}, {ITEM_ULTRA_BALL, 40},
    {ITEM_DIVE_BALL, 20}, {ITEM_NET_BALL, 20},
    {ITEM_RARE_CANDY, 50},
    {ITEM_FULL_RESTORE, 30}, {ITEM_REVIVE, 20}, {ITEM_MAX_REPEL, 20},
    {ITEM_ESCAPE_ROPE, 10},
};

// Kanto 1-8 are FLAG_BADGE01_GET..FLAG_BADGE08_GET; these are the four Orange
// ones, which are ordinary flags rather than a run.
static const u16 sOrangeBadges[] =
{
    FLAG_BADGE_CORAL_EYE, FLAG_BADGE_SPIKE_SHELL,
    FLAG_BADGE_SEA_RUBY, FLAG_BADGE_JADE_STAR,
};

// Built straight into the party slots rather than through ScriptGiveMon,
// which allocates: this runs inside NewGameInitData, before anything has a
// reason to touch the heap.
static void GiveTestParty(void)
{
    u8 i, j;

    for (i = 0; i < NELEMS(sTestParty); i++)
    {
        struct Pokemon *mon = &gPlayerParty[i];
        u16 dex;

        CreateMon(mon, sTestParty[i].species, TEST_LEVEL, 31, FALSE, 0,
                  OT_ID_PLAYER_ID, 0);
        for (j = 0; j < MAX_MON_MOVES; j++)
            SetMonMoveSlot(mon, sTestParty[i].moves[j], j);
        dex = SpeciesToNationalPokedexNum(sTestParty[i].species);
        GetSetPokedexFlag(dex, FLAG_SET_SEEN);
        GetSetPokedexFlag(dex, FLAG_SET_CAUGHT);
    }
    CalculatePlayerPartyCount();
}

void OrangeTestStart(void)
{
    u8 i;

    // the systems the Pallet Town intro would normally have switched on
    FlagSet(FLAG_SYS_POKEMON_GET);
    FlagSet(FLAG_SYS_POKEDEX_GET);
    FlagSet(FLAG_SYS_NATIONAL_DEX);
    FlagSet(FLAG_SYS_B_DASH);

    for (i = 0; i < NUM_BADGES; i++)
        FlagSet(FLAG_BADGE01_GET + i);
    for (i = 0; i < NELEMS(sOrangeBadges); i++)
        FlagSet(sOrangeBadges[i]);
    FlagSet(FLAG_GOT_SUNKIST_TICKET);

    // Scene 3 is "the S.S. Anne has sailed and the Seagallop dock is open".
    // The ferry sailor and the pier trigger both check it before they will
    // look at a ticket at all, so without it the Valencia run is invisible.
    VarSet(VAR_MAP_SCENE_VERMILION_CITY, 3);

    // VAR_ORANGE_STORY is left at 0 on purpose: arriving at Valencia should
    // fire Prof. Ivy's dock scene, and her lab should be the thing that calls
    // OrangeChallenge_Begin and hands over a starter.

    GiveTestParty();
    for (i = 0; i < NELEMS(sTestBag); i++)
        AddBagItem(sTestBag[i].item, sTestBag[i].count);   // HMs bring the TM Case with them
    SetMoney(&gSaveBlock1Ptr->money, 99999);

#ifdef ORANGE_TEST_BOAT
    // In front of the arena desk, two decks down on the battle boat. Fainting
    // out of a factory round does not white out -- those battles run on the
    // TRAINER TOWER battle type, which battle_main.c leaves out of the
    // whiteout path -- but the heal point is set to KINNOW so that flying or
    // a blackout from anything else lands somewhere sane.
    SetLastHealLocationWarp(SPAWN_ORANGE_KINNOW);
    SetWarpDestination(MAP_GROUP(ORANGE_ISLANDS_KINNOW_BATTLE_BOAT_ARENA),
                       MAP_NUM(ORANGE_ISLANDS_KINNOW_BATTLE_BOAT_ARENA),
                       -1, 4, 4);
#else
    // One tile north of the ticket gate at 22-23,33, facing the pier: walk
    // down and the sailor asks about the SUNKIST TICKET.
    SetLastHealLocationWarp(SPAWN_VERMILION_CITY);
    SetWarpDestination(MAP_GROUP(VERMILION_CITY), MAP_NUM(VERMILION_CITY),
                       -1, 23, 31);
#endif
    WarpIntoMap();
}

#endif // ORANGE_TEST_START
