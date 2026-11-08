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
    { .species = SPECIES_DONPHAN,  .iv = 100 },
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
//    (TRAINER_NURSE_JOY is 926; there is room up to MAX_TRAINERS_COUNT, 1504):
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
//        };
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

// ---------------------------------------------------------------------------------------------
// Beauty

static const struct PoolMon sPool_Beauty_Intermediate[] =
{
    { .species = SPECIES_NINETALES, .iv = 100 },
    { .species = SPECIES_WIGGLYTUFF, .iv = 100 },
    { .species = SPECIES_SEAKING, .iv = 100 },
    { .species = SPECIES_STARMIE, .iv = 100 },
    { .species = SPECIES_BELLOSSOM, .iv = 100 },
    { .species = SPECIES_VAPOREON, .iv = 100 },
    { .species = SPECIES_ESPEON, .iv = 100 },
    { .species = SPECIES_PERSIAN, .iv = 100 },
    { .species = SPECIES_CLEFABLE, .iv = 100 },
    { .species = SPECIES_AZUMARILL, .iv = 100 },
    { .species = SPECIES_DELCATTY, .iv = 100 },
    POOL_END
};

static const struct PoolMon sPool_Beauty_Advanced[] =
{
    { .species = SPECIES_NINETALES, .heldItem = ITEM_LEFTOVERS, .iv = 255, .ability = 1, // Flash Fire
      .moves = {MOVE_FIRE_BLAST, MOVE_IRON_TAIL, MOVE_TOXIC, MOVE_DOUBLE_EDGE} },
    { .species = SPECIES_WIGGLYTUFF, .heldItem = ITEM_SILK_SCARF, .iv = 255, .ability = 1, // Cute Charm
      .moves = {MOVE_DOUBLE_EDGE, MOVE_FIRE_BLAST, MOVE_ICE_BEAM, MOVE_THUNDER_WAVE} },
    { .species = SPECIES_VAPOREON, .heldItem = ITEM_SITRUS_BERRY, .iv = 255, .ability = 1, // Water Absorb
      .moves = {MOVE_HYDRO_PUMP, MOVE_ICE_BEAM, MOVE_SHADOW_BALL, MOVE_TOXIC} },
    { .species = SPECIES_ESPEON, .heldItem = ITEM_TWISTED_SPOON, .iv = 255, .ability = 1, // Synchronize
      .moves = {MOVE_PSYCHIC, MOVE_SHADOW_BALL, MOVE_IRON_TAIL, MOVE_MORNING_SUN} },
    { .species = SPECIES_UMBREON, .heldItem = ITEM_LEFTOVERS, .iv = 255, .ability = 1, // Synchronize
      .moves = {MOVE_FAINT_ATTACK, MOVE_PSYCHIC, MOVE_SHADOW_BALL, MOVE_MOONLIGHT} },
    { .species = SPECIES_BELLOSSOM, .heldItem = ITEM_SCOPE_LENS, .iv = 255, .ability = 1, // Chlorophyll
      .moves = {MOVE_GIGA_DRAIN, MOVE_SLUDGE_BOMB, MOVE_TOXIC, MOVE_DOUBLE_EDGE} },
    { .species = SPECIES_MILOTIC, .heldItem = ITEM_LEFTOVERS, .iv = 255, .ability = 1, // Marvel Scale
      .moves = {MOVE_HYDRO_PUMP, MOVE_ICE_BEAM, MOVE_IRON_TAIL, MOVE_RECOVER} },
    { .species = SPECIES_LAPRAS, .heldItem = ITEM_MYSTIC_WATER, .iv = 255, .ability = 1, // Water Absorb
      .moves = {MOVE_HYDRO_PUMP, MOVE_ICE_BEAM, MOVE_THUNDERBOLT, MOVE_TOXIC} },
    { .species = SPECIES_STARMIE, .heldItem = ITEM_LEFTOVERS, .iv = 255, .ability = 2, // Natural Cure
      .moves = {MOVE_SURF, MOVE_PSYCHIC, MOVE_ICE_BEAM, MOVE_RECOVER} },
    { .species = SPECIES_PERSIAN, .heldItem = ITEM_LUM_BERRY, .iv = 255, .ability = 1, // Limber
      .moves = {MOVE_DOUBLE_EDGE, MOVE_THUNDERBOLT, MOVE_CRUNCH, MOVE_TOXIC} },
    { .species = SPECIES_KINGDRA, .heldItem = ITEM_QUICK_CLAW, .iv = 255, .ability = 1, // Swift Swim
      .moves = {MOVE_HYDRO_PUMP, MOVE_TWISTER, MOVE_ICE_BEAM, MOVE_TOXIC} },
    POOL_END
};
// ---------------------------------------------------------------------------------------------
// BirdKeeper

static const struct PoolMon sPool_BirdKeeper_Basic[] =
{
    { .species = SPECIES_PIDGEY },
    { .species = SPECIES_SPEAROW },
    { .species = SPECIES_DODUO },
    { .species = SPECIES_HOOTHOOT },
    { .species = SPECIES_NATU },
    { .species = SPECIES_TAILLOW },
    { .species = SPECIES_WINGULL },
    { .species = SPECIES_SWABLU },
    { .species = SPECIES_FARFETCHD },
    { .species = SPECIES_MURKROW },
    POOL_END
};

static const struct PoolMon sPool_BirdKeeper_Intermediate[] =
{
    { .species = SPECIES_PIDGEOTTO, .iv = 100 },
    { .species = SPECIES_FEAROW, .iv = 100 },
    { .species = SPECIES_DODRIO, .iv = 100 },
    { .species = SPECIES_NOCTOWL, .iv = 100 },
    { .species = SPECIES_XATU, .iv = 100 },
    { .species = SPECIES_SWELLOW, .iv = 100 },
    { .species = SPECIES_PELIPPER, .iv = 100 },
    { .species = SPECIES_ALTARIA, .iv = 100 },
    { .species = SPECIES_FARFETCHD, .iv = 100 },
    { .species = SPECIES_MURKROW, .iv = 100 },
    { .species = SPECIES_SKARMORY, .iv = 100 },
    POOL_END
};

static const struct PoolMon sPool_BirdKeeper_Advanced[] =
{
    { .species = SPECIES_PIDGEOT, .heldItem = ITEM_QUICK_CLAW, .iv = 255, .ability = 1, // Keen Eye
      .moves = {MOVE_DOUBLE_EDGE, MOVE_WING_ATTACK, MOVE_STEEL_WING, MOVE_TOXIC} },
    { .species = SPECIES_FEAROW, .heldItem = ITEM_SCOPE_LENS, .iv = 255, .ability = 1, // Keen Eye
      .moves = {MOVE_DOUBLE_EDGE, MOVE_DRILL_PECK, MOVE_STEEL_WING, MOVE_TOXIC} },
    { .species = SPECIES_NOCTOWL, .heldItem = ITEM_LEFTOVERS, .iv = 255, .ability = 1, // Insomnia
      .moves = {MOVE_DOUBLE_EDGE, MOVE_AERIAL_ACE, MOVE_PSYCHIC, MOVE_TOXIC} },
    { .species = SPECIES_DODRIO, .heldItem = ITEM_SILK_SCARF, .iv = 255, .ability = 2, // Early Bird
      .moves = {MOVE_DOUBLE_EDGE, MOVE_DRILL_PECK, MOVE_STEEL_WING, MOVE_TOXIC} },
    { .species = SPECIES_FARFETCHD, .heldItem = ITEM_SITRUS_BERRY, .iv = 255, .ability = 2, // Inner Focus
      .moves = {MOVE_DOUBLE_EDGE, MOVE_AERIAL_ACE, MOVE_IRON_TAIL, MOVE_SWORDS_DANCE} },
    { .species = SPECIES_XATU, .heldItem = ITEM_LUM_BERRY, .iv = 255, .ability = 1, // Synchronize
      .moves = {MOVE_PSYCHIC, MOVE_AERIAL_ACE, MOVE_SHADOW_BALL, MOVE_THUNDER_WAVE} },
    { .species = SPECIES_SKARMORY, .heldItem = ITEM_QUICK_CLAW, .iv = 255, .ability = 2, // Sturdy
      .moves = {MOVE_STEEL_WING, MOVE_AERIAL_ACE, MOVE_ROCK_SLIDE, MOVE_TOXIC} },
    { .species = SPECIES_CROBAT, .heldItem = ITEM_SCOPE_LENS, .iv = 255, .ability = 1, // Inner Focus
      .moves = {MOVE_SLUDGE_BOMB, MOVE_WING_ATTACK, MOVE_SHADOW_BALL, MOVE_TOXIC} },
    { .species = SPECIES_SWELLOW, .heldItem = ITEM_LEFTOVERS, .iv = 255, .ability = 1, // Guts
      .moves = {MOVE_DOUBLE_EDGE, MOVE_WING_ATTACK, MOVE_STEEL_WING, MOVE_TOXIC} },
    { .species = SPECIES_ALTARIA, .heldItem = ITEM_LEFTOVERS, .iv = 255, .ability = 1, // Natural Cure
      .moves = {MOVE_DRAGON_CLAW, MOVE_AERIAL_ACE, MOVE_FIRE_BLAST, MOVE_DRAGON_DANCE} },
    { .species = SPECIES_PELIPPER, .heldItem = ITEM_SITRUS_BERRY, .iv = 255, .ability = 1, // Keen Eye
      .moves = {MOVE_HYDRO_PUMP, MOVE_WING_ATTACK, MOVE_ICE_BEAM, MOVE_TOXIC} },
    { .species = SPECIES_MURKROW, .heldItem = ITEM_LUM_BERRY, .iv = 255, .ability = 1, // Insomnia
      .moves = {MOVE_FAINT_ATTACK, MOVE_AERIAL_ACE, MOVE_SHADOW_BALL, MOVE_THUNDER_WAVE} },
    POOL_END
};
// ---------------------------------------------------------------------------------------------
// BlackBelt

static const struct PoolMon sPool_BlackBelt_Basic[] =
{
    { .species = SPECIES_MACHOP },
    { .species = SPECIES_MANKEY },
    { .species = SPECIES_MAKUHITA },
    { .species = SPECIES_MEDITITE },
    { .species = SPECIES_TYROGUE },
    { .species = SPECIES_SHROOMISH },
    POOL_END
};

static const struct PoolMon sPool_BlackBelt_Intermediate[] =
{
    { .species = SPECIES_MACHOKE, .iv = 100 },
    { .species = SPECIES_PRIMEAPE, .iv = 100 },
    { .species = SPECIES_HITMONLEE, .iv = 100 },
    { .species = SPECIES_HITMONCHAN, .iv = 100 },
    { .species = SPECIES_HITMONTOP, .iv = 100 },
    { .species = SPECIES_HARIYAMA, .iv = 100 },
    { .species = SPECIES_MEDICHAM, .iv = 100 },
    { .species = SPECIES_BRELOOM, .iv = 100 },
    { .species = SPECIES_HERACROSS, .iv = 100 },
    POOL_END
};

static const struct PoolMon sPool_BlackBelt_Advanced[] =
{
    { .species = SPECIES_MACHAMP, .heldItem = ITEM_LUM_BERRY, .iv = 255, .ability = 1, // Guts
      .moves = {MOVE_CROSS_CHOP, MOVE_EARTHQUAKE, MOVE_ROCK_SLIDE, MOVE_BULK_UP} },
    { .species = SPECIES_PRIMEAPE, .heldItem = ITEM_LEFTOVERS, .iv = 255, .ability = 1, // Vital Spirit
      .moves = {MOVE_CROSS_CHOP, MOVE_EARTHQUAKE, MOVE_IRON_TAIL, MOVE_BULK_UP} },
    { .species = SPECIES_HITMONLEE, .heldItem = ITEM_SITRUS_BERRY, .iv = 255, .ability = 1, // Limber
      .moves = {MOVE_HI_JUMP_KICK, MOVE_EARTHQUAKE, MOVE_ROCK_SLIDE, MOVE_BULK_UP} },
    { .species = SPECIES_HITMONCHAN, .heldItem = ITEM_LUM_BERRY, .iv = 255, .ability = 1, // Keen Eye
      .moves = {MOVE_SKY_UPPERCUT, MOVE_EARTHQUAKE, MOVE_ROCK_SLIDE, MOVE_BULK_UP} },
    { .species = SPECIES_HITMONTOP, .heldItem = ITEM_LEFTOVERS, .iv = 255, .ability = 1, // Intimidate
      .moves = {MOVE_BRICK_BREAK, MOVE_EARTHQUAKE, MOVE_ROCK_SLIDE, MOVE_BULK_UP} },
    { .species = SPECIES_POLIWRATH, .heldItem = ITEM_SITRUS_BERRY, .iv = 255, .ability = 1, // Water Absorb
      .moves = {MOVE_HYDRO_PUMP, MOVE_BRICK_BREAK, MOVE_EARTHQUAKE, MOVE_BULK_UP} },
    { .species = SPECIES_HERACROSS, .heldItem = ITEM_LUM_BERRY, .iv = 255, .ability = 2, // Guts
      .moves = {MOVE_MEGAHORN, MOVE_BRICK_BREAK, MOVE_EARTHQUAKE, MOVE_SWORDS_DANCE} },
    { .species = SPECIES_HARIYAMA, .heldItem = ITEM_LEFTOVERS, .iv = 255, .ability = 1, // Thick Fat
      .moves = {MOVE_BRICK_BREAK, MOVE_EARTHQUAKE, MOVE_ROCK_SLIDE, MOVE_BULK_UP} },
    { .species = SPECIES_MEDICHAM, .heldItem = ITEM_SITRUS_BERRY, .iv = 255, .ability = 1, // Pure Power
      .moves = {MOVE_HI_JUMP_KICK, MOVE_PSYCHIC, MOVE_SHADOW_BALL, MOVE_BULK_UP} },
    { .species = SPECIES_BRELOOM, .heldItem = ITEM_LUM_BERRY, .iv = 255, .ability = 1, // Effect Spore
      .moves = {MOVE_GIGA_DRAIN, MOVE_SKY_UPPERCUT, MOVE_IRON_TAIL, MOVE_SWORDS_DANCE} },
    POOL_END
};
// ---------------------------------------------------------------------------------------------
// BugCatcher

static const struct PoolMon sPool_BugCatcher_Basic[] =
{
    { .species = SPECIES_CATERPIE },
    { .species = SPECIES_WEEDLE },
    { .species = SPECIES_METAPOD },
    { .species = SPECIES_KAKUNA },
    { .species = SPECIES_PARAS },
    { .species = SPECIES_VENONAT },
    { .species = SPECIES_LEDYBA },
    { .species = SPECIES_SPINARAK },
    { .species = SPECIES_WURMPLE },
    { .species = SPECIES_SURSKIT },
    { .species = SPECIES_NINCADA },
    POOL_END
};

static const struct PoolMon sPool_BugCatcher_Intermediate[] =
{
    { .species = SPECIES_BUTTERFREE, .iv = 100 },
    { .species = SPECIES_BEEDRILL, .iv = 100 },
    { .species = SPECIES_PARASECT, .iv = 100 },
    { .species = SPECIES_VENOMOTH, .iv = 100 },
    { .species = SPECIES_LEDIAN, .iv = 100 },
    { .species = SPECIES_ARIADOS, .iv = 100 },
    { .species = SPECIES_BEAUTIFLY, .iv = 100 },
    { .species = SPECIES_DUSTOX, .iv = 100 },
    { .species = SPECIES_MASQUERAIN, .iv = 100 },
    { .species = SPECIES_NINJASK, .iv = 100 },
    { .species = SPECIES_PINSIR, .iv = 100 },
    { .species = SPECIES_SCYTHER, .iv = 100 },
    POOL_END
};

static const struct PoolMon sPool_BugCatcher_Advanced[] =
{
    { .species = SPECIES_BUTTERFREE, .heldItem = ITEM_QUICK_CLAW, .iv = 255, .ability = 1, // Compound Eyes
      .moves = {MOVE_SIGNAL_BEAM, MOVE_AERIAL_ACE, MOVE_PSYCHIC, MOVE_SLEEP_POWDER} },
    { .species = SPECIES_BEEDRILL, .heldItem = ITEM_SITRUS_BERRY, .iv = 255, .ability = 1, // Swarm
      .moves = {MOVE_SILVER_WIND, MOVE_SLUDGE_BOMB, MOVE_BRICK_BREAK, MOVE_SWORDS_DANCE} },
    { .species = SPECIES_ARIADOS, .heldItem = ITEM_LEFTOVERS, .iv = 255, .ability = 1, // Swarm
      .moves = {MOVE_LEECH_LIFE, MOVE_SLUDGE_BOMB, MOVE_PSYCHIC, MOVE_TOXIC} },
    { .species = SPECIES_LEDIAN, .heldItem = ITEM_LEFTOVERS, .iv = 255, .ability = 2, // Early Bird
      .moves = {MOVE_AERIAL_ACE, MOVE_BRICK_BREAK, MOVE_GIGA_DRAIN, MOVE_SWORDS_DANCE} },
    { .species = SPECIES_SCIZOR, .heldItem = ITEM_SITRUS_BERRY, .iv = 255, .ability = 1, // Swarm
      .moves = {MOVE_FURY_CUTTER, MOVE_STEEL_WING, MOVE_AERIAL_ACE, MOVE_SWORDS_DANCE} },
    { .species = SPECIES_HERACROSS, .heldItem = ITEM_LUM_BERRY, .iv = 255, .ability = 2, // Guts
      .moves = {MOVE_MEGAHORN, MOVE_BRICK_BREAK, MOVE_EARTHQUAKE, MOVE_SWORDS_DANCE} },
    { .species = SPECIES_PINSIR, .heldItem = ITEM_LEFTOVERS, .iv = 255, .ability = 1, // Hyper Cutter
      .moves = {MOVE_MEGAHORN, MOVE_EARTHQUAKE, MOVE_SUPERPOWER, MOVE_SWORDS_DANCE} },
    { .species = SPECIES_VENOMOTH, .heldItem = ITEM_SCOPE_LENS, .iv = 255, .ability = 1, // Shield Dust
      .moves = {MOVE_SIGNAL_BEAM, MOVE_SLUDGE_BOMB, MOVE_PSYCHIC, MOVE_SLEEP_POWDER} },
    { .species = SPECIES_FORRETRESS, .heldItem = ITEM_LEFTOVERS, .iv = 255, .ability = 1, // Sturdy
      .moves = {MOVE_EARTHQUAKE, MOVE_ROCK_SLIDE, MOVE_GIGA_DRAIN, MOVE_TOXIC} },
    { .species = SPECIES_SHUCKLE, .heldItem = ITEM_HARD_STONE, .iv = 255, .ability = 1, // Sturdy
      .moves = {MOVE_ROCK_SLIDE, MOVE_EARTHQUAKE, MOVE_SLUDGE_BOMB, MOVE_TOXIC} },
    { .species = SPECIES_PARASECT, .heldItem = ITEM_SITRUS_BERRY, .iv = 255, .ability = 1, // Effect Spore
      .moves = {MOVE_SIGNAL_BEAM, MOVE_GIGA_DRAIN, MOVE_SLUDGE_BOMB, MOVE_SPORE} },
    { .species = SPECIES_YANMA, .heldItem = ITEM_LUM_BERRY, .iv = 255, .ability = 1, // Speed Boost
      .moves = {MOVE_WING_ATTACK, MOVE_SHADOW_BALL, MOVE_STEEL_WING, MOVE_TOXIC} },
    POOL_END
};
// ---------------------------------------------------------------------------------------------
// Camper

static const struct PoolMon sPool_Camper_Basic[] =
{
    { .species = SPECIES_MANKEY },
    { .species = SPECIES_GROWLITHE },
    { .species = SPECIES_PONYTA },
    { .species = SPECIES_NIDORAN_M },
    { .species = SPECIES_SANDSHREW },
    { .species = SPECIES_GEODUDE },
    { .species = SPECIES_MACHOP },
    { .species = SPECIES_PHANPY },
    { .species = SPECIES_TEDDIURSA },
    POOL_END
};

static const struct PoolMon sPool_Camper_Intermediate[] =
{
    { .species = SPECIES_PRIMEAPE, .iv = 100 },
    { .species = SPECIES_ARCANINE, .iv = 100 },
    { .species = SPECIES_RAPIDASH, .iv = 100 },
    { .species = SPECIES_NIDORINO, .iv = 100 },
    { .species = SPECIES_SANDSLASH, .iv = 100 },
    { .species = SPECIES_GRAVELER, .iv = 100 },
    { .species = SPECIES_MACHOKE, .iv = 100 },
    { .species = SPECIES_DONPHAN, .iv = 100 },
    { .species = SPECIES_URSARING, .iv = 100 },
    POOL_END
};

static const struct PoolMon sPool_Camper_Advanced[] =
{
    { .species = SPECIES_ARCANINE, .heldItem = ITEM_LEFTOVERS, .iv = 255, .ability = 1, // Intimidate
      .moves = {MOVE_FIRE_BLAST, MOVE_IRON_TAIL, MOVE_AERIAL_ACE, MOVE_TOXIC} },
    { .species = SPECIES_SANDSLASH, .heldItem = ITEM_LEFTOVERS, .iv = 255, .ability = 1, // Sand Veil
      .moves = {MOVE_EARTHQUAKE, MOVE_BRICK_BREAK, MOVE_IRON_TAIL, MOVE_SWORDS_DANCE} },
    { .species = SPECIES_PRIMEAPE, .heldItem = ITEM_SITRUS_BERRY, .iv = 255, .ability = 1, // Vital Spirit
      .moves = {MOVE_CROSS_CHOP, MOVE_EARTHQUAKE, MOVE_IRON_TAIL, MOVE_BULK_UP} },
    { .species = SPECIES_NIDOKING, .heldItem = ITEM_LUM_BERRY, .iv = 255, .ability = 1, // Poison Point
      .moves = {MOVE_SLUDGE_BOMB, MOVE_EARTHQUAKE, MOVE_MEGAHORN, MOVE_TOXIC} },
    { .species = SPECIES_RAPIDASH, .heldItem = ITEM_QUICK_CLAW, .iv = 255, .ability = 2, // Flash Fire
      .moves = {MOVE_FIRE_BLAST, MOVE_IRON_TAIL, MOVE_TOXIC, MOVE_DOUBLE_EDGE} },
    { .species = SPECIES_URSARING, .heldItem = ITEM_SITRUS_BERRY, .iv = 255, .ability = 1, // Guts
      .moves = {MOVE_DOUBLE_EDGE, MOVE_EARTHQUAKE, MOVE_BRICK_BREAK, MOVE_SWORDS_DANCE} },
    { .species = SPECIES_DONPHAN, .heldItem = ITEM_LEFTOVERS, .iv = 255, .ability = 1, // Sturdy
      .moves = {MOVE_EARTHQUAKE, MOVE_IRON_TAIL, MOVE_ROCK_SLIDE, MOVE_TOXIC} },
    { .species = SPECIES_TAUROS, .heldItem = ITEM_SILK_SCARF, .iv = 255, .ability = 1, // Intimidate
      .moves = {MOVE_DOUBLE_EDGE, MOVE_EARTHQUAKE, MOVE_IRON_TAIL, MOVE_TOXIC} },
    { .species = SPECIES_POLIWRATH, .heldItem = ITEM_SITRUS_BERRY, .iv = 255, .ability = 1, // Water Absorb
      .moves = {MOVE_HYDRO_PUMP, MOVE_BRICK_BREAK, MOVE_EARTHQUAKE, MOVE_BULK_UP} },
    { .species = SPECIES_MACHAMP, .heldItem = ITEM_LUM_BERRY, .iv = 255, .ability = 1, // Guts
      .moves = {MOVE_CROSS_CHOP, MOVE_EARTHQUAKE, MOVE_ROCK_SLIDE, MOVE_BULK_UP} },
    POOL_END
};
// ---------------------------------------------------------------------------------------------
// CooltrainerF

static const struct PoolMon sPool_CooltrainerF_Intermediate[] =
{
    { .species = SPECIES_IVYSAUR, .iv = 100 },
    { .species = SPECIES_WARTORTLE, .iv = 100 },
    { .species = SPECIES_CHARMELEON, .iv = 100 },
    { .species = SPECIES_FLAAFFY, .iv = 100 },
    { .species = SPECIES_NIDORINA, .iv = 100 },
    { .species = SPECIES_STARMIE, .iv = 100 },
    { .species = SPECIES_VAPOREON, .iv = 100 },
    { .species = SPECIES_JOLTEON, .iv = 100 },
    { .species = SPECIES_FLAREON, .iv = 100 },
    { .species = SPECIES_KIRLIA, .iv = 100 },
    { .species = SPECIES_SWABLU, .iv = 100 },
    { .species = SPECIES_DRAGONAIR, .iv = 100 },
    POOL_END
};

static const struct PoolMon sPool_CooltrainerF_Advanced[] =
{
    { .species = SPECIES_BLASTOISE, .heldItem = ITEM_LEFTOVERS, .iv = 255, .ability = 1, // Torrent
      .moves = {MOVE_HYDRO_PUMP, MOVE_EARTHQUAKE, MOVE_ICE_BEAM, MOVE_TOXIC} },
    { .species = SPECIES_VENUSAUR, .heldItem = ITEM_LEFTOVERS, .iv = 255, .ability = 1, // Overgrow
      .moves = {MOVE_GIGA_DRAIN, MOVE_SLUDGE_BOMB, MOVE_EARTHQUAKE, MOVE_SWORDS_DANCE} },
    { .species = SPECIES_AMPHAROS, .heldItem = ITEM_SITRUS_BERRY, .iv = 255, .ability = 1, // Static
      .moves = {MOVE_THUNDERBOLT, MOVE_IRON_TAIL, MOVE_BRICK_BREAK, MOVE_THUNDER_WAVE} },
    { .species = SPECIES_ESPEON, .heldItem = ITEM_TWISTED_SPOON, .iv = 255, .ability = 1, // Synchronize
      .moves = {MOVE_PSYCHIC, MOVE_SHADOW_BALL, MOVE_IRON_TAIL, MOVE_MORNING_SUN} },
    { .species = SPECIES_UMBREON, .heldItem = ITEM_LEFTOVERS, .iv = 255, .ability = 1, // Synchronize
      .moves = {MOVE_FAINT_ATTACK, MOVE_PSYCHIC, MOVE_SHADOW_BALL, MOVE_MOONLIGHT} },
    { .species = SPECIES_VAPOREON, .heldItem = ITEM_SCOPE_LENS, .iv = 255, .ability = 1, // Water Absorb
      .moves = {MOVE_HYDRO_PUMP, MOVE_ICE_BEAM, MOVE_SHADOW_BALL, MOVE_TOXIC} },
    { .species = SPECIES_JOLTEON, .heldItem = ITEM_LEFTOVERS, .iv = 255, .ability = 1, // Volt Absorb
      .moves = {MOVE_THUNDERBOLT, MOVE_SHADOW_BALL, MOVE_IRON_TAIL, MOVE_THUNDER_WAVE} },
    { .species = SPECIES_FLAREON, .heldItem = ITEM_CHARCOAL, .iv = 255, .ability = 1, // Flash Fire
      .moves = {MOVE_FIRE_BLAST, MOVE_SHADOW_BALL, MOVE_IRON_TAIL, MOVE_TOXIC} },
    { .species = SPECIES_STARMIE, .heldItem = ITEM_LEFTOVERS, .iv = 255, .ability = 2, // Natural Cure
      .moves = {MOVE_SURF, MOVE_PSYCHIC, MOVE_ICE_BEAM, MOVE_RECOVER} },
    { .species = SPECIES_LAPRAS, .heldItem = ITEM_LUM_BERRY, .iv = 255, .ability = 1, // Water Absorb
      .moves = {MOVE_HYDRO_PUMP, MOVE_ICE_BEAM, MOVE_THUNDERBOLT, MOVE_TOXIC} },
    { .species = SPECIES_CLEFABLE, .heldItem = ITEM_LEFTOVERS, .iv = 255, .ability = 1, // Cute Charm
      .moves = {MOVE_DOUBLE_EDGE, MOVE_FIRE_BLAST, MOVE_ICE_BEAM, MOVE_CALM_MIND} },
    { .species = SPECIES_GARDEVOIR, .heldItem = ITEM_SITRUS_BERRY, .iv = 255, .ability = 1, // Synchronize
      .moves = {MOVE_PSYCHIC, MOVE_THUNDERBOLT, MOVE_SHADOW_BALL, MOVE_CALM_MIND} },
    { .species = SPECIES_ALTARIA, .heldItem = ITEM_LUM_BERRY, .iv = 255, .ability = 1, // Natural Cure
      .moves = {MOVE_DRAGON_CLAW, MOVE_AERIAL_ACE, MOVE_FIRE_BLAST, MOVE_DRAGON_DANCE} },
    POOL_END
};
// ---------------------------------------------------------------------------------------------
// CooltrainerM

static const struct PoolMon sPool_CooltrainerM_Intermediate[] =
{
    { .species = SPECIES_CHARMELEON, .iv = 100 },
    { .species = SPECIES_WARTORTLE, .iv = 100 },
    { .species = SPECIES_IVYSAUR, .iv = 100 },
    { .species = SPECIES_CROCONAW, .iv = 100 },
    { .species = SPECIES_QUILAVA, .iv = 100 },
    { .species = SPECIES_BAYLEEF, .iv = 100 },
    { .species = SPECIES_NIDORINO, .iv = 100 },
    { .species = SPECIES_DRAGONAIR, .iv = 100 },
    { .species = SPECIES_MACHOKE, .iv = 100 },
    { .species = SPECIES_HAUNTER, .iv = 100 },
    { .species = SPECIES_MAGNETON, .iv = 100 },
    { .species = SPECIES_RHYHORN, .iv = 100 },
    POOL_END
};

static const struct PoolMon sPool_CooltrainerM_Advanced[] =
{
    { .species = SPECIES_DRAGONITE, .heldItem = ITEM_LUM_BERRY, .iv = 255, .ability = 1, // Inner Focus
      .moves = {MOVE_DRAGON_CLAW, MOVE_WING_ATTACK, MOVE_EARTHQUAKE, MOVE_DRAGON_DANCE} },
    { .species = SPECIES_KINGDRA, .heldItem = ITEM_MYSTIC_WATER, .iv = 255, .ability = 1, // Swift Swim
      .moves = {MOVE_HYDRO_PUMP, MOVE_TWISTER, MOVE_ICE_BEAM, MOVE_TOXIC} },
    { .species = SPECIES_RHYDON, .heldItem = ITEM_SITRUS_BERRY, .iv = 255, .ability = 1, // Lightning Rod
      .moves = {MOVE_EARTHQUAKE, MOVE_ROCK_SLIDE, MOVE_MEGAHORN, MOVE_SWORDS_DANCE} },
    { .species = SPECIES_NIDOKING, .heldItem = ITEM_LUM_BERRY, .iv = 255, .ability = 1, // Poison Point
      .moves = {MOVE_SLUDGE_BOMB, MOVE_EARTHQUAKE, MOVE_MEGAHORN, MOVE_TOXIC} },
    { .species = SPECIES_EXEGGUTOR, .heldItem = ITEM_QUICK_CLAW, .iv = 255, .ability = 1, // Chlorophyll
      .moves = {MOVE_GIGA_DRAIN, MOVE_PSYCHIC, MOVE_SLUDGE_BOMB, MOVE_TOXIC} },
    { .species = SPECIES_MAGNETON, .heldItem = ITEM_SCOPE_LENS, .iv = 255, .ability = 2, // Sturdy
      .moves = {MOVE_THUNDERBOLT, MOVE_METAL_CLAW, MOVE_THUNDER_WAVE, MOVE_DOUBLE_EDGE} },
    { .species = SPECIES_CHARIZARD, .heldItem = ITEM_LUM_BERRY, .iv = 255, .ability = 1, // Blaze
      .moves = {MOVE_FIRE_BLAST, MOVE_WING_ATTACK, MOVE_EARTHQUAKE, MOVE_SWORDS_DANCE} },
    { .species = SPECIES_FERALIGATR, .heldItem = ITEM_LEFTOVERS, .iv = 255, .ability = 1, // Torrent
      .moves = {MOVE_HYDRO_PUMP, MOVE_EARTHQUAKE, MOVE_BRICK_BREAK, MOVE_SWORDS_DANCE} },
    { .species = SPECIES_TYPHLOSION, .heldItem = ITEM_SITRUS_BERRY, .iv = 255, .ability = 1, // Blaze
      .moves = {MOVE_FIRE_BLAST, MOVE_EARTHQUAKE, MOVE_BRICK_BREAK, MOVE_TOXIC} },
    { .species = SPECIES_MEGANIUM, .heldItem = ITEM_LUM_BERRY, .iv = 255, .ability = 1, // Overgrow
      .moves = {MOVE_GIGA_DRAIN, MOVE_EARTHQUAKE, MOVE_IRON_TAIL, MOVE_SWORDS_DANCE} },
    { .species = SPECIES_GENGAR, .heldItem = ITEM_QUICK_CLAW, .iv = 255, .ability = 1, // Levitate
      .moves = {MOVE_SHADOW_BALL, MOVE_SLUDGE_BOMB, MOVE_THUNDERBOLT, MOVE_TOXIC} },
    { .species = SPECIES_SCIZOR, .heldItem = ITEM_SITRUS_BERRY, .iv = 255, .ability = 1, // Swarm
      .moves = {MOVE_FURY_CUTTER, MOVE_STEEL_WING, MOVE_AERIAL_ACE, MOVE_SWORDS_DANCE} },
    { .species = SPECIES_HERACROSS, .heldItem = ITEM_LUM_BERRY, .iv = 255, .ability = 2, // Guts
      .moves = {MOVE_MEGAHORN, MOVE_BRICK_BREAK, MOVE_EARTHQUAKE, MOVE_SWORDS_DANCE} },
    POOL_END
};
// ---------------------------------------------------------------------------------------------
// Fisherman

static const struct PoolMon sPool_Fisherman_Basic[] =
{
    { .species = SPECIES_MAGIKARP },
    { .species = SPECIES_GOLDEEN },
    { .species = SPECIES_POLIWAG },
    { .species = SPECIES_TENTACOOL },
    { .species = SPECIES_KRABBY },
    { .species = SPECIES_REMORAID },
    { .species = SPECIES_SHELLDER },
    { .species = SPECIES_HORSEA },
    { .species = SPECIES_CHINCHOU },
    { .species = SPECIES_QWILFISH },
    { .species = SPECIES_BARBOACH },
    { .species = SPECIES_CARVANHA },
    POOL_END
};

static const struct PoolMon sPool_Fisherman_Intermediate[] =
{
    { .species = SPECIES_GYARADOS, .iv = 100 },
    { .species = SPECIES_SEAKING, .iv = 100 },
    { .species = SPECIES_KINGLER, .iv = 100 },
    { .species = SPECIES_OCTILLERY, .iv = 100 },
    { .species = SPECIES_TENTACRUEL, .iv = 100 },
    { .species = SPECIES_QWILFISH, .iv = 100 },
    { .species = SPECIES_LANTURN, .iv = 100 },
    { .species = SPECIES_POLIWHIRL, .iv = 100 },
    { .species = SPECIES_WHISCASH, .iv = 100 },
    { .species = SPECIES_SHARPEDO, .iv = 100 },
    { .species = SPECIES_CLOYSTER, .iv = 100 },
    { .species = SPECIES_SEADRA, .iv = 100 },
    POOL_END
};

static const struct PoolMon sPool_Fisherman_Advanced[] =
{
    { .species = SPECIES_GYARADOS, .heldItem = ITEM_LUM_BERRY, .iv = 255, .ability = 1, // Intimidate
      .moves = {MOVE_WATERFALL, MOVE_EARTHQUAKE, MOVE_FIRE_BLAST, MOVE_DRAGON_DANCE} },
    { .species = SPECIES_SEAKING, .heldItem = ITEM_QUICK_CLAW, .iv = 255, .ability = 1, // Swift Swim
      .moves = {MOVE_SURF, MOVE_ICE_BEAM, MOVE_MEGAHORN, MOVE_TOXIC} },
    { .species = SPECIES_KINGLER, .heldItem = ITEM_SITRUS_BERRY, .iv = 255, .ability = 1, // Hyper Cutter
      .moves = {MOVE_CRABHAMMER, MOVE_METAL_CLAW, MOVE_ROCK_TOMB, MOVE_SWORDS_DANCE} },
    { .species = SPECIES_OCTILLERY, .heldItem = ITEM_LEFTOVERS, .iv = 255, .ability = 1, // Suction Cups
      .moves = {MOVE_SURF, MOVE_FIRE_BLAST, MOVE_ICE_BEAM, MOVE_THUNDER_WAVE} },
    { .species = SPECIES_QWILFISH, .heldItem = ITEM_LEFTOVERS, .iv = 255, .ability = 2, // Swift Swim
      .moves = {MOVE_WATERFALL, MOVE_SLUDGE_BOMB, MOVE_SHADOW_BALL, MOVE_SWORDS_DANCE} },
    { .species = SPECIES_LANTURN, .heldItem = ITEM_SITRUS_BERRY, .iv = 255, .ability = 1, // Volt Absorb
      .moves = {MOVE_HYDRO_PUMP, MOVE_THUNDERBOLT, MOVE_ICE_BEAM, MOVE_THUNDER_WAVE} },
    { .species = SPECIES_POLIWRATH, .heldItem = ITEM_LUM_BERRY, .iv = 255, .ability = 1, // Water Absorb
      .moves = {MOVE_HYDRO_PUMP, MOVE_BRICK_BREAK, MOVE_EARTHQUAKE, MOVE_BULK_UP} },
    { .species = SPECIES_POLITOED, .heldItem = ITEM_QUICK_CLAW, .iv = 255, .ability = 1, // Water Absorb
      .moves = {MOVE_SURF, MOVE_ICE_BEAM, MOVE_PSYCHIC, MOVE_TOXIC} },
    { .species = SPECIES_TENTACRUEL, .heldItem = ITEM_SCOPE_LENS, .iv = 255, .ability = 1, // Clear Body
      .moves = {MOVE_HYDRO_PUMP, MOVE_SLUDGE_BOMB, MOVE_ICE_BEAM, MOVE_TOXIC} },
    { .species = SPECIES_QUAGSIRE, .heldItem = ITEM_LEFTOVERS, .iv = 255, .ability = 2, // Water Absorb
      .moves = {MOVE_SURF, MOVE_EARTHQUAKE, MOVE_SLUDGE_BOMB, MOVE_TOXIC} },
    { .species = SPECIES_WHISCASH, .heldItem = ITEM_MYSTIC_WATER, .iv = 255, .ability = 1, // Oblivious
      .moves = {MOVE_SURF, MOVE_EARTHQUAKE, MOVE_ICE_BEAM, MOVE_TOXIC} },
    { .species = SPECIES_CLOYSTER, .heldItem = ITEM_SITRUS_BERRY, .iv = 255, .ability = 1, // Shell Armor
      .moves = {MOVE_SURF, MOVE_ICE_BEAM, MOVE_TOXIC, MOVE_DOUBLE_EDGE} },
    POOL_END
};
// ---------------------------------------------------------------------------------------------
// Juggler

static const struct PoolMon sPool_Juggler_Advanced[] =
{
    { .species = SPECIES_ELECTRODE, .heldItem = ITEM_MAGNET, .iv = 255, .ability = 2, // Static
      .moves = {MOVE_THUNDERBOLT, MOVE_THIEF, MOVE_THUNDER_WAVE, MOVE_SWIFT} },
    { .species = SPECIES_MR_MIME, .heldItem = ITEM_SITRUS_BERRY, .iv = 255, .ability = 1, // Soundproof
      .moves = {MOVE_PSYCHIC, MOVE_THUNDERBOLT, MOVE_MAGICAL_LEAF, MOVE_CALM_MIND} },
    { .species = SPECIES_HYPNO, .heldItem = ITEM_LUM_BERRY, .iv = 255, .ability = 1, // Insomnia
      .moves = {MOVE_PSYCHIC, MOVE_SHADOW_BALL, MOVE_BRICK_BREAK, MOVE_THUNDER_WAVE} },
    { .species = SPECIES_ALAKAZAM, .heldItem = ITEM_LEFTOVERS, .iv = 255, .ability = 1, // Synchronize
      .moves = {MOVE_PSYCHIC, MOVE_SHOCK_WAVE, MOVE_SHADOW_BALL, MOVE_CALM_MIND} },
    { .species = SPECIES_XATU, .heldItem = ITEM_SCOPE_LENS, .iv = 255, .ability = 1, // Synchronize
      .moves = {MOVE_PSYCHIC, MOVE_AERIAL_ACE, MOVE_SHADOW_BALL, MOVE_THUNDER_WAVE} },
    { .species = SPECIES_STARMIE, .heldItem = ITEM_LEFTOVERS, .iv = 255, .ability = 2, // Natural Cure
      .moves = {MOVE_SURF, MOVE_PSYCHIC, MOVE_ICE_BEAM, MOVE_RECOVER} },
    { .species = SPECIES_EXEGGUTOR, .heldItem = ITEM_MIRACLE_SEED, .iv = 255, .ability = 1, // Chlorophyll
      .moves = {MOVE_GIGA_DRAIN, MOVE_PSYCHIC, MOVE_SLUDGE_BOMB, MOVE_TOXIC} },
    { .species = SPECIES_GOLEM, .heldItem = ITEM_SITRUS_BERRY, .iv = 255, .ability = 2, // Sturdy
      .moves = {MOVE_ROCK_SLIDE, MOVE_EARTHQUAKE, MOVE_BRICK_BREAK, MOVE_TOXIC} },
    { .species = SPECIES_CLAYDOL, .heldItem = ITEM_LUM_BERRY, .iv = 255, .ability = 1, // Levitate
      .moves = {MOVE_EARTHQUAKE, MOVE_PSYCHIC, MOVE_ICE_BEAM, MOVE_TOXIC} },
    { .species = SPECIES_MISDREAVUS, .heldItem = ITEM_LEFTOVERS, .iv = 255, .ability = 1, // Levitate
      .moves = {MOVE_SHADOW_BALL, MOVE_THUNDERBOLT, MOVE_PSYCHIC, MOVE_CALM_MIND} },
    POOL_END
};
// ---------------------------------------------------------------------------------------------
// Kindler

static const struct PoolMon sPool_Kindler_Advanced[] =
{
    { .species = SPECIES_MAGMAR, .heldItem = ITEM_CHARCOAL, .iv = 255, .ability = 1, // Flame Body
      .moves = {MOVE_FIRE_BLAST, MOVE_PSYCHIC, MOVE_BRICK_BREAK, MOVE_TOXIC} },
    { .species = SPECIES_ARCANINE, .heldItem = ITEM_SITRUS_BERRY, .iv = 255, .ability = 1, // Intimidate
      .moves = {MOVE_FIRE_BLAST, MOVE_IRON_TAIL, MOVE_AERIAL_ACE, MOVE_TOXIC} },
    { .species = SPECIES_RAPIDASH, .heldItem = ITEM_LUM_BERRY, .iv = 255, .ability = 2, // Flash Fire
      .moves = {MOVE_FIRE_BLAST, MOVE_IRON_TAIL, MOVE_TOXIC, MOVE_DOUBLE_EDGE} },
    { .species = SPECIES_NINETALES, .heldItem = ITEM_QUICK_CLAW, .iv = 255, .ability = 1, // Flash Fire
      .moves = {MOVE_FIRE_BLAST, MOVE_IRON_TAIL, MOVE_TOXIC, MOVE_DOUBLE_EDGE} },
    { .species = SPECIES_HOUNDOOM, .heldItem = ITEM_SCOPE_LENS, .iv = 255, .ability = 2, // Flash Fire
      .moves = {MOVE_CRUNCH, MOVE_FIRE_BLAST, MOVE_SLUDGE_BOMB, MOVE_TOXIC} },
    { .species = SPECIES_TYPHLOSION, .heldItem = ITEM_LEFTOVERS, .iv = 255, .ability = 1, // Blaze
      .moves = {MOVE_FIRE_BLAST, MOVE_EARTHQUAKE, MOVE_BRICK_BREAK, MOVE_TOXIC} },
    { .species = SPECIES_TORKOAL, .heldItem = ITEM_CHARCOAL, .iv = 255, .ability = 1, // White Smoke
      .moves = {MOVE_FIRE_BLAST, MOVE_SLUDGE_BOMB, MOVE_IRON_TAIL, MOVE_TOXIC} },
    { .species = SPECIES_CAMERUPT, .heldItem = ITEM_SITRUS_BERRY, .iv = 255, .ability = 1, // Magma Armor
      .moves = {MOVE_FIRE_BLAST, MOVE_EARTHQUAKE, MOVE_ROCK_SLIDE, MOVE_TOXIC} },
    { .species = SPECIES_MAGCARGO, .heldItem = ITEM_LUM_BERRY, .iv = 255, .ability = 2, // Flame Body
      .moves = {MOVE_FIRE_BLAST, MOVE_ROCK_SLIDE, MOVE_EARTHQUAKE, MOVE_TOXIC} },
    { .species = SPECIES_FLAREON, .heldItem = ITEM_QUICK_CLAW, .iv = 255, .ability = 1, // Flash Fire
      .moves = {MOVE_FIRE_BLAST, MOVE_SHADOW_BALL, MOVE_IRON_TAIL, MOVE_TOXIC} },
    { .species = SPECIES_CHARIZARD, .heldItem = ITEM_SITRUS_BERRY, .iv = 255, .ability = 1, // Blaze
      .moves = {MOVE_FIRE_BLAST, MOVE_WING_ATTACK, MOVE_EARTHQUAKE, MOVE_SWORDS_DANCE} },
    POOL_END
};
// ---------------------------------------------------------------------------------------------
// Lass

static const struct PoolMon sPool_Lass_Basic[] =
{
    { .species = SPECIES_PIDGEY },
    { .species = SPECIES_JIGGLYPUFF },
    { .species = SPECIES_CLEFAIRY },
    { .species = SPECIES_ODDISH },
    { .species = SPECIES_BELLSPROUT },
    { .species = SPECIES_HOPPIP },
    { .species = SPECIES_SKITTY },
    { .species = SPECIES_MARILL },
    { .species = SPECIES_NIDORAN_F },
    { .species = SPECIES_SNUBBULL },
    POOL_END
};

static const struct PoolMon sPool_Lass_Intermediate[] =
{
    { .species = SPECIES_PIDGEOTTO, .iv = 100 },
    { .species = SPECIES_WIGGLYTUFF, .iv = 100 },
    { .species = SPECIES_CLEFABLE, .iv = 100 },
    { .species = SPECIES_GLOOM, .iv = 100 },
    { .species = SPECIES_WEEPINBELL, .iv = 100 },
    { .species = SPECIES_SKIPLOOM, .iv = 100 },
    { .species = SPECIES_DELCATTY, .iv = 100 },
    { .species = SPECIES_AZUMARILL, .iv = 100 },
    { .species = SPECIES_NIDORINA, .iv = 100 },
    { .species = SPECIES_GRANBULL, .iv = 100 },
    POOL_END
};

static const struct PoolMon sPool_Lass_Advanced[] =
{
    { .species = SPECIES_CLEFABLE, .heldItem = ITEM_LEFTOVERS, .iv = 255, .ability = 1, // Cute Charm
      .moves = {MOVE_DOUBLE_EDGE, MOVE_FIRE_BLAST, MOVE_ICE_BEAM, MOVE_CALM_MIND} },
    { .species = SPECIES_WIGGLYTUFF, .heldItem = ITEM_SCOPE_LENS, .iv = 255, .ability = 1, // Cute Charm
      .moves = {MOVE_DOUBLE_EDGE, MOVE_FIRE_BLAST, MOVE_ICE_BEAM, MOVE_THUNDER_WAVE} },
    { .species = SPECIES_VILEPLUME, .heldItem = ITEM_LEFTOVERS, .iv = 255, .ability = 1, // Chlorophyll
      .moves = {MOVE_GIGA_DRAIN, MOVE_SLUDGE_BOMB, MOVE_TOXIC, MOVE_DOUBLE_EDGE} },
    { .species = SPECIES_PERSIAN, .heldItem = ITEM_SILK_SCARF, .iv = 255, .ability = 1, // Limber
      .moves = {MOVE_DOUBLE_EDGE, MOVE_THUNDERBOLT, MOVE_CRUNCH, MOVE_TOXIC} },
    { .species = SPECIES_AZUMARILL, .heldItem = ITEM_SITRUS_BERRY, .iv = 255, .ability = 2, // Huge Power
      .moves = {MOVE_HYDRO_PUMP, MOVE_ICE_BEAM, MOVE_BRICK_BREAK, MOVE_TOXIC} },
    { .species = SPECIES_DELCATTY, .heldItem = ITEM_LUM_BERRY, .iv = 255, .ability = 1, // Cute Charm
      .moves = {MOVE_DOUBLE_EDGE, MOVE_ICE_BEAM, MOVE_THUNDERBOLT, MOVE_CALM_MIND} },
    { .species = SPECIES_NINETALES, .heldItem = ITEM_QUICK_CLAW, .iv = 255, .ability = 1, // Flash Fire
      .moves = {MOVE_FIRE_BLAST, MOVE_IRON_TAIL, MOVE_TOXIC, MOVE_DOUBLE_EDGE} },
    { .species = SPECIES_JYNX, .heldItem = ITEM_SITRUS_BERRY, .iv = 255, .ability = 1, // Oblivious
      .moves = {MOVE_ICE_BEAM, MOVE_PSYCHIC, MOVE_WATER_PULSE, MOVE_CALM_MIND} },
    { .species = SPECIES_GRANBULL, .heldItem = ITEM_LUM_BERRY, .iv = 255, .ability = 1, // Intimidate
      .moves = {MOVE_DOUBLE_EDGE, MOVE_EARTHQUAKE, MOVE_BRICK_BREAK, MOVE_BULK_UP} },
    { .species = SPECIES_LEDIAN, .heldItem = ITEM_LEFTOVERS, .iv = 255, .ability = 2, // Early Bird
      .moves = {MOVE_AERIAL_ACE, MOVE_BRICK_BREAK, MOVE_GIGA_DRAIN, MOVE_SWORDS_DANCE} },
    { .species = SPECIES_BELLOSSOM, .heldItem = ITEM_SITRUS_BERRY, .iv = 255, .ability = 1, // Chlorophyll
      .moves = {MOVE_GIGA_DRAIN, MOVE_SLUDGE_BOMB, MOVE_TOXIC, MOVE_DOUBLE_EDGE} },
    POOL_END
};
// ---------------------------------------------------------------------------------------------
// Picnicker

static const struct PoolMon sPool_Picnicker_Basic[] =
{
    { .species = SPECIES_ODDISH },
    { .species = SPECIES_HOPPIP },
    { .species = SPECIES_PIDGEY },
    { .species = SPECIES_VULPIX },
    { .species = SPECIES_SNUBBULL },
    { .species = SPECIES_MARILL },
    { .species = SPECIES_BELLSPROUT },
    { .species = SPECIES_SKITTY },
    { .species = SPECIES_SHROOMISH },
    { .species = SPECIES_NIDORAN_F },
    POOL_END
};

static const struct PoolMon sPool_Picnicker_Intermediate[] =
{
    { .species = SPECIES_GLOOM, .iv = 100 },
    { .species = SPECIES_SKIPLOOM, .iv = 100 },
    { .species = SPECIES_PIDGEOTTO, .iv = 100 },
    { .species = SPECIES_NINETALES, .iv = 100 },
    { .species = SPECIES_GRANBULL, .iv = 100 },
    { .species = SPECIES_AZUMARILL, .iv = 100 },
    { .species = SPECIES_WEEPINBELL, .iv = 100 },
    { .species = SPECIES_DELCATTY, .iv = 100 },
    { .species = SPECIES_BRELOOM, .iv = 100 },
    { .species = SPECIES_NIDORINA, .iv = 100 },
    POOL_END
};

static const struct PoolMon sPool_Picnicker_Advanced[] =
{
    { .species = SPECIES_CLEFABLE, .heldItem = ITEM_LUM_BERRY, .iv = 255, .ability = 1, // Cute Charm
      .moves = {MOVE_DOUBLE_EDGE, MOVE_FIRE_BLAST, MOVE_ICE_BEAM, MOVE_CALM_MIND} },
    { .species = SPECIES_WIGGLYTUFF, .heldItem = ITEM_QUICK_CLAW, .iv = 255, .ability = 1, // Cute Charm
      .moves = {MOVE_DOUBLE_EDGE, MOVE_FIRE_BLAST, MOVE_ICE_BEAM, MOVE_THUNDER_WAVE} },
    { .species = SPECIES_NIDOQUEEN, .heldItem = ITEM_SCOPE_LENS, .iv = 255, .ability = 1, // Poison Point
      .moves = {MOVE_SLUDGE_BOMB, MOVE_EARTHQUAKE, MOVE_FIRE_BLAST, MOVE_TOXIC} },
    { .species = SPECIES_BELLOSSOM, .heldItem = ITEM_LEFTOVERS, .iv = 255, .ability = 1, // Chlorophyll
      .moves = {MOVE_GIGA_DRAIN, MOVE_SLUDGE_BOMB, MOVE_TOXIC, MOVE_DOUBLE_EDGE} },
    { .species = SPECIES_JUMPLUFF, .heldItem = ITEM_LEFTOVERS, .iv = 255, .ability = 1, // Chlorophyll
      .moves = {MOVE_GIGA_DRAIN, MOVE_AERIAL_ACE, MOVE_SWORDS_DANCE, MOVE_DOUBLE_EDGE} },
    { .species = SPECIES_AZUMARILL, .heldItem = ITEM_SITRUS_BERRY, .iv = 255, .ability = 2, // Huge Power
      .moves = {MOVE_HYDRO_PUMP, MOVE_ICE_BEAM, MOVE_BRICK_BREAK, MOVE_TOXIC} },
    { .species = SPECIES_AMPHAROS, .heldItem = ITEM_LUM_BERRY, .iv = 255, .ability = 1, // Static
      .moves = {MOVE_THUNDERBOLT, MOVE_IRON_TAIL, MOVE_BRICK_BREAK, MOVE_THUNDER_WAVE} },
    { .species = SPECIES_GRANBULL, .heldItem = ITEM_LEFTOVERS, .iv = 255, .ability = 1, // Intimidate
      .moves = {MOVE_DOUBLE_EDGE, MOVE_EARTHQUAKE, MOVE_BRICK_BREAK, MOVE_BULK_UP} },
    { .species = SPECIES_TOGETIC, .heldItem = ITEM_SILK_SCARF, .iv = 255, .ability = 2, // Serene Grace
      .moves = {MOVE_DOUBLE_EDGE, MOVE_AERIAL_ACE, MOVE_FIRE_BLAST, MOVE_SOFT_BOILED} },
    { .species = SPECIES_DELCATTY, .heldItem = ITEM_LUM_BERRY, .iv = 255, .ability = 1, // Cute Charm
      .moves = {MOVE_DOUBLE_EDGE, MOVE_ICE_BEAM, MOVE_THUNDERBOLT, MOVE_CALM_MIND} },
    { .species = SPECIES_MILTANK, .heldItem = ITEM_SILK_SCARF, .iv = 255, .ability = 1, // Thick Fat
      .moves = {MOVE_DOUBLE_EDGE, MOVE_EARTHQUAKE, MOVE_BRICK_BREAK, MOVE_MILK_DRINK} },
    POOL_END
};
// ---------------------------------------------------------------------------------------------
// Pokefan

static const struct PoolMon sPool_Pokefan_Advanced[] =
{
    { .species = SPECIES_CLEFABLE, .heldItem = ITEM_LEFTOVERS, .iv = 255, .ability = 1, // Cute Charm
      .moves = {MOVE_DOUBLE_EDGE, MOVE_FIRE_BLAST, MOVE_ICE_BEAM, MOVE_CALM_MIND} },
    { .species = SPECIES_WIGGLYTUFF, .heldItem = ITEM_SITRUS_BERRY, .iv = 255, .ability = 1, // Cute Charm
      .moves = {MOVE_DOUBLE_EDGE, MOVE_FIRE_BLAST, MOVE_ICE_BEAM, MOVE_THUNDER_WAVE} },
    { .species = SPECIES_FURRET, .heldItem = ITEM_LUM_BERRY, .iv = 255, .ability = 1, // Run Away
      .moves = {MOVE_DOUBLE_EDGE, MOVE_BRICK_BREAK, MOVE_IRON_TAIL, MOVE_TOXIC} },
    { .species = SPECIES_RAICHU, .heldItem = ITEM_QUICK_CLAW, .iv = 255, .ability = 1, // Static
      .moves = {MOVE_THUNDERBOLT, MOVE_IRON_TAIL, MOVE_BRICK_BREAK, MOVE_THUNDER_WAVE} },
    { .species = SPECIES_TOGETIC, .heldItem = ITEM_SILK_SCARF, .iv = 255, .ability = 2, // Serene Grace
      .moves = {MOVE_DOUBLE_EDGE, MOVE_AERIAL_ACE, MOVE_FIRE_BLAST, MOVE_SOFT_BOILED} },
    { .species = SPECIES_MILTANK, .heldItem = ITEM_LEFTOVERS, .iv = 255, .ability = 1, // Thick Fat
      .moves = {MOVE_DOUBLE_EDGE, MOVE_EARTHQUAKE, MOVE_BRICK_BREAK, MOVE_MILK_DRINK} },
    { .species = SPECIES_AZUMARILL, .heldItem = ITEM_MYSTIC_WATER, .iv = 255, .ability = 2, // Huge Power
      .moves = {MOVE_HYDRO_PUMP, MOVE_ICE_BEAM, MOVE_BRICK_BREAK, MOVE_TOXIC} },
    { .species = SPECIES_DELCATTY, .heldItem = ITEM_SITRUS_BERRY, .iv = 255, .ability = 1, // Cute Charm
      .moves = {MOVE_DOUBLE_EDGE, MOVE_ICE_BEAM, MOVE_THUNDERBOLT, MOVE_CALM_MIND} },
    { .species = SPECIES_PLUSLE, .heldItem = ITEM_LUM_BERRY, .iv = 255, .ability = 1, // Plus
      .moves = {MOVE_THUNDERBOLT, MOVE_IRON_TAIL, MOVE_THUNDER_WAVE, MOVE_DOUBLE_EDGE} },
    { .species = SPECIES_MINUN, .heldItem = ITEM_QUICK_CLAW, .iv = 255, .ability = 1, // Minus
      .moves = {MOVE_THUNDERBOLT, MOVE_IRON_TAIL, MOVE_THUNDER_WAVE, MOVE_DOUBLE_EDGE} },
    { .species = SPECIES_SNORLAX, .heldItem = ITEM_SCOPE_LENS, .iv = 255, .ability = 2, // Thick Fat
      .moves = {MOVE_DOUBLE_EDGE, MOVE_EARTHQUAKE, MOVE_BRICK_BREAK, MOVE_TOXIC} },
    { .species = SPECIES_GRANBULL, .heldItem = ITEM_LUM_BERRY, .iv = 255, .ability = 1, // Intimidate
      .moves = {MOVE_DOUBLE_EDGE, MOVE_EARTHQUAKE, MOVE_BRICK_BREAK, MOVE_BULK_UP} },
    POOL_END
};
// ---------------------------------------------------------------------------------------------
// Pokemaniac

static const struct PoolMon sPool_Pokemaniac_Advanced[] =
{
    { .species = SPECIES_NIDOKING, .heldItem = ITEM_QUICK_CLAW, .iv = 255, .ability = 1, // Poison Point
      .moves = {MOVE_SLUDGE_BOMB, MOVE_EARTHQUAKE, MOVE_MEGAHORN, MOVE_TOXIC} },
    { .species = SPECIES_NIDOQUEEN, .heldItem = ITEM_SCOPE_LENS, .iv = 255, .ability = 1, // Poison Point
      .moves = {MOVE_SLUDGE_BOMB, MOVE_EARTHQUAKE, MOVE_FIRE_BLAST, MOVE_TOXIC} },
    { .species = SPECIES_RHYDON, .heldItem = ITEM_LUM_BERRY, .iv = 255, .ability = 1, // Lightning Rod
      .moves = {MOVE_EARTHQUAKE, MOVE_ROCK_SLIDE, MOVE_MEGAHORN, MOVE_SWORDS_DANCE} },
    { .species = SPECIES_SLOWBRO, .heldItem = ITEM_LEFTOVERS, .iv = 255, .ability = 1, // Oblivious
      .moves = {MOVE_SURF, MOVE_PSYCHIC, MOVE_FIRE_BLAST, MOVE_CALM_MIND} },
    { .species = SPECIES_LICKITUNG, .heldItem = ITEM_SITRUS_BERRY, .iv = 255, .ability = 1, // Own Tempo
      .moves = {MOVE_DOUBLE_EDGE, MOVE_FIRE_BLAST, MOVE_ICE_BEAM, MOVE_TOXIC} },
    { .species = SPECIES_KANGASKHAN, .heldItem = ITEM_LUM_BERRY, .iv = 255, .ability = 1, // Early Bird
      .moves = {MOVE_DOUBLE_EDGE, MOVE_EARTHQUAKE, MOVE_BRICK_BREAK, MOVE_TOXIC} },
    { .species = SPECIES_MAROWAK, .heldItem = ITEM_LEFTOVERS, .iv = 255, .ability = 1, // Rock Head
      .moves = {MOVE_EARTHQUAKE, MOVE_BRICK_BREAK, MOVE_IRON_TAIL, MOVE_SWORDS_DANCE} },
    { .species = SPECIES_AGGRON, .heldItem = ITEM_SCOPE_LENS, .iv = 255, .ability = 1, // Sturdy
      .moves = {MOVE_IRON_TAIL, MOVE_ROCK_SLIDE, MOVE_EARTHQUAKE, MOVE_THUNDER_WAVE} },
    { .species = SPECIES_FLYGON, .heldItem = ITEM_LEFTOVERS, .iv = 255, .ability = 1, // Levitate
      .moves = {MOVE_EARTHQUAKE, MOVE_DRAGON_CLAW, MOVE_FIRE_BLAST, MOVE_TOXIC} },
    { .species = SPECIES_STEELIX, .heldItem = ITEM_METAL_COAT, .iv = 255, .ability = 2, // Sturdy
      .moves = {MOVE_IRON_TAIL, MOVE_EARTHQUAKE, MOVE_ROCK_SLIDE, MOVE_TOXIC} },
    { .species = SPECIES_TROPIUS, .heldItem = ITEM_SITRUS_BERRY, .iv = 255, .ability = 1, // Chlorophyll
      .moves = {MOVE_GIGA_DRAIN, MOVE_AERIAL_ACE, MOVE_EARTHQUAKE, MOVE_SWORDS_DANCE} },
    { .species = SPECIES_FERALIGATR, .heldItem = ITEM_LUM_BERRY, .iv = 255, .ability = 1, // Torrent
      .moves = {MOVE_HYDRO_PUMP, MOVE_EARTHQUAKE, MOVE_BRICK_BREAK, MOVE_SWORDS_DANCE} },
    POOL_END
};
// ---------------------------------------------------------------------------------------------
// Psychic

static const struct PoolMon sPool_Psychic_Intermediate[] =
{
    { .species = SPECIES_KADABRA, .iv = 100 },
    { .species = SPECIES_HYPNO, .iv = 100 },
    { .species = SPECIES_SLOWBRO, .iv = 100 },
    { .species = SPECIES_XATU, .iv = 100 },
    { .species = SPECIES_GIRAFARIG, .iv = 100 },
    { .species = SPECIES_MR_MIME, .iv = 100 },
    { .species = SPECIES_KIRLIA, .iv = 100 },
    { .species = SPECIES_GRUMPIG, .iv = 100 },
    { .species = SPECIES_NATU, .iv = 100 },
    { .species = SPECIES_ESPEON, .iv = 100 },
    POOL_END
};

static const struct PoolMon sPool_Psychic_Advanced[] =
{
    { .species = SPECIES_ALAKAZAM, .heldItem = ITEM_LEFTOVERS, .iv = 255, .ability = 1, // Synchronize
      .moves = {MOVE_PSYCHIC, MOVE_SHOCK_WAVE, MOVE_SHADOW_BALL, MOVE_CALM_MIND} },
    { .species = SPECIES_HYPNO, .heldItem = ITEM_SITRUS_BERRY, .iv = 255, .ability = 1, // Insomnia
      .moves = {MOVE_PSYCHIC, MOVE_SHADOW_BALL, MOVE_BRICK_BREAK, MOVE_THUNDER_WAVE} },
    { .species = SPECIES_SLOWBRO, .heldItem = ITEM_LUM_BERRY, .iv = 255, .ability = 1, // Oblivious
      .moves = {MOVE_SURF, MOVE_PSYCHIC, MOVE_FIRE_BLAST, MOVE_CALM_MIND} },
    { .species = SPECIES_SLOWKING, .heldItem = ITEM_LEFTOVERS, .iv = 255, .ability = 1, // Oblivious
      .moves = {MOVE_SURF, MOVE_PSYCHIC, MOVE_FIRE_BLAST, MOVE_CALM_MIND} },
    { .species = SPECIES_XATU, .heldItem = ITEM_SCOPE_LENS, .iv = 255, .ability = 1, // Synchronize
      .moves = {MOVE_PSYCHIC, MOVE_AERIAL_ACE, MOVE_SHADOW_BALL, MOVE_THUNDER_WAVE} },
    { .species = SPECIES_GIRAFARIG, .heldItem = ITEM_LUM_BERRY, .iv = 255, .ability = 2, // Early Bird
      .moves = {MOVE_DOUBLE_EDGE, MOVE_PSYCHIC, MOVE_THUNDERBOLT, MOVE_CALM_MIND} },
    { .species = SPECIES_ESPEON, .heldItem = ITEM_TWISTED_SPOON, .iv = 255, .ability = 1, // Synchronize
      .moves = {MOVE_PSYCHIC, MOVE_SHADOW_BALL, MOVE_IRON_TAIL, MOVE_MORNING_SUN} },
    { .species = SPECIES_MR_MIME, .heldItem = ITEM_SITRUS_BERRY, .iv = 255, .ability = 1, // Soundproof
      .moves = {MOVE_PSYCHIC, MOVE_THUNDERBOLT, MOVE_MAGICAL_LEAF, MOVE_CALM_MIND} },
    { .species = SPECIES_STARMIE, .heldItem = ITEM_MYSTIC_WATER, .iv = 255, .ability = 2, // Natural Cure
      .moves = {MOVE_SURF, MOVE_PSYCHIC, MOVE_ICE_BEAM, MOVE_RECOVER} },
    { .species = SPECIES_GARDEVOIR, .heldItem = ITEM_LEFTOVERS, .iv = 255, .ability = 1, // Synchronize
      .moves = {MOVE_PSYCHIC, MOVE_THUNDERBOLT, MOVE_SHADOW_BALL, MOVE_CALM_MIND} },
    { .species = SPECIES_GRUMPIG, .heldItem = ITEM_SITRUS_BERRY, .iv = 255, .ability = 1, // Thick Fat
      .moves = {MOVE_PSYCHIC, MOVE_SHADOW_BALL, MOVE_SHOCK_WAVE, MOVE_CALM_MIND} },
    { .species = SPECIES_CLAYDOL, .heldItem = ITEM_LEFTOVERS, .iv = 255, .ability = 1, // Levitate
      .moves = {MOVE_EARTHQUAKE, MOVE_PSYCHIC, MOVE_ICE_BEAM, MOVE_TOXIC} },
    POOL_END
};
// ---------------------------------------------------------------------------------------------
// Sailor

static const struct PoolMon sPool_Sailor_Basic[] =
{
    { .species = SPECIES_MACHOP },
    { .species = SPECIES_TENTACOOL },
    { .species = SPECIES_SHELLDER },
    { .species = SPECIES_KRABBY },
    { .species = SPECIES_POLIWAG },
    { .species = SPECIES_WINGULL },
    { .species = SPECIES_MAKUHITA },
    { .species = SPECIES_PSYDUCK },
    { .species = SPECIES_HORSEA },
    POOL_END
};

static const struct PoolMon sPool_Sailor_Intermediate[] =
{
    { .species = SPECIES_MACHOKE, .iv = 100 },
    { .species = SPECIES_TENTACRUEL, .iv = 100 },
    { .species = SPECIES_KINGLER, .iv = 100 },
    { .species = SPECIES_POLIWHIRL, .iv = 100 },
    { .species = SPECIES_PELIPPER, .iv = 100 },
    { .species = SPECIES_HARIYAMA, .iv = 100 },
    { .species = SPECIES_GOLDUCK, .iv = 100 },
    { .species = SPECIES_CLOYSTER, .iv = 100 },
    { .species = SPECIES_SHARPEDO, .iv = 100 },
    { .species = SPECIES_SEADRA, .iv = 100 },
    { .species = SPECIES_WAILMER, .iv = 100 },
    POOL_END
};

static const struct PoolMon sPool_Sailor_Advanced[] =
{
    { .species = SPECIES_MACHAMP, .heldItem = ITEM_LUM_BERRY, .iv = 255, .ability = 1, // Guts
      .moves = {MOVE_CROSS_CHOP, MOVE_EARTHQUAKE, MOVE_ROCK_SLIDE, MOVE_BULK_UP} },
    { .species = SPECIES_POLIWRATH, .heldItem = ITEM_LEFTOVERS, .iv = 255, .ability = 1, // Water Absorb
      .moves = {MOVE_HYDRO_PUMP, MOVE_BRICK_BREAK, MOVE_EARTHQUAKE, MOVE_BULK_UP} },
    { .species = SPECIES_HITMONCHAN, .heldItem = ITEM_SITRUS_BERRY, .iv = 255, .ability = 1, // Keen Eye
      .moves = {MOVE_SKY_UPPERCUT, MOVE_EARTHQUAKE, MOVE_ROCK_SLIDE, MOVE_BULK_UP} },
    { .species = SPECIES_TENTACRUEL, .heldItem = ITEM_LUM_BERRY, .iv = 255, .ability = 1, // Clear Body
      .moves = {MOVE_HYDRO_PUMP, MOVE_SLUDGE_BOMB, MOVE_ICE_BEAM, MOVE_TOXIC} },
    { .species = SPECIES_KINGLER, .heldItem = ITEM_LEFTOVERS, .iv = 255, .ability = 1, // Hyper Cutter
      .moves = {MOVE_CRABHAMMER, MOVE_METAL_CLAW, MOVE_ROCK_TOMB, MOVE_SWORDS_DANCE} },
    { .species = SPECIES_PELIPPER, .heldItem = ITEM_SCOPE_LENS, .iv = 255, .ability = 1, // Keen Eye
      .moves = {MOVE_HYDRO_PUMP, MOVE_WING_ATTACK, MOVE_ICE_BEAM, MOVE_TOXIC} },
    { .species = SPECIES_WAILORD, .heldItem = ITEM_LEFTOVERS, .iv = 255, .ability = 1, // Water Veil
      .moves = {MOVE_HYDRO_PUMP, MOVE_EARTHQUAKE, MOVE_ICE_BEAM, MOVE_TOXIC} },
    { .species = SPECIES_SHARPEDO, .heldItem = ITEM_MYSTIC_WATER, .iv = 255, .ability = 1, // Rough Skin
      .moves = {MOVE_SURF, MOVE_CRUNCH, MOVE_EARTHQUAKE, MOVE_TOXIC} },
    { .species = SPECIES_GYARADOS, .heldItem = ITEM_SITRUS_BERRY, .iv = 255, .ability = 1, // Intimidate
      .moves = {MOVE_WATERFALL, MOVE_EARTHQUAKE, MOVE_FIRE_BLAST, MOVE_DRAGON_DANCE} },
    { .species = SPECIES_HARIYAMA, .heldItem = ITEM_LUM_BERRY, .iv = 255, .ability = 1, // Thick Fat
      .moves = {MOVE_BRICK_BREAK, MOVE_EARTHQUAKE, MOVE_ROCK_SLIDE, MOVE_BULK_UP} },
    POOL_END
};
// ---------------------------------------------------------------------------------------------
// SchoolKid

static const struct PoolMon sPool_SchoolKid_Advanced[] =
{
    { .species = SPECIES_ALAKAZAM, .heldItem = ITEM_LUM_BERRY, .iv = 255, .ability = 1, // Synchronize
      .moves = {MOVE_PSYCHIC, MOVE_SHOCK_WAVE, MOVE_SHADOW_BALL, MOVE_CALM_MIND} },
    { .species = SPECIES_NINETALES, .heldItem = ITEM_QUICK_CLAW, .iv = 255, .ability = 1, // Flash Fire
      .moves = {MOVE_FIRE_BLAST, MOVE_IRON_TAIL, MOVE_TOXIC, MOVE_DOUBLE_EDGE} },
    { .species = SPECIES_ELECTRODE, .heldItem = ITEM_SCOPE_LENS, .iv = 255, .ability = 2, // Static
      .moves = {MOVE_THUNDERBOLT, MOVE_THIEF, MOVE_THUNDER_WAVE, MOVE_SWIFT} },
    { .species = SPECIES_MAGNETON, .heldItem = ITEM_LEFTOVERS, .iv = 255, .ability = 2, // Sturdy
      .moves = {MOVE_THUNDERBOLT, MOVE_METAL_CLAW, MOVE_THUNDER_WAVE, MOVE_DOUBLE_EDGE} },
    { .species = SPECIES_RAICHU, .heldItem = ITEM_MAGNET, .iv = 255, .ability = 1, // Static
      .moves = {MOVE_THUNDERBOLT, MOVE_IRON_TAIL, MOVE_BRICK_BREAK, MOVE_THUNDER_WAVE} },
    { .species = SPECIES_SUNFLORA, .heldItem = ITEM_SITRUS_BERRY, .iv = 255, .ability = 1, // Chlorophyll
      .moves = {MOVE_GIGA_DRAIN, MOVE_SLUDGE_BOMB, MOVE_TOXIC, MOVE_DOUBLE_EDGE} },
    { .species = SPECIES_NOCTOWL, .heldItem = ITEM_LUM_BERRY, .iv = 255, .ability = 1, // Insomnia
      .moves = {MOVE_DOUBLE_EDGE, MOVE_AERIAL_ACE, MOVE_PSYCHIC, MOVE_TOXIC} },
    { .species = SPECIES_JOLTEON, .heldItem = ITEM_QUICK_CLAW, .iv = 255, .ability = 1, // Volt Absorb
      .moves = {MOVE_THUNDERBOLT, MOVE_SHADOW_BALL, MOVE_IRON_TAIL, MOVE_THUNDER_WAVE} },
    { .species = SPECIES_CROBAT, .heldItem = ITEM_SCOPE_LENS, .iv = 255, .ability = 1, // Inner Focus
      .moves = {MOVE_SLUDGE_BOMB, MOVE_WING_ATTACK, MOVE_SHADOW_BALL, MOVE_TOXIC} },
    { .species = SPECIES_QUAGSIRE, .heldItem = ITEM_LEFTOVERS, .iv = 255, .ability = 2, // Water Absorb
      .moves = {MOVE_SURF, MOVE_EARTHQUAKE, MOVE_SLUDGE_BOMB, MOVE_TOXIC} },
    POOL_END
};
// ---------------------------------------------------------------------------------------------
// SwimmerF

static const struct PoolMon sPool_SwimmerF_Basic[] =
{
    { .species = SPECIES_STARYU },
    { .species = SPECIES_GOLDEEN },
    { .species = SPECIES_HORSEA },
    { .species = SPECIES_PSYDUCK },
    { .species = SPECIES_MARILL },
    { .species = SPECIES_SEEL },
    { .species = SPECIES_SHELLDER },
    { .species = SPECIES_CORSOLA },
    { .species = SPECIES_WINGULL },
    { .species = SPECIES_LOTAD },
    { .species = SPECIES_LUVDISC },
    { .species = SPECIES_TENTACOOL },
    POOL_END
};

static const struct PoolMon sPool_SwimmerF_Intermediate[] =
{
    { .species = SPECIES_STARMIE, .iv = 100 },
    { .species = SPECIES_SEAKING, .iv = 100 },
    { .species = SPECIES_SEADRA, .iv = 100 },
    { .species = SPECIES_GOLDUCK, .iv = 100 },
    { .species = SPECIES_AZUMARILL, .iv = 100 },
    { .species = SPECIES_DEWGONG, .iv = 100 },
    { .species = SPECIES_CORSOLA, .iv = 100 },
    { .species = SPECIES_PELIPPER, .iv = 100 },
    { .species = SPECIES_LOMBRE, .iv = 100 },
    { .species = SPECIES_LUVDISC, .iv = 100 },
    { .species = SPECIES_LANTURN, .iv = 100 },
    { .species = SPECIES_MANTINE, .iv = 100 },
    POOL_END
};

static const struct PoolMon sPool_SwimmerF_Advanced[] =
{
    { .species = SPECIES_STARMIE, .heldItem = ITEM_LEFTOVERS, .iv = 255, .ability = 2, // Natural Cure
      .moves = {MOVE_SURF, MOVE_PSYCHIC, MOVE_ICE_BEAM, MOVE_RECOVER} },
    { .species = SPECIES_SEAKING, .heldItem = ITEM_LUM_BERRY, .iv = 255, .ability = 1, // Swift Swim
      .moves = {MOVE_SURF, MOVE_ICE_BEAM, MOVE_MEGAHORN, MOVE_TOXIC} },
    { .species = SPECIES_LAPRAS, .heldItem = ITEM_QUICK_CLAW, .iv = 255, .ability = 1, // Water Absorb
      .moves = {MOVE_HYDRO_PUMP, MOVE_ICE_BEAM, MOVE_THUNDERBOLT, MOVE_TOXIC} },
    { .species = SPECIES_AZUMARILL, .heldItem = ITEM_SCOPE_LENS, .iv = 255, .ability = 2, // Huge Power
      .moves = {MOVE_HYDRO_PUMP, MOVE_ICE_BEAM, MOVE_BRICK_BREAK, MOVE_TOXIC} },
    { .species = SPECIES_DEWGONG, .heldItem = ITEM_LEFTOVERS, .iv = 255, .ability = 1, // Thick Fat
      .moves = {MOVE_SURF, MOVE_ICE_BEAM, MOVE_SIGNAL_BEAM, MOVE_TOXIC} },
    { .species = SPECIES_MANTINE, .heldItem = ITEM_MYSTIC_WATER, .iv = 255, .ability = 1, // Swift Swim
      .moves = {MOVE_SURF, MOVE_WING_ATTACK, MOVE_ICE_BEAM, MOVE_TOXIC} },
    { .species = SPECIES_CORSOLA, .heldItem = ITEM_SITRUS_BERRY, .iv = 255, .ability = 2, // Natural Cure
      .moves = {MOVE_SURF, MOVE_ROCK_SLIDE, MOVE_ICE_BEAM, MOVE_CALM_MIND} },
    { .species = SPECIES_VAPOREON, .heldItem = ITEM_LUM_BERRY, .iv = 255, .ability = 1, // Water Absorb
      .moves = {MOVE_HYDRO_PUMP, MOVE_ICE_BEAM, MOVE_SHADOW_BALL, MOVE_TOXIC} },
    { .species = SPECIES_MILOTIC, .heldItem = ITEM_LEFTOVERS, .iv = 255, .ability = 1, // Marvel Scale
      .moves = {MOVE_HYDRO_PUMP, MOVE_ICE_BEAM, MOVE_IRON_TAIL, MOVE_RECOVER} },
    { .species = SPECIES_LANTURN, .heldItem = ITEM_SCOPE_LENS, .iv = 255, .ability = 1, // Volt Absorb
      .moves = {MOVE_HYDRO_PUMP, MOVE_THUNDERBOLT, MOVE_ICE_BEAM, MOVE_THUNDER_WAVE} },
    { .species = SPECIES_GOLDUCK, .heldItem = ITEM_LUM_BERRY, .iv = 255, .ability = 1, // Damp
      .moves = {MOVE_HYDRO_PUMP, MOVE_ICE_BEAM, MOVE_PSYCHIC, MOVE_CALM_MIND} },
    { .species = SPECIES_SLOWBRO, .heldItem = ITEM_LEFTOVERS, .iv = 255, .ability = 1, // Oblivious
      .moves = {MOVE_SURF, MOVE_PSYCHIC, MOVE_FIRE_BLAST, MOVE_CALM_MIND} },
    POOL_END
};
// ---------------------------------------------------------------------------------------------
// SwimmerM

static const struct PoolMon sPool_SwimmerM_Basic[] =
{
    { .species = SPECIES_TENTACOOL },
    { .species = SPECIES_HORSEA },
    { .species = SPECIES_KRABBY },
    { .species = SPECIES_PSYDUCK },
    { .species = SPECIES_SHELLDER },
    { .species = SPECIES_POLIWAG },
    { .species = SPECIES_STARYU },
    { .species = SPECIES_WINGULL },
    { .species = SPECIES_MARILL },
    { .species = SPECIES_REMORAID },
    { .species = SPECIES_CHINCHOU },
    { .species = SPECIES_SEEL },
    POOL_END
};

static const struct PoolMon sPool_SwimmerM_Intermediate[] =
{
    { .species = SPECIES_TENTACRUEL, .iv = 100 },
    { .species = SPECIES_SEADRA, .iv = 100 },
    { .species = SPECIES_KINGLER, .iv = 100 },
    { .species = SPECIES_GOLDUCK, .iv = 100 },
    { .species = SPECIES_CLOYSTER, .iv = 100 },
    { .species = SPECIES_POLIWHIRL, .iv = 100 },
    { .species = SPECIES_STARMIE, .iv = 100 },
    { .species = SPECIES_PELIPPER, .iv = 100 },
    { .species = SPECIES_AZUMARILL, .iv = 100 },
    { .species = SPECIES_OCTILLERY, .iv = 100 },
    { .species = SPECIES_LANTURN, .iv = 100 },
    { .species = SPECIES_DEWGONG, .iv = 100 },
    POOL_END
};

static const struct PoolMon sPool_SwimmerM_Advanced[] =
{
    { .species = SPECIES_TENTACRUEL, .heldItem = ITEM_SITRUS_BERRY, .iv = 255, .ability = 1, // Clear Body
      .moves = {MOVE_HYDRO_PUMP, MOVE_SLUDGE_BOMB, MOVE_ICE_BEAM, MOVE_TOXIC} },
    { .species = SPECIES_GOLDUCK, .heldItem = ITEM_LUM_BERRY, .iv = 255, .ability = 1, // Damp
      .moves = {MOVE_HYDRO_PUMP, MOVE_ICE_BEAM, MOVE_PSYCHIC, MOVE_CALM_MIND} },
    { .species = SPECIES_KINGLER, .heldItem = ITEM_LEFTOVERS, .iv = 255, .ability = 1, // Hyper Cutter
      .moves = {MOVE_CRABHAMMER, MOVE_METAL_CLAW, MOVE_ROCK_TOMB, MOVE_SWORDS_DANCE} },
    { .species = SPECIES_GYARADOS, .heldItem = ITEM_SITRUS_BERRY, .iv = 255, .ability = 1, // Intimidate
      .moves = {MOVE_WATERFALL, MOVE_EARTHQUAKE, MOVE_FIRE_BLAST, MOVE_DRAGON_DANCE} },
    { .species = SPECIES_QUAGSIRE, .heldItem = ITEM_LEFTOVERS, .iv = 255, .ability = 2, // Water Absorb
      .moves = {MOVE_SURF, MOVE_EARTHQUAKE, MOVE_SLUDGE_BOMB, MOVE_TOXIC} },
    { .species = SPECIES_MANTINE, .heldItem = ITEM_MYSTIC_WATER, .iv = 255, .ability = 1, // Swift Swim
      .moves = {MOVE_SURF, MOVE_WING_ATTACK, MOVE_ICE_BEAM, MOVE_TOXIC} },
    { .species = SPECIES_CLOYSTER, .heldItem = ITEM_SITRUS_BERRY, .iv = 255, .ability = 1, // Shell Armor
      .moves = {MOVE_SURF, MOVE_ICE_BEAM, MOVE_TOXIC, MOVE_DOUBLE_EDGE} },
    { .species = SPECIES_DEWGONG, .heldItem = ITEM_LUM_BERRY, .iv = 255, .ability = 1, // Thick Fat
      .moves = {MOVE_SURF, MOVE_ICE_BEAM, MOVE_SIGNAL_BEAM, MOVE_TOXIC} },
    { .species = SPECIES_LANTURN, .heldItem = ITEM_QUICK_CLAW, .iv = 255, .ability = 1, // Volt Absorb
      .moves = {MOVE_HYDRO_PUMP, MOVE_THUNDERBOLT, MOVE_ICE_BEAM, MOVE_THUNDER_WAVE} },
    { .species = SPECIES_STARMIE, .heldItem = ITEM_MYSTIC_WATER, .iv = 255, .ability = 2, // Natural Cure
      .moves = {MOVE_SURF, MOVE_PSYCHIC, MOVE_ICE_BEAM, MOVE_RECOVER} },
    { .species = SPECIES_SHARPEDO, .heldItem = ITEM_LEFTOVERS, .iv = 255, .ability = 1, // Rough Skin
      .moves = {MOVE_SURF, MOVE_CRUNCH, MOVE_EARTHQUAKE, MOVE_TOXIC} },
    { .species = SPECIES_CRAWDAUNT, .heldItem = ITEM_MYSTIC_WATER, .iv = 255, .ability = 1, // Hyper Cutter
      .moves = {MOVE_SURF, MOVE_CRUNCH, MOVE_SLUDGE_BOMB, MOVE_TOXIC} },
    POOL_END
};
// ---------------------------------------------------------------------------------------------
// Tuber

static const struct PoolMon sPool_Tuber_Basic[] =
{
    { .species = SPECIES_MARILL },
    { .species = SPECIES_PSYDUCK },
    { .species = SPECIES_POLIWAG },
    { .species = SPECIES_AZURILL },
    { .species = SPECIES_WINGULL },
    { .species = SPECIES_LOTAD },
    { .species = SPECIES_HORSEA },
    { .species = SPECIES_GOLDEEN },
    { .species = SPECIES_SPHEAL },
    { .species = SPECIES_SURSKIT },
    POOL_END
};

static const struct PoolMon sPool_Tuber_Intermediate[] =
{
    { .species = SPECIES_AZUMARILL, .iv = 100 },
    { .species = SPECIES_GOLDUCK, .iv = 100 },
    { .species = SPECIES_POLIWHIRL, .iv = 100 },
    { .species = SPECIES_PELIPPER, .iv = 100 },
    { .species = SPECIES_LOMBRE, .iv = 100 },
    { .species = SPECIES_SEADRA, .iv = 100 },
    { .species = SPECIES_SEAKING, .iv = 100 },
    { .species = SPECIES_MASQUERAIN, .iv = 100 },
    { .species = SPECIES_SEALEO, .iv = 100 },
    POOL_END
};
// ---------------------------------------------------------------------------------------------
// Twins

static const struct PoolMon sPool_Twins_Advanced[] =
{
    { .species = SPECIES_CLEFABLE, .heldItem = ITEM_SITRUS_BERRY, .iv = 255, .ability = 1, // Cute Charm
      .moves = {MOVE_DOUBLE_EDGE, MOVE_FIRE_BLAST, MOVE_ICE_BEAM, MOVE_CALM_MIND} },
    { .species = SPECIES_WIGGLYTUFF, .heldItem = ITEM_LEFTOVERS, .iv = 255, .ability = 1, // Cute Charm
      .moves = {MOVE_DOUBLE_EDGE, MOVE_FIRE_BLAST, MOVE_ICE_BEAM, MOVE_THUNDER_WAVE} },
    { .species = SPECIES_PLUSLE, .heldItem = ITEM_MAGNET, .iv = 255, .ability = 1, // Plus
      .moves = {MOVE_THUNDERBOLT, MOVE_IRON_TAIL, MOVE_THUNDER_WAVE, MOVE_DOUBLE_EDGE} },
    { .species = SPECIES_MINUN, .heldItem = ITEM_SITRUS_BERRY, .iv = 255, .ability = 1, // Minus
      .moves = {MOVE_THUNDERBOLT, MOVE_IRON_TAIL, MOVE_THUNDER_WAVE, MOVE_DOUBLE_EDGE} },
    { .species = SPECIES_AZUMARILL, .heldItem = ITEM_LUM_BERRY, .iv = 255, .ability = 2, // Huge Power
      .moves = {MOVE_HYDRO_PUMP, MOVE_ICE_BEAM, MOVE_BRICK_BREAK, MOVE_TOXIC} },
    { .species = SPECIES_LEDIAN, .heldItem = ITEM_LEFTOVERS, .iv = 255, .ability = 2, // Early Bird
      .moves = {MOVE_AERIAL_ACE, MOVE_BRICK_BREAK, MOVE_GIGA_DRAIN, MOVE_SWORDS_DANCE} },
    { .species = SPECIES_JUMPLUFF, .heldItem = ITEM_SITRUS_BERRY, .iv = 255, .ability = 1, // Chlorophyll
      .moves = {MOVE_GIGA_DRAIN, MOVE_AERIAL_ACE, MOVE_SWORDS_DANCE, MOVE_DOUBLE_EDGE} },
    { .species = SPECIES_TOGETIC, .heldItem = ITEM_LEFTOVERS, .iv = 255, .ability = 2, // Serene Grace
      .moves = {MOVE_DOUBLE_EDGE, MOVE_AERIAL_ACE, MOVE_FIRE_BLAST, MOVE_SOFT_BOILED} },
    { .species = SPECIES_RAICHU, .heldItem = ITEM_MAGNET, .iv = 255, .ability = 1, // Static
      .moves = {MOVE_THUNDERBOLT, MOVE_IRON_TAIL, MOVE_BRICK_BREAK, MOVE_THUNDER_WAVE} },
    { .species = SPECIES_DELCATTY, .heldItem = ITEM_SITRUS_BERRY, .iv = 255, .ability = 1, // Cute Charm
      .moves = {MOVE_DOUBLE_EDGE, MOVE_ICE_BEAM, MOVE_THUNDERBOLT, MOVE_CALM_MIND} },
    POOL_END
};
// ---------------------------------------------------------------------------------------------
// Youngster

static const struct PoolMon sPool_Youngster_Basic[] =
{
    { .species = SPECIES_RATTATA },
    { .species = SPECIES_SPEAROW },
    { .species = SPECIES_EKANS },
    { .species = SPECIES_SANDSHREW },
    { .species = SPECIES_SENTRET },
    { .species = SPECIES_ZIGZAGOON },
    { .species = SPECIES_POOCHYENA },
    { .species = SPECIES_TAILLOW },
    { .species = SPECIES_MEOWTH },
    { .species = SPECIES_HOOTHOOT },
    POOL_END
};

static const struct PoolMon sPool_Youngster_Intermediate[] =
{
    { .species = SPECIES_RATICATE, .iv = 100 },
    { .species = SPECIES_FEAROW, .iv = 100 },
    { .species = SPECIES_ARBOK, .iv = 100 },
    { .species = SPECIES_SANDSLASH, .iv = 100 },
    { .species = SPECIES_FURRET, .iv = 100 },
    { .species = SPECIES_LINOONE, .iv = 100 },
    { .species = SPECIES_MIGHTYENA, .iv = 100 },
    { .species = SPECIES_SWELLOW, .iv = 100 },
    { .species = SPECIES_PERSIAN, .iv = 100 },
    { .species = SPECIES_NOCTOWL, .iv = 100 },
    POOL_END
};

static const struct PoolMon sPool_Youngster_Advanced[] =
{
    { .species = SPECIES_RATICATE, .heldItem = ITEM_LUM_BERRY, .iv = 255, .ability = 2, // Guts
      .moves = {MOVE_DOUBLE_EDGE, MOVE_IRON_TAIL, MOVE_CRUNCH, MOVE_SWORDS_DANCE} },
    { .species = SPECIES_FEAROW, .heldItem = ITEM_QUICK_CLAW, .iv = 255, .ability = 1, // Keen Eye
      .moves = {MOVE_DOUBLE_EDGE, MOVE_DRILL_PECK, MOVE_STEEL_WING, MOVE_TOXIC} },
    { .species = SPECIES_FURRET, .heldItem = ITEM_SCOPE_LENS, .iv = 255, .ability = 1, // Run Away
      .moves = {MOVE_DOUBLE_EDGE, MOVE_BRICK_BREAK, MOVE_IRON_TAIL, MOVE_TOXIC} },
    { .species = SPECIES_NOCTOWL, .heldItem = ITEM_LEFTOVERS, .iv = 255, .ability = 1, // Insomnia
      .moves = {MOVE_DOUBLE_EDGE, MOVE_AERIAL_ACE, MOVE_PSYCHIC, MOVE_TOXIC} },
    { .species = SPECIES_ARBOK, .heldItem = ITEM_POISON_BARB, .iv = 255, .ability = 1, // Intimidate
      .moves = {MOVE_SLUDGE_BOMB, MOVE_EARTHQUAKE, MOVE_IRON_TAIL, MOVE_TOXIC} },
    { .species = SPECIES_SANDSLASH, .heldItem = ITEM_SITRUS_BERRY, .iv = 255, .ability = 1, // Sand Veil
      .moves = {MOVE_EARTHQUAKE, MOVE_BRICK_BREAK, MOVE_IRON_TAIL, MOVE_SWORDS_DANCE} },
    { .species = SPECIES_PERSIAN, .heldItem = ITEM_LUM_BERRY, .iv = 255, .ability = 1, // Limber
      .moves = {MOVE_DOUBLE_EDGE, MOVE_THUNDERBOLT, MOVE_CRUNCH, MOVE_TOXIC} },
    { .species = SPECIES_GRANBULL, .heldItem = ITEM_LEFTOVERS, .iv = 255, .ability = 1, // Intimidate
      .moves = {MOVE_DOUBLE_EDGE, MOVE_EARTHQUAKE, MOVE_BRICK_BREAK, MOVE_BULK_UP} },
    { .species = SPECIES_LINOONE, .heldItem = ITEM_SCOPE_LENS, .iv = 255, .ability = 1, // Pickup
      .moves = {MOVE_DOUBLE_EDGE, MOVE_IRON_TAIL, MOVE_SHADOW_BALL, MOVE_THUNDER_WAVE} },
    { .species = SPECIES_SWELLOW, .heldItem = ITEM_LEFTOVERS, .iv = 255, .ability = 1, // Guts
      .moves = {MOVE_DOUBLE_EDGE, MOVE_WING_ATTACK, MOVE_STEEL_WING, MOVE_TOXIC} },
    { .species = SPECIES_MIGHTYENA, .heldItem = ITEM_BLACK_GLASSES, .iv = 255, .ability = 1, // Intimidate
      .moves = {MOVE_CRUNCH, MOVE_IRON_TAIL, MOVE_SHADOW_BALL, MOVE_TOXIC} },
    POOL_END
};

// ---------------------------------------------------------------------------------------------
// Channeler -- also used by the ghost crew of the MORO (Orange Islands).

static const struct PoolMon sPool_Channeler_Basic[] =
{
    { .species = SPECIES_GASTLY },
    { .species = SPECIES_HAUNTER },
    { .species = SPECIES_DROWZEE },
    { .species = SPECIES_SHUPPET },
    { .species = SPECIES_DUSKULL },
    { .species = SPECIES_MISDREAVUS },
    { .species = SPECIES_NATU },
    { .species = SPECIES_SPOINK },
    { .species = SPECIES_ZUBAT },
    { .species = SPECIES_GOLBAT },
    POOL_END
};

static const struct PoolMon sPool_Channeler_Intermediate[] =
{
    { .species = SPECIES_HAUNTER, .iv = 150, .moves = {MOVE_SHADOW_BALL, MOVE_CONFUSE_RAY, MOVE_HYPNOSIS, MOVE_DREAM_EATER} },
    { .species = SPECIES_KADABRA, .iv = 150 },
    { .species = SPECIES_HYPNO, .iv = 150, .moves = {MOVE_HYPNOSIS, MOVE_DREAM_EATER, MOVE_CONFUSION, MOVE_HEADBUTT} },
    { .species = SPECIES_BANETTE, .iv = 150 },
    { .species = SPECIES_DUSCLOPS, .iv = 150 },
    { .species = SPECIES_SABLEYE, .iv = 150 },
    { .species = SPECIES_MISDREAVUS, .iv = 150 },
    { .species = SPECIES_GRUMPIG, .iv = 150 },
    { .species = SPECIES_GOLBAT, .iv = 150 },
    POOL_END
};
// --- build_battle_boat.py pools ---
// ---------------------------------------------------------------------------
// The KINNOW battle boat. Chartered out of HOENN, crewed and patronised by
// HOENN trainers, so both pools are HOENN-dex only (build_battle_boat.py).
//
// These are the people lounging on the decks, not the arena. KINNOW's own
// trainers are level twenty-two and these run twenty-four to thirty, so the
// pools stay at first and middle stages -- a fully evolved team would read as
// the arena's level rather than the deck's.

static const struct PoolMon sPool_HoennCrew_Basic[] =
{
    { .species = SPECIES_ZIGZAGOON, .iv = 60 },
    { .species = SPECIES_POOCHYENA, .iv = 60 },
    { .species = SPECIES_WINGULL,   .iv = 60 },
    { .species = SPECIES_TAILLOW,   .iv = 60 },
    { .species = SPECIES_SHROOMISH, .iv = 60 },
    { .species = SPECIES_WHISMUR,   .iv = 60 },
    { .species = SPECIES_ARON,      .iv = 60 },
    { .species = SPECIES_NUMEL,     .iv = 60 },
    { .species = SPECIES_CARVANHA,  .iv = 60 },
    { .species = SPECIES_ELECTRIKE, .iv = 60 },
    { .species = SPECIES_MAKUHITA,  .iv = 60 },
    { .species = SPECIES_SWABLU,    .iv = 60 },
    { .species = SPECIES_SPOINK,    .iv = 60 },
    { .species = SPECIES_BARBOACH,  .iv = 60 },
    POOL_END
};

static const struct PoolMon sPool_HoennCrew_Intermediate[] =
{
    { .species = SPECIES_LINOONE,   .iv = 100 },
    { .species = SPECIES_MIGHTYENA, .iv = 100 },
    { .species = SPECIES_PELIPPER,  .iv = 100 },
    { .species = SPECIES_SWELLOW,   .iv = 100 },
    { .species = SPECIES_LOMBRE,    .iv = 100 },
    { .species = SPECIES_MARSHTOMP, .iv = 100 },
    { .species = SPECIES_COMBUSKEN, .iv = 100 },
    { .species = SPECIES_GROVYLE,   .iv = 100 },
    { .species = SPECIES_LAIRON,    .iv = 100, .heldItem = ITEM_HARD_STONE },
    { .species = SPECIES_VIBRAVA,   .iv = 100 },
    { .species = SPECIES_WAILMER,   .iv = 100 },
    { .species = SPECIES_KIRLIA,    .iv = 100 },
    { .species = SPECIES_NINCADA,   .iv = 100 },
    { .species = SPECIES_SANDSHREW, .iv = 100 },
    POOL_END
};
// --- end build_battle_boat.py pools ---
