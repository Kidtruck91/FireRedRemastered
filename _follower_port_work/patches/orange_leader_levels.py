# Run after orange_leaders.py, from the repo root.
#  - Rescales the Orange League teams to the soft-reset level caps:
#    Cissy 20, Danny 28, Rudy 38, Luana 44, Drake 50 (ace at the cap, the rest just below).
#    Species and moves are legal for their level (level-up, pre-evolution, TM/HM or tutor).
#  - Scales the leaders' bag items down to match.
#  - Level cap: while FLAG_ORANGE_CHALLENGE is set (and Nuzlocke caps are on), the cap is
#    20 / 28 / 38 / 44 / 50 by how many Orange leaders have been beaten, in any order.
import re
TEAMS = [
 ('sParty_LeaderCissy', 'TRAINER_LEADER_CISSY', 'ITEM_SUPER_POTION', [
   (18,'HORSEA','MYSTIC_WATER',['WATER_PULSE','SMOKESCREEN','LEER','BUBBLE']),
   (18,'STARYU','ORAN_BERRY',['WATER_PULSE','RAPID_SPIN','RECOVER','HARDEN']),
   (19,'CHINCHOU','ORAN_BERRY',['WATER_GUN','SHOCK_WAVE','THUNDER_WAVE','SUPERSONIC']),
   (20,'WARTORTLE','SITRUS_BERRY',['WATER_PULSE','BITE','WITHDRAW','PROTECT'])]),
 ('sParty_LeaderDanny', 'TRAINER_LEADER_DANNY', 'ITEM_SUPER_POTION, ITEM_SUPER_POTION', [
   (26,'GRAVELER','QUICK_CLAW',['ROCK_TOMB','MAGNITUDE','DEFENSE_CURL','SELF_DESTRUCT']),
   (27,'VOLTORB','MAGNET',['SPARK','SONIC_BOOM','SCREECH','SELF_DESTRUCT']),
   (27,'SCYTHER','SILVER_POWDER',['WING_ATTACK','FURY_CUTTER','AGILITY','QUICK_ATTACK']),
   (28,'NIDOKING','SOFT_SAND',['DOUBLE_KICK','HORN_ATTACK','DIG','POISON_STING'])]),
 ('sParty_LeaderRudy', 'TRAINER_LEADER_RUDY', 'ITEM_HYPER_POTION, ITEM_HYPER_POTION', [
   (36,'EXEGGUTOR','MIRACLE_SEED',['GIGA_DRAIN','EGG_BOMB','CONFUSION','STUN_SPORE']),
   (36,'STARMIE','LEFTOVERS',['SURF','THUNDERBOLT','RECOVER','CONFUSE_RAY']),
   (37,'KINGLER','KINGS_ROCK',['SURF','MUD_SHOT','METAL_CLAW','STOMP']),
   (38,'ELECTABUZZ','MAGNET',['THUNDER_PUNCH','BRICK_BREAK','QUICK_ATTACK','LIGHT_SCREEN'])]),
 ('sParty_LeaderLuana', 'TRAINER_LEADER_LUANA', 'ITEM_HYPER_POTION, ITEM_HYPER_POTION', [
   (42,'STEELIX','METAL_COAT',['IRON_TAIL','DRAGON_BREATH','ROCK_TOMB','SCREECH']),
   (42,'KANGASKHAN','SILK_SCARF',['FAKE_OUT','BODY_SLAM','CRUNCH','BRICK_BREAK']),
   (43,'MAROWAK','THICK_CLUB',['BONEMERANG','ROCK_TOMB','HEADBUTT','FOCUS_ENERGY']),
   (44,'ALAKAZAM','TWISTED_SPOON',['PSYCHIC','CALM_MIND','RECOVER','REFLECT'])]),
 ('sParty_ChampionDrake', 'TRAINER_CHAMPION_DRAKE', 'ITEM_FULL_RESTORE, ITEM_FULL_RESTORE', [
   (46,'DITTO','QUICK_CLAW',['TRANSFORM','NONE','NONE','NONE']),
   (47,'STEELIX','LEFTOVERS',['IRON_TAIL','EARTHQUAKE','DRAGON_BREATH','ROCK_TOMB']),
   (48,'GENGAR','SPELL_TAG',['SHADOW_BALL','THUNDERBOLT','HYPNOSIS','DREAM_EATER']),
   (48,'ELECTABUZZ','MAGNET',['THUNDERBOLT','BRICK_BREAK','QUICK_ATTACK','LIGHT_SCREEN']),
   (49,'VENUSAUR','MIRACLE_SEED',['GIGA_DRAIN','SLUDGE_BOMB','SLEEP_POWDER','LEECH_SEED']),
   (50,'DRAGONITE','LEFTOVERS',['DRAGON_CLAW','EARTHQUAKE','FLAMETHROWER','THUNDER_WAVE'])]),
]

p = 'src/data/trainer_parties.h'; t = open(p).read()
for sym, const, items, party in TEAMS:
    body = ''.join('    {\n        .iv = 250,\n        .lvl = %d,\n        .species = SPECIES_%s,\n        .heldItem = ITEM_%s,\n        .moves = {%s},\n    },\n' % (
        lvl, sp, it, ', '.join('MOVE_' + m for m in mv)) for lvl, sp, it, mv in party)
    pat = re.compile(r'(static const struct TrainerMonItemCustomMoves %s\[\] = \{\n).*?(\n\};)' % sym, re.S)
    assert pat.search(t), sym
    t = pat.sub(lambda m: m.group(1) + body.rstrip('\n') + m.group(2), t, count=1)
open(p, 'w', newline='\n').write(t)

p = 'src/data/trainers.h'; t = open(p).read()
for sym, const, items, party in TEAMS:
    i = t.index('    [%s] = {' % const); j = t.index('    },\n', i)
    seg = re.sub(r'\.items = \{[^}]*\}', '.items = {%s}' % items, t[i:j], count=1)
    t = t[:i] + seg + t[j:]
open(p, 'w', newline='\n').write(t)

p = 'src/level_cap.c'; t = open(p).read()
if 'sOrangeLevelCaps' not in t:
    t = t.replace('#include "constants/flags.h"\n', '#include "constants/flags.h"\n#include "constants/opponents.h"\n', 1)
    t = t.replace('// To change the caps, edit the table in CalculateLevelCap.\n',
        '// To change the caps, edit the table in CalculateLevelCap.\n\n'
        '// Orange Islands challenge: cap by how many Orange leaders are beaten (any order).\n'
        'static const u8 sOrangeLevelCaps[] = {20, 28, 38, 44, 50};\n'
        'static const u16 sOrangeLeaders[] = {\n'
        '    TRAINER_LEADER_CISSY, TRAINER_LEADER_DANNY, TRAINER_LEADER_RUDY, TRAINER_LEADER_LUANA,\n'
        '};\n', 1)
    old = '        return MAX_LEVEL; // no cap\n\n'
    assert old in t
    t = t.replace(old, old +
        '    if (FlagGet(FLAG_ORANGE_CHALLENGE) && !FlagGet(TRAINER_FLAGS_START + TRAINER_CHAMPION_DRAKE))\n'
        '    {\n'
        '        u8 i, beaten = 0;\n'
        '        for (i = 0; i < ARRAY_COUNT(sOrangeLeaders); i++)\n'
        '            if (FlagGet(TRAINER_FLAGS_START + sOrangeLeaders[i]))\n'
        '                beaten++;\n'
        '        return sOrangeLevelCaps[beaten];\n'
        '    }\n\n', 1)
    open(p, 'w', newline='\n').write(t)
print('orange leader levels ok')
