# Run from the repo root, after orange_leader_art.py.
# Rudy (Trovita Gym) counter-picks: the player brings three POKeMON that share no type, and Rudy
# builds his team at the start of the battle from a pool, choosing for each of the player's three
# the pool POKeMON with the best matchup against it (its moves vs. their types, and the player's
# moves vs. its types). His lead is picked against the player's lead, and so on.
#  - Picker: OrangeGym_ChooseDistinctTypes (CHOOSE_MONS_FOR_TYPE_TRIAD with gTypeTriadTypes[0] =
#    TYPE_NONE means "any three, no shared type").
#  - Team: OrangeGym_CreateRudyParty (src/orange_islands.c), called from CreateNPCTrainerParty
#    while FLAG_ORANGE_PARTY_REDUCED is set. Without it Rudy falls back to his fixed anime trio.
import re

def rd(p): return open(p, encoding='utf-8').read()
def wr(p, s): open(p, 'w', newline='\n', encoding='utf-8').write(s)
def sub1(t, old, new, label):
    assert old in t, label
    return t.replace(old, new, 1)

# ---------------------------------------------------------------- party menu: distinct-type mode
p = 'src/party_menu.c'; t = rd(p)
if 'TypeTriadDistinct' not in t:
    t = sub1(t, '''    case CHOOSE_MONS_FOR_TYPE_TRIAD:
        if (GetMonData(mon, MON_DATA_HP) == 0)
            return FALSE;
        species = GetMonData(mon, MON_DATA_SPECIES);
''', '''    case CHOOSE_MONS_FOR_TYPE_TRIAD:
        if (GetMonData(mon, MON_DATA_HP) == 0)
            return FALSE;
        if (gTypeTriadTypes[0] == TYPE_NONE) // any three that share no type
            return TRUE;
        species = GetMonData(mon, MON_DATA_SPECIES);
''', 'eligibility')
    t = sub1(t, '''// TRUE if the 3 chosen mons can be matched one-to-one with the 3 required types
static bool8 TypeTriadCovered(const u8 *order)
{
    static const u8 perms[6][3] = {{0, 1, 2}, {0, 2, 1}, {1, 0, 2}, {1, 2, 0}, {2, 0, 1}, {2, 1, 0}};
    u8 p, i;

''', '''// TRUE if no two of the 3 chosen mons share a type
static bool8 TypeTriadDistinct(const u8 *order)
{
    u8 i, j, a, b;

    for (i = 0; i < 3; i++)
    {
        u16 si = GetMonData(&gPlayerParty[order[i] - 1], MON_DATA_SPECIES);

        for (j = i + 1; j < 3; j++)
        {
            u16 sj = GetMonData(&gPlayerParty[order[j] - 1], MON_DATA_SPECIES);

            for (a = 0; a < 2; a++)
                for (b = 0; b < 2; b++)
                    if (gSpeciesInfo[si].types[a] == gSpeciesInfo[sj].types[b])
                        return FALSE;
        }
    }
    return TRUE;
}

// TRUE if the 3 chosen mons can be matched one-to-one with the 3 required types
// (or, with gTypeTriadTypes[0] == TYPE_NONE, if they share no type)
static bool8 TypeTriadCovered(const u8 *order)
{
    static const u8 perms[6][3] = {{0, 1, 2}, {0, 2, 1}, {1, 0, 2}, {1, 2, 0}, {2, 0, 1}, {2, 1, 0}};
    u8 p, i;

    if (gTypeTriadTypes[0] == TYPE_NONE)
        return TypeTriadDistinct(order);
''', 'covered')
    wr(p, t)

# ---------------------------------------------------------------- strings
p = 'src/strings.c'; t = rd(p)
if 'gText_OrangeTriad_NoSharedType' not in t:
    t = sub1(t, 'const u8 gText_OrangeTriad_Pokemon[] = _(" POKéMON.");\n',
             'const u8 gText_OrangeTriad_Pokemon[] = _(" POKéMON.");\n'
             'const u8 gText_OrangeTriad_NoSharedType[] = _("No two may share a type.");\n', 'strings')
    wr(p, t)
p = 'include/strings.h'; t = rd(p)
if 'gText_OrangeTriad_NoSharedType' not in t:
    t = sub1(t, 'extern const u8 gText_OrangeTriad_ChooseOne[];\n',
             'extern const u8 gText_OrangeTriad_ChooseOne[];\nextern const u8 gText_OrangeTriad_NoSharedType[];\n', 'strings.h')
    wr(p, t)

# ---------------------------------------------------------------- orange_islands.c
p = 'src/orange_islands.c'; t = rd(p)
if 'OrangeGym_CreateRudyParty' not in t:
    t = sub1(t, '#include "constants/vars.h"\n', '''#include "constants/vars.h"
#include "constants/abilities.h"
#include "constants/items.h"
#include "constants/moves.h"
#include "constants/species.h"
#include "constants/trainers.h"
#include "trainer_pools.h"
''', 'includes')

    t = sub1(t, '''// Puts the 3 battlers back in their original slots and restores the rest.''', r'''// Rudy: the player picks any three POKeMON that share no type.
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
    { .species = SPECIES_KINGLER, .heldItem = ITEM_KINGS_ROCK, .iv = 250, .ability = 1,
      .moves = {MOVE_CRABHAMMER, MOVE_MUD_SHOT, MOVE_METAL_CLAW, MOVE_SWORDS_DANCE} },
    { .species = SPECIES_POLIWRATH, .heldItem = ITEM_LEFTOVERS, .iv = 250, .ability = 1,
      .moves = {MOVE_SURF, MOVE_BRICK_BREAK, MOVE_ICE_BEAM, MOVE_HYPNOSIS} },
    { .species = SPECIES_CRAWDAUNT, .heldItem = ITEM_SCOPE_LENS, .iv = 250, .ability = 1,
      .moves = {MOVE_CRABHAMMER, MOVE_CRUNCH, MOVE_KNOCK_OFF, MOVE_SWORDS_DANCE} },
    // FIRE
    { .species = SPECIES_ARCANINE, .heldItem = ITEM_CHARCOAL, .iv = 250, .ability = 1,
      .moves = {MOVE_FLAMETHROWER, MOVE_BITE, MOVE_IRON_TAIL, MOVE_AGILITY} },
    { .species = SPECIES_NINETALES, .heldItem = ITEM_BRIGHT_POWDER, .iv = 250, .ability = 1,
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
    { .species = SPECIES_XATU, .heldItem = ITEM_TWISTED_SPOON, .iv = 250, .ability = 1,
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

// Puts the 3 battlers back in their original slots and restores the rest.''', 'rudy funcs')
    wr(p, t)

# header for battle_main.c
p = 'include/battle_main.h'; t = rd(p)
if 'OrangeGym_CreateRudyParty' not in t:
    t = sub1(t, 'extern const u8 gTypeEffectiveness[336];\n',
             'extern const u8 gTypeEffectiveness[336];\n\nu8 OrangeGym_CreateRudyParty(struct Pokemon *party); // src/orange_islands.c\n', 'bm.h')
    wr(p, t)

# ---------------------------------------------------------------- battle_main.c hook
p = 'src/battle_main.c'; t = rd(p)
if 'OrangeGym_CreateRudyParty' not in t:
    t = sub1(t, '''        ZeroEnemyPartyMons();
        if (gTrainers[trainerNum].partyFlags & F_TRAINER_PARTY_POOL)''', '''        ZeroEnemyPartyMons();
        if (trainerNum == TRAINER_LEADER_RUDY && FlagGet(FLAG_ORANGE_PARTY_REDUCED))
        {
            u8 count = OrangeGym_CreateRudyParty(party);
            if (count != 0)
                return count;
        }
        if (gTrainers[trainerNum].partyFlags & F_TRAINER_PARTY_POOL)''', 'hook')
    wr(p, t)

# ---------------------------------------------------------------- Rudy's fallback team: the anime trio
p = 'src/data/trainer_parties.h'; t = rd(p)
m = re.search(r'static const struct TrainerMonItemCustomMoves sParty_LeaderRudy\[\] = \{.*?\n\};\n', t, re.S)
assert m, 'rudy party'
t = t[:m.start()] + '''static const struct TrainerMonItemCustomMoves sParty_LeaderRudy[] = {
    // Only used if the counter team can't be built (see OrangeGym_CreateRudyParty)
    {
        .iv = 250,
        .lvl = 36,
        .species = SPECIES_ELECTABUZZ,
        .heldItem = ITEM_MAGNET,
        .moves = {MOVE_THUNDERBOLT, MOVE_BRICK_BREAK, MOVE_ICE_PUNCH, MOVE_DOUBLE_TEAM},
    },
    {
        .iv = 250,
        .lvl = 37,
        .species = SPECIES_EXEGGUTOR,
        .heldItem = ITEM_MIRACLE_SEED,
        .moves = {MOVE_GIGA_DRAIN, MOVE_PSYCHIC, MOVE_SLEEP_POWDER, MOVE_EGG_BOMB},
    },
    {
        .iv = 250,
        .lvl = 38,
        .species = SPECIES_STARMIE,
        .heldItem = ITEM_LEFTOVERS,
        .moves = {MOVE_SURF, MOVE_PSYCHIC, MOVE_THUNDERBOLT, MOVE_RECOVER},
    },
};
''' + t[m.end():]
wr(p, t)

# ---------------------------------------------------------------- gym script + text
p = 'data/maps/OrangeIslands_Trovita_Gym/scripts.inc'; t = rd(p)
t = t.replace('''	setvar VAR_0x8005, TYPE_FIRE
	setvar VAR_0x8006, TYPE_WATER
	setvar VAR_0x8007, TYPE_GRASS
	callnative OrangeGym_ChooseTypeTriad
''', '''	callnative OrangeGym_ChooseDistinctTypes
''')
t = t.replace('@ Type triad: the player battles with one FIRE, one WATER and one GRASS POKéMON.',
              "@ The player brings three POKéMON that share no type; Rudy builds a team to counter them.")
def text(label, body):
    global t
    m = re.search(r'(' + label + r'::\n)((?:\t\.string .*\n)+)', t)
    assert m, label
    t = t[:m.start(2)] + ''.join('\t.string "%s"\n' % l for l in body) + t[m.end(2):]
text('OrangeIslands_Trovita_Gym_Text_RudyChallenge', [
    "Welcome to TROVITA. I'm RUDY.\\p",
    "A battle is a dance. I watch my\\n",
    "partner, then I make my move.\\p",
    "Bring me three POKéMON, no two\\n",
    "sharing a type. I'll see what\\l",
    "you've got, then pick my team.\\p",
    "Shall we?$"])
text('OrangeIslands_Trovita_Gym_Text_ChooseThree', [
    "Pick three POKéMON. No two may\\n",
    "share a type.$"])
text('OrangeIslands_Trovita_Gym_Text_RudyDeclined', [
    "Come back when you have three\\n",
    "POKéMON that share no type.$"])
text('OrangeIslands_Trovita_Gym_Text_RudyIntro', [
    "I see… Yes, I know just who to\\n",
    "send against each of those.\\p",
    "Let's dance!$"])
text('OrangeIslands_Trovita_Gym_Text_RudyDefeat', [
    "You stayed a step ahead of me.\\n",
    "You've earned this.$"])
text('OrangeIslands_Trovita_Gym_Text_ExplainTM10', [
    "TM10 is HIDDEN POWER. Its type\\n",
    "is different for every POKéMON.\\p",
    "Good for surprising someone who\\n",
    "thinks they've read you!$"])
text('OrangeIslands_Trovita_Gym_Text_GymGuyAdvice', [
    "Yo! RUDY's rule: three POKéMON,\\n",
    "and no two can share a type.\\p",
    "Here's the catch. RUDY looks at\\n",
    "your three and picks a POKéMON\\l",
    "to beat each one!\\p",
    "So bring three that cover each\\n",
    "other. If he counters one, the\\l",
    "next should beat his counter!$"])
text('OrangeIslands_Trovita_Gym_Text_GymGuyAfter', [
    "RUDY read your team and you\\n",
    "still won! Nicely done!$"])
text('OrangeIslands_Trovita_Gym_Text_RowanPostBattle', [
    "RUDY picks his team after he\\n",
    "sees yours. Watch out for moves\\l",
    "that cover his weak spots!$"])
wr(p, t)
print('rudy counter ok')
