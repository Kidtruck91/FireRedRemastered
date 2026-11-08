// Scripted wild POKeMON that are more than a species and a level.
//
// setwildbattle builds a plain wild mon: the last four moves its species
// learns by that level, and whichever of its two ordinary abilities the
// personality happens to roll. That is right for a bush. It is not right for
// something standing in the overworld that the player has walked a long way to
// reach, so these run after setwildbattle and reshape the mon before the
// battle starts.
//
//   setwildbattle SPECIES_STEELIX, 25
//   callnative OrangeStaticMon_SunburstSteelix
//   setflag FLAG_SYS_SPECIAL_WILD_BATTLE
//   special StartLegendaryBattle
//
// Reshaping after setwildbattle rather than writing a second party builder
// keeps the encounter on the engine's own path: the mon is still a wild mon,
// so it can be caught, it keeps the held item setwildbattle gave it, and
// nothing about the capture or the summary screen is special-cased.

#include "global.h"
#include "event_data.h"
#include "pokemon.h"
#include "constants/abilities.h"
#include "constants/moves.h"
#include "constants/species.h"

static void GiveMoves(struct Pokemon *mon, const u16 *moves)
{
    u8 i;

    for (i = 0; i < MAX_MON_MOVES; i++)
    {
        u16 move = moves[i];
        u8 pp = gBattleMoves[move].pp;

        SetMonData(mon, MON_DATA_MOVE1 + i, &move);
        SetMonData(mon, MON_DATA_PP1 + i, &pp);
    }
}

// The STEELIX at the bottom of SUNBURST CAVE.
//
// SHEER FORCE is STEELIX's hidden ability, and it is the reason for this
// moveset rather than a decoration on top of it: it trades a move's secondary
// effect for thirty percent more power, so ROCK SLIDE and CRUNCH hit far
// harder than their numbers suggest while giving up the flinch and the defence
// drop. SCREECH is the move that is not boosted, and it is in there on
// purpose -- it is how the fight turns if it goes long, and it is the one
// thing the player can play around.
//
// DIG rather than EARTHQUAKE: EARTHQUAKE has no secondary effect for SHEER
// FORCE to trade away, and a turn spent underground is a turn the player gets
// to set up, which is the difference between a wall and an unwinnable one.
void OrangeStaticMon_SunburstSteelix(void)
{
    static const u16 sMoves[MAX_MON_MOVES] = {
        MOVE_ROCK_SLIDE, MOVE_CRUNCH, MOVE_DIG, MOVE_SCREECH,
    };
    struct Pokemon *mon = &gEnemyParty[0];
    u8 abilityNum = ABILITY_NUM_HIDDEN;

    GiveMoves(mon, sMoves);
    SetMonData(mon, MON_DATA_ABILITY_NUM, &abilityNum);
    CalculateMonStats(mon);
}
