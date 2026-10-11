// Advanced trainer AI (AI_LEVEL_ADVANCED in src/data/trainers.h).
//
// This runs on top of the normal script AI (data/battle_ai_scripts.s). The scripts score
// every move first; this file then nudges those scores using real damage calcs and what
// the trainer has *seen* the player's Pokemon do this battle. It never reads the moves the
// player hasn't used yet - for those it assumes the Pokemon may have a decent STAB attack.
//
// What it does:
//   Kill priority  - a KO it is sure to land (it moves first, or uses a priority move) is
//                    taken over almost anything else; accurate KO moves beat inaccurate ones.
//   Setup          - boosting moves are valued when the player can't seriously threaten it
//                    (it survives at least two of the player's best known hits), and dropped
//                    once it is already at +2 or when it is in danger.
//   Danger         - if it is about to be KO'd before it moves, it stops setting up and uses
//                    its strongest hit (or a priority move, or Explosion).
//   Switch predict - if the player is in a bad matchup and has a Pokemon the trainer has seen
//                    that walls its best attack, it may pick a move that also hits that one.
//   Own switching  - it switches out of a matchup where it would be KO'd first (or where it
//                    can't do damage) into the party member that best takes the player's
//                    known moves, and picks replacements the same way after a faint.
//
// Tweak the numbers in the "Tuning" block below to make it more or less aggressive.

#include "global.h"
#include "battle.h"
#include "battle_main.h"
#include "battle_anim.h"
#include "util.h"
#include "battle_controllers.h"
#include "battle_script_commands.h"
#include "battle_ai_advanced.h"
#include "pokemon.h"
#include "random.h"
#include "constants/abilities.h"
#include "constants/battle_ai.h"
#include "constants/battle_move_effects.h"
#include "constants/moves.h"
#include "constants/trainers.h"

// ---------------------------------------------------------------------------------------
// Tuning
#define SCORE_KO_FIRST          8   // a KO that lands before the player moves
#define SCORE_KO_PRIORITY_BONUS 2   // ...and it uses a priority move to get there
#define SCORE_KO_SLOWER         6   // a KO after the player moves, when it survives the hit
#define SCORE_NOT_KO_WHEN_KO    3   // penalty for damaging non-KO moves when a sure KO exists
#define SCORE_STATUS_WHEN_KO    6   // penalty for status moves when a sure KO exists
#define SCORE_BEST_DAMAGE       3   // its highest damage move
#define SCORE_PRIORITY_CHIP     4   // a priority attack when it will be KO'd before moving
#define SCORE_STATUS_IN_DANGER  6   // penalty for status moves when it will be KO'd first
#define SCORE_EXPLODE_IN_DANGER 6   // Explosion/Self-Destruct when it will be KO'd first
#define SCORE_SETUP_SAFE        5   // first boost while safe
#define SCORE_SETUP_SAFE_AGAIN  3   // second boost while safe
#define SCORE_SETUP_ENOUGH      6   // penalty once already at +2
#define SCORE_SETUP_UNSAFE      8   // penalty for boosting while threatened
#define SCORE_PREDICT_SE        3   // move is super effective on the predicted switch-in
#define SCORE_PREDICT_NEUTRAL   1
#define SCORE_PREDICT_RESISTED  1   // penalty
#define SCORE_PREDICT_IMMUNE    3   // penalty
#define PREDICT_CHANCE          50  // % of turns it tries to read a switch
#define STAY_IN_CHANCE          25  // % of the time it stays in even when it "should" switch
#define UNKNOWN_MOVE_PERCENT    80  // how hard it assumes an unseen STAB attack hits (% of a calc)
// ---------------------------------------------------------------------------------------

#define MULT_NEUTRAL 10

// Every move each of the player's party members has used this battle.
static u16 sSeenMoves[PARTY_SIZE][MAX_MON_MOVES];
static u8 sSeenMons;                 // bit per player party slot that has been sent out
static u8 sLastAISwitchTurn;         // stops the AI switching back and forth every turn

// A typical STAB attack for each type, used to guess at moves the player hasn't shown yet.
static const u16 sTypicalStabMove[] =
{
    [TYPE_NORMAL]   = MOVE_STRENGTH,
    [TYPE_FIGHTING] = MOVE_BRICK_BREAK,
    [TYPE_FLYING]   = MOVE_DRILL_PECK,
    [TYPE_POISON]   = MOVE_SLUDGE_BOMB,
    [TYPE_GROUND]   = MOVE_DIG,
    [TYPE_ROCK]     = MOVE_ROCK_SLIDE,
    [TYPE_BUG]      = MOVE_SIGNAL_BEAM,
    [TYPE_GHOST]    = MOVE_SHADOW_BALL,
    [TYPE_STEEL]    = MOVE_STEEL_WING,
    [TYPE_MYSTERY]  = MOVE_NONE,
    [TYPE_FIRE]     = MOVE_FLAMETHROWER,
    [TYPE_WATER]    = MOVE_SURF,
    [TYPE_GRASS]    = MOVE_GIGA_DRAIN,
    [TYPE_ELECTRIC] = MOVE_THUNDERBOLT,
    [TYPE_PSYCHIC]  = MOVE_PSYCHIC,
    [TYPE_ICE]      = MOVE_ICE_BEAM,
    [TYPE_DRAGON]   = MOVE_DRAGON_CLAW,
    [TYPE_DARK]     = MOVE_CRUNCH,
};

// ---------------------------------------------------------------------------------------
// Who gets the advanced AI

u32 AI_GetTrainerAIFlags(void)
{
    if (!(gBattleTypeFlags & BATTLE_TYPE_TRAINER))
        return 0;
    if (gBattleTypeFlags & (BATTLE_TYPE_TRAINER_TOWER | BATTLE_TYPE_EREADER_TRAINER | BATTLE_TYPE_BATTLE_TOWER
                          | BATTLE_TYPE_SAFARI | BATTLE_TYPE_LINK | BATTLE_TYPE_FIRST_BATTLE))
        return 0;
    if (gTrainerBattleOpponent_A == TRAINER_SECRET_BASE)
        return 0;
    return gTrainers[gTrainerBattleOpponent_A].aiFlags;
}

bool8 AI_IsAdvanced(void)
{
    return (AI_GetTrainerAIFlags() & AI_SCRIPT_ADVANCED) != 0;
}

// ---------------------------------------------------------------------------------------
// Memory of the player's moves

void AI_ResetPlayerMoveMemory(void)
{
    s32 i, j;

    for (i = 0; i < PARTY_SIZE; i++)
        for (j = 0; j < MAX_MON_MOVES; j++)
            sSeenMoves[i][j] = MOVE_NONE;
    sSeenMons = 0;
    sLastAISwitchTurn = 0xFF;
}

void AI_RecordPlayerMove(u8 battler, u16 move)
{
    s32 i;
    u8 partyId;

    if (GetBattlerSide(battler) != B_SIDE_PLAYER || move == MOVE_NONE || move == MOVE_UNAVAILABLE || move == MOVE_STRUGGLE)
        return;
    partyId = gBattlerPartyIndexes[battler];
    if (partyId >= PARTY_SIZE)
        return;
    sSeenMons |= 1 << partyId;
    for (i = 0; i < MAX_MON_MOVES; i++)
    {
        if (sSeenMoves[partyId][i] == move)
            return;
        if (sSeenMoves[partyId][i] == MOVE_NONE)
        {
            sSeenMoves[partyId][i] = move;
            return;
        }
    }
}

// Notes which of the player's Pokemon are out (and anything they just used).
void AI_ObservePlayerSide(void)
{
    u8 battler;

    for (battler = 0; battler < gBattlersCount; battler++)
    {
        if (GetBattlerSide(battler) != B_SIDE_PLAYER || (gAbsentBattlerFlags & gBitTable[battler]))
            continue;
        if (gBattlerPartyIndexes[battler] < PARTY_SIZE)
            sSeenMons |= 1 << gBattlerPartyIndexes[battler];
        AI_RecordPlayerMove(battler, gLastMoves[battler]);
    }
}

// The AI scripts' "if_has_move AI_TARGET" checks read BattleHistory, which in vanilla is one
// shared list for the whole side. Give them the current Pokemon's own remembered moves instead.
void AI_LoadTargetMoveHistory(u8 target)
{
    s32 i;
    u8 partyId;

    if (GetBattlerSide(target) != B_SIDE_PLAYER)
        return;
    partyId = gBattlerPartyIndexes[target];
    if (partyId >= PARTY_SIZE)
        return;
    for (i = 0; i < 8; i++)
        gBattleResources->battleHistory->usedMoves[target >> 1][i] = (i < MAX_MON_MOVES) ? sSeenMoves[partyId][i] : MOVE_NONE;
}

// ---------------------------------------------------------------------------------------
// Helpers

static bool8 IsDamagingMove(u16 move)
{
    return move != MOVE_NONE && move != MOVE_UNAVAILABLE && gBattleMoves[move].power != 0;
}

// Type multiplier x10 (0, 2, 5, 10, 20, 40). Neutral is 10.
static u16 TypeMultiplier(u8 moveType, u8 defType1, u8 defType2)
{
    s32 i = 0;
    u16 mult = MULT_NEUTRAL;

    while (TYPE_EFFECT_ATK_TYPE(i) != TYPE_ENDTABLE)
    {
        if (TYPE_EFFECT_ATK_TYPE(i) == TYPE_FORESIGHT)
        {
            i += 3;
            continue;
        }
        if (TYPE_EFFECT_ATK_TYPE(i) == moveType)
        {
            if (TYPE_EFFECT_DEF_TYPE(i) == defType1)
                mult = mult * TYPE_EFFECT_MULTIPLIER(i) / 10;
            if (TYPE_EFFECT_DEF_TYPE(i) == defType2 && defType1 != defType2)
                mult = mult * TYPE_EFFECT_MULTIPLIER(i) / 10;
        }
        i += 3;
    }
    return mult;
}

// Type multiplier of a move against a Pokemon, counting immunity abilities.
static u16 MoveMultVsTypes(u16 move, u8 type1, u8 type2, u8 ability)
{
    u8 moveType = gBattleMoves[move].type;
    u16 mult;

    if (ability == ABILITY_LEVITATE && moveType == TYPE_GROUND)
        return 0;
    if (ability == ABILITY_FLASH_FIRE && moveType == TYPE_FIRE)
        return 0;
    if (ability == ABILITY_WATER_ABSORB && moveType == TYPE_WATER)
        return 0;
    if (ability == ABILITY_VOLT_ABSORB && moveType == TYPE_ELECTRIC)
        return 0;
    if ((ability == ABILITY_STORM_DRAIN || ability == ABILITY_DRY_SKIN) && moveType == TYPE_WATER)
        return 0;
    if (ability == ABILITY_SAP_SIPPER && moveType == TYPE_GRASS)
        return 0;
    mult = TypeMultiplier(moveType, type1, type2);
    if (ability == ABILITY_WONDER_GUARD && mult <= MULT_NEUTRAL)
        return 0;
    return mult;
}

static u16 MoveMultVsPartyMon(u16 move, struct Pokemon *mon)
{
    u16 species = GetMonData(mon, MON_DATA_SPECIES);
    return MoveMultVsTypes(move, gSpeciesInfo[species].types[0], gSpeciesInfo[species].types[1], GetMonAbility(mon));
}

static u16 MoveMultVsBattler(u16 move, u8 battler)
{
    return MoveMultVsTypes(move, gBattleMons[battler].type1, gBattleMons[battler].type2, gBattleMons[battler].ability);
}

// Real damage calc (no crit), as a % of the damage roll (85 = lowest roll, 100 = highest).
static u16 CalcDamage(u8 attacker, u8 defender, u16 move, u8 rollPercent)
{
    u16 savedMove = gCurrentMove;
    s32 savedDamage = gBattleMoveDamage;
    u8 savedResult = gMoveResultFlags;
    s32 dmg;
    u8 flags;

    if (!IsDamagingMove(move))
        return 0;

    gDynamicBasePower = 0;
    gBattleStruct->dynamicMoveType = 0;
    gBattleScripting.dmgMultiplier = 1;
    gMoveResultFlags = 0;
    gCritMultiplier = 1;
    gCurrentMove = move;
    AI_CalcDmg(attacker, defender);
    flags = TypeCalc(move, attacker, defender);
    dmg = gBattleMoveDamage;

    if (flags & (MOVE_RESULT_MISSED | MOVE_RESULT_DOESNT_AFFECT_FOE) || MoveMultVsBattler(move, defender) == 0)
    {
        dmg = 0;
    }
    else
    {
        switch (gBattleMoves[move].effect)
        {
        case EFFECT_LEVEL_DAMAGE:
            dmg = gBattleMons[attacker].level;
            break;
        case EFFECT_DRAGON_RAGE:
            dmg = 40;
            break;
        case EFFECT_SONICBOOM:
            dmg = 20;
            break;
        case EFFECT_SUPER_FANG:
            dmg = gBattleMons[defender].hp / 2;
            break;
        default:
            if (gBattleMoves[move].power == 1) // other variable-damage moves: don't guess
                dmg = 0;
            else
                dmg = dmg * rollPercent / 100;
            break;
        }
        if (dmg == 0 && gBattleMoves[move].power > 1)
            dmg = 1;
    }

    gCurrentMove = savedMove;
    gBattleMoveDamage = savedDamage;
    gMoveResultFlags = savedResult;
    if (dmg > 0xFFFF)
        dmg = 0xFFFF;
    return dmg;
}

static u8 CountKnownMoves(u8 partyId)
{
    s32 i;
    u8 count = 0;

    for (i = 0; i < MAX_MON_MOVES; i++)
        if (sSeenMoves[partyId][i] != MOVE_NONE)
            count++;
    return count;
}

// Highest damage the player's battler can do to the AI's battler, using only moves it has
// seen, plus a guess at an unseen STAB attack if it hasn't seen all four moves yet.
static u16 EstimateThreat(u8 player, u8 aiBattler)
{
    s32 i;
    u16 best = 0, dmg, move;
    u8 partyId = gBattlerPartyIndexes[player];

    if (partyId >= PARTY_SIZE)
        return 0;
    for (i = 0; i < MAX_MON_MOVES; i++)
    {
        move = sSeenMoves[partyId][i];
        dmg = CalcDamage(player, aiBattler, move, 100);
        if (dmg > best)
            best = dmg;
    }
    if (CountKnownMoves(partyId) < MAX_MON_MOVES)
    {
        u8 types[2];
        types[0] = gBattleMons[player].type1;
        types[1] = gBattleMons[player].type2;
        for (i = 0; i < 2; i++)
        {
            if (types[i] >= ARRAY_COUNT(sTypicalStabMove))
                continue;
            move = sTypicalStabMove[types[i]];
            dmg = CalcDamage(player, aiBattler, move, 100) * UNKNOWN_MOVE_PERCENT / 100;
            if (dmg > best)
                best = dmg;
        }
    }
    return best;
}

// Worst type multiplier the player's battler can hit a Pokemon with (seen moves + STAB guess).
static u16 WorstIncomingMult(u8 player, u8 type1, u8 type2, u8 ability)
{
    s32 i;
    u16 worst = 0, mult, move;
    u8 partyId = gBattlerPartyIndexes[player];

    for (i = 0; i < MAX_MON_MOVES; i++)
    {
        move = sSeenMoves[partyId][i];
        if (!IsDamagingMove(move) || gBattleMoves[move].power == 1)
            continue;
        mult = MoveMultVsTypes(move, type1, type2, ability);
        if (mult > worst)
            worst = mult;
    }
    if (CountKnownMoves(partyId) < MAX_MON_MOVES)
    {
        u8 types[2];
        types[0] = gBattleMons[player].type1;
        types[1] = gBattleMons[player].type2;
        for (i = 0; i < 2; i++)
        {
            if (types[i] >= ARRAY_COUNT(sTypicalStabMove) || sTypicalStabMove[types[i]] == MOVE_NONE)
                continue;
            mult = MoveMultVsTypes(sTypicalStabMove[types[i]], type1, type2, ability);
            if (mult > worst)
                worst = mult;
        }
    }
    return worst;
}

static bool8 IsSetupEffect(u16 effect)
{
    switch (effect)
    {
    case EFFECT_ATTACK_UP:
    case EFFECT_ATTACK_UP_2:
    case EFFECT_DEFENSE_UP:
    case EFFECT_DEFENSE_UP_2:
    case EFFECT_SPEED_UP:
    case EFFECT_SPEED_UP_2:
    case EFFECT_SPECIAL_ATTACK_UP:
    case EFFECT_SPECIAL_ATTACK_UP_2:
    case EFFECT_SPECIAL_DEFENSE_UP:
    case EFFECT_SPECIAL_DEFENSE_UP_2:
    case EFFECT_CALM_MIND:
    case EFFECT_DRAGON_DANCE:
    case EFFECT_BULK_UP:
    case EFFECT_COSMIC_POWER:
    case EFFECT_BELLY_DRUM:
    case EFFECT_CURSE:
        return TRUE;
    }
    return FALSE;
}

static u8 GetSetupStat(u16 effect)
{
    switch (effect)
    {
    case EFFECT_DEFENSE_UP:
    case EFFECT_DEFENSE_UP_2:
    case EFFECT_COSMIC_POWER:
        return STAT_DEF;
    case EFFECT_SPEED_UP:
    case EFFECT_SPEED_UP_2:
        return STAT_SPEED;
    case EFFECT_SPECIAL_ATTACK_UP:
    case EFFECT_SPECIAL_ATTACK_UP_2:
    case EFFECT_CALM_MIND:
        return STAT_SPATK;
    case EFFECT_SPECIAL_DEFENSE_UP:
    case EFFECT_SPECIAL_DEFENSE_UP_2:
        return STAT_SPDEF;
    default:
        return STAT_ATK;
    }
}

static bool8 HasDamagingMoveOfCategory(u8 battler, bool8 physical)
{
    s32 i;
    u16 move;

    for (i = 0; i < MAX_MON_MOVES; i++)
    {
        move = gBattleMons[battler].moves[i];
        if (!IsDamagingMove(move))
            continue;
        if (physical ? IS_MOVE_PHYSICAL(move, gBattleMoves[move].type) : IS_MOVE_SPECIAL(move, gBattleMoves[move].type))
            return TRUE;
    }
    return FALSE;
}

static s16 GetSetupScore(u8 attacker, u16 effect, u16 threat, bool8 aiFaster)
{
    u8 stat = GetSetupStat(effect);
    u8 stage = gBattleMons[attacker].statStages[stat];
    u16 hpAfter = gBattleMons[attacker].hp;

    if (effect == EFFECT_CURSE && IS_BATTLER_OF_TYPE(attacker, TYPE_GHOST))
        return 0; // Ghost Curse is not a boost; leave it to the scripts
    if (effect == EFFECT_BELLY_DRUM)
    {
        if (hpAfter <= gBattleMons[attacker].maxHP / 2)
            return -20;
        hpAfter -= gBattleMons[attacker].maxHP / 2;
    }
    // Safe only if it survives at least two more of the player's best known hits.
    if ((u32)threat * 2 >= hpAfter)
        return -SCORE_SETUP_UNSAFE;
    if (stat == STAT_SPEED && aiFaster && effect != EFFECT_DRAGON_DANCE)
        return -SCORE_SETUP_ENOUGH; // already outspeeds
    if (stage >= DEFAULT_STAT_STAGE + 2)
        return -SCORE_SETUP_ENOUGH;
    if (stat == STAT_ATK && !HasDamagingMoveOfCategory(attacker, TRUE))
        return -SCORE_SETUP_ENOUGH;
    if (stat == STAT_SPATK && !HasDamagingMoveOfCategory(attacker, FALSE))
        return -SCORE_SETUP_ENOUGH;
    if (effect == EFFECT_DRAGON_DANCE && !aiFaster)
        return SCORE_SETUP_SAFE + 2;
    return (stage == DEFAULT_STAT_STAGE) ? SCORE_SETUP_SAFE : SCORE_SETUP_SAFE_AGAIN;
}

static s16 AccuracyPenalty(u16 move)
{
    u8 acc = gBattleMoves[move].accuracy;

    if (acc == 0 || acc >= 100)
        return 0;
    return (100 - acc) / 5;
}

// The player's seen, healthy party member that best walls `move` (or PARTY_SIZE).
static u8 PredictSwitchIn(u8 target, u16 move, u16 *outMult)
{
    s32 i;
    u8 best = PARTY_SIZE;
    u16 bestMult = MULT_NEUTRAL, mult, species;

    for (i = 0; i < PARTY_SIZE; i++)
    {
        if (!(sSeenMons & (1 << i)) || i == gBattlerPartyIndexes[target])
            continue;
        species = GetMonData(&gPlayerParty[i], MON_DATA_SPECIES_OR_EGG);
        if (species == SPECIES_NONE || species == SPECIES_EGG || GetMonData(&gPlayerParty[i], MON_DATA_HP) == 0)
            continue;
        mult = MoveMultVsPartyMon(move, &gPlayerParty[i]);
        if (mult < bestMult)
        {
            bestMult = mult;
            best = i;
        }
    }
    *outMult = bestMult;
    return best;
}

// ---------------------------------------------------------------------------------------
// Move scoring

void AI_AdvancedScoreMoves(u8 attacker, u8 target, s8 *scores)
{
    s32 i;
    s16 newScore[MAX_MON_MOVES];
    u16 dmgLow[MAX_MON_MOVES];
    bool8 koFirst[MAX_MON_MOVES], koSlow[MAX_MON_MOVES];
    bool8 anyKOFirst = FALSE;
    u16 move, threat, tgtHp, aiHp, bestDmg = 0;
    u8 bestIdx = MAX_MON_MOVES;
    bool8 aiFaster, inDanger;

    if (GetBattlerSide(target) == GetBattlerSide(attacker) || gBattleMons[target].hp == 0)
        return;

    tgtHp = gBattleMons[target].hp;
    aiHp = gBattleMons[attacker].hp;
    threat = EstimateThreat(target, attacker);
    aiFaster = (GetWhoStrikesFirst(attacker, target, TRUE) == 0);
    inDanger = (threat >= aiHp && !aiFaster);

    for (i = 0; i < MAX_MON_MOVES; i++)
    {
        u16 dmgHigh;

        newScore[i] = scores[i];
        koFirst[i] = koSlow[i] = FALSE;
        dmgLow[i] = 0;
        move = gBattleMons[attacker].moves[i];
        if (scores[i] <= 0 || move == MOVE_NONE)
            continue;
        dmgLow[i] = CalcDamage(attacker, target, move, 85);
        dmgHigh = CalcDamage(attacker, target, move, 100);
        if (dmgHigh > bestDmg)
        {
            bestDmg = dmgHigh;
            bestIdx = i;
        }
        if (dmgLow[i] != 0 && dmgLow[i] >= tgtHp)
        {
            if (aiFaster || gBattleMoves[move].priority > 0)
            {
                koFirst[i] = TRUE;
                anyKOFirst = TRUE;
            }
            else
            {
                koSlow[i] = TRUE;
            }
        }
    }

    for (i = 0; i < MAX_MON_MOVES; i++)
    {
        u16 effect;

        move = gBattleMons[attacker].moves[i];
        if (scores[i] <= 0 || move == MOVE_NONE)
            continue;
        effect = gBattleMoves[move].effect;

        if (koFirst[i])
        {
            newScore[i] += SCORE_KO_FIRST - AccuracyPenalty(move);
            if (!aiFaster && gBattleMoves[move].priority > 0)
                newScore[i] += SCORE_KO_PRIORITY_BONUS;
        }
        else if (anyKOFirst)
        {
            newScore[i] -= IsDamagingMove(move) ? SCORE_NOT_KO_WHEN_KO : SCORE_STATUS_WHEN_KO;
        }
        else if (koSlow[i] && threat < aiHp)
        {
            newScore[i] += SCORE_KO_SLOWER - AccuracyPenalty(move);
        }
        else
        {
            if (i == bestIdx)
                newScore[i] += SCORE_BEST_DAMAGE;
            if (inDanger)
            {
                if (effect == EFFECT_EXPLOSION)
                    newScore[i] += SCORE_EXPLODE_IN_DANGER;
                else if (IsDamagingMove(move) && gBattleMoves[move].priority > 0 && dmgLow[i] != 0)
                    newScore[i] += SCORE_PRIORITY_CHIP;
                else if (!IsDamagingMove(move))
                    newScore[i] -= SCORE_STATUS_IN_DANGER;
            }
            if (IsSetupEffect(effect))
                newScore[i] += GetSetupScore(attacker, effect, threat, aiFaster);
        }
    }

    // Read a switch: the player's Pokemon is in trouble and the trainer has seen a teammate
    // that walls its best attack, so favour moves that also hurt that teammate.
    if (!anyKOFirst && bestIdx != MAX_MON_MOVES && !(gBattleTypeFlags & BATTLE_TYPE_DOUBLE)
        && (Random() % 100) < PREDICT_CHANCE)
    {
        u16 bestMove = gBattleMons[attacker].moves[bestIdx];
        u16 predictedMult;
        u8 predicted;

        if (MoveMultVsBattler(bestMove, target) > MULT_NEUTRAL || (u32)bestDmg * 2 >= tgtHp)
        {
            predicted = PredictSwitchIn(target, bestMove, &predictedMult);
            if (predicted != PARTY_SIZE && predictedMult < MULT_NEUTRAL)
            {
                for (i = 0; i < MAX_MON_MOVES; i++)
                {
                    u16 mult;

                    move = gBattleMons[attacker].moves[i];
                    if (scores[i] <= 0 || !IsDamagingMove(move))
                        continue;
                    mult = MoveMultVsPartyMon(move, &gPlayerParty[predicted]);
                    if (mult > MULT_NEUTRAL)
                        newScore[i] += SCORE_PREDICT_SE;
                    else if (mult == MULT_NEUTRAL)
                        newScore[i] += SCORE_PREDICT_NEUTRAL;
                    else if (mult == 0)
                        newScore[i] -= SCORE_PREDICT_IMMUNE;
                    else
                        newScore[i] -= SCORE_PREDICT_RESISTED;
                }
            }
        }
    }

    for (i = 0; i < MAX_MON_MOVES; i++)
    {
        if (scores[i] <= 0)
            continue;
        if (newScore[i] < 1)
            newScore[i] = 1; // still usable, just not wanted
        if (newScore[i] > 127)
            newScore[i] = 127;
        scores[i] = newScore[i];
    }
}

// ---------------------------------------------------------------------------------------
// Switching

static bool8 IsValidSwitchIn(u8 partyId, u8 battlerIn1, u8 battlerIn2)
{
    u16 species = GetMonData(&gEnemyParty[partyId], MON_DATA_SPECIES_OR_EGG);

    if (species == SPECIES_NONE || species == SPECIES_EGG || GetMonData(&gEnemyParty[partyId], MON_DATA_HP) == 0)
        return FALSE;
    if (partyId == gBattlerPartyIndexes[battlerIn1] || partyId == gBattlerPartyIndexes[battlerIn2])
        return FALSE;
    if (partyId == gBattleStruct->monToSwitchIntoId[battlerIn1] || partyId == gBattleStruct->monToSwitchIntoId[battlerIn2])
        return FALSE;
    return TRUE;
}

// How well a party member would do coming in against the player's battler.
static s16 ScoreSwitchIn(struct Pokemon *mon, u8 player, u16 *incomingOut)
{
    s32 i;
    u16 species = GetMonData(mon, MON_DATA_SPECIES);
    u8 type1 = gSpeciesInfo[species].types[0], type2 = gSpeciesInfo[species].types[1];
    u16 incoming = WorstIncomingMult(player, type1, type2, GetMonAbility(mon));
    u32 outgoing = 0, value;
    u16 move, hp, maxHp;
    s16 score;

    for (i = 0; i < MAX_MON_MOVES; i++)
    {
        move = GetMonData(mon, MON_DATA_MOVE1 + i);
        if (!IsDamagingMove(move) || gBattleMoves[move].power == 1)
            continue;
        value = MoveMultVsBattler(move, player) * gBattleMoves[move].power;
        if (gBattleMoves[move].type == type1 || gBattleMoves[move].type == type2)
            value = value * 15 / 10;
        if (value > outgoing)
            outgoing = value;
    }
    hp = GetMonData(mon, MON_DATA_HP);
    maxHp = GetMonData(mon, MON_DATA_MAX_HP);

    score = outgoing / 40;                 // neutral STAB 80bp = 30, super effective = 60
    score -= incoming * 3;                 // neutral = -30, resisted = -15, weak = -60
    score += (hp * 20) / (maxHp ? maxHp : 1);
    if (GetMonData(mon, MON_DATA_SPEED) > gBattleMons[player].speed)
        score += 10;
    *incomingOut = incoming;
    return score;
}

static u8 FindBestSwitchIn(u8 aiBattler, u8 player, s16 *bestScoreOut, u16 *bestIncomingOut)
{
    s32 i;
    u8 battlerIn1 = aiBattler, battlerIn2 = aiBattler;
    u8 best = PARTY_SIZE;
    s16 score, bestScore = -0x7FFF;
    u16 incoming, bestIncoming = 0xFFFF;

    if (gBattleTypeFlags & BATTLE_TYPE_DOUBLE)
    {
        u8 partner = GetBattlerAtPosition(GetBattlerPosition(aiBattler) ^ BIT_FLANK);
        if (!(gAbsentBattlerFlags & gBitTable[partner]))
            battlerIn2 = partner;
    }
    for (i = 0; i < PARTY_SIZE; i++)
    {
        if (!IsValidSwitchIn(i, battlerIn1, battlerIn2))
            continue;
        score = ScoreSwitchIn(&gEnemyParty[i], player, &incoming);
        if (score > bestScore)
        {
            bestScore = score;
            bestIncoming = incoming;
            best = i;
        }
    }
    *bestScoreOut = bestScore;
    *bestIncomingOut = bestIncoming;
    return best;
}

bool8 AI_AdvancedShouldSwitch(void)
{
    u8 ai = gActiveBattler;
    u8 player = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);
    u8 best;
    s32 i;
    u16 move, threat, bestDmg = 0, dmg, curIncoming, bestIncoming;
    s16 bestScore;
    bool8 aiFaster, inDanger, walled;

    if (gBattleTypeFlags & BATTLE_TYPE_DOUBLE)
        return FALSE;
    if (gBattleMons[player].hp == 0 || (gAbsentBattlerFlags & gBitTable[player]))
        return FALSE;
    if (sLastAISwitchTurn != 0xFF && gBattleResults.battleTurnCounter - sLastAISwitchTurn < 2)
        return FALSE; // don't ping-pong

    aiFaster = (GetWhoStrikesFirst(ai, player, TRUE) == 0);
    for (i = 0; i < MAX_MON_MOVES; i++)
    {
        move = gBattleMons[ai].moves[i];
        if (move == MOVE_NONE || gBattleMons[ai].pp[i] == 0)
            continue;
        dmg = CalcDamage(ai, player, move, 85);
        if (dmg >= gBattleMons[player].hp && (aiFaster || gBattleMoves[move].priority > 0))
            return FALSE; // it can take the KO, so stay in
        if (dmg > bestDmg)
            bestDmg = dmg;
    }

    threat = EstimateThreat(player, ai);
    inDanger = (threat >= gBattleMons[ai].hp && !aiFaster);
    walled = ((u32)bestDmg * 8 < gBattleMons[player].maxHP);
    if (!inDanger && !walled)
        return FALSE;

    best = FindBestSwitchIn(ai, player, &bestScore, &bestIncoming);
    if (best == PARTY_SIZE)
        return FALSE;

    curIncoming = WorstIncomingMult(player, gBattleMons[ai].type1, gBattleMons[ai].type2, gBattleMons[ai].ability);
    if (inDanger)
    {
        // Only switch into something that takes the hit a lot better.
        if (!(bestIncoming == 0 || bestIncoming * 2 <= curIncoming))
            return FALSE;
    }
    else // walled
    {
        if (bestIncoming > curIncoming || bestScore < 20)
            return FALSE;
    }
    if ((Random() % 100) < STAY_IN_CHANCE)
        return FALSE;

    sLastAISwitchTurn = gBattleResults.battleTurnCounter;
    gBattleStruct->AI_monToSwitchIntoId[GetBattlerPosition(ai) >> 1] = best;
    BtlController_EmitTwoReturnValues(1, B_ACTION_SWITCH, 0);
    return TRUE;
}

// Replacement after a faint (or when another rule decided to switch).
u8 AI_AdvancedGetBestSwitchIn(void)
{
    u8 player = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);
    s16 score;
    u16 incoming;

    if (gBattleTypeFlags & BATTLE_TYPE_DOUBLE)
        return PARTY_SIZE;
    if (gAbsentBattlerFlags & gBitTable[player])
        return PARTY_SIZE;
    return FindBestSwitchIn(gActiveBattler, player, &score, &incoming);
}
