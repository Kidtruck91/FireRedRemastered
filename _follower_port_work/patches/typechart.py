"""Read gTypeEffectiveness straight out of the ROM source, so quiz answers can
never drift from the chart the battle engine actually uses."""
import os
import re

REPO = os.environ.get('FR_REPO') or os.path.abspath(
    os.path.join(os.path.dirname(__file__), '..', '..'))
SRC = os.path.join(REPO, 'src/battle_main.c')
HDR = os.path.join(REPO, 'include/constants/pokemon.h')


def load():
    names = {}
    for m in re.finditer(r'#define (TYPE_[A-Z_]+)\s+(\d+)', open(HDR).read()):
        names[m.group(1)] = int(m.group(2))
    mul = {'TYPE_MUL_NO_EFFECT': 0, 'TYPE_MUL_NOT_EFFECTIVE': 5,
           'TYPE_MUL_NORMAL': 10, 'TYPE_MUL_SUPER_EFFECTIVE': 20}
    body = re.search(r'const u8 gTypeEffectiveness\[336\] =\s*\{(.*?)\n\};',
                     open(SRC).read(), re.S).group(1)
    body = re.sub(r'//.*', '', body)
    toks = [t.strip() for t in body.split(',') if t.strip()]
    chart = {}
    for i in range(0, len(toks), 3):
        a, d, m = toks[i:i + 3]
        if a == 'TYPE_ENDTABLE':
            break
        if a == 'TYPE_FORESIGHT':
            # a marker row, not a matchup. The rows after it are still part of
            # the chart -- Normal/Fighting vs Ghost -- they are just the ones
            # Foresight is allowed to ignore. Dropping them silently turned
            # Ghost into a type nothing was immune against.
            continue
        chart[(names[a], names[d])] = mul[m]
    rev = {v: k.replace('TYPE_', '').title() for k, v in names.items()
           if v < 18 and k not in ('TYPE_MYSTERY',)}
    return chart, names, rev


CHART, T, NAME = load()
REAL = [v for k, v in T.items()
        if k.startswith('TYPE_') and v < 18
        and k not in ('TYPE_MYSTERY', 'TYPE_NONE', 'TYPE_EGG')]
REAL = sorted(set(REAL) - {T['TYPE_MYSTERY']})


def mult(atk, defs):
    """Damage multiplier of atk against a defender of one or two types, x10."""
    m = 10
    for d in defs:
        m = m * CHART.get((atk, d), 10) // 10
    return m
