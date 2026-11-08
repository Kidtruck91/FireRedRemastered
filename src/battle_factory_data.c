// Data the BATTLE FACTORY rents from, lifted from Emerald.
//
// Emerald keeps these in battle_tower.c beside its own frontier facilities.
// FireRed's battle_tower.c is a different file with a different job, so the
// factory's data lives here instead and nothing of FireRed's is disturbed.

#include "global.h"
#include "battle_factory.h"
#include "constants/battle_frontier_items.h"
#include "constants/battle_frontier_mons.h"
#include "constants/items.h"
#include "constants/moves.h"
#include "constants/pokemon.h"
#include "constants/species.h"
#include "constants/easy_chat.h"
#include "constants/trainers.h"

// Which mon table the facility is renting from, and which trainer table it is
// drawing opponents from. Emerald swapped these between the frontier and the
// battle tents; here only one of each exists, but the indirection stays so the
// ported code reads as it did.
// EWRAM_DATA, not a plain global. ld_script.ld names the sections it keeps
// from src/ and there is no src/*.o(.data) among them, so an initialised
// mutable global is silently discarded at link time; FireRed keeps this kind
// of thing in EWRAM instead. The ported code points them at the tables below
// before it reads them, which is where Emerald chose between the frontier and
// its battle tents -- here there is only ever one of each.
EWRAM_DATA const struct FacilityMon *gFacilityTrainerMons = NULL;
EWRAM_DATA const struct BattleFrontierTrainer *gFacilityTrainers = NULL;

const u16 gBattleFrontierHeldItems[] =

{
    [BATTLE_FRONTIER_ITEM_NONE]           = ITEM_NONE,
    [BATTLE_FRONTIER_ITEM_KINGS_ROCK]     = ITEM_KINGS_ROCK,
    [BATTLE_FRONTIER_ITEM_SITRUS_BERRY]   = ITEM_SITRUS_BERRY,
    [BATTLE_FRONTIER_ITEM_ORAN_BERRY]     = ITEM_ORAN_BERRY,
    [BATTLE_FRONTIER_ITEM_CHESTO_BERRY]   = ITEM_CHESTO_BERRY,
    [BATTLE_FRONTIER_ITEM_HARD_STONE]     = ITEM_HARD_STONE,
    [BATTLE_FRONTIER_ITEM_FOCUS_BAND]     = ITEM_FOCUS_BAND,
    [BATTLE_FRONTIER_ITEM_PERSIM_BERRY]   = ITEM_PERSIM_BERRY,
    [BATTLE_FRONTIER_ITEM_MIRACLE_SEED]   = ITEM_MIRACLE_SEED,
    [BATTLE_FRONTIER_ITEM_BERRY_JUICE]    = ITEM_BERRY_JUICE,
    [BATTLE_FRONTIER_ITEM_MACHO_BRACE]    = ITEM_MACHO_BRACE,
    [BATTLE_FRONTIER_ITEM_SILVER_POWDER]  = ITEM_SILVER_POWDER,
    [BATTLE_FRONTIER_ITEM_CHERI_BERRY]    = ITEM_CHERI_BERRY,
    [BATTLE_FRONTIER_ITEM_BLACK_GLASSES]  = ITEM_BLACK_GLASSES,
    [BATTLE_FRONTIER_ITEM_BLACK_BELT]     = ITEM_BLACK_BELT,
    [BATTLE_FRONTIER_ITEM_SOUL_DEW]       = ITEM_SOUL_DEW,
    [BATTLE_FRONTIER_ITEM_CHOICE_BAND]    = ITEM_CHOICE_BAND,
    [BATTLE_FRONTIER_ITEM_MAGNET]         = ITEM_MAGNET,
    [BATTLE_FRONTIER_ITEM_SILK_SCARF]     = ITEM_SILK_SCARF,
    [BATTLE_FRONTIER_ITEM_WHITE_HERB]     = ITEM_WHITE_HERB,
    [BATTLE_FRONTIER_ITEM_DEEP_SEA_SCALE] = ITEM_DEEP_SEA_SCALE,
    [BATTLE_FRONTIER_ITEM_DEEP_SEA_TOOTH] = ITEM_DEEP_SEA_TOOTH,
    [BATTLE_FRONTIER_ITEM_MYSTIC_WATER]   = ITEM_MYSTIC_WATER,
    [BATTLE_FRONTIER_ITEM_SHARP_BEAK]     = ITEM_SHARP_BEAK,
    [BATTLE_FRONTIER_ITEM_QUICK_CLAW]     = ITEM_QUICK_CLAW,
    [BATTLE_FRONTIER_ITEM_LEFTOVERS]      = ITEM_LEFTOVERS,
    [BATTLE_FRONTIER_ITEM_RAWST_BERRY]    = ITEM_RAWST_BERRY,
    [BATTLE_FRONTIER_ITEM_LIGHT_BALL]     = ITEM_LIGHT_BALL,
    [BATTLE_FRONTIER_ITEM_POISON_BARB]    = ITEM_POISON_BARB,
    [BATTLE_FRONTIER_ITEM_NEVER_MELT_ICE] = ITEM_NEVER_MELT_ICE,
    [BATTLE_FRONTIER_ITEM_ASPEAR_BERRY]   = ITEM_ASPEAR_BERRY,
    [BATTLE_FRONTIER_ITEM_SPELL_TAG]      = ITEM_SPELL_TAG,
    [BATTLE_FRONTIER_ITEM_BRIGHT_POWDER]  = ITEM_BRIGHT_POWDER,
    [BATTLE_FRONTIER_ITEM_LEPPA_BERRY]    = ITEM_LEPPA_BERRY,
    [BATTLE_FRONTIER_ITEM_SCOPE_LENS]     = ITEM_SCOPE_LENS,
    [BATTLE_FRONTIER_ITEM_TWISTED_SPOON]  = ITEM_TWISTED_SPOON,
    [BATTLE_FRONTIER_ITEM_METAL_COAT]     = ITEM_METAL_COAT,
    [BATTLE_FRONTIER_ITEM_MENTAL_HERB]    = ITEM_MENTAL_HERB,
    [BATTLE_FRONTIER_ITEM_CHARCOAL]       = ITEM_CHARCOAL,
    [BATTLE_FRONTIER_ITEM_PECHA_BERRY]    = ITEM_PECHA_BERRY,
    [BATTLE_FRONTIER_ITEM_SOFT_SAND]      = ITEM_SOFT_SAND,
    [BATTLE_FRONTIER_ITEM_LUM_BERRY]      = ITEM_LUM_BERRY,
    [BATTLE_FRONTIER_ITEM_DRAGON_SCALE]   = ITEM_DRAGON_SCALE,
    [BATTLE_FRONTIER_ITEM_DRAGON_FANG]    = ITEM_DRAGON_FANG,
    [BATTLE_FRONTIER_ITEM_IAPAPA_BERRY]   = ITEM_IAPAPA_BERRY,
    [BATTLE_FRONTIER_ITEM_WIKI_BERRY]     = ITEM_WIKI_BERRY,
    [BATTLE_FRONTIER_ITEM_SEA_INCENSE]    = ITEM_SEA_INCENSE,
    [BATTLE_FRONTIER_ITEM_SHELL_BELL]     = ITEM_SHELL_BELL,
    [BATTLE_FRONTIER_ITEM_SALAC_BERRY]    = ITEM_SALAC_BERRY,
    [BATTLE_FRONTIER_ITEM_LANSAT_BERRY]   = ITEM_LANSAT_BERRY,
    [BATTLE_FRONTIER_ITEM_APICOT_BERRY]   = ITEM_APICOT_BERRY,
    [BATTLE_FRONTIER_ITEM_STARF_BERRY]    = ITEM_STARF_BERRY,
    [BATTLE_FRONTIER_ITEM_LIECHI_BERRY]   = ITEM_LIECHI_BERRY,
    [BATTLE_FRONTIER_ITEM_STICK]          = ITEM_STICK,
    [BATTLE_FRONTIER_ITEM_LAX_INCENSE]    = ITEM_LAX_INCENSE,
    [BATTLE_FRONTIER_ITEM_AGUAV_BERRY]    = ITEM_AGUAV_BERRY,
    [BATTLE_FRONTIER_ITEM_FIGY_BERRY]     = ITEM_FIGY_BERRY,
    [BATTLE_FRONTIER_ITEM_THICK_CLUB]     = ITEM_THICK_CLUB,
    [BATTLE_FRONTIER_ITEM_MAGO_BERRY]     = ITEM_MAGO_BERRY,
    [BATTLE_FRONTIER_ITEM_METAL_POWDER]   = ITEM_METAL_POWDER,
    [BATTLE_FRONTIER_ITEM_PETAYA_BERRY]   = ITEM_PETAYA_BERRY,
    [BATTLE_FRONTIER_ITEM_LUCKY_PUNCH]    = ITEM_LUCKY_PUNCH,
    [BATTLE_FRONTIER_ITEM_GANLON_BERRY]   = ITEM_GANLON_BERRY,

};

#include "data/battle_frontier/battle_frontier_mons.h"

