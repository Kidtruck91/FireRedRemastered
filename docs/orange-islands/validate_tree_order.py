"""Check that tiled scenery -- forest canopy above all -- is laid out in the
right order, and that it lines up with the map's border.

Scenery like the Berry Forest canopy is not one metatile repeated: it is a
block of them that has to tile in a fixed order. Route 2's is three wide and
two tall:

      0x290 0x291 0x292
      0x298 0x299 0x29A

Put one of those six in the wrong place and the map still loads and still
looks like trees, but the canopy has a seam in it. By eye, in six thousand
tiles, that is close to invisible.

The block does not have to be guessed at: a map's border.bin *is* that block,
since the border is what the game repeats outside the map edge. So for every
layout this reads the border, works out each metatile's place in the block,
and then checks two things.

  order   Every two neighbouring tiles that are both part of the block must be
          neighbours in the block too, wrapping at its edges. Right of column
          c comes column (c+1) mod width; below row r comes row (r+1) mod
          height. This is phase-independent, so a clump of trees sitting at
          any offset is fine as long as it is internally consistent.

  seam    The border repeats outside the map aligned to the map's origin
          (GetBorderBlockAt in src/fieldmap.c takes the map coordinate modulo
          the border size), so a block tile on the outermost row or column has
          only one phase that lines up with it: (y mod height, x mod width)
          must be its place in the block.

          This one is reported but never failed on. Vanilla breaks it in
          sixteen maps -- Route 15, the Safari Zone, Saffron -- and mostly
          gets away with it, because a border of generic water or grass looks
          the same at any phase. It is worth looking at when the border is
          something with structure, like trees.

Layouts whose border repeats a metatile (a solid border of one tile, say)
carry no order to check and are skipped.

    python3 docs/orange-islands/validate_tree_order.py              # every layout
    python3 docs/orange-islands/validate_tree_order.py LAYOUT_...   # just these
    python3 docs/orange-islands/validate_tree_order.py --quiet      # counts only

Exit status is 1 if anything failed.
"""
import json
import os
import struct
import sys

REPO = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
MAX_REPORTED = 12


def load_block(layout):
    """The border as {metatile: (row, col)}, or None if it carries no order."""
    bw, bh = layout.get('border_width', 2), layout.get('border_height', 2)
    path = os.path.join(REPO, layout['border_filepath'])
    if not os.path.exists(path):
        return None, bw, bh
    raw = open(path, 'rb').read()
    if len(raw) < bw * bh * 2:
        return None, bw, bh
    block = {}
    for r in range(bh):
        for c in range(bw):
            m = struct.unpack_from('<H', raw, (r * bw + c) * 2)[0] & 0x3FF
            if m in block:
                return None, bw, bh      # repeats, so there is no order to check
            block[m] = (r, c)
    if len(block) < 2:
        return None, bw, bh
    return block, bw, bh


def check_layout(layout):
    """Returns (order_problems, seam_problems, tiles_checked) or None if skipped."""
    block, bw, bh = load_block(layout)
    if block is None:
        return None
    w, h = layout['width'], layout['height']
    path = os.path.join(REPO, layout['blockdata_filepath'])
    if not os.path.exists(path):
        return None
    raw = open(path, 'rb').read()
    if len(raw) < w * h * 2:
        return None

    def at(x, y):
        return struct.unpack_from('<H', raw, 2 * (y * w + x))[0] & 0x3FF

    order, seam, n = [], [], 0
    for y in range(h):
        for x in range(w):
            m = at(x, y)
            if m not in block:
                continue
            n += 1
            r, c = block[m]
            # order, rightwards and downwards; each pair is checked once
            if x + 1 < w:
                o = at(x + 1, y)
                if o in block:
                    r2, c2 = block[o]
                    if r2 != r or c2 != (c + 1) % bw:
                        order.append((x, y, m, x + 1, y, o, 'right'))
            if y + 1 < h:
                o = at(x, y + 1)
                if o in block:
                    r2, c2 = block[o]
                    if c2 != c or r2 != (r + 1) % bh:
                        order.append((x, y, m, x, y + 1, o, 'below'))
            # seam against the border, which repeats from the map origin
            if x == 0 or y == 0 or x == w - 1 or y == h - 1:
                if (y % bh, x % bw) != (r, c):
                    seam.append((x, y, m, (r, c), (y % bh, x % bw)))
    return order, seam, n


def main(argv):
    quiet = '--quiet' in argv
    wanted = {a for a in argv if not a.startswith('--')}
    layouts = [l for l in json.load(
        open(os.path.join(REPO, 'data/layouts/layouts.json')))['layouts'] if 'id' in l]

    checked = skipped = 0
    bad_order = bad_seam = 0
    for l in layouts:
        if wanted and l['id'] not in wanted:
            continue
        res = check_layout(l)
        if res is None:
            skipped += 1
            continue
        order, seam, n = res
        checked += 1
        bad_order += len(order)
        bad_seam += len(seam)
        if not order and not (seam and wanted):
            continue
        print('%s  (%d block tiles)%s' % (l['id'], n,
              '' if wanted or not seam else '  [%d seam notes]' % len(seam)))
        for x, y, m, x2, y2, m2, which in order[:MAX_REPORTED]:
            print('    order: (%d,%d) 0x%03X has 0x%03X %s it at (%d,%d) -- '
                  'not the next tile in the block'
                  % (x, y, m, m2, which, x2, y2))
        if len(order) > MAX_REPORTED:
            print('    ... and %d more order problems' % (len(order) - MAX_REPORTED))
        for x, y, m, want, got in (seam[:MAX_REPORTED] if wanted else []):
            print('    seam:  (%d,%d) 0x%03X sits at phase %s but belongs at %s, '
                  'so it does not line up with the border'
                  % (x, y, m, got, want))
        if wanted and len(seam) > MAX_REPORTED:
            print('    ... and %d more seam notes' % (len(seam) - MAX_REPORTED))

    print('\nchecked %d layouts, skipped %d (border carries no order)' % (checked, skipped))
    print('order problems: %d' % bad_order)
    print('border seams (advisory, vanilla has these too): %d' % bad_seam)
    return 1 if bad_order else 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
