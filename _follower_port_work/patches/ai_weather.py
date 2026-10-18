# Run from the repo root.
# Advanced AI: values weather moves (Rain Dance, Sunny Day, Sandstorm, Hail) by how much its
# own team gains from that weather, and backs off when the weather is already up, when the
# setter is low on HP, when Cloud Nine / Air Lock is out, or when it mostly helps the player.
p = 'src/battle_ai_advanced.c'; t = open(p).read()
if 'GetWeatherSetScore' not in t:
    t = t.replace('''//   Own switching  - it switches out of a matchup where it would be KO'd first (or where it
//                    can't do damage) into the party member that best takes the player's
//                    known moves, and picks replacements the same way after a faint.
''', '''//   Own switching  - it switches out of a matchup where it would be KO'd first (or where it
//                    can't do damage) into the party member that best takes the player's
//                    known moves, and picks replacements the same way after a faint.
//   Weather        - Rain Dance / Sunny Day / Sandstorm / Hail are valued by how many of its
//                    team gain from that weather (abilities like Swift Swim count double), and
//                    skipped when that weather is already up, when the setter is low on HP,
//                    or when Cloud Nine / Air Lock is on the field.
''', 1)
    t = t.replace('''#define UNKNOWN_MOVE_PERCENT    80  // how hard it assumes an unseen STAB attack hits (% of a calc)
''', '''#define UNKNOWN_MOVE_PERCENT    80  // how hard it assumes an unseen STAB attack hits (% of a calc)
#define SCORE_WEATHER_PER_GAIN  2   // per point of team benefit (a type/move = 1, an ability = 2)
#define SCORE_WEATHER_MAX       8   // cap on the weather bonus
#define SCORE_WEATHER_UP        10  // penalty: that weather is already up and not about to end
#define SCORE_WEATHER_LOW_HP    4   // penalty: the setter is below WEATHER_LOW_HP_PERCENT
#define WEATHER_LOW_HP_PERCENT  40
#define SCORE_WEATHER_HELPS_FOE 2   // penalty per point the player's active Pokemon gains
#define SCORE_WEATHER_SUPPRESSED 10 // penalty: Cloud Nine / Air Lock is on the field
''', 1)
    block = r'''
// ---------------------------------------------------------------------------------------
// Weather

static u16 WeatherForEffect(u16 effect)
{
    switch (effect)
    {
    case EFFECT_RAIN_DANCE: return B_WEATHER_RAIN;
    case EFFECT_SUNNY_DAY:  return B_WEATHER_SUN;
    case EFFECT_SANDSTORM:  return B_WEATHER_SANDSTORM;
    case EFFECT_HAIL:       return B_WEATHER_HAIL;
    }
    return 0;
}

// How much one Pokemon gains from `weather`: 2 for an ability built around it,
// otherwise 1 if its type or one of its moves benefits, else 0.
static u8 WeatherGain(u16 weather, u8 type1, u8 type2, u8 ability, const u16 *moves)
{
    s32 i;
    u8 gain = 0;

    if ((weather == B_WEATHER_RAIN && (ability == ABILITY_SWIFT_SWIM || ability == ABILITY_RAIN_DISH))
     || (weather == B_WEATHER_SUN && ability == ABILITY_CHLOROPHYLL)
     || (weather == B_WEATHER_SANDSTORM && ability == ABILITY_SAND_VEIL))
        return 2;

    switch (weather)
    {
    case B_WEATHER_RAIN:
        if (type1 == TYPE_WATER || type2 == TYPE_WATER)
            gain = 1;
        break;
    case B_WEATHER_SUN:
        if (type1 == TYPE_FIRE || type2 == TYPE_FIRE)
            gain = 1;
        break;
    case B_WEATHER_SANDSTORM: // immune to the chip damage
        if (type1 == TYPE_ROCK || type2 == TYPE_ROCK || type1 == TYPE_GROUND || type2 == TYPE_GROUND
         || type1 == TYPE_STEEL || type2 == TYPE_STEEL)
            gain = 1;
        break;
    case B_WEATHER_HAIL:
        if (type1 == TYPE_ICE || type2 == TYPE_ICE)
            gain = 1;
        break;
    }
    if (gain || moves == NULL)
        return gain;

    for (i = 0; i < MAX_MON_MOVES; i++)
    {
        u16 move = moves[i], effect;
        u8 moveType;

        if (move == MOVE_NONE)
            continue;
        effect = gBattleMoves[move].effect;
        moveType = gBattleMoves[move].type;
        if (effect == EFFECT_WEATHER_BALL)
            return 1;
        if (weather == B_WEATHER_RAIN && (effect == EFFECT_THUNDER || (moveType == TYPE_WATER && IsDamagingMove(move))))
            return 1;
        if (weather == B_WEATHER_SUN && (effect == EFFECT_SOLAR_BEAM || effect == EFFECT_SYNTHESIS
         || effect == EFFECT_MORNING_SUN || effect == EFFECT_MOONLIGHT || (moveType == TYPE_FIRE && IsDamagingMove(move))))
            return 1;
    }
    return 0;
}

static s16 GetWeatherSetScore(u8 attacker, u8 target, u16 effect)
{
    u16 weather = WeatherForEffect(effect);
    struct Pokemon *party = (GetBattlerSide(attacker) == B_SIDE_PLAYER) ? gPlayerParty : gEnemyParty;
    s32 i, j;
    s16 score;
    u16 gain = 0;

    for (i = 0; i < gBattlersCount; i++)
    {
        if (gBattleMons[i].hp != 0 && (gBattleMons[i].ability == ABILITY_CLOUD_NINE || gBattleMons[i].ability == ABILITY_AIR_LOCK))
            return -SCORE_WEATHER_SUPPRESSED;
    }
    // Already up: only worth renewing a temporary weather on its last turn.
    if (gBattleWeather & weather)
    {
        if (!(gBattleWeather & (B_WEATHER_RAIN_TEMPORARY | B_WEATHER_SUN_TEMPORARY | B_WEATHER_SANDSTORM_TEMPORARY | B_WEATHER_HAIL_TEMPORARY))
         || gWishFutureKnock.weatherDuration > 1)
            return -SCORE_WEATHER_UP;
    }
    if (gBattleMons[attacker].hp * 100 < gBattleMons[attacker].maxHP * WEATHER_LOW_HP_PERCENT)
        return -SCORE_WEATHER_LOW_HP;

    for (i = 0; i < PARTY_SIZE; i++)
    {
        u16 species = GetMonData(&party[i], MON_DATA_SPECIES_OR_EGG), moves[MAX_MON_MOVES];

        if (species == SPECIES_NONE || species == SPECIES_EGG || GetMonData(&party[i], MON_DATA_HP) == 0)
            continue;
        for (j = 0; j < MAX_MON_MOVES; j++)
            moves[j] = GetMonData(&party[i], MON_DATA_MOVE1 + j);
        gain += WeatherGain(weather, gSpeciesInfo[species].types[0], gSpeciesInfo[species].types[1],
                            GetMonAbility(&party[i]), moves);
    }
    if (gain == 0)
        return 0; // nothing to gain; leave it to the scripts
    score = gain * SCORE_WEATHER_PER_GAIN;
    if (score > SCORE_WEATHER_MAX)
        score = SCORE_WEATHER_MAX;
    // The player's Pokemon in front of it (type and ability only; its moves aren't known).
    score -= WeatherGain(weather, gBattleMons[target].type1, gBattleMons[target].type2,
                         gBattleMons[target].ability, NULL) * SCORE_WEATHER_HELPS_FOE;
    return score;
}

// ---------------------------------------------------------------------------------------
// Move scoring
'''
    t = t.replace('''
// ---------------------------------------------------------------------------------------
// Move scoring
''', block, 1)
    old = '''            if (IsSetupEffect(effect))
                newScore[i] += GetSetupScore(attacker, effect, threat, aiFaster);
'''
    assert old in t
    t = t.replace(old, old + '''            if (WeatherForEffect(effect))
                newScore[i] += GetWeatherSetScore(attacker, target, effect);
''', 1)
    open(p, 'w', newline='\n').write(t)

# Gym rule note: Cissy's rain arena.
p = 'src/orange_islands.c'; t = open(p).read()
old = '''// No items (Danny): setflag FLAG_NO_BATTLE_ITEMS before the gauntlet and
// clearflag it afterwards; it is also cleared if the player loses.
'''
if 'Rain arena' not in t:
    assert old in t
    t = t.replace(old, old + '''//
// Rain arena (Cissy): overworld rain turns into permanent rain in battle, so
//   setweather WEATHER_RAIN / doweather   before trainerbattle_single, and
//   resetweather / doweather              in the post-battle script.
// A loss whites out to a Pokemon Center, which resets the weather by itself.
''', 1)
    open(p, 'w', newline='\n').write(t)

# A walled (but not threatened) weather setter stays in to set its weather instead of switching.
p = 'src/battle_ai_advanced.c'; t = open(p).read()
old = ("    if (!inDanger && !walled)\n"
       "        return FALSE;\n")
new = old + (
    "    if (!inDanger) // walled, but it can still set up weather its team gains from\n"
    "    {\n"
    "        for (i = 0; i < MAX_MON_MOVES; i++)\n"
    "        {\n"
    "            move = gBattleMons[ai].moves[i];\n"
    "            if (move != MOVE_NONE && gBattleMons[ai].pp[i] != 0 && WeatherForEffect(gBattleMoves[move].effect)\n"
    "             && GetWeatherSetScore(ai, player, gBattleMoves[move].effect) > 0)\n"
    "                return FALSE;\n"
    "        }\n"
    "    }\n")
if 'can still set up weather' not in t:
    assert old in t
    t = t.replace(old, new, 1)
    open(p, 'w', newline='\n').write(t)
print('ai weather ok')
