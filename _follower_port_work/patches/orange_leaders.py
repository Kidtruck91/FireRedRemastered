#!/usr/bin/env python3
# Orange League trainers + bigger trainer flag space.
#  - MAX_TRAINERS_COUNT 992 -> 1504 (512 more trainer flags; the system flags move up with it, so saves made
#    before this change won't line up; new saves are fine).
#  - Gym leaders Cissy, Danny, Rudy, Luana and Champion Drake (IDs 927-931), with anime-based teams.
#    Levels assume the Orange League sits between the Kanto Champion (~63) and Johto (65+).
#    Trainer pics are placeholders until custom art exists.
# Idempotent. Run from the repo root.
import re
NEW = [  # (constant, class, pic, name, music_gender, mugshot, double, items, party symbol, party)
 ('TRAINER_LEADER_CISSY', 'TRAINER_CLASS_LEADER', 'TRAINER_PIC_LEADER_MISTY', 'CISSY', 'TRAINER_ENCOUNTER_MUSIC_FEMALE | F_TRAINER_FEMALE', 'MUGSHOT_COLOR_BLUE', False,
  'ITEM_HYPER_POTION, ITEM_HYPER_POTION', 'sParty_LeaderCissy', [
  (52, 'SEADRA', 'MYSTIC_WATER', ('SURF', 'ICE_BEAM', 'DRAGON_BREATH', 'AGILITY')),
  (52, 'STARMIE', 'LEFTOVERS', ('SURF', 'THUNDERBOLT', 'PSYCHIC', 'RECOVER')),
  (53, 'LANTURN', 'SITRUS_BERRY', ('SURF', 'THUNDERBOLT', 'CONFUSE_RAY', 'THUNDER_WAVE')),
  (55, 'BLASTOISE', 'MYSTIC_WATER', ('HYDRO_PUMP', 'ICE_BEAM', 'EARTHQUAKE', 'PROTECT'))]),
 ('TRAINER_LEADER_DANNY', 'TRAINER_CLASS_LEADER', 'TRAINER_PIC_BLACK_BELT', 'DANNY', 'TRAINER_ENCOUNTER_MUSIC_MALE', 'MUGSHOT_COLOR_BROWN_GREY', False,
  'ITEM_HYPER_POTION, ITEM_FULL_RESTORE', 'sParty_LeaderDanny', [
  (55, 'GOLEM', 'QUICK_CLAW', ('EARTHQUAKE', 'ROCK_SLIDE', 'EXPLOSION', 'PROTECT')),
  (55, 'ELECTRODE', 'MAGNET', ('THUNDERBOLT', 'LIGHT_SCREEN', 'THUNDER_WAVE', 'EXPLOSION')),
  (56, 'SCYTHER', 'SILVER_POWDER', ('SWORDS_DANCE', 'WING_ATTACK', 'SILVER_WIND', 'QUICK_ATTACK')),
  (58, 'NIDOKING', 'SOFT_SAND', ('EARTHQUAKE', 'MEGAHORN', 'ICE_BEAM', 'THUNDERBOLT'))]),
 ('TRAINER_LEADER_RUDY', 'TRAINER_CLASS_LEADER', 'TRAINER_PIC_COOLTRAINER_M', 'RUDY', 'TRAINER_ENCOUNTER_MUSIC_MALE', 'MUGSHOT_COLOR_YELLOW', False,
  'ITEM_FULL_RESTORE, ITEM_FULL_RESTORE', 'sParty_LeaderRudy', [
  (58, 'EXEGGUTOR', 'MIRACLE_SEED', ('GIGA_DRAIN', 'PSYCHIC', 'SLEEP_POWDER', 'EGG_BOMB')),
  (58, 'STARMIE', 'LEFTOVERS', ('SURF', 'PSYCHIC', 'THUNDERBOLT', 'RECOVER')),
  (59, 'KINGLER', 'KINGS_ROCK', ('CRABHAMMER', 'SWORDS_DANCE', 'STOMP', 'PROTECT')),
  (61, 'ELECTABUZZ', 'MAGNET', ('THUNDERBOLT', 'CROSS_CHOP', 'FIRE_PUNCH', 'ICE_PUNCH'))]),
 ('TRAINER_LEADER_LUANA', 'TRAINER_CLASS_LEADER', 'TRAINER_PIC_LADY', 'LUANA', 'TRAINER_ENCOUNTER_MUSIC_FEMALE | F_TRAINER_FEMALE', 'MUGSHOT_COLOR_PURPLE', True,
  'ITEM_FULL_RESTORE, ITEM_FULL_RESTORE', 'sParty_LeaderLuana', [
  (60, 'STEELIX', 'METAL_COAT', ('IRON_TAIL', 'EARTHQUAKE', 'ROCK_SLIDE', 'PROTECT')),
  (60, 'KANGASKHAN', 'SILK_SCARF', ('RETURN', 'EARTHQUAKE', 'FAKE_OUT', 'PROTECT')),
  (61, 'MAROWAK', 'THICK_CLUB', ('BONEMERANG', 'ROCK_SLIDE', 'SWORDS_DANCE', 'PROTECT')),
  (62, 'ALAKAZAM', 'TWISTED_SPOON', ('PSYCHIC', 'CALM_MIND', 'FIRE_PUNCH', 'RECOVER'))]),
 ('TRAINER_CHAMPION_DRAKE', 'TRAINER_CLASS_CHAMPION', 'TRAINER_PIC_ELITE_FOUR_LANCE', 'DRAKE', 'TRAINER_ENCOUNTER_MUSIC_MALE', 'MUGSHOT_COLOR_DARK_RED', False,
  'ITEM_FULL_RESTORE, ITEM_FULL_RESTORE, ITEM_FULL_RESTORE', 'sParty_ChampionDrake', [
  (64, 'DITTO', 'QUICK_CLAW', ('TRANSFORM', 'NONE', 'NONE', 'NONE')),
  (64, 'STEELIX', 'LEFTOVERS', ('IRON_TAIL', 'EARTHQUAKE', 'ROCK_SLIDE', 'CURSE')),
  (65, 'GENGAR', 'SPELL_TAG', ('SHADOW_BALL', 'THUNDERBOLT', 'HYPNOSIS', 'DREAM_EATER')),
  (65, 'ELECTABUZZ', 'MAGNET', ('THUNDERBOLT', 'CROSS_CHOP', 'ICE_PUNCH', 'FIRE_PUNCH')),
  (66, 'VENUSAUR', 'MIRACLE_SEED', ('GIGA_DRAIN', 'SLUDGE_BOMB', 'EARTHQUAKE', 'SLEEP_POWDER')),
  (68, 'DRAGONITE', 'LEFTOVERS', ('DRAGON_DANCE', 'OUTRAGE', 'EARTHQUAKE', 'FIRE_BLAST'))]),
]
p = 'include/constants/opponents.h'; s = open(p).read()
s = re.sub(r'#define MAX_TRAINERS_COUNT\s+\d+', '#define MAX_TRAINERS_COUNT                       1504', s)
s = s.replace('only space for 25 additional trainers before trainer flag space overflows.',
              'room for NUM_TRAINERS up to MAX_TRAINERS_COUNT before trainer flag space overflows.')
if 'TRAINER_LEADER_CISSY' not in s:
    last = max(int(n) for n in re.findall(r'#define TRAINER_\w+\s+(\d+)\n', s.split('// NOTE')[0]))
    block = ''.join('#define %-40s %d\n' % (c, last + 1 + i) for i, (c, *_) in enumerate(NEW))
    s = s.replace('#define TRAINER_NURSE_JOY                       926\n', '#define TRAINER_NURSE_JOY                       926\n' + block, 1)
    s = re.sub(r'#define NUM_TRAINERS\s+\d+', '#define NUM_TRAINERS                             %d' % (last + 1 + len(NEW)), s)
open(p, 'w', newline='\n').write(s)
p = 'src/data/trainer_pools.h'; t = open(p).read()
t = t.replace('there is room up to MAX_TRAINERS_COUNT, 992', 'there is room up to MAX_TRAINERS_COUNT, 1504'); open(p, 'w', newline='\n').write(t)
p = 'src/data/trainer_parties.h'; t = open(p).read()
if 'sParty_LeaderCissy' not in t:
    for c, cls, pic, name, mus, mug, dbl, items, sym, party in NEW:
        t = t.rstrip('\n') + '\n\nstatic const struct TrainerMonItemCustomMoves %s[] = {\n' % sym
        for lvl, sp, item, mv in party:
            t += '    {\n        .iv = 250,\n        .lvl = %d,\n        .species = SPECIES_%s,\n        .heldItem = ITEM_%s,\n        .moves = {%s},\n    },\n' % (
                lvl, sp, item, ', '.join('MOVE_' + m for m in mv))
        t += '};\n'
    open(p, 'w', newline='\n').write(t)
p = 'src/data/trainers.h'; t = open(p).read()
if '[TRAINER_LEADER_CISSY]' not in t:
    ent = ''
    for c, cls, pic, name, mus, mug, dbl, items, sym, party in NEW:
        ent += ('    [%s] = {\n        .trainerClass = %s,\n        .encounterMusic_gender = %s,\n        .trainerPic = %s,\n'
                '        .trainerName = _("%s"),\n        .items = {%s},\n        .doubleBattle = %s,\n        .mugshotEnabled = TRUE,\n'
                '        .mugshotColor = %s,\n        .aiFlags = AI_LEVEL_ADVANCED,\n        .party = ITEM_CUSTOM_MOVES(%s),\n    },\n') % (
                c, cls, mus, pic, name, items, 'TRUE' if dbl else 'FALSE', mug, sym)
    i = t.index('    [TRAINER_NURSE_JOY] = {'); j = t.index('    },\n', i) + len('    },\n')
    t = t[:j] + ent + t[j:]
    open(p, 'w', newline='\n').write(t)
# Drake uses TRAINER_CLASS_CHAMPION (arena/music) but must not take the rival's name.
p = 'src/battle_message.c'; t = open(p).read()
old = ('                     || gTrainers[gTrainerBattleOpponent_A].trainerClass == TRAINER_CLASS_CHAMPION)\n'
       '                        toCpy = GetExpandedPlaceholder(PLACEHOLDER_ID_RIVAL);')
new = ('                     || (gTrainers[gTrainerBattleOpponent_A].trainerClass == TRAINER_CLASS_CHAMPION\n'
       '                      && gTrainerBattleOpponent_A != TRAINER_CHAMPION_DRAKE))\n'
       '                        toCpy = GetExpandedPlaceholder(PLACEHOLDER_ID_RIVAL);')
if 'TRAINER_CHAMPION_DRAKE' not in t:
    assert old in t; t = t.replace(old, new, 1); open(p, 'w', newline='\n').write(t)
p = 'src/quest_log_battle.c'; t = open(p).read()
old = ('            case TRAINER_CLASS_CHAMPION:\n                eventId = QL_EVENT_DEFEATED_CHAMPION;\n                break;')
new = ('            case TRAINER_CLASS_CHAMPION:\n'
       '                if (gTrainerBattleOpponent_A == TRAINER_CHAMPION_DRAKE)\n'
       '                    eventId = QL_EVENT_DEFEATED_GYM_LEADER; // Orange champion: log by own name\n'
       '                else\n'
       '                    eventId = QL_EVENT_DEFEATED_CHAMPION;\n                break;')
if 'TRAINER_CHAMPION_DRAKE' not in t:
    assert old in t; t = t.replace(old, new, 1); open(p, 'w', newline='\n').write(t)
for p in ('src/battle_message.c', 'src/quest_log_battle.c'):
    t = open(p).read()
    if '#include "constants/opponents.h"' not in t:
        t = t.replace('#include "global.h"\n', '#include "global.h"\n#include "constants/opponents.h"\n', 1)
        open(p, 'w', newline='\n').write(t)
print('orange leaders ok')
