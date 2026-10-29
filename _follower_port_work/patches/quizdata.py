"""Rudy's question set, and the checker that refuses to ship a broken one.

A question names four options and what it is asking for. Which option is the
answer is never written down -- it is worked out from the ROM's own data, and
a question whose options do not yield exactly one answer is a build failure.
"""
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import typechart as tc
import gamedata as gd

ty = lambda n: tc.T['TYPE_' + n.upper()]


def Q(**kw):
    return kw


# Six panels. Each draws one of its two questions at random, so a second run
# through the gym is not the same six answers.
GATES = [
    # 1 -- plain super effective
    [Q(kind='type', defs=('Water', 'Flying'), mode='best',
       opts=['Rock', 'Ice', 'Electric', 'Grass'],
       text="A WATER and FLYING POKeMON.\\nWhich type hits it hardest?"),
     Q(kind='type', defs=('Bug', 'Steel'), mode='best',
       opts=['Fighting', 'Fire', 'Ground', 'Flying'],
       text="A BUG and STEEL POKeMON.\\nWhich type hits it hardest?")],
    # 2 -- the second type cancels the obvious answer
    [Q(kind='type', defs=('Grass', 'Dragon'), mode='best',
       opts=['Fire', 'Bug', 'Dragon', 'Ice'],
       text="A GRASS and DRAGON POKeMON.\\nWhich type hits it hardest?"),
     Q(kind='type', defs=('Water', 'Ground'), mode='best',
       opts=['Grass', 'Electric', 'Ice', 'Water'],
       text="A WATER and GROUND POKeMON.\\nWhich type hits it hardest?")],
    # 3 -- immunity and resistance
    [Q(kind='type', defs=('Normal', 'Flying'), mode='none',
       opts=['Rock', 'Fighting', 'Bug', 'Ghost'],
       text="A NORMAL and FLYING POKeMON.\\nWhich type does nothing at all\\lto it?"),
     Q(kind='type', defs=('Rock', 'Ground'), mode='worst',
       opts=['Poison', 'Fighting', 'Ice', 'Grass'],
       text="A ROCK and GROUND POKeMON.\\nWhich type does it shrug off\\lhardest?")],
    # 4 -- what a move leaves behind
    [Q(kind='move_status', status='paralysis',
       opts=['MOVE_RAZOR_LEAF', 'MOVE_STUN_SPORE', 'MOVE_ABSORB', 'MOVE_GROWTH'],
       text="Which of these can leave a\\nPOKeMON PARALYZED?"),
     Q(kind='move_status', status='burn',
       opts=['MOVE_WILL_O_WISP', 'MOVE_CONFUSE_RAY', 'MOVE_NIGHT_SHADE', 'MOVE_SPITE'],
       text="Which of these can leave a\\nPOKeMON BURNED?")],
    # 5 -- an ability that makes a move pointless
    [Q(kind='ability_vs_move', ability='ABILITY_LIMBER',
       opts=['MOVE_TOXIC', 'MOVE_THUNDER_WAVE', 'MOVE_SLEEP_POWDER', 'MOVE_WILL_O_WISP'],
       text="A POKeMON with LIMBER.\\nWhich move is wasted on it?"),
     Q(kind='ability_vs_move', ability='ABILITY_WATER_VEIL',
       opts=['MOVE_STUN_SPORE', 'MOVE_SPORE', 'MOVE_WILL_O_WISP', 'MOVE_POISON_POWDER'],
       text="A POKeMON with WATER VEIL.\\nWhich move is wasted on it?")],
    # 6 -- the answer from the other side, and the item that undoes it
    [Q(kind='move_vs_ability', move='MOVE_THUNDER_WAVE',
       opts=['ABILITY_INSOMNIA', 'ABILITY_IMMUNITY', 'ABILITY_LIMBER', 'ABILITY_MAGMA_ARMOR'],
       text="Which ability makes THUNDER WAVE\\npointless?"),
     Q(kind='berry', status='paralysis',
       opts=['ITEM_PECHA_BERRY', 'ITEM_RAWST_BERRY', 'ITEM_CHERI_BERRY', 'ITEM_ASPEAR_BERRY'],
       text="A POKeMON is PARALYZED.\\nWhich BERRY would cure it?")],
]

SYMBOL = {'type': lambda o: 'gTypeNames[TYPE_%s]' % o.upper(),
          'move': lambda o: 'gMoveNames[%s]' % o,
          'ability': lambda o: 'gAbilityNames[%s]' % o,
          'item': lambda o: 'gItems[%s].name' % o}


def option_kind(q):
    return {'type': 'type', 'move_status': 'move', 'ability_vs_move': 'move',
            'move_vs_ability': 'ability', 'berry': 'item'}[q['kind']]


def score(q):
    """What each option is worth, as the ROM's own data has it."""
    k = q['kind']
    if k == 'type':
        d = tuple(ty(x) for x in q['defs'])
        return [tc.mult(ty(o), d) for o in q['opts']]
    if k == 'move_status':
        return [1 if gd.MOVE_STATUS.get(o) == q['status'] else 0 for o in q['opts']]
    if k == 'ability_vs_move':
        blocked = gd.ABILITY_IMMUNE.get(q['ability'], set())
        if not blocked:
            raise SystemExit('%s has no immunity in battle_util.c' % q['ability'])
        return [1 if gd.MOVE_STATUS.get(o) in blocked else 0 for o in q['opts']]
    if k == 'move_vs_ability':
        st = gd.MOVE_STATUS.get(q['move'])
        if st is None:
            raise SystemExit('%s inflicts no status' % q['move'])
        return [1 if st in gd.ABILITY_IMMUNE.get(o, set()) else 0 for o in q['opts']]
    if k == 'berry':
        return [1 if gd.BERRY_CURES.get(o) == q['status'] else 0 for o in q['opts']]
    raise SystemExit('unknown question kind %r' % k)


def solve(q, where=''):
    s = score(q)
    if q['kind'] == 'type':
        target = {'best': max, 'worst': min}[q['mode']](s) if q['mode'] != 'none' else 0
        if q['mode'] == 'none' and target != 0:
            raise SystemExit('%s: no immunity among the options' % where)
    else:
        target = 1
    hits = [i for i, v in enumerate(s) if v == target]
    if len(hits) != 1:
        raise SystemExit('%s: %d options answer it, not 1 -> %s'
                         % (where, len(hits), list(zip(q['opts'], s))))
    return hits[0], s


def check_all():
    for gi, pool in enumerate(GATES):
        for pi, q in enumerate(pool):
            solve(q, 'gate %d %s' % (gi + 1, 'AB'[pi]))
    return True
