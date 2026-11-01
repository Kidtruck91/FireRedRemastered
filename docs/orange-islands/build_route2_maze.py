"""Rebuild Route 2 as a forest maze, three tiles wide throughout, ending at
the pond.

The canopy is not a flat wallpaper. A tree is three tiles wide, and each of
its three columns is its own sequence running bottom-up from a wooden base:

    left            middle          right
    ...  298        ...  291        ...  29A
         290             299             292
         298             291             29A
         290             299             292       <- alternating upwards
         2A3             2A1             2A5       <- 290 / 2A1 / 292 row
                         2A4                       <- the 3-tile wooden base
                                                      is 2A3 2A4 2A5

Read bottom-up it is deterministic: the base row is 2A3 2A4 2A5, the row above
it is 290 2A1 292 -- the middle column's single trunk tile -- and above that
each column alternates its own pair. At the top, a column whose last tile
comes out as 298 / 291 / 29A may instead take the rounded cap 288 / 281 / 28A.
Every sequence this emits was checked against the ones in the hand-painted
map it replaces.

That fixes the lattice. Trees are three wide, so every wall is three wide and
starts on a multiple of three; paths are three wide to match. Vertically the
bands are anchored to the bottom of the map so that a tree's base always lands
on a band boundary, whatever the map height.

    python3 docs/orange-islands/build_route2_maze.py [seed]

docs/orange-islands/validate_tree_order.py checks the result.
"""
import json
import os
import random
import struct
import sys

REPO = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
LAYOUT = 'LAYOUT_ORANGE_ISLANDS_ROUTE_2'

GRASS = 0x3009
TALL_GRASS = 0x300D
COLL = 1 << 10            # trees block; collision 1, elevation 0

# column 0 (left), 1 (middle), 2 (right) of a tree
BASE = (0x2A3, 0x2A4, 0x2A5)      # the wooden base row
ABOVE_BASE = (0x290, 0x2A1, 0x292)  # 2A1 is the middle's single trunk tile
ALT = ((0x298, 0x290), (0x299, 0x291), (0x29A, 0x292))  # upwards from there
# The outer columns may cap a top 298 / 29A with a rounded 288 / 28A. The
# middle's cap, 281, is not one of these: in the hand-painted map it never
# sits beside 290 or 292, but one row higher, alone in the grass above the
# middle of a capped tree, with 299 below it. It is a walkable overlay
# (block 0x3281), and it is placed separately once the trees are down.
CAP = (0x288, None, 0x28A)
CAPPABLE = (0x298, None, 0x29A)
MID_CAP = 0x3281
MIN_RUN = 3                       # the original has no tree run shorter than this

BAND = 3                  # paths and walls are both three tiles
# The pond, with a clearing round it, rounded out to whole bands so no
# part-height tree slivers can form against its edge. Water is kept from the
# old map tile for tile; the rest of the region is plain grass.
POND = (21, 22, 35, 33)
WATER = {0x1A3, 0x1A4, 0x1A5, 0x1A6, 0x1A7, 0x1AB, 0x1AC, 0x1AD, 0x1AE,
         0x1AF, 0x1B3, 0x1B4, 0x222, 0x223, 0x224, 0x225, 0x226, 0x2AA}


def tree_column(col, height):
    """The metatiles of one tree column, top-down, for a run of `height`."""
    out = [BASE[col]]
    if height >= 2:
        out.append(ABOVE_BASE[col])
    a, bnd = ALT[col]
    for i in range(height - 2):
        out.append(a if i % 2 == 0 else bnd)
    out.reverse()
    if CAP[col] is not None and out[0] == CAPPABLE[col]:
        out[0] = CAP[col]
    return out


def main(seed=11):
    rnd = random.Random(seed)
    L = {l['id']: l for l in json.load(
        open(os.path.join(REPO, 'data/layouts/layouts.json')))['layouts'] if 'id' in l}
    l = L[LAYOUT]
    w, h = l['width'], l['height']
    path = os.path.join(REPO, l['blockdata_filepath'])
    old = open(path, 'rb').read()
    cur = lambda x, y: struct.unpack_from('<H', old, 2 * (y * w + x))[0]

    ncols = w // BAND                       # 24 column bands of three tiles
    # Bands are anchored to the bottom so a tree's base lands on a boundary.
    nrows = h // BAND                       # 29 row bands; h % BAND rows spare on top
    spare = h % BAND
    bx = lambda c: c * BAND
    by = lambda r: spare + r * BAND

    # corridors on odd column bands, walls on even ones, so both map edges are
    # trees; the last band is left as wall when the count comes out even
    corridor_cols = [c for c in range(ncols) if c % 2 == 1 and c != ncols - 1]
    # The bottom of the map is kept out of the maze: the way in from Valencia
    # needs a corridor of its own down to the warp tiles at y=82. Without
    # that, the maze's own bottom row would run along y=82 and the warp would
    # sit in the middle of a long open row, to be stepped on by accident.
    WAY_IN_TOP = 79
    corridor_rows = [r for r in range(nrows)
                     if r % 2 == 1 and by(r) + BAND - 1 < WAY_IN_TOP]

    px0, py0, px1, py1 = POND

    def overlaps_pond(c, r):
        x0, y0 = bx(c), by(r)
        return not (x0 + BAND - 1 < px0 or x0 > px1
                    or y0 + BAND - 1 < py0 or y0 > py1)

    # The clearing is bounded by wall bands on three sides; only the row of
    # bands directly below it is a corridor row, so that row is where it can
    # be entered. All of those bands but one are held back as trees, which
    # leaves the clearing exactly one way in and keeps the maze's single
    # route to the pond intact.
    pond_rows = [r for r in range(nrows) if any(overlaps_pond(c, r) for c in range(ncols))]
    pond_cols = [c for c in range(ncols) if any(overlaps_pond(c, r) for r in range(nrows))]
    door_row = max(pond_rows) + 1
    door_col = [c for c in pond_cols if c in corridor_cols][len(
        [c for c in pond_cols if c in corridor_cols]) // 2]

    def reserved(c, r):
        if overlaps_pond(c, r):
            return True
        if r == door_row and c in pond_cols and c != door_col:
            return True
        return False

    cells = [(ci, ri) for ci in range(len(corridor_cols))
             for ri in range(len(corridor_rows))
             if not reserved(corridor_cols[ci], corridor_rows[ri])]
    cellset = set(cells)
    print('  lattice %d x %d cells (%d bands of %d tiles), %d usable'
          % (len(corridor_cols), len(corridor_rows), ncols, BAND, len(cells)))

    # --- the goal: the cell whose band sits just under the pond -------------
    goal = (corridor_cols.index(door_col), corridor_rows.index(door_row))
    assert goal in cellset, 'the way into the pond clearing is not a maze cell'
    entrance = max((c for c in cells if bx(corridor_cols[c[0]]) <= 27 <= bx(corridor_cols[c[0]]) + 2),
                   key=lambda c: c[1])
    print('  pond clearing x %d-%d y %d-%d, its one door at x=%d y=%d'
          % (px0, px1, py0, py1, bx(door_col), by(door_row)))
    print('  entrance cell at x=%d y=%d'
          % (bx(corridor_cols[entrance[0]]), by(corridor_rows[entrance[1]])))

    # --- perfect maze, grown from the goal ----------------------------------
    seen = {goal}
    stack = [goal]
    links = []
    while stack:
        ci, ri = stack[-1]
        nb = [n for n in ((ci + 1, ri), (ci - 1, ri), (ci, ri + 1), (ci, ri - 1))
              if n in cellset and n not in seen]
        if not nb:
            stack.pop()
            continue
        n = rnd.choice(nb)
        seen.add(n)
        links.append(((ci, ri), n))
        stack.append(n)
    assert entrance in seen, 'the way in from Valencia is not connected'
    print('  maze reaches %d of %d cells' % (len(seen), len(cells)))

    # --- carve, on the band grid --------------------------------------------
    walk = [[False] * w for _ in range(h)]

    def open_band(c, r):
        for y in range(by(r), by(r) + BAND):
            for x in range(bx(c), bx(c) + BAND):
                if 0 <= x < w and 0 <= y < h:
                    walk[y][x] = True

    for (ci, ri) in seen:
        open_band(corridor_cols[ci], corridor_rows[ri])
    for a, b in links:
        (c1, r1), (c2, r2) = a, b
        if r1 == r2:
            open_band(corridor_cols[min(c1, c2)] + 1, corridor_rows[r1])
        else:
            open_band(corridor_cols[c1], corridor_rows[min(r1, r2)] + 1)

    # the way in from Valencia: its own corridor below the maze, so the warp
    # at y=82 cannot be wandered onto from a long open bottom row
    ex = bx(corridor_cols[entrance[0]])
    for y in range(by(corridor_rows[entrance[1]]), 83):  # down to the warp row
        for x in range(ex, ex + BAND):
            walk[y][x] = True
    print('  way in from Valencia at x %d-%d, down to y=82' % (ex, ex + BAND - 1))

    # --- paint ---------------------------------------------------------------
    grid = [[0] * w for _ in range(h)]
    in_pond = lambda x, y: px0 <= x <= px1 and py0 <= y <= py1
    for y in range(h):
        for x in range(w):
            if in_pond(x, y):
                # water exactly as it was painted; everything else a clearing
                grid[y][x] = cur(x, y) if (cur(x, y) & 0x3FF) in WATER else GRASS
            elif walk[y][x]:
                grid[y][x] = GRASS

    # trees, one three-column group at a time, bottom-up per vertical run
    emitted = []
    for c in range(ncols):
        x0 = bx(c)
        y = h - 1
        while y >= 0:
            solid = lambda yy: (not walk[yy][x0] and not in_pond(x0, yy)
                                and not walk[yy][min(x0 + 2, w - 1)]
                                and not in_pond(x0 + 2, yy))
            if not solid(y):
                y -= 1
                continue
            bottom = y
            while y >= 0 and solid(y):
                y -= 1
            top = y + 1
            height = bottom - top + 1
            # An exposed top only takes the rounded cap when the alternation
            # lands on 298 / 29A there, which depends on the run's height.
            # When it does not, drop the top row to grass: that flips the
            # parity, so the tree gets its cap instead of the canopy simply
            # starting mid-pattern. Runs that reach the top of the map are
            # cut off rather than exposed, so they are left alone.
            if (top > 0 and height - 1 >= MIN_RUN
                    and tree_column(0, height)[0] != CAP[0]):
                for col in range(BAND):
                    if not in_pond(x0 + col, top):
                        grid[top][x0 + col] = GRASS
                top += 1
                height -= 1
            for col in range(BAND):
                seq = tree_column(col, height)
                for k, mt in enumerate(seq):
                    grid[top + k][x0 + col] = mt | COLL
            emitted.append(height)
            # the middle's rounded top, one row above a capped tree
            if (CAP[0] is not None and height >= MIN_RUN
                    and grid[top][x0] == (CAP[0] | COLL)
                    and top > 0 and grid[top - 1][x0 + 1] == GRASS):
                grid[top - 1][x0 + 1] = MID_CAP
    print('  %d tree masses, heights %s'
          % (len(emitted), sorted(set(emitted))))

    # --- tall grass ----------------------------------------------------------
    patches = [c for c in seen if c not in (entrance, goal)]
    rnd.shuffle(patches)
    n = 0
    for (ci, ri) in patches[:16]:
        for y in range(by(corridor_rows[ri]), by(corridor_rows[ri]) + BAND):
            for x in range(bx(corridor_cols[ci]), bx(corridor_cols[ci]) + BAND):
                if grid[y][x] == GRASS:
                    grid[y][x] = TALL_GRASS
                    n += 1
    print('  %d tall grass tiles' % n)

    out = b''.join(struct.pack('<H', grid[y][x]) for y in range(h) for x in range(w))
    assert len(out) == len(old)
    open(path, 'wb').write(out)
    print('  written: %d walkable tiles of %d'
          % (sum(1 for y in range(h) for x in range(w)
                 if ((grid[y][x] >> 10) & 3) == 0), w * h))


if __name__ == '__main__':
    main(int(sys.argv[1]) if len(sys.argv) > 1 else 11)
