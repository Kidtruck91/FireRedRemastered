#!/usr/bin/env python3
"""Luana's Kumquat Gym: three double battles, each built around one idea.

Every fight in this gym is a double, and the three of them were default-move
filler. They now each teach one thing the single-battle game never asks you to
think about, and the abilities are pinned rather than left to chance, because
every one of them is the point of the fight:

  ANA & BEL   a spread move and a partner who does not care about it.
              DUGTRIO throws EARTHQUAKE every turn -- MOVE_TARGET_FOES_AND_ALLY,
              so it hits its own side too -- and DODRIO is FLYING, so it never
              feels it. ARENA TRAP means you cannot switch away from it either.

  LEX & NYA   weather, then a spread that does spare the partner.
              RAPIDASH sets SUNNY DAY: its FIRE BLAST grows, VILEPLUME's
              SOLARBEAM stops needing a charge turn, and CHLOROPHYLL doubles
              VILEPLUME's speed. One move, three payoffs. The back pair swap to
              GOLDUCK's SURF, which is MOVE_TARGET_BOTH -- both of yours, neither
              of theirs -- and PERSIAN opens with FAKE OUT.

  LUANA       support and control. MAROWAK takes LIGHTNING ROD so it pulls
              ELECTRIC attacks off whoever stands beside it, and both it and
              STEELIX carry ROCK SLIDE rather than EARTHQUAKE, so she never
              hits her own side. KANGASKHAN keeps INNER FOCUS, so your own
              FAKE OUT does nothing to her lead.

GOLDUCK is pinned to its first ability on purpose: its second is CLOUD NINE,
which would cancel the sun its own partner just set up.

Run from the repo root. Re-running rewrites the same three parties.
"""
import os
import re
import sys

# (symbol, trainer constant, [(species, level, item, ability slot, [moves])])
#   ability slot: 1 or 2 = that slot, 4 = hidden, None = leave as it was
TEAMS = [
    ('sParty_TwinsKumquatAnaBel', 'TRAINER_TWINS_KUMQUAT_ANA_BEL', [
        ('DUGTRIO', 41, 'ITEM_SOFT_SAND', 2,
         ['EARTHQUAKE', 'ROCK_SLIDE', 'SAND_ATTACK', 'SLASH']),
        ('DODRIO', 41, 'ITEM_SHARP_BEAK', 2,
         ['DRILL_PECK', 'TRI_ATTACK', 'STEEL_WING', 'AGILITY']),
    ]),
    ('sParty_CoolCoupleKumquatLexNya', 'TRAINER_COOL_COUPLE_KUMQUAT_LEX_NYA', [
        ('RAPIDASH', 42, 'ITEM_CHARCOAL', 2,
         ['SUNNY_DAY', 'FIRE_BLAST', 'STOMP', 'AGILITY']),
        ('VILEPLUME', 42, 'ITEM_MIRACLE_SEED', 1,
         ['SOLAR_BEAM', 'SLUDGE_BOMB', 'SLEEP_POWDER', 'GIGA_DRAIN']),
        ('GOLDUCK', 42, 'ITEM_MYSTIC_WATER', 1,
         ['SURF', 'ICE_BEAM', 'PSYCHIC', 'DISABLE']),
        ('PERSIAN', 42, 'ITEM_SILK_SCARF', 1,
         ['FAKE_OUT', 'SLASH', 'SCREECH', 'BITE']),
    ]),
    ('sParty_LeaderLuana', 'TRAINER_LEADER_LUANA', [
        ('KANGASKHAN', 42, 'ITEM_SILK_SCARF', 4,
         ['FAKE_OUT', 'BODY_SLAM', 'CRUNCH', 'BRICK_BREAK']),
        ('STEELIX', 42, 'ITEM_METAL_COAT', 2,
         ['ROCK_SLIDE', 'IRON_TAIL', 'DRAGON_BREATH', 'SCREECH']),
        ('MAROWAK', 43, 'ITEM_THICK_CLUB', 2,
         ['BONEMERANG', 'ROCK_SLIDE', 'SWORDS_DANCE', 'HEADBUTT']),
        ('ALAKAZAM', 44, 'ITEM_TWISTED_SPOON', 4,
         ['PSYCHIC', 'CALM_MIND', 'REFLECT', 'RECOVER']),
    ]),
]


def party_block(sym, mons):
    out = ['static const struct TrainerMonItemCustomMoves %s[] = {' % sym]
    for sp, lvl, item, ab, moves in mons:
        out.append('    {')
        out.append('        .iv = 250,')
        out.append('        .lvl = %d,' % lvl)
        out.append('        .species = SPECIES_%s,' % sp)
        if ab is not None:
            out.append('        .ability = %s,'
                       % ('TRAINER_MON_HIDDEN_ABILITY' if ab == 4 else str(ab)))
        out.append('        .heldItem = %s,' % item)
        out.append('        .moves = {%s},' % ', '.join('MOVE_' + m for m in moves))
        out.append('    },')
    out.append('};')
    return '\n'.join(out)


def main(repo='.'):
    p = os.path.join(repo, 'src/data/trainer_parties.h')
    s = open(p).read()
    for sym, _tr, mons in TEAMS:
        pat = re.compile(r'static const struct TrainerMon\w+ %s\[\] = \{.*?\n\};' % sym, re.S)
        if not pat.search(s):
            raise SystemExit('could not find %s' % sym)
        s = pat.sub(lambda _m, b=party_block(sym, mons): b, s, count=1)
    open(p, 'w').write(s)

    # all three now carry items and custom movesets
    p = os.path.join(repo, 'src/data/trainers.h')
    s = open(p).read()
    for sym, tr, _m in TEAMS:
        blk = re.search(r'(\[%s\] = \{.*?\n    \},)' % tr, s, re.S)
        new = re.sub(r'\.party = \w+\(%s\)' % sym,
                     '.party = ITEM_CUSTOM_MOVES(%s)' % sym, blk.group(1))
        s = s.replace(blk.group(1), new, 1)
    open(p, 'w').write(s)
    print('kumquat gym: %d double-battle teams rebuilt (%d mon)'
          % (len(TEAMS), sum(len(m) for _s, _t, m in TEAMS)))


if __name__ == '__main__':
    main(sys.argv[1] if len(sys.argv) > 1 else '.')
