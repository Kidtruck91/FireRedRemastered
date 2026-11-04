"""Build the lower two thirds of Trovita Falls as a route, not a river.

The map was 48x56: a gorge with walkable banks, one waterfall, and a plain corridor out
to Trovita at the bottom. It is now 48x96, and the forty rows below the author's
original 56 are a continuation of the same route rather than open water -- banks you
walk on, tall grass, and two more waterfalls, so the climb from the Trovita entrance up
to RUDY's GYM passes three falls in all.

Nothing here is invented. Every new row is a verbatim copy of one of the author's own
rows, picked so the water columns line up at each seam, which keeps the cliff faces,
bank edges and water-edge tiles in arrangements that already exist in the map:

    rows 21-25 of the original are a waterfall, six wide, with the rock face either side
    rows 26-46 are the gorge: water emerging below a fall, banks both sides, narrowing

Composed downward from the author's row 55, each band is one of those two:

    56-69  gorge        (original 26-39)
    70-74  WATERFALL    (original 21-25)
    75-82  gorge        (original 26-33)
    83-87  WATERFALL    (original 21-25)
    88-95  gorge, and the way in from Trovita (original 26-33)

Only the tall grass is then re-laid, so the copied bands do not read as the same stretch
three times. Grass and plain ground are both collision 0 elevation 3, so moving it
changes nothing but where the encounters are.

The waterfalls need no scripting. MB_WATERFALL is gated by the engine itself:
field_control_avatar.c allows the climb only with FLAG_BADGE07_GET while surfing north,
and standing on one pushes the player back down, so coming down again is free.

    python3 docs/orange-islands/build_trovita_falls.py
"""
import json
import os
import random
import struct

REPO = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
LAYOUT = 'LAYOUT_ORANGE_ISLANDS_TROVITA_FALLS'
KEEP = 56                        # the author's rows, left alone
NEW_H = 96

GRASS, GROUND = 0x00D, 0x001     # both collision 0, elevation 3
WALK = 0x3 << 12                 # elevation 3, collision 0

# (source first row, source last row) in the author's half, in order going down
BANDS = [(26, 39), (21, 25), (26, 33), (21, 25), (26, 33)]

ENTRY = (22, 89)                 # where Trovita lands the player: below every tier
EXIT_ROW = NEW_H - 1

# Tall grass for the new banks: (row, row, x, x) rectangles, laid on plain ground only.
PATCHES = [(58, 63, 27, 34), (65, 69, 11, 17), (76, 80, 28, 36),
           (88, 92, 11, 16), (91, 94, 29, 35)]


def main():
    lp = os.path.join(REPO, 'data/layouts/layouts.json')
    doc = json.load(open(lp))
    layout = next(l for l in doc['layouts'] if l.get('id') == LAYOUT)
    w = layout['width']
    path = os.path.join(REPO, layout['blockdata_filepath'])
    raw = open(path, 'rb').read()
    have_h = len(raw) // 2 // w
    cell = lambda x, y: struct.unpack_from('<H', raw, 2 * (y * w + x))[0]

    rows = [[cell(x, y) for x in range(w)] for y in range(KEEP)]
    assert have_h >= KEEP, 'the author half is missing'

    y = KEEP
    built = []
    for a, b in BANDS:
        for src in range(a, b + 1):
            rows.append([cell(x, src) for x in range(w)])
        built.append((y, y + (b - a), a, b))
        y += b - a + 1
    assert y == NEW_H, 'bands come to %d rows, wanted %d' % (y - KEEP, NEW_H - KEEP)

    # --- re-lay the grass so the copied bands do not read as repeats ---------------
    for yy in range(KEEP, NEW_H):
        for xx in range(w):
            if (rows[yy][xx] & 0x3FF) == GRASS:
                rows[yy][xx] = GROUND | WALK
    laid = 0
    for (y0, y1, x0, x1) in PATCHES:
        for yy in range(y0, min(y1 + 1, NEW_H)):
            for xx in range(x0, min(x1 + 1, w)):
                if (rows[yy][xx] & 0x3FF) == GROUND and ((rows[yy][xx] >> 10) & 3) == 0:
                    rows[yy][xx] = GRASS | WALK
                    laid += 1

    out = b''.join(struct.pack('<H', rows[yy][xx]) for yy in range(NEW_H) for xx in range(w))
    open(path, 'wb').write(out)
    layout['height'] = NEW_H
    with open(lp, 'w', newline='\n') as f:
        json.dump(doc, f, indent=2); f.write('\n')
    print('  %s -> %dx%d (%d bytes), %d tiles of tall grass on the new banks'
          % (LAYOUT, w, NEW_H, len(out), laid))
    for (y0, y1, a, b) in built:
        kind = 'WATERFALL' if (a, b) == (21, 25) else 'gorge    '
        print('     rows %2d-%2d  %s  (copied from the author rows %d-%d)' % (y0, y1, kind, a, b))

    # --- the way out sits on whatever water the bottom row actually has -------------
    water = [x for x in range(w)
             if ((rows[EXIT_ROW][x] >> 10) & 3) == 0 and ((rows[EXIT_ROW][x] >> 12) & 15) == 1]
    mp = os.path.join(REPO, 'data/maps/OrangeIslands_Trovita_Falls/map.json')
    mj = json.load(open(mp))
    mj['coord_events'] = [c for c in mj['coord_events']
                          if 'WalkToTrovita' not in (c.get('script') or '')]
    for x in water:
        mj['coord_events'].append({"type": "trigger", "x": x, "y": EXIT_ROW, "elevation": 0,
                                   "var": "VAR_TEMP_0", "var_value": "0",
                                   "script": "OrangeIslands_Trovita_Falls_EventScript_WalkToTrovita"})
    with open(mp, 'w', newline='\n') as f:
        json.dump(mj, f, indent=2); f.write('\n')
    print('  way out: row %d, x %d-%d' % (EXIT_ROW, water[0], water[-1]))

    sp = os.path.join(REPO, 'data/maps/OrangeIslands_Trovita/scripts.inc')
    s = open(sp, encoding='utf-8').read()
    import re
    s2 = re.sub(r'warp MAP_ORANGE_ISLANDS_TROVITA_FALLS, \d+, \d+',
                'warp MAP_ORANGE_ISLANDS_TROVITA_FALLS, %d, %d' % ENTRY, s)
    if s2 != s:
        open(sp, 'w', encoding='utf-8', newline='\n').write(s2)
    print('  Trovita lands the player at %s' % (ENTRY,))


if __name__ == '__main__':
    main()
