"""Re-fit the sea-lane crossings to the barrier rocks.

Barrier rocks went in along Navel, Kinnow and Route 17, and they are precise:
each one carves the coast down to a channel a few tiles wide. Navel's west edge
is now open at exactly y 54-61 and solid everywhere else; Route 9's east edge at
exactly y 106-113; Kinnow's west at y 2-17 and its east at y 47-63. The triggers
were moved and pruned to suit -- Navel's Route 9 band is a column on its west
edge now, not a row along its south coast.

What did not move is the arithmetic inside the scripts, and that is where a
crossing actually lands. Six legs were left aiming at coast that is now stone:

    Route 9 -> Navel        landed on (2..9, 63), the whole band in rock
    Navel -> Route 9        still read the player's x on a band that is a column,
                            so every tile of it clamped to one spot
    Route 10 -> Kinnow      landed on (1, 0) and (1, 1)
    Kinnow -> Route 10      landed on Route 10 (102, 40..42)
    Route 8 -> Kinnow       landed on (70, 64) and (70, 65)
    Kinnow -> Route 8       landed on Route 8 (1, 1)

So each leg is re-aimed at the channel the rocks left, and the two ends of a
crossing are made to agree: the band is whichever stretch both channels share,
and the clamp carries the overhang into it rather than pruning the triggers. A
tile of channel with no trigger on it is a tile where you bump the edge and
nothing happens, and leaving the clamp to absorb it means neither side has one.

Navel's crossing gets simpler out of it. It used to turn a corner -- a column of
water meeting a row -- which took reading the player's position into the
registers the other way round. Both ends are columns now, so the swap is gone.

    python3 docs/orange-islands/refit_sea_lanes.py
"""
import os
import re

REPO = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))

# map, script, the axis read and the band on this side, the shift, where the
# player lands on the far side, and the channel each end now has.
LEGS = [
    dict(src='OrangeIslands_Navel', name='SeaEdgeToRoute9',
         dest='MAP_ORANGE_ISLANDS_ROUTE_9', axis='y', lo=54, hi=61,
         shift=+52, fixed=('x', 50),
         was='read the player x on a band that is a column, so all eight tiles '
             'clamped to Route 9 (50,106)',
         note='Navel west, open y 54-61 <-> Route 9 east, open y 106-113'),
    dict(src='OrangeIslands_Route_9', name='SeaEdgeToNavel',
         dest='MAP_ORANGE_ISLANDS_NAVEL', axis='y', lo=106, hi=113,
         shift=-52, fixed=('x', 1),
         was='landed along Navel row 63, every tile of it now rock',
         note='the return of the crossing above'),
    dict(src='OrangeIslands_Route_10', name='SeaEdgeToKinnow',
         dest='MAP_ORANGE_ISLANDS_KINNOW', axis='y', lo=43, hi=57,
         shift=-40, fixed=('x', 1),
         was='clamped to y 40-67 and landed on Kinnow (1,0) and (1,1)',
         note="Kinnow's west channel is y 2-17, and its triggers start at 3"),
    dict(src='OrangeIslands_Kinnow', name='SeaEdgeToRoute10',
         dest='MAP_ORANGE_ISLANDS_ROUTE_10', axis='y', lo=3, hi=17,
         shift=+40, fixed=('x', 102),
         was='clamped from y 0, landing on Route 10 (102,40) through (102,42)',
         note='the return of the crossing above'),
    dict(src='OrangeIslands_Route_8', name='SeaEdgeToKinnow',
         dest='MAP_ORANGE_ISLANDS_KINNOW', axis='y', lo=2, hi=17,
         shift=+46, fixed=('x', 70),
         was='clamped to y 0-19 and landed on Kinnow (70,64) and (70,65)',
         note="Kinnow's east channel is y 47-63; Route 8's own is y 2-19"),
    dict(src='OrangeIslands_Kinnow', name='SeaEdgeToRoute8',
         dest='MAP_ORANGE_ISLANDS_ROUTE_8', axis='y', lo=48, hi=63,
         shift=-46, fixed=('x', 1),
         was='clamped from y 47, landing on Route 8 (1,1)',
         note='the return of the crossing above'),
]


def block(label, leg):
    """One crossing, in the shape the rest of the region already uses."""
    var = 'VAR_0x8004' if leg['axis'] == 'x' else 'VAR_0x8005'
    fixed_axis, fixed_val = leg['fixed']
    fixed_var = 'VAR_0x8004' if fixed_axis == 'x' else 'VAR_0x8005'
    L = ['@ %s' % leg['note'],
         '%s::' % label,
         '\tgetplayerxy VAR_0x8004, VAR_0x8005',
         '\tcompare %s, %d' % (var, leg['lo']),
         '\tgoto_if_lt %s_Lo' % label,
         '\tcompare %s, %d' % (var, leg['hi']),
         '\tgoto_if_gt %s_Hi' % label,
         '%s_Go:' % label]
    if leg['shift'] > 0:
        L.append('\taddvar %s, %d' % (var, leg['shift']))
    elif leg['shift'] < 0:
        L.append('\tsubvar %s, %d' % (var, -leg['shift']))
    L += ['\tsetvar %s, %d' % (fixed_var, fixed_val),
          '\twarp %s, VAR_0x8004, VAR_0x8005' % leg['dest'],
          '\twaitstate',
          '\tend',
          '%s_Lo:' % label,
          '\tsetvar %s, %d' % (var, leg['lo']),
          '\tgoto %s_Go' % label,
          '%s_Hi:' % label,
          '\tsetvar %s, %d' % (var, leg['hi']),
          '\tgoto %s_Go' % label]
    return '\n'.join(L)


def main():
    for leg in LEGS:
        path = os.path.join(REPO, 'data/maps', leg['src'], 'scripts.inc')
        s = open(path, encoding='utf-8').read()
        label = '%s_EventScript_%s' % (leg['src'], leg['name'])

        start = s.index(label + '::')
        head = s.rfind('\n@', 0, start)           # take the comment line above
        if head != -1 and s.count('\n', head, start) == 1:
            start = head + 1
        # the block ends with the last line of its _Hi stanza
        tail = re.compile(re.escape(label) + r'_Hi:\n\tsetvar [^\n]*\n\tgoto '
                          + re.escape(label) + r'_Go\n')
        end = tail.search(s, start).end()

        s = s[:start] + block(label, leg) + '\n' + s[end:]
        open(path, 'w', encoding='utf-8', newline='\n').write(s)
        print('  %-24s %-18s band %s %d-%d, lands %s=%d  (was: %s)'
              % (leg['src'].replace('OrangeIslands_', ''), leg['name'],
                 leg['axis'], leg['lo'], leg['hi'],
                 leg['fixed'][0], leg['fixed'][1], leg['was']))


if __name__ == '__main__':
    main()
