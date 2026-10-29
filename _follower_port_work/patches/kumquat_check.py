"""Check the Kumquat doubles do what they are meant to, against the ROM's data.

A double battle is the one place a team can hurt itself, so the things worth
checking are mechanical: does a spread move land on its own partner, and is
every ability that the fight depends on actually the slot that was pinned.
"""
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import typechart as tc

REPO = os.environ.get('FR_REPO') or os.path.abspath(os.path.join(HERE, '..', '..'))
from kumquat_gym import TEAMS


def move_targets():
    s = open(os.path.join(REPO, 'src/data/battle_moves.h')).read()
    out = {}
    for m in re.finditer(r'\[(MOVE_\w+)\]\s*=\s*\{(.*?)\n    \},', s, re.S):
        t = re.search(r'\.target\s*=\s*([\w |]+?),\n', m.group(2))
        ty = re.search(r'\.type\s*=\s*(TYPE_\w+)', m.group(2))
        out[m.group(1)] = (t.group(1).strip() if t else '?',
                           ty.group(1) if ty else None)
    return out


def species_info():
    import glob
    src = ''
    for f in glob.glob(os.path.join(REPO, 'src/data/pokemon/*.h')):
        t = open(f).read()
        if '.abilities' in t and '[SPECIES_PIKACHU]' in t:
            src = t
            break
    out = {}
    for m in re.finditer(r'\[SPECIES_(\w+)\]\s*=\s*\{(.*?)\n    \},', src, re.S):
        b = m.group(2)
        ab = re.search(r'\.abilities\s*=\s*\{(.*?)\}', b, re.S)
        hid = re.search(r'\.hiddenAbility\s*=\s*(\w+)', b)
        ty = re.findall(r'\.types\s*=\s*MON_TYPES\((.*?)\)', b) or \
             re.findall(r'\.types\s*=\s*\{(.*?)\}', b)
        out[m.group(1)] = {
            'abilities': [x.strip() for x in ab.group(1).split(',')] if ab else [],
            'hidden': hid.group(1) if hid else 'ABILITY_NONE',
            'types': [x.strip() for x in ty[0].split(',')] if ty else [],
        }
    return out


TARGET = move_targets()
SPECIES = species_info()
PROBLEMS = []


def ability_of(sp, slot):
    info = SPECIES.get(sp, {})
    if slot == 4:
        return info.get('hidden', '?')
    if slot in (1, 2):
        a = info.get('abilities', [])
        i = slot - 1
        return a[i] if i < len(a) and a[i] != 'ABILITY_NONE' else (a[0] if a else '?')
    return 'either'


def types_of(sp):
    return [t for t in SPECIES.get(sp, {}).get('types', []) if t]


def check():
    for sym, tr, mons in TEAMS:
        name = tr.replace('TRAINER_', '')
        # 1. does any spread move land on a partner that is not immune to it?
        for i, (sp, _lv, _it, _ab, moves) in enumerate(mons):
            for mv in moves:
                tgt, mtype = TARGET.get('MOVE_' + mv, ('?', None))
                if 'FOES_AND_ALLY' not in tgt:
                    continue
                for j, (psp, *_r) in enumerate(mons):
                    if j == i:
                        continue
                    # only the pair that is out together matters: 0/1 and 2/3
                    if i // 2 != j // 2:
                        continue
                    mult = tc.mult(tc.T[mtype], tuple(tc.T[t] for t in types_of(psp)))
                    verdict = 'IMMUNE, deliberate' if mult == 0 else 'HITS ITS PARTNER'
                    print('  %-26s %s %-12s -> %-9s x%g  %s'
                          % (name, sp, mv, psp, mult / 10, verdict))
                    if mult != 0:
                        PROBLEMS.append('%s: %s %s hits %s for x%g'
                                        % (name, sp, mv, psp, mult / 10))
        # 2. every pinned ability resolves to the one the fight relies on
        for sp, _lv, _it, slot, _m in mons:
            if slot is None:
                continue
            print('  %-26s %-11s slot %s -> %s'
                  % (name, sp, slot, ability_of(sp, slot).replace('ABILITY_', '')))
    return PROBLEMS


WANT = {'DUGTRIO': 'ABILITY_ARENA_TRAP', 'VILEPLUME': 'ABILITY_CHLOROPHYLL',
        'RAPIDASH': 'ABILITY_FLASH_FIRE', 'MAROWAK': 'ABILITY_LIGHTNING_ROD',
        'KANGASKHAN': 'ABILITY_INNER_FOCUS'}

if __name__ == '__main__':
    print('spread moves and pinned abilities:')
    probs = check()
    print('\nabilities the fights depend on:')
    for sym, tr, mons in TEAMS:
        for sp, _l, _i, slot, _m in mons:
            if sp in WANT:
                got = ability_of(sp, slot)
                ok = got == WANT[sp]
                print('  %-11s wants %-22s got %-22s %s'
                      % (sp, WANT[sp].replace('ABILITY_', ''),
                         got.replace('ABILITY_', ''), 'ok' if ok else 'WRONG SLOT'))
                if not ok:
                    probs.append('%s resolved to %s, not %s' % (sp, got, WANT[sp]))
    # Golduck must not get Cloud Nine, which would cancel its own side's sun
    g = ability_of('GOLDUCK', 1)
    print('\n  GOLDUCK slot 1 -> %s (slot 2 is %s, which would cancel the sun)'
          % (g.replace('ABILITY_', ''), ability_of('GOLDUCK', 2).replace('ABILITY_', '')))
    if 'CLOUD_NINE' in g or 'AIR_LOCK' in g:
        probs.append('GOLDUCK would cancel its own weather')
    print('\nproblems: %s' % (probs or 'none'))
    sys.exit(1 if probs else 0)
