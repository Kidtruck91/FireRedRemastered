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
         lead='west of the stretch Route 5 uses',
         dest='MAP_ORANGE_ISLANDS_ROUTE_9', axis='y', lo=54, hi=61,
         shift=+52, fixed=('x', 50),
         was='read the player x on a band that is a column, so all eight tiles '
             'clamped to Route 9 (50,106)',
         note='Navel west, open y 54-61 <-> Route 9 east, open y 106-113'),
    dict(src='OrangeIslands_Route_9', name='SeaEdgeToNavel',
         lead="Navel's south-west corner; Route 5 has x 23-66 of that coast.",
         dest='MAP_ORANGE_ISLANDS_NAVEL', axis='y', lo=106, hi=113,
         shift=-52, fixed=('x', 1),
         was='landed along Navel row 63, every tile of it now rock',
         note='the return of the crossing above'),
    dict(src='OrangeIslands_Route_10', name='SeaEdgeToKinnow',
         lead='Sea edge: any surfable cell on this edge, landing at the matching spot on Kinnow',
         dest='MAP_ORANGE_ISLANDS_KINNOW', axis='y', lo=43, hi=57,
         shift=-40, fixed=('x', 1),
         was='clamped to y 40-67 and landed on Kinnow (1,0) and (1,1)',
         note="Kinnow's west channel is y 2-17, and its triggers start at 3"),
    dict(src='OrangeIslands_Kinnow', name='SeaEdgeToRoute10',
         lead='Sea edge: any surfable cell on this edge, landing at the matching spot on Route 10',
         dest='MAP_ORANGE_ISLANDS_ROUTE_10', axis='y', lo=3, hi=17,
         shift=+40, fixed=('x', 102),
         was='clamped from y 0, landing on Route 10 (102,40) through (102,42)',
         note='the return of the crossing above'),
    dict(src='OrangeIslands_Route_8', name='SeaEdgeToKinnow',
         lead='Sea edge: any surfable cell on this edge, landing at the matching spot on Kinnow',
         dest='MAP_ORANGE_ISLANDS_KINNOW', axis='y', lo=2, hi=17,
         shift=+46, fixed=('x', 70),
         was='clamped to y 0-19 and landed on Kinnow (70,64) and (70,65)',
         note="Kinnow's east channel is y 47-63; Route 8's own is y 2-19"),
    dict(src='OrangeIslands_Kinnow', name='SeaEdgeToRoute8',
         lead='Sea edge: any surfable cell on this edge, landing at the matching spot on Route 8',
         dest='MAP_ORANGE_ISLANDS_ROUTE_8', axis='y', lo=48, hi=63,
         shift=-46, fixed=('x', 1),
         was='clamped from y 47, landing on Route 8 (1,1)',
         note='the return of the crossing above'),

    # ROUTE 7's south coast is walled: a line of boulders on row 86 and a
    # second on row 85, staggered so their gaps never line up. The one way
    # through is at the east end, where row 85 opens at x 54-67 and rows 86
    # and 87 at 56-66. Everything west of that is pockets of water sealed
    # between the two lines and the bottom of the map.
    #
    # The crossing was landing on row 86 -- the near boulder line itself --
    # anywhere along a band of fifty-seven tiles. It lands in the channel
    # now, on row 87 below the boulders, and the clamp carries the rest of
    # SUNBURST's coast into it.
    dict(src='OrangeIslands_Sunburst', name='SeaEdgeToRoute7',
         lead='Sea edge: any surfable cell on this edge, landing at the matching spot on Route 7',
         dest='MAP_ORANGE_ISLANDS_ROUTE_7', axis='x', lo=50, hi=59,
         shift=+6, fixed=('y', 87),
         was='band x 2-60 landing on row 86, the boulder line, for all but '
             'seven tiles of it',
         note="SUNBURST north x 50-59 <-> ROUTE 7's east channel, row 87"),
    dict(src='OrangeIslands_Route_7', name='SeaEdgeToSunburst',
         lead='Sea edge: any surfable cell on this edge, landing at the matching spot on Sunburst',
         dest='MAP_ORANGE_ISLANDS_SUNBURST', axis='x', lo=56, hi=65,
         shift=-6, fixed=('y', 1),
         was='band x 6-69 landing on SUNBURST x 0-63, the first three and '
             'last four of them rock',
         note='the return of the crossing above'),
]

# ROUTE 7's south triggers belong on the channel and nowhere else: a trigger
# on one of the sealed pockets would be a crossing you can never reach. This
# re-asserts the eleven that are there rather than adding any.

# ROUTE 7's south triggers only covered x 56-66, one channel of the four, so
# the return crossing did not exist along the rest of a coast the outbound one
# lands you on. They are laid along every open tile of row 87 the band
# reaches.
TRIGGERS = [
    dict(map='OrangeIslands_Route_7', script='OrangeIslands_Route_7_EventScript_SeaEdgeToSunburst',
         y=87, lo=56, hi=66),
]


def block(label, leg):
    """One crossing, in the shape the rest of the region already uses."""
    var = 'VAR_0x8004' if leg['axis'] == 'x' else 'VAR_0x8005'
    fixed_axis, fixed_val = leg['fixed']
    fixed_var = 'VAR_0x8004' if fixed_axis == 'x' else 'VAR_0x8005'
    L = []
    if leg.get('lead'):
        L.append('@ %s' % leg['lead'])
    L += ['@ %s' % leg['note'],
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
    for n, (bad_lo, bad_hi, _snap) in enumerate(leg.get('avoid', ()), 1):
        for bad in range(bad_lo, bad_hi + 1):
            L += ['\tcompare %s, %d' % (var, bad),
                  '\tgoto_if_eq %s_Snap%d' % (label, n)]
    if leg.get('avoid'):
        L.append('%s_Warp:' % label)
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
    for n, (_lo, _hi, snap) in enumerate(leg.get('avoid', ()), 1):
        L += ['%s_Snap%d:' % (label, n),
              '\tsetvar %s, %d' % (var, snap),
              '\tgoto %s_Warp' % label]
    return '\n'.join(L)


def lay_triggers():
    """Put a trigger on every open tile of the landing row the band reaches."""
    import json
    import struct

    lj = json.load(open(os.path.join(REPO, 'data/layouts/layouts.json')))
    for t in TRIGGERS:
        mp = os.path.join(REPO, 'data/maps', t['map'], 'map.json')
        mj = json.load(open(mp))
        lay = next(l for l in lj['layouts'] if l and l['id'] == mj['layout'])
        raw = open(os.path.join(REPO, lay['blockdata_filepath']), 'rb').read()
        blocks = struct.unpack('<%dH' % (len(raw) // 2), raw)
        w = lay['width']

        mine = [c for c in mj['coord_events'] if c.get('script') == t['script']]
        kept = [c for c in mj['coord_events'] if c.get('script') != t['script']]
        open_x = [x for x in range(t['lo'], t['hi'] + 1)
                  if not blocks[t['y'] * w + x] & 0xc00]
        if sorted((c['x'], c['y']) for c in mine) == [(x, t['y']) for x in open_x]:
            print('  %-24s %d triggers on row %d, x %d-%d (already right)'
                  % (t['map'].replace('OrangeIslands_', ''), len(mine),
                     t['y'], t['lo'], t['hi']))
            continue
        for x in open_x:
            kept.append({"type": "trigger", "x": x, "y": t['y'], "elevation": 0,
                         "var": "VAR_TEMP_0", "var_value": "0",
                         "script": t['script']})
        if kept != mj['coord_events']:
            mj['coord_events'] = kept
            with open(mp, 'w', newline='\n') as f:
                json.dump(mj, f, indent=2)
                f.write('\n')
        print('  %-24s %d triggers on row %d, x %d-%d'
              % (t['map'].replace('OrangeIslands_', ''), len(open_x),
                 t['y'], t['lo'], t['hi']))


def main():
    for leg in LEGS:
        path = os.path.join(REPO, 'data/maps', leg['src'], 'scripts.inc')
        s = open(path, encoding='utf-8').read()
        label = '%s_EventScript_%s' % (leg['src'], leg['name'])

        start = s.index(label + '::')
        # Take every comment line immediately above with it, so re-running
        # replaces the note rather than stacking another copy on top of it.
        while True:
            head = s.rfind('\n@', 0, start)
            if head == -1 or s.count('\n', head + 1, start) != 1:
                break
            start = head + 1
        # the block ends with the last line of its _Hi stanza
        tail = re.compile(re.escape(label) + r'_Hi:\n\tsetvar [^\n]*\n\tgoto '
                          + re.escape(label) + r'_Go\n'
                          + r'(?:' + re.escape(label) + r'_Snap\d:\n\tsetvar [^\n]*'
                          + r'\n\tgoto ' + re.escape(label) + r'_Warp\n)*')
        end = tail.search(s, start).end()

        s = s[:start] + block(label, leg) + '\n' + s[end:]
        open(path, 'w', encoding='utf-8', newline='\n').write(s)
        print('  %-24s %-18s band %s %d-%d, lands %s=%d  (was: %s)'
              % (leg['src'].replace('OrangeIslands_', ''), leg['name'],
                 leg['axis'], leg['lo'], leg['hi'],
                 leg['fixed'][0], leg['fixed'][1], leg['was']))
    lay_triggers()


if __name__ == '__main__':
    main()
