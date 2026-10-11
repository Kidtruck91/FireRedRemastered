#ifndef GUARD_TRAINER_POOLS_H
#define GUARD_TRAINER_POOLS_H

// Trainer team pools.
//
// Instead of writing a fixed team for every trainer, a trainer can roll its team from a pool:
//
//     .party = POOL_PARTY(sPool_Hiker_Advanced, 3, 5, 50, 15),
//
// gives that trainer 3-5 different Pokemon from sPool_Hiker_Advanced, each at level 50 + 0..15.
// The roll is seeded from the trainer and the save file's Trainer ID, so the team is the same
// every time you fight that trainer in one playthrough (including after a loss), but different
// in a new game. VS Seeker rematches are separate trainer entries, so they roll their own team.
// Double battle trainers always get at least 2 Pokemon.
//
// Pools live in src/data/trainer_pools.h. An entry only needs a species; everything else is
// optional:
//     { .species = SPECIES_GRAVELER },                                  // level-up moves, no item
//     { .species = SPECIES_GOLEM, .heldItem = ITEM_QUICK_CLAW, .iv = 255, .ability = 2,
//       .moves = {MOVE_EARTHQUAKE, MOVE_ROCK_SLIDE, MOVE_EXPLOSION, MOVE_COUNTER} },
// and every pool ends with POOL_END.
//
// .ability: 0 or 3 = either, 1 = the species' first ability, 2 = its second ability
//           (ignored if the species only has one).
// .iv:      0-255, same scale as normal trainer parties (255 = perfect IVs).
//
// Pools whose name ends in _Advanced are checked when you build (scripts/check_trainer_pools.py):
// every entry must be fully evolved, have four moves, a held item and an ability choice.

struct PoolMon
{
    u16 species;
    u16 heldItem;
    u16 moves[MAX_MON_MOVES]; // all MOVE_NONE = normal level-up moves
    u8 iv;
    u8 ability;
};

#define POOL_END { .species = SPECIES_NONE }

#endif // GUARD_TRAINER_POOLS_H
