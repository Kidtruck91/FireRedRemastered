// Trainer team pools. See include/trainer_pools.h for how pools work.
//
// To give a trainer a pooled team, set its party in src/data/trainers.h to e.g.
//     .party = POOL_PARTY(sPool_Hiker_Intermediate, 3, 5, 30, 8),
// (pool, fewest mons, most mons, base level, + up to this many random levels).
// Handwritten teams are untouched; only trainers you point at a pool use one.
//
// Tiers (a naming convention, copy the Hiker pools for other trainer types):
//   _Basic         early Pokemon, just a species (they use their normal level-up moves)
//   _Intermediate  mid-stage / some final forms, a few chosen items or moves where it matters
//   _Advanced      fully evolved only, full competitive sets (4 moves, item, ability).
//                  `make` refuses to build if an _Advanced entry breaks those rules.
//
// A species may appear more than once (e.g. two different Golem sets); a team never gets the
// same species twice.

// ---------------------------------------------------------------------------------------------
// Hiker

static const struct PoolMon sPool_Hiker_Basic[] =
{
    { .species = SPECIES_GEODUDE },
    { .species = SPECIES_ONIX },
    { .species = SPECIES_MACHOP },
    { .species = SPECIES_DIGLETT },
    { .species = SPECIES_SANDSHREW },
    { .species = SPECIES_CUBONE },
    { .species = SPECIES_MANKEY },
    { .species = SPECIES_RHYHORN },
    { .species = SPECIES_PHANPY },
    { .species = SPECIES_SWINUB },
    { .species = SPECIES_LARVITAR },
    { .species = SPECIES_GLIGAR },
    POOL_END
};

static const struct PoolMon sPool_Hiker_Intermediate[] =
{
    { .species = SPECIES_GRAVELER, .iv = 100 },
    { .species = SPECIES_MACHOKE,  .iv = 100, .heldItem = ITEM_BLACK_BELT },
    { .species = SPECIES_ONIX,     .iv = 100, .ability = 2 }, // Sturdy
    { .species = SPECIES_MAROWAK,  .iv = 100, .heldItem = ITEM_THICK_CLUB, .ability = 1 }, // Rock Head
    { .species = SPECIES_SANDSLASH, .iv = 100 },
    { .species = SPECIES_DUGTRIO,  .iv = 100, .heldItem = ITEM_SOFT_SAND },
    { .species = SPECIES_RHYHORN,  .iv = 100, .ability = 2 }, // Rock Head
    { .species = SPECIES_SUDOWOODO, .iv = 100,
      .moves = {MOVE_ROCK_SLIDE, MOVE_LOW_KICK, MOVE_FAINT_ATTACK, MOVE_MIMIC} },
    { .species = SPECIES_PILOSWINE, .iv = 100 },
    { .species = SPECIES_DONPHAN,  .iv = 100, .ability = 4 }, // Sand Veil (hidden)
    { .species = SPECIES_PUPITAR,  .iv = 100, .heldItem = ITEM_HARD_STONE },
    { .species = SPECIES_GLIGAR,   .iv = 100 },
    POOL_END
};

static const struct PoolMon sPool_Hiker_Advanced[] =
{
    { .species = SPECIES_GOLEM, .heldItem = ITEM_QUICK_CLAW, .iv = 255, .ability = 2, // Sturdy
      .moves = {MOVE_EARTHQUAKE, MOVE_ROCK_SLIDE, MOVE_EXPLOSION, MOVE_COUNTER} },
    { .species = SPECIES_MACHAMP, .heldItem = ITEM_LEFTOVERS, .iv = 255, .ability = 1, // Guts
      .moves = {MOVE_CROSS_CHOP, MOVE_ROCK_SLIDE, MOVE_EARTHQUAKE, MOVE_BULK_UP} },
    { .species = SPECIES_STEELIX, .heldItem = ITEM_LEFTOVERS, .iv = 255, .ability = 1, // Rock Head
      .moves = {MOVE_EARTHQUAKE, MOVE_IRON_TAIL, MOVE_EXPLOSION, MOVE_ROAR} },
    { .species = SPECIES_RHYDON, .heldItem = ITEM_QUICK_CLAW, .iv = 255, .ability = 2, // Rock Head
      .moves = {MOVE_EARTHQUAKE, MOVE_ROCK_SLIDE, MOVE_MEGAHORN, MOVE_SWORDS_DANCE} },
    { .species = SPECIES_MAROWAK, .heldItem = ITEM_THICK_CLUB, .iv = 255, .ability = 1, // Rock Head: no Double-Edge recoil
      .moves = {MOVE_BONEMERANG, MOVE_ROCK_SLIDE, MOVE_DOUBLE_EDGE, MOVE_SWORDS_DANCE} },
    { .species = SPECIES_DONPHAN, .heldItem = ITEM_LEFTOVERS, .iv = 255, .ability = 1, // Sturdy
      .moves = {MOVE_EARTHQUAKE, MOVE_ROCK_SLIDE, MOVE_RAPID_SPIN, MOVE_ROAR} },
    { .species = SPECIES_TYRANITAR, .heldItem = ITEM_LUM_BERRY, .iv = 255, .ability = 1, // Sand Stream
      .moves = {MOVE_DRAGON_DANCE, MOVE_ROCK_SLIDE, MOVE_EARTHQUAKE, MOVE_CRUNCH} },
    { .species = SPECIES_SUDOWOODO, .heldItem = ITEM_LEFTOVERS, .iv = 255, .ability = 2, // Rock Head
      .moves = {MOVE_ROCK_SLIDE, MOVE_EARTHQUAKE, MOVE_BRICK_BREAK, MOVE_EXPLOSION} },
    { .species = SPECIES_PILOSWINE, .heldItem = ITEM_LEFTOVERS, .iv = 255, .ability = 1, // Oblivious
      .moves = {MOVE_EARTHQUAKE, MOVE_ICE_BEAM, MOVE_ROCK_SLIDE, MOVE_PROTECT} },
    { .species = SPECIES_DUGTRIO, .heldItem = ITEM_CHOICE_BAND, .iv = 255, .ability = 2, // Arena Trap
      .moves = {MOVE_EARTHQUAKE, MOVE_ROCK_SLIDE, MOVE_AERIAL_ACE, MOVE_SUBSTITUTE} },
    { .species = SPECIES_GLIGAR, .heldItem = ITEM_SITRUS_BERRY, .iv = 255, .ability = 1, // Hyper Cutter
      .moves = {MOVE_EARTHQUAKE, MOVE_AERIAL_ACE, MOVE_SWORDS_DANCE, MOVE_ROCK_SLIDE} },
    POOL_END
};

// =============================================================================================
// DEMO: making a trainer that uses a pool (copy these three steps and uncomment)
// =============================================================================================
//
// 1) Give the trainer an ID in include/constants/opponents.h, after the last one
//    (TRAINER_NURSE_JOY is 926; there is room up to MAX_TRAINERS_COUNT, 992):
//
//        #define TRAINER_HIKER_DEMO                       927
//        #define TRAINER_HIKER_DEMO_2                     928   // optional rematch
//        #define TRAINER_SWIMMER_PAIR_DEMO                929   // optional double battle
//
//    and bump NUM_TRAINERS to match (927 -> 930 here).
//
// 2) Add the trainer to src/data/trainers.h, inside gTrainers[] like any other:
//
//    [TRAINER_HIKER_DEMO] = {
//        .trainerClass = TRAINER_CLASS_HIKER,
//        .encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_HIKER,
//        .trainerPic = TRAINER_PIC_HIKER,
//        .trainerName = _("DEMO"),
//        .items = {},
//        .doubleBattle = FALSE,
//        .aiFlags = AI_LEVEL_NORMAL,
//        .party = POOL_PARTY(sPool_Hiker_Intermediate, 3, 5, 30, 8),   // 3-5 mons, Lv 30-38
//    },
//
//    // A VS Seeker rematch is a separate entry, so it rolls its own (harder) team:
//    [TRAINER_HIKER_DEMO_2] = {
//        .trainerClass = TRAINER_CLASS_HIKER,
//        .encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_HIKER,
//        .trainerPic = TRAINER_PIC_HIKER,
//        .trainerName = _("DEMO"),
//        .items = {ITEM_HYPER_POTION},
//        .doubleBattle = FALSE,
//        .aiFlags = AI_LEVEL_ADVANCED,
//        .party = POOL_PARTY(sPool_Hiker_Advanced, 4, 5, 50, 15),     // 4-5 mons, Lv 50-65
//    },
//
//    // Double battle: always gets at least 2 Pokemon, even if the minimum says 1.
//    [TRAINER_SWIMMER_PAIR_DEMO] = {
//        .trainerClass = TRAINER_CLASS_SWIMMER_M,
//        .encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_SWIMMER,
//        .trainerPic = TRAINER_PIC_SWIMMER_M,
//        .trainerName = _("DEMO"),
//        .items = {},
//        .doubleBattle = TRUE,
//        .aiFlags = AI_LEVEL_NORMAL,
//        .party = POOL_PARTY(sPool_Hiker_Basic, 2, 4, 20, 5),           // (use a Swimmer pool)
//    },
//
//    A handwritten team can pick abilities too, without using a pool:
//        static const struct TrainerMonNoItemDefaultMoves sParty_Demo[] = {
//            { .iv = 0, .lvl = 12, .species = SPECIES_GEODUDE, .ability = 2 },   // Sturdy
//            { .iv = 0, .lvl = 12, .species = SPECIES_PHANPY,  .ability = 4 },   // Sand Veil (hidden)
//        };
//    .ability = 4 asks for the hidden ability (.hiddenAbility in src/data/pokemon/species_info.h);
//    `make` refuses a pool entry with .ability = 4 on a species that has none.
//
// 3) Battle them from a map script (data/maps/<Map>/scripts.inc), same as any trainer:
//
//    Route_EventScript_HikerDemo::
//        trainerbattle_single TRAINER_HIKER_DEMO, Route_Text_HikerDemoIntro, Route_Text_HikerDemoDefeat
//        msgbox Route_Text_HikerDemoPostBattle, MSGBOX_AUTOCLOSE
//        end
//
//    and give the trainer's object in the map's map.json that script plus a trainer_type /
//    sight range (Porymap: "Trainer Type: Normal", "Sight Radius: 3").
//
// The team that trainer gets is fixed for that save file, so losing and retrying shows the
// same team; a new game rolls a new one.
