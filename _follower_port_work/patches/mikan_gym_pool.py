#!/usr/bin/env python3
"""Mikan Gym, stages 2 and 3: give the other two platforms a reason to exist.

The bottom maze (rows 22-28) is untouched. It still delivers you to platform A at
(2,23). What changes is the main pool above it, rows 7..21, which is currently one
big sheet of open water: once you clear the bottom maze you can swim anywhere,
including straight to the row 6 funnel. Marco's and Coral's platforms are just
islands you pass.

This paints the pool as two more stages built from the same funnel idiom the row 6
lane-choice already uses -- a row that sweeps you out to the side chutes except at
one column -- so the climb is A -> C -> B -> lane -> Cissy, with a swimmer guarding
each landing. Only the currents the map already uses are painted (0x1EC..0x1EF),
and the platforms, the chutes, row 6 and everything below row 21 are left alone.
"""
import json, struct, sys, os
sys.path.insert(0, '/home/claude/fr2/docs/orange-islands')

NORTH, SOUTH, WEST, EAST = 0x1EC, 0x1ED, 0x1EE, 0x1EF
STILL = 0x291
# Keep each tile's own collision and elevation bits: the pool is elevation 1 and
# repainting it to anything else makes every tile "surf water at walking height".
HI_MASK = 0xFC00

# rows 7..21, columns x=2..21. '.' = leave this tile exactly as it is.
PLAN = """\
~~~~~~~~~~~~~~~~~~~~
~~~~~~~~~~~~~~~~~~~~
..vvvvvvvvvvvvvvvvvv
..vvvvvvvvvvvvvvvvvv
~~~~vvvvvvvvvvvvvvvv
vvv^vvvvvvvvvvvvvvvv
vvv^vvvvvvvvvvvvvvvv
vvv^vvvvvvvvvvvvvvvv
<<<^>>>>>>>>>>>>>>>>
~~~~~~~~~~~~~~~~~~~~
vvvvvvvvvvvvvvvvvv..
vvvvvvvvvvvvvvvvvv..
vvvvvvvvvvvvvvvvvv^v
<<<<<<<<<<<<<<<<<<^v
~~~~~~~~~~~~~~~~~~~~
"""
CH = {'v': SOUTH, '^': NORTH, '<': WEST, '>': EAST, '~': STILL}
Y0, X0 = 7, 2


def main(repo):
    L = {l['id']: l for l in json.load(open(repo + '/data/layouts/layouts.json'))['layouts']}
    lay = L[json.load(open(repo + '/data/maps/OrangeIslands_Mikan_Gym/map.json'))['layout']]
    path = os.path.join(repo, lay['blockdata_filepath'])
    blocks = [v[0] for v in struct.iter_unpack('<H', open(path, 'rb').read())]
    w = lay['width']
    rows = PLAN.rstrip('\n').split('\n')
    n = 0
    for dy, row in enumerate(rows):
        for dx, c in enumerate(row):
            if c == '.':
                continue
            x, y = X0 + dx, Y0 + dy
            old = blocks[y * w + x]
            new = CH[c] | (old & HI_MASK)
            if old != new:
                blocks[y * w + x] = new
                n += 1
    open(path, 'wb').write(b''.join(struct.pack('<H', v) for v in blocks))
    print('mikan pool: %d tiles repainted in rows %d-%d' % (n, Y0, Y0 + len(rows) - 1))


if __name__ == '__main__':
    main(sys.argv[1] if len(sys.argv) > 1 else '/home/claude/fr2')
