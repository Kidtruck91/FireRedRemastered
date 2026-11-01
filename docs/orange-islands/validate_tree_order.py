"""Check that tiled scenery -- forest canopy above all -- is laid out in the
right order.

A tree in the Berry Forest tileset is not one metatile repeated. It is three
columns wide, and each column is a sequence running bottom-up from a wooden
base:

    left            middle          right
    ...  298        ...  291        ...  29A
         290             299             292
         2A3             2A1             2A5     <- 290 / 2A1 / 292 row
                         2A4                     <- 3-tile base: 2A3 2A4 2A5

Get it wrong and the map still loads and still looks like trees. Canopy with
no trunk under it, or a base row with a piece missing, reads as a seam rather
than as an obvious error, and in six thousand tiles it is close to invisible.

Three checks, all learned from the maps rather than hardcoded, so this works
for any tileset and not only this one.

  pairs   Every two neighbouring scenery tiles must be a pair that some other
          map built on the same tilesets also uses. This catches a tile put
          next to one it never belongs beside.

  ends    The tile a vertical run of scenery starts and ends on must be one
          that other maps also start and end runs on. This is the check that
          catches canopy simply stopping with no trunk or base under it --
          the pairs inside such a column are all perfectly legal, and only
          the way it ends gives it away. Runs that touch the top or bottom
          edge of the map are cut off there, so their ends are not judged.

  border  A map's border.bin is the block the game repeats outside the edge.
          Where that block holds several different metatiles they have a
          fixed order, and neighbouring tiles in the map that both belong to
          it must be neighbours in the block too, wrapping at its edges.

The reference for the first two is every other layout sharing both tilesets.
That makes this a tool to point at one map, not a gate for the whole repo:
where a tileset has only one or two maps on it the reference is too thin to
tell a mistake from something simply not done before, and a sweep of every
layout reports tens of thousands of those. Name the layouts you care about.

    python3 docs/orange-islands/validate_tree_order.py LAYOUT_ORANGE_ISLANDS_ROUTE_2
    python3 docs/orange-islands/validate_tree_order.py           # survey, advisory

Exit status is 1 only when layouts were named and something failed.
"""
import collections
import json
import os
import struct
import sys

REPO = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
MAX_REPORTED = 10

# Scenery is taken to be the secondary tileset's metatiles plus anything in
# the border block: that is where multi-tile trees, cliffs and the like live.
# Water is excluded -- it is in the secondary set on some maps but it is a
# surface, not a structure, and has no run ends to speak of.
SECONDARY_BASE = 640
WATER_BEHAVIOURS = {
    0x02, 0x03, 0x04, 0x05, 0x06, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16,
    0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22,
}


def tileset_dir(label):
    # Directory names split on digit runs as well as capitals, so
    # gTileset_SeviiIslands45Condensed is sevii_islands_45_condensed.
    # Match by comparing with the underscores removed rather than trying to
    # reproduce the rule.
    key = label[len('gTileset_'):].lower() if label else ''
    for sub in ('primary', 'secondary'):
        d = os.path.join(REPO, 'data/tilesets', sub)
        if not os.path.isdir(d):
            continue
        for name in os.listdir(d):
            if name.replace('_', '') == key:
                p = os.path.join(d, name, 'metatile_attributes.bin')
                if os.path.exists(p):
                    return p
    return None


_water_cache = {}


def water_metatiles(l):
    """Secondary metatile ids on this layout whose behaviour is a water one."""
    label = l.get('secondary_tileset')
    if label in _water_cache:
        return _water_cache[label]
    out = set()
    p = tileset_dir(label) if label else None
    if p:
        a = open(p, 'rb').read()
        for i in range(len(a) // 4):
            if (struct.unpack_from('<I', a, i * 4)[0] & 0x1FF) in WATER_BEHAVIOURS:
                out.add(SECONDARY_BASE + i)
    _water_cache[label] = out
    return out


def layouts():
    return [l for l in json.load(
        open(os.path.join(REPO, 'data/layouts/layouts.json')))['layouts'] if 'id' in l]


def read_grid(l):
    w, h = l['width'], l['height']
    p = os.path.join(REPO, l['blockdata_filepath'])
    if not os.path.exists(p):
        return None
    raw = open(p, 'rb').read()
    if len(raw) < w * h * 2:
        return None
    return [[struct.unpack_from('<H', raw, 2 * (y * w + x))[0] & 0x3FF
             for x in range(w)] for y in range(h)], w, h


def read_block(l):
    """border.bin as {metatile: (row, col)}, or None when it carries no order."""
    bw, bh = l.get('border_width', 2), l.get('border_height', 2)
    p = os.path.join(REPO, l['border_filepath'])
    if not os.path.exists(p):
        return None, bw, bh
    raw = open(p, 'rb').read()
    if len(raw) < bw * bh * 2:
        return None, bw, bh
    block = {}
    for r in range(bh):
        for c in range(bw):
            m = struct.unpack_from('<H', raw, (r * bw + c) * 2)[0] & 0x3FF
            if m in block:
                return None, bw, bh
            block[m] = (r, c)
    return (block if len(block) > 1 else None), bw, bh


def scenery_set(l, block):
    s = set(block) if block else set()
    return s, SECONDARY_BASE, water_metatiles(l)


def survey(l):
    """What this layout shows: allowed pairs, and run ends."""
    g = read_grid(l)
    if g is None:
        return None
    grid, w, h = g
    block, _, _ = read_block(l)
    known, base, wet = scenery_set(l, block)
    is_s = lambda m: (m >= base or m in known) and m not in wet
    V, H, tops, bottoms = set(), set(), set(), set()
    for y in range(h):
        for x in range(w):
            m = grid[y][x]
            if not is_s(m):
                continue
            if y + 1 < h and is_s(grid[y + 1][x]):
                V.add((m, grid[y + 1][x]))
            if x + 1 < w and is_s(grid[y][x + 1]):
                H.add((m, grid[y][x + 1]))
    for x in range(w):
        y = 0
        while y < h:
            if not is_s(grid[y][x]):
                y += 1
                continue
            s = y
            while y < h and is_s(grid[y][x]):
                y += 1
            if s > 0:
                tops.add(grid[s][x])
            if y < h:
                bottoms.add(grid[y - 1][x])
    return V, H, tops, bottoms


def main(argv):
    wanted = {a for a in argv if not a.startswith('--')}
    ls = layouts()
    by_tileset = collections.defaultdict(list)
    for l in ls:
        by_tileset[(l.get('primary_tileset'), l.get('secondary_tileset'))].append(l)

    surveys = {}
    for l in ls:
        s = survey(l)
        if s:
            surveys[l['id']] = s

    checked = skipped = 0
    n_pair = n_end = n_border = 0
    for l in ls:
        if wanted and l['id'] not in wanted:
            continue
        g = read_grid(l)
        if g is None:
            continue
        grid, w, h = g
        block, bw, bh = read_block(l)
        known, base, wet = scenery_set(l, block)
        is_s = lambda m: (m >= base or m in known) and m not in wet

        siblings = [o for o in by_tileset[(l.get('primary_tileset'), l.get('secondary_tileset'))]
                    if o['id'] != l['id'] and o['id'] in surveys]
        pair_problems, end_problems, border_problems = [], [], []

        if siblings:
            V = set().union(*(surveys[o['id']][0] for o in siblings))
            H = set().union(*(surveys[o['id']][1] for o in siblings))
            T = set().union(*(surveys[o['id']][2] for o in siblings))
            B = set().union(*(surveys[o['id']][3] for o in siblings))
            for y in range(h):
                for x in range(w):
                    m = grid[y][x]
                    if not is_s(m):
                        continue
                    if y + 1 < h and is_s(grid[y + 1][x]) and (m, grid[y + 1][x]) not in V:
                        pair_problems.append((x, y, m, grid[y + 1][x], 'above'))
                    if x + 1 < w and is_s(grid[y][x + 1]) and (m, grid[y][x + 1]) not in H:
                        pair_problems.append((x, y, m, grid[y][x + 1], 'left of'))
            for x in range(w):
                y = 0
                while y < h:
                    if not is_s(grid[y][x]):
                        y += 1
                        continue
                    s = y
                    while y < h and is_s(grid[y][x]):
                        y += 1
                    if s > 0 and grid[s][x] not in T:
                        end_problems.append((x, s, grid[s][x], 'starts'))
                    if y < h and grid[y - 1][x] not in B:
                        end_problems.append((x, y - 1, grid[y - 1][x], 'ends'))
            checked += 1
        else:
            skipped += 1

        if block:
            for y in range(h):
                for x in range(w):
                    m = grid[y][x]
                    if m not in block:
                        continue
                    r, c = block[m]
                    if x + 1 < w and grid[y][x + 1] in block:
                        r2, c2 = block[grid[y][x + 1]]
                        if r2 != r or c2 != (c + 1) % bw:
                            border_problems.append((x, y, m, grid[y][x + 1], 'right'))
                    if y + 1 < h and grid[y + 1][x] in block:
                        r2, c2 = block[grid[y + 1][x]]
                        if c2 != c or r2 != (r + 1) % bh:
                            border_problems.append((x, y, m, grid[y + 1][x], 'below'))

        n_pair += len(pair_problems)
        n_end += len(end_problems)
        n_border += len(border_problems)
        if not (pair_problems or end_problems or border_problems):
            continue
        print('%s' % l['id'])
        for x, y, m, m2, rel in pair_problems[:MAX_REPORTED]:
            print('    pair:   (%d,%d) 0x%03X is %s 0x%03X -- no other map on these '
                  'tilesets puts them together' % (x, y, m, rel, m2))
        if len(pair_problems) > MAX_REPORTED:
            print('    ... and %d more' % (len(pair_problems) - MAX_REPORTED))
        for x, y, m, which in end_problems[:MAX_REPORTED]:
            print('    end:    (%d,%d) a column of scenery %s on 0x%03X -- no other map '
                  'on these tilesets ends one that way (missing trunk or base?)'
                  % (x, y, which, m))
        if len(end_problems) > MAX_REPORTED:
            print('    ... and %d more' % (len(end_problems) - MAX_REPORTED))
        for x, y, m, m2, rel in border_problems[:MAX_REPORTED]:
            print('    border: (%d,%d) 0x%03X has 0x%03X %s it -- not the next tile in '
                  'the border block' % (x, y, m, m2, rel))
        if len(border_problems) > MAX_REPORTED:
            print('    ... and %d more' % (len(border_problems) - MAX_REPORTED))

    print('\nchecked %d layouts against their siblings, %d had none to compare with'
          % (checked, skipped))
    print('pair problems: %d' % n_pair)
    print('run-end problems: %d' % n_end)
    print('border-block order problems: %d' % n_border)
    if not wanted:
        print('(survey of every layout: most of these are tilesets with too few\n'
              ' other maps to compare against, not real mistakes. Name a layout\n'
              ' to get an answer worth acting on.)')
        return 0
    return 1 if (n_pair or n_end or n_border) else 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
