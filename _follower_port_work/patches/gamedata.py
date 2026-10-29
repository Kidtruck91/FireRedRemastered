"""Move, ability and item facts read out of the ROM source.

Same rule as typechart.py: nothing here is written from memory. Every answer
Rudy's panels accept is derived from the data the battle engine itself uses, so
a change to a move's effect or an ability's immunity breaks the generator
instead of quietly leaving a wrong answer in the gym.
"""
import json
import os
import re

REPO = os.environ.get('FR_REPO') or os.path.abspath(
    os.path.join(os.path.dirname(__file__), '..', '..'))

# which status each move effect inflicts, by the effect's own name
EFFECT_STATUS = [
    (r'^EFFECT_(SLEEP|SLEEP_HIT)$', 'sleep'),
    (r'^EFFECT_(POISON|POISON_HIT|TOXIC|POISON_FANG|POISON_TAIL|TWINEEDLE)$', 'poison'),
    (r'^EFFECT_(BURN_HIT|WILL_O_WISP|BLAZE_KICK)$', 'burn'),
    (r'^EFFECT_FREEZE_HIT$', 'freeze'),
    (r'^EFFECT_(PARALYZE|PARALYZE_HIT|THUNDER)$', 'paralysis'),
    (r'^EFFECT_(CONFUSE|CONFUSE_HIT|SWAGGER|FLATTER)$', 'confusion'),
]

STATUS_FLAG = {'sleep': 'STATUS1_SLEEP', 'poison': 'STATUS1_POISON',
               'burn': 'STATUS1_BURN', 'freeze': 'STATUS1_FREEZE',
               'paralysis': 'STATUS1_PARALYSIS', 'confusion': 'STATUS2_CONFUSION'}
HOLD_STATUS = {'HOLD_EFFECT_CURE_PAR': 'paralysis', 'HOLD_EFFECT_CURE_SLP': 'sleep',
               'HOLD_EFFECT_CURE_PSN': 'poison', 'HOLD_EFFECT_CURE_BRN': 'burn',
               'HOLD_EFFECT_CURE_FRZ': 'freeze',
               'HOLD_EFFECT_CURE_CONFUSION': 'confusion'}


def move_status():
    """MOVE_x -> the status it can inflict, or None."""
    s = open(os.path.join(REPO, 'src/data/battle_moves.h')).read()
    out = {}
    for m in re.finditer(r'\[(MOVE_\w+)\]\s*=\s*\{(.*?)\n    \},', s, re.S):
        eff = re.search(r'\.effect\s*=\s*(\w+)', m.group(2))
        if not eff:
            continue
        for pat, st in EFFECT_STATUS:
            if re.match(pat, eff.group(1)):
                out[m.group(1)] = st
                break
        else:
            out[m.group(1)] = None
    return out


def ability_immunities():
    """ABILITY_x -> the set of statuses it clears, read from battle_util.c's
    ABILITYEFFECT_IMMUNITY switch rather than from memory."""
    s = open(os.path.join(REPO, 'src/battle_util.c')).read()
    i = s.index('ABILITYEFFECT_IMMUNITY')
    body = s[i:i + 6000]
    out, pending = {}, []
    for line in body.split('\n'):
        c = re.match(r'\s*case (ABILITY_\w+):', line)
        if c:
            pending.append(c.group(1))
            continue
        if pending and ('status1 &' in line or 'status2 &' in line):
            found = {st for st, flag in STATUS_FLAG.items() if flag in line}
            if found:
                for ab in pending:
                    out.setdefault(ab, set()).update(found)
            pending = []
        elif re.match(r'\s*(break|default)', line):
            pending = []
    return out


def berry_cures():
    """ITEM_x -> the status its hold effect cures."""
    items = json.load(open(os.path.join(REPO, 'src/data/items.json')))['items']
    return {it['itemId']: HOLD_STATUS[it['holdEffect']]
            for it in items if it['holdEffect'] in HOLD_STATUS}


MOVE_STATUS = move_status()
ABILITY_IMMUNE = ability_immunities()
BERRY_CURES = berry_cures()
