// Orange Islands challenge (FLAG_ORANGE_CHALLENGE). The trip has its own team and its own PC.
// In RAM there is only ever one of each: the Kanto PC + home team at home, the Orange PC
// (ORANGE_BOXES_COUNT boxes) + Orange team on the trip. The pair not in use is kept in its own
// flash sectors (src/save.c, SaveStoredPC). Flash writes only ever store what is in RAM, and the
// main save only changes when the player saves, so a reset before saving can't lose anything.
//
// Script use:
//   callnative OrangeChallenge_Begin   @ VAR_RESULT: TRUE ok, FALSE flash error (nothing changed)
//   callnative OrangeChallenge_End     @ VAR_RESULT: TRUE ok, FALSE flash error (nothing changed)
//   setvar VAR_0x8004, 0/1  callnative OrangePC_ShowBoxSet  @ after Drake: Kanto/Orange boxes in any PC
#include "global.h"
#include "event_data.h"
#include "follow_me.h"
#include "pokemon.h"
#include "pokemon_storage_system.h"
#include "battle_main.h"
#include "load_save.h"
#include "main.h"
#include "overworld.h"
#include "party_menu.h"
#include "save.h"
#include "script_pokemon_util.h"
#include "string_util.h"
#include "strings.h"
#include "constants/party_menu.h"
#include "constants/flags.h"
#include "constants/opponents.h"
#include "constants/vars.h"
#include "constants/abilities.h"
#include "constants/items.h"
#include "constants/moves.h"
#include "constants/species.h"
#include "constants/trainers.h"
#include "trainer_pools.h"

// The Orange PC: currentBox + its boxes (the start of struct PokemonStorage), its box names and
// wallpapers, and the Orange team while it waits at home.
#define ORANGE_PC_HEAD_BYTES (offsetof(struct PokemonStorage, boxes) + sizeof(struct BoxPokemon) * IN_BOX_COUNT * ORANGE_BOXES_COUNT)

static const u8 sText_Orange[] = _("ORANGE ");

u8 GetActiveBoxCount(void)
{
    return FlagGet(FLAG_PC_SHOWS_ORANGE) ? ORANGE_BOXES_COUNT : TOTAL_BOXES_COUNT;
}

// `team` is the team stored with that PC (NULL: leave it out when loading).
static void GetKantoParts(struct StoredPCPart *parts, struct Pokemon *team)
{
    parts[0].data = gPokemonStoragePtr;
    parts[0].size = sizeof(struct PokemonStorage);
    parts[1].data = team;
    parts[1].size = sizeof(gPlayerParty);
}

static void GetOrangeParts(struct StoredPCPart *parts, struct Pokemon *team)
{
    parts[0].data = gPokemonStoragePtr;
    parts[0].size = ORANGE_PC_HEAD_BYTES;
    parts[1].data = gPokemonStoragePtr->boxNames;
    parts[1].size = sizeof(gPokemonStoragePtr->boxNames[0]) * ORANGE_BOXES_COUNT;
    parts[2].data = gPokemonStoragePtr->boxWallpapers;
    parts[2].size = ORANGE_BOXES_COUNT;
    parts[3].data = team;
    parts[3].size = sizeof(gPlayerParty);
}

static void ResetOrangePC(void)
{
    u8 box, pos;

    gPokemonStoragePtr->currentBox = 0;
    for (box = 0; box < ORANGE_BOXES_COUNT; box++)
    {
        u8 *name = StringCopy(gPokemonStoragePtr->boxNames[box], sText_Orange);
        ConvertIntToDecimalStringN(name, box + 1, STR_CONV_MODE_LEFT_ALIGN, 1);
        gPokemonStoragePtr->boxWallpapers[box] = WALLPAPER_BEACH;
        for (pos = 0; pos < IN_BOX_COUNT; pos++)
            ZeroBoxMonData(&gPokemonStoragePtr->boxes[box][pos]);
    }
}

// After loading the Orange PC into RAM: the boxes past ORANGE_BOXES_COUNT hold leftovers.
static void TidyOrangePC(void)
{
    u8 box, pos;

    for (box = ORANGE_BOXES_COUNT; box < TOTAL_BOXES_COUNT; box++)
        for (pos = 0; pos < IN_BOX_COUNT; pos++)
            ZeroBoxMonData(&gPokemonStoragePtr->boxes[box][pos]);
    if (gPokemonStoragePtr->currentBox >= ORANGE_BOXES_COUNT)
        gPokemonStoragePtr->currentBox = 0;
}

static void AfterPartyChange(void)
{
    CompactPartySlots();
    CalculatePlayerPartyCount();
    if (gPlayerPartyCount == 0)
        DestroyFollower();
    else
        UpdateFollowerPokemonGraphic();
}

// Stores the PC that is in RAM. The team kept with that stored copy (the home team with the
// Kanto PC during the trip, the Orange team with the Orange PC at home) is carried over as is;
// otherwise the slot is unused and gets the current party as filler.
static bool8 StoreShownPC(void)
{
    struct StoredPCPart parts[4];
    bool8 orange = FlagGet(FLAG_PC_SHOWS_ORANGE);
    bool8 keepTeam = orange ? !FlagGet(FLAG_ORANGE_CHALLENGE) : FlagGet(FLAG_ORANGE_CHALLENGE);
    struct Pokemon *team = gPlayerParty;

    if (keepTeam && (!orange || FlagGet(FLAG_ORANGE_PC_SAVED)))
    {
        // gEnemyParty is free outside battle
        if (orange)
        {
            GetOrangeParts(parts, gEnemyParty);
            parts[0].data = parts[1].data = parts[2].data = NULL;
            if (!LoadStoredPC(STORED_PC_ORANGE, parts, 4))
                return FALSE;
        }
        else
        {
            GetKantoParts(parts, gEnemyParty);
            parts[0].data = NULL;
            if (!LoadStoredPC(STORED_PC_KANTO, parts, 2))
                return FALSE;
        }
        team = gEnemyParty;
    }
    if (orange)
    {
        GetOrangeParts(parts, team);
        if (!SaveStoredPC(STORED_PC_ORANGE, parts, 4))
            return FALSE;
        FlagSet(FLAG_ORANGE_PC_SAVED);
    }
    else
    {
        GetKantoParts(parts, team);
        if (!SaveStoredPC(STORED_PC_KANTO, parts, 2))
            return FALSE;
    }
    return TRUE;
}

// Puts the Kanto (FALSE) or Orange (TRUE) boxes in RAM. The party is not touched.
static bool8 ShowBoxSet(bool8 orange)
{
    struct StoredPCPart parts[4];

    if (FlagGet(FLAG_PC_SHOWS_ORANGE) == orange)
        return TRUE;
    if (!StoreShownPC())
        return FALSE;
    if (orange)
    {
        GetOrangeParts(parts, NULL);
        if (!FlagGet(FLAG_ORANGE_PC_SAVED) || !LoadStoredPC(STORED_PC_ORANGE, parts, 4))
            ResetOrangePC();
        TidyOrangePC();
        FlagSet(FLAG_PC_SHOWS_ORANGE);
    }
    else
    {
        GetKantoParts(parts, NULL);
        if (!LoadStoredPC(STORED_PC_KANTO, parts, 2))
            return FALSE; // RAM still holds the Orange PC, nothing lost
        FlagClear(FLAG_PC_SHOWS_ORANGE);
    }
    return TRUE;
}

// After beating Drake. VAR_0x8004: 0 = Kanto boxes, 1 = Orange boxes. VAR_RESULT: TRUE ok.
void OrangePC_ShowBoxSet(void)
{
    gSpecialVar_Result = ShowBoxSet(gSpecialVar_0x8004 != 0);
}

void OrangeChallenge_Begin(void)
{
    struct StoredPCPart parts[4];
    u8 i;

    gSpecialVar_Result = FALSE;
    if (FlagGet(FLAG_ORANGE_CHALLENGE))
        return;
    if (FlagGet(TRAINER_FLAGS_START + TRAINER_CHAMPION_DRAKE))
    {
        gSpecialVar_Result = TRUE; // after Drake the trip is a plain visit: no team swap, no caps
        return;
    }
    if (!ShowBoxSet(FALSE)) // the Kanto boxes may be switched out (after Drake)
        return;
    GetKantoParts(parts, gPlayerParty);
    if (!SaveStoredPC(STORED_PC_KANTO, parts, 2))
        return;

    // Kanto PC + home team are safe in flash: bring in the Orange PC + Orange team.
    GetOrangeParts(parts, gPlayerParty);
    if (!FlagGet(FLAG_ORANGE_PC_SAVED) || !LoadStoredPC(STORED_PC_ORANGE, parts, 4))
    {
        // First trip: a fresh Orange PC and no team (Prof. Ivy gives a starter).
        ResetOrangePC();
        for (i = 0; i < PARTY_SIZE; i++)
            ZeroMonData(&gPlayerParty[i]);
    }
    TidyOrangePC();
    FlagSet(FLAG_PC_SHOWS_ORANGE);
    FlagSet(FLAG_ORANGE_CHALLENGE);
    AfterPartyChange();
    gSpecialVar_Result = TRUE;
}

void OrangeChallenge_End(void)
{
    struct StoredPCPart parts[4];

    gSpecialVar_Result = FALSE;
    if (!FlagGet(FLAG_ORANGE_CHALLENGE))
        return;
    if (!ShowBoxSet(TRUE)) // the Orange boxes may be switched out (after Drake)
        return;
    GetOrangeParts(parts, gPlayerParty);
    if (!SaveStoredPC(STORED_PC_ORANGE, parts, 4))
        return;
    FlagSet(FLAG_ORANGE_PC_SAVED);

    // Only now replace RAM; a damaged Kanto copy leaves everything as it was.
    GetKantoParts(parts, gPlayerParty);
    if (!LoadStoredPC(STORED_PC_KANTO, parts, 2))
        return;

    FlagClear(FLAG_PC_SHOWS_ORANGE);
    FlagClear(FLAG_ORANGE_CHALLENGE);
    AfterPartyChange();
    gSpecialVar_Result = TRUE;
}

// ---------------------------------------------------------------------------
// Gym rules
//
// Type triad (Rudy): the player brings exactly 3 Pokemon, one of each type in
// VAR_0x8005/6/7 (e.g. TYPE_FIRE, TYPE_WATER, TYPE_GRASS).
//   setvar VAR_0x8005, TYPE_FIRE ... callnative OrangeGym_ChooseTypeTriad
//   waitstate   @ VAR_RESULT TRUE = party is now those 3, FALSE = cancelled
//   trainerbattle_single ...   @ the full party comes back when the battle ends
//
// No items (Danny): setflag FLAG_NO_BATTLE_ITEMS before the gauntlet and
// clearflag it afterwards; it is also cleared if the player loses.
//
// Rain arena (Cissy): overworld rain turns into permanent rain in battle, so
//   setweather WEATHER_RAIN / doweather   before trainerbattle_single, and
//   resetweather / doweather              in the post-battle script.
// A loss whites out to a Pokemon Center, which resets the weather by itself.
// ---------------------------------------------------------------------------

static void CB2_ReturnFromChooseTypeTriad(void)
{
    u8 i;
    u16 packed = 0;

    if (gSelectedOrderFromParty[0] == 0)
    {
        gSpecialVar_Result = FALSE;
    }
    else
    {
        for (i = 0; i < 3; i++)
            packed |= gSelectedOrderFromParty[i] << (i * 3);
        VarSet(VAR_ORANGE_TRIAD_ORDER, packed);
        SavePlayerParty();
        ReducePlayerPartyToThree();
        FlagSet(FLAG_ORANGE_PARTY_REDUCED);
        gSpecialVar_Result = TRUE;
    }
    SetMainCallback2(CB2_ReturnToFieldContinueScriptPlayMapMusic);
}

void OrangeGym_ChooseTypeTriad(void)
{
    u8 *str;
    u8 i;

    gTypeTriadTypes[0] = gSpecialVar_0x8005;
    gTypeTriadTypes[1] = gSpecialVar_0x8006;
    gTypeTriadTypes[2] = gSpecialVar_0x8007;

    // "Choose one FIRE, one WATER and one GRASS POKéMON."
    str = StringCopy(gTypeTriadMessage, gText_OrangeTriad_ChooseOne);
    for (i = 0; i < 3; i++)
    {
        str = StringCopy(str, gTypeNames[gTypeTriadTypes[i]]);
        if (i == 0)
            str = StringCopy(str, gText_OrangeTriad_CommaOne);
        else if (i == 1)
            str = StringCopy(str, gText_OrangeTriad_AndOne);
    }
    StringCopy(str, gText_OrangeTriad_Pokemon);

    gMain.savedCallback = CB2_ReturnFromChooseTypeTriad;
    InitChooseMonsForBattle(CHOOSE_MONS_FOR_TYPE_TRIAD);
}

// Rudy: the player picks any three POKeMON that share no type.
//   callnative OrangeGym_ChooseDistinctTypes
//   waitstate   @ VAR_RESULT TRUE = party is now those 3, FALSE = cancelled
void OrangeGym_ChooseDistinctTypes(void)
{
    gTypeTriadTypes[0] = TYPE_NONE;
    gTypeTriadTypes[1] = TYPE_NONE;
    gTypeTriadTypes[2] = TYPE_NONE;
    StringCopy(gTypeTriadMessage, gText_OrangeTriad_NoSharedType);
    gMain.savedCallback = CB2_ReturnFromChooseTypeTriad;
    InitChooseMonsForBattle(CHOOSE_MONS_FOR_TYPE_TRIAD);
}

// ---------------------------------------------------------------------------
// Rudy's counter team. For each of the player's three, Rudy sends the pool
// POKeMON with the best matchup against it; no species twice. Every entry
// has a dance or evasion move, his gym's style.
// ---------------------------------------------------------------------------
static const struct PoolMon sRudyCounterPool[] =
{
    // ELECTRIC
    { .species = SPECIES_ELECTABUZZ, .heldItem = ITEM_MAGNET, .iv = 250, .ability = 1,
      .moves = {MOVE_THUNDERBOLT, MOVE_BRICK_BREAK, MOVE_ICE_PUNCH, MOVE_DOUBLE_TEAM} },
    { .species = SPECIES_JOLTEON, .heldItem = ITEM_BRIGHT_POWDER, .iv = 250, .ability = 1,
      .moves = {MOVE_THUNDERBOLT, MOVE_PIN_MISSILE, MOVE_DOUBLE_KICK, MOVE_AGILITY} },
    { .species = SPECIES_MANECTRIC, .heldItem = ITEM_QUICK_CLAW, .iv = 250, .ability = 1,
      .moves = {MOVE_THUNDERBOLT, MOVE_BITE, MOVE_THUNDER_WAVE, MOVE_DOUBLE_TEAM} },
    // GRASS
    { .species = SPECIES_EXEGGUTOR, .heldItem = ITEM_MIRACLE_SEED, .iv = 250, .ability = 1,
      .moves = {MOVE_GIGA_DRAIN, MOVE_PSYCHIC, MOVE_SLEEP_POWDER, MOVE_EGG_BOMB} },
    { .species = SPECIES_BELLOSSOM, .heldItem = ITEM_LEFTOVERS, .iv = 250, .ability = 1,
      .moves = {MOVE_PETAL_DANCE, MOVE_MAGICAL_LEAF, MOVE_STUN_SPORE, MOVE_DOUBLE_TEAM} },
    { .species = SPECIES_LUDICOLO, .heldItem = ITEM_LEFTOVERS, .iv = 250, .ability = 1,
      .moves = {MOVE_SURF, MOVE_GIGA_DRAIN, MOVE_ICE_BEAM, MOVE_RAIN_DANCE} },
    // WATER
    { .species = SPECIES_STARMIE, .heldItem = ITEM_LEFTOVERS, .iv = 250, .ability = 2,
      .moves = {MOVE_SURF, MOVE_PSYCHIC, MOVE_THUNDERBOLT, MOVE_RECOVER} },
    { .species = SPECIES_GOLDUCK, .heldItem = ITEM_MYSTIC_WATER, .iv = 250, .ability = 1,
      .moves = {MOVE_SURF, MOVE_ICE_BEAM, MOVE_PSYCHIC, MOVE_DOUBLE_TEAM} },
    { .species = SPECIES_KINGLER, .heldItem = ITEM_KINGS_ROCK, .iv = 250, .ability = TRAINER_MON_HIDDEN_ABILITY,
      .moves = {MOVE_CRABHAMMER, MOVE_MUD_SHOT, MOVE_METAL_CLAW, MOVE_SWORDS_DANCE} },
    { .species = SPECIES_POLIWRATH, .heldItem = ITEM_LEFTOVERS, .iv = 250, .ability = 1,
      .moves = {MOVE_SURF, MOVE_BRICK_BREAK, MOVE_ICE_BEAM, MOVE_HYPNOSIS} },
    { .species = SPECIES_CRAWDAUNT, .heldItem = ITEM_SCOPE_LENS, .iv = 250, .ability = TRAINER_MON_HIDDEN_ABILITY,
      .moves = {MOVE_CRABHAMMER, MOVE_CRUNCH, MOVE_KNOCK_OFF, MOVE_SWORDS_DANCE} },
    // FIRE
    { .species = SPECIES_ARCANINE, .heldItem = ITEM_CHARCOAL, .iv = 250, .ability = 1,
      .moves = {MOVE_FLAMETHROWER, MOVE_BITE, MOVE_IRON_TAIL, MOVE_AGILITY} },
    { .species = SPECIES_NINETALES, .heldItem = ITEM_BRIGHT_POWDER, .iv = 250, .ability = TRAINER_MON_HIDDEN_ABILITY,
      .moves = {MOVE_FLAMETHROWER, MOVE_WILL_O_WISP, MOVE_CONFUSE_RAY, MOVE_QUICK_ATTACK} },
    { .species = SPECIES_MAGMAR, .heldItem = ITEM_CHARCOAL, .iv = 250, .ability = 1,
      .moves = {MOVE_FLAMETHROWER, MOVE_THUNDER_PUNCH, MOVE_CONFUSE_RAY, MOVE_SMOKESCREEN} },
    { .species = SPECIES_HOUNDOOM, .heldItem = ITEM_BLACK_GLASSES, .iv = 250, .ability = 2,
      .moves = {MOVE_FLAMETHROWER, MOVE_CRUNCH, MOVE_ROAR, MOVE_SMOG} },
    // GROUND / ROCK
    { .species = SPECIES_DUGTRIO, .heldItem = ITEM_SOFT_SAND, .iv = 250, .ability = 1,
      .moves = {MOVE_DIG, MOVE_ROCK_SLIDE, MOVE_SLASH, MOVE_SAND_ATTACK} },
    { .species = SPECIES_MAROWAK, .heldItem = ITEM_HARD_STONE, .iv = 250, .ability = 2,
      .moves = {MOVE_BONEMERANG, MOVE_ROCK_SLIDE, MOVE_DOUBLE_TEAM, MOVE_SWORDS_DANCE} },
    { .species = SPECIES_RHYDON, .heldItem = ITEM_QUICK_CLAW, .iv = 250, .ability = 2,
      .moves = {MOVE_EARTHQUAKE, MOVE_ROCK_SLIDE, MOVE_IRON_TAIL, MOVE_SCARY_FACE} },
    { .species = SPECIES_KABUTOPS, .heldItem = ITEM_SCOPE_LENS, .iv = 250, .ability = 1,
      .moves = {MOVE_ROCK_SLIDE, MOVE_SURF, MOVE_SLASH, MOVE_SWORDS_DANCE} },
    // FIGHTING
    { .species = SPECIES_HITMONLEE, .heldItem = ITEM_BLACK_BELT, .iv = 250, .ability = 1,
      .moves = {MOVE_HI_JUMP_KICK, MOVE_ROCK_SLIDE, MOVE_EARTHQUAKE, MOVE_DOUBLE_TEAM} },
    { .species = SPECIES_HITMONCHAN, .heldItem = ITEM_BLACK_BELT, .iv = 250, .ability = 1,
      .moves = {MOVE_MACH_PUNCH, MOVE_FIRE_PUNCH, MOVE_ICE_PUNCH, MOVE_THUNDER_PUNCH} },
    { .species = SPECIES_MACHOKE, .heldItem = ITEM_QUICK_CLAW, .iv = 250, .ability = 1,
      .moves = {MOVE_BRICK_BREAK, MOVE_ROCK_SLIDE, MOVE_SEISMIC_TOSS, MOVE_BULK_UP} },
    // ICE / PSYCHIC
    { .species = SPECIES_DEWGONG, .heldItem = ITEM_NEVER_MELT_ICE, .iv = 250, .ability = 1,
      .moves = {MOVE_ICE_BEAM, MOVE_SURF, MOVE_HEADBUTT, MOVE_DOUBLE_TEAM} },
    { .species = SPECIES_JYNX, .heldItem = ITEM_NEVER_MELT_ICE, .iv = 250, .ability = 1,
      .moves = {MOVE_ICE_PUNCH, MOVE_PSYCHIC, MOVE_LOVELY_KISS, MOVE_DOUBLE_TEAM} },
    { .species = SPECIES_XATU, .heldItem = ITEM_TWISTED_SPOON, .iv = 250, .ability = TRAINER_MON_HIDDEN_ABILITY,
      .moves = {MOVE_PSYCHIC, MOVE_WING_ATTACK, MOVE_CONFUSE_RAY, MOVE_NIGHT_SHADE} },
    // FLYING / BUG
    { .species = SPECIES_PIDGEOT, .heldItem = ITEM_SHARP_BEAK, .iv = 250, .ability = 1,
      .moves = {MOVE_AERIAL_ACE, MOVE_QUICK_ATTACK, MOVE_FEATHER_DANCE, MOVE_SAND_ATTACK} },
    { .species = SPECIES_SWELLOW, .heldItem = ITEM_SHARP_BEAK, .iv = 250, .ability = 1,
      .moves = {MOVE_AERIAL_ACE, MOVE_FACADE, MOVE_QUICK_ATTACK, MOVE_DOUBLE_TEAM} },
    { .species = SPECIES_SCYTHER, .heldItem = ITEM_SILVER_POWDER, .iv = 250, .ability = 1,
      .moves = {MOVE_SILVER_WIND, MOVE_WING_ATTACK, MOVE_SWORDS_DANCE, MOVE_DOUBLE_TEAM} },
    // GHOST / STEEL / DRAGON / POISON
    { .species = SPECIES_GENGAR, .heldItem = ITEM_SPELL_TAG, .iv = 250, .ability = 1,
      .moves = {MOVE_SHADOW_BALL, MOVE_THUNDERBOLT, MOVE_HYPNOSIS, MOVE_CONFUSE_RAY} },
    { .species = SPECIES_SKARMORY, .heldItem = ITEM_METAL_COAT, .iv = 250, .ability = 1,
      .moves = {MOVE_STEEL_WING, MOVE_DRILL_PECK, MOVE_AGILITY, MOVE_SAND_ATTACK} },
    { .species = SPECIES_ALTARIA, .heldItem = ITEM_DRAGON_FANG, .iv = 250, .ability = 1,
      .moves = {MOVE_DRAGON_BREATH, MOVE_AERIAL_ACE, MOVE_DRAGON_DANCE, MOVE_TAKE_DOWN} },
    { .species = SPECIES_MUK, .heldItem = ITEM_POISON_BARB, .iv = 250, .ability = 2,
      .moves = {MOVE_SLUDGE_BOMB, MOVE_FIRE_PUNCH, MOVE_THUNDER_PUNCH, MOVE_MINIMIZE} },
};

static const u8 sRudyCounterLevels[3] = {36, 37, 38};

// Type multiplier of an attack type against a target, in hundredths (0, 25, 50, 100, 200, 400),
// including the abilities that change it.
static u16 RudyTypeMultiplier(u8 atkType, u16 species, u8 ability)
{
    u16 mult = 100;
    s32 i;

    if ((ability == ABILITY_LEVITATE && atkType == TYPE_GROUND)
     || (ability == ABILITY_VOLT_ABSORB && atkType == TYPE_ELECTRIC)
     || (ability == ABILITY_WATER_ABSORB && atkType == TYPE_WATER)
     || (ability == ABILITY_FLASH_FIRE && atkType == TYPE_FIRE))
        return 0;
    for (i = 0; TYPE_EFFECT_ATK_TYPE(i) != TYPE_ENDTABLE; i += 3)
    {
        if (TYPE_EFFECT_ATK_TYPE(i) == TYPE_FORESIGHT)
            break; // the rest only applies after Foresight
        if (TYPE_EFFECT_ATK_TYPE(i) != atkType)
            continue;
        if (TYPE_EFFECT_DEF_TYPE(i) == gSpeciesInfo[species].types[0])
            mult = mult * TYPE_EFFECT_MULTIPLIER(i) / TYPE_MUL_NORMAL;
        if (TYPE_EFFECT_DEF_TYPE(i) == gSpeciesInfo[species].types[1]
         && gSpeciesInfo[species].types[1] != gSpeciesInfo[species].types[0])
            mult = mult * TYPE_EFFECT_MULTIPLIER(i) / TYPE_MUL_NORMAL;
    }
    if (ability == ABILITY_THICK_FAT && (atkType == TYPE_FIRE || atkType == TYPE_ICE))
        mult /= 2;
    if (ability == ABILITY_WONDER_GUARD && mult <= 100)
        return 0;
    return mult;
}

// The best a moveset can do to a target: type multiplier x 1.5 for STAB.
// A moveset with no damaging moves falls back to its user's own types.
static u16 RudyBestHit(const u16 *moves, u16 atkSpecies, u16 defSpecies, u8 defAbility)
{
    u16 best = 0, score;
    bool8 any = FALSE;
    s32 i;

    for (i = 0; i < MAX_MON_MOVES; i++)
    {
        u8 type;

        if (moves[i] == MOVE_NONE || gBattleMoves[moves[i]].power <= 1)
            continue;
        any = TRUE;
        type = gBattleMoves[moves[i]].type;
        score = RudyTypeMultiplier(type, defSpecies, defAbility);
        if (type == gSpeciesInfo[atkSpecies].types[0] || type == gSpeciesInfo[atkSpecies].types[1])
            score = score * 3 / 2;
        if (score > best)
            best = score;
    }
    if (!any)
    {
        for (i = 0; i < 2; i++)
        {
            score = RudyTypeMultiplier(gSpeciesInfo[atkSpecies].types[i], defSpecies, defAbility) * 3 / 2;
            if (score > best)
                best = score;
        }
    }
    return best;
}

static u8 RudyPoolAbility(const struct PoolMon *poolMon)
{
    if (poolMon->ability == TRAINER_MON_HIDDEN_ABILITY && gSpeciesInfo[poolMon->species].hiddenAbility != ABILITY_NONE)
        return gSpeciesInfo[poolMon->species].hiddenAbility;
    if (poolMon->ability == 2 && gSpeciesInfo[poolMon->species].abilities[1] != ABILITY_NONE)
        return gSpeciesInfo[poolMon->species].abilities[1];
    return gSpeciesInfo[poolMon->species].abilities[0];
}

// Builds Rudy's team against gPlayerParty[0..2]. Returns the party size (0 = use his fixed team).
u8 OrangeGym_CreateRudyParty(struct Pokemon *party)
{
    s16 score[3][NELEMS(sRudyCounterPool)];
    s32 best = -0x7FFFFFFF, total;
    u8 pick[3] = {0, 0, 0};
    u8 count, i, j, k, a, b, c;

    count = 0;
    for (i = 0; i < 3 && i < gPlayerPartyCount; i++)
        if (GetMonData(&gPlayerParty[i], MON_DATA_SPECIES) != SPECIES_NONE)
            count++;
    if (count == 0)
        return 0;

    // score[i][k]: how well pool entry k does against the player's mon i
    for (i = 0; i < count; i++)
    {
        u16 moves[MAX_MON_MOVES];
        u16 species = GetMonData(&gPlayerParty[i], MON_DATA_SPECIES);
        u8 ability = GetMonAbility(&gPlayerParty[i]);

        for (j = 0; j < MAX_MON_MOVES; j++)
            moves[j] = GetMonData(&gPlayerParty[i], MON_DATA_MOVE1 + j);
        for (k = 0; k < NELEMS(sRudyCounterPool); k++)
        {
            const struct PoolMon *poolMon = &sRudyCounterPool[k];
            u16 offense = RudyBestHit(poolMon->moves, poolMon->species, species, ability);
            u16 defense = RudyBestHit(moves, species, poolMon->species, RudyPoolAbility(poolMon));

            score[i][k] = 2 * offense - defense;
        }
    }

    // Best combination with no species twice (the pool has one entry per species)
    for (a = 0; a < NELEMS(sRudyCounterPool); a++)
    {
        if (count == 1)
        {
            if (score[0][a] > best)
                best = score[0][a], pick[0] = a;
            continue;
        }
        for (b = 0; b < NELEMS(sRudyCounterPool); b++)
        {
            if (b == a)
                continue;
            if (count == 2)
            {
                total = score[0][a] + score[1][b];
                if (total > best)
                    best = total, pick[0] = a, pick[1] = b;
                continue;
            }
            for (c = 0; c < NELEMS(sRudyCounterPool); c++)
            {
                if (c == a || c == b)
                    continue;
                total = score[0][a] + score[1][b] + score[2][c];
                if (total > best)
                    best = total, pick[0] = a, pick[1] = b, pick[2] = c;
            }
        }
    }

    for (i = 0; i < count; i++)
    {
        const struct PoolMon *poolMon = &sRudyCounterPool[pick[i]];
        u32 personality = 0x88 | ((u32)poolMon->species << 8) | ((u32)(i + 1) << 20);
        u8 abilityNum = (poolMon->ability == 2 && gSpeciesInfo[poolMon->species].abilities[1] != ABILITY_NONE);
        u8 iv = poolMon->iv * MAX_PER_STAT_IVS / 255;

        if (poolMon->ability == TRAINER_MON_HIDDEN_ABILITY && gSpeciesInfo[poolMon->species].hiddenAbility != ABILITY_NONE)
            abilityNum = ABILITY_NUM_HIDDEN;

        CreateMon(&party[i], poolMon->species, sRudyCounterLevels[3 - count + i], iv, TRUE, personality, OT_ID_RANDOM_NO_SHINY, 0);
        SetMonData(&party[i], MON_DATA_HELD_ITEM, &poolMon->heldItem);
        for (j = 0; j < MAX_MON_MOVES; j++)
        {
            SetMonData(&party[i], MON_DATA_MOVE1 + j, &poolMon->moves[j]);
            SetMonData(&party[i], MON_DATA_PP1 + j, &gBattleMoves[poolMon->moves[j]].pp);
        }
        SetMonData(&party[i], MON_DATA_ABILITY_NUM, &abilityNum);
    }
    return count;
}

// Puts the 3 battlers back in their original slots and restores the rest.
void OrangeGym_RestoreParty(void)
{
    u8 i;
    u16 packed;

    if (!FlagGet(FLAG_ORANGE_PARTY_REDUCED))
        return;
    packed = VarGet(VAR_ORANGE_TRIAD_ORDER);
    for (i = 0; i < 3; i++)
        gEnemyParty[i] = gPlayerParty[i];
    LoadPlayerParty();
    for (i = 0; i < 3; i++)
    {
        u8 slot = (packed >> (i * 3)) & 7;

        if (slot != 0)
            gPlayerParty[slot - 1] = gEnemyParty[i];
        ZeroMonData(&gEnemyParty[i]);
    }
    CalculatePlayerPartyCount();
    FlagClear(FLAG_ORANGE_PARTY_REDUCED);
}

// Called when any trainer battle ends, before a whiteout.
void OrangeGym_OnTrainerBattleEnd(bool8 playerLost)
{
    OrangeGym_RestoreParty();
    if (playerLost)
        FlagClear(FLAG_NO_BATTLE_ITEMS);
}
