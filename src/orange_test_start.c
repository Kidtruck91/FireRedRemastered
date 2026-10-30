// A new game that starts on Valencia with every badge, every HM, a team that
// can use them, and a bag full of balls and candy. Build it with:
//
//     make ORANGE_TEST=1          -> pokefirered_orangetest.gba
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

    // Prof. Ivy's arrival scene and her three starter balls are already done:
    // VAR_ORANGE_STORY 3 is the state just after taking one. Without this the
    // dock trigger on Valencia would fire and drag the player into the lab.
    VarSet(VAR_ORANGE_STORY, 3);
    FlagSet(FLAG_HIDE_IVY_AT_DOCK);
    FlagSet(FLAG_HIDE_IVY_STARTER_BALLS);

    // The trip has its own PC and its own team. Normally Ivy's lab calls
    // OrangeChallenge_Begin, which first writes the Kanto PC and home team out
    // to flash; at new game both are empty and flash is not somewhere to be
    // writing yet, so this just puts RAM into the same state.
    OrangeTest_EnterChallenge();

    GiveTestParty();
    for (i = 0; i < NELEMS(sTestBag); i++)
        AddBagItem(sTestBag[i].item, sTestBag[i].count);   // HMs bring the TM Case with them
    SetMoney(&gSaveBlock1Ptr->money, 99999);

    SetLastHealLocationWarp(SPAWN_ORANGE_VALENCIA);
    SetWarpDestination(MAP_GROUP(ORANGE_ISLANDS_VALENCIA),
                       MAP_NUM(ORANGE_ISLANDS_VALENCIA), -1, 51, 45);
    WarpIntoMap();
}

#endif // ORANGE_TEST_START
