"""Rebuild Route 2 as a forest maze whose one solution ends at the pond.

The canopy is a three-wide, two-tall wallpaper (the six metatiles that make up
the map's border). It has no edge pieces -- a tree mass simply stops -- so as
long as every canopy tile is written at its own phase, metatile
0x290 + (y % 2) * 8 + (x % 3), the tile order is right everywhere by
construction. docs/orange-islands/validate_tree_order.py then confirms it.

The maze is a perfect maze: exactly one route between any two points, so the
way in from Valencia reaches the pond by a single path, with everything else a
dead end. The pond and the ring of shore around it are copied over from the
current map rather than generated, so the water the author painted, and the
dive tiles in it, are untouched.
"""
import json
import os
import random
import struct

REPO = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
LAYOUT = 'LAYOUT_ORANGE_ISLANDS_ROUTE_2'

GRASS = 0x3009        # plain floor, collision 0, elevation 3
TALL_GRASS = 0x300D   # encounter grass
CANOPY_BASE = 0x290

# The pond plus a tile of shore, copied verbatim from the current map.
POND = (21, 22, 33, 31)        # x0, y0, x1, y1 inclusive

# Maze lattice: corridors two tiles wide, walls three thick.
PITCH = 5
OX, OY = 2, 1
COLS, ROWS = 14, 16            # i: x = OX+5i, OX+5i+1   j: y = OY+5j, OY+5j+1

ENTRANCE = (5, 15)             # x 27-28, y 76-77: the maze mouth above the way in
GOAL = (5, 7)                  # the cell the pond's south shore opens onto


def canopy(x, y):
    return (CANOPY_BASE + (y % 2) * 8 + (x % 3)) | (1 << 10)


def cell_tiles(i, j):
    x, y = OX + PITCH * i, OY + PITCH * j
    return [(x + dx, y + dy) for dy in range(2) for dx in range(2)]


def wall_tiles(a, b):
    """The tiles between two neighbouring cells."""
    (i1, j1), (i2, j2) = a, b
    x1, y1 = OX + PITCH * i1, OY + PITCH * j1
    out = []
    if i1 == i2:                       # vertical neighbour
        top = min(j1, j2)
        y = OY + PITCH * top + 2
        for dy in range(3):
            for dx in range(2):
                out.append((x1 + dx, y + dy))
    else:                              # horizontal neighbour
        left = min(i1, i2)
        x = OX + PITCH * left + 2
        for dx in range(3):
            for dy in range(2):
                out.append((x + dx, y1 + dy))
    return out


def main(seed=7):
    rnd = random.Random(seed)
    L = {l['id']: l for l in json.load(
        open(os.path.join(REPO, 'data/layouts/layouts.json')))['layouts'] if 'id' in l}
    l = L[LAYOUT]
    w, h = l['width'], l['height']
    path = os.path.join(REPO, l['blockdata_filepath'])
    old = open(path, 'rb').read()
    cur = lambda x, y: struct.unpack_from('<H', old, 2 * (y * w + x))[0]

    grid = [[canopy(x, y) for x in range(w)] for y in range(h)]

    # --- which cells the pond sits on; the maze is carved around them --------
    px0, py0, px1, py1 = POND
    blocked = set()
    for j in range(ROWS):
        for i in range(COLS):
            for (x, y) in cell_tiles(i, j):
                if px0 - 1 <= x <= px1 + 1 and py0 - 1 <= y <= py1 + 1:
                    blocked.add((i, j))
    print('  %d of %d cells sit on the pond and are left out of the maze'
          % (len(blocked), COLS * ROWS))

    # --- perfect maze over the rest, grown from the goal --------------------
    assert GOAL not in blocked and ENTRANCE not in blocked
    seen = {GOAL}
    stack = [GOAL]
    carved = [GOAL]
    links = []
    while stack:
        i, j = stack[-1]
        nbrs = []
        for di, dj in ((1, 0), (-1, 0), (0, 1), (0, -1)):
            n = (i + di, j + dj)
            if (0 <= n[0] < COLS and 0 <= n[1] < ROWS
                    and n not in seen and n not in blocked):
                nbrs.append(n)
        if not nbrs:
            stack.pop()
            continue
        n = rnd.choice(nbrs)
        seen.add(n)
        links.append(((i, j), n))
        carved.append(n)
        stack.append(n)
    print('  maze reaches %d of %d free cells' % (len(seen), COLS * ROWS - len(blocked)))
    assert ENTRANCE in seen, 'the way in from Valencia is not connected'

    for c in carved:
        for (x, y) in cell_tiles(c[0], c[1]):
            grid[y][x] = GRASS
    for a, b in links:
        for (x, y) in wall_tiles(a, b):
            grid[y][x] = GRASS

    # --- the way in from Valencia -------------------------------------------
    # The coord_events sit at (27,82), (28,82) and (29,82). Give them a
    # corridor of their own below the maze rather than letting the bottom row
    # of cells run along y=82, which would put the warp to Valencia in the
    # middle of an open row for the player to wander onto by accident.
    for y in range(OY + PITCH * ENTRANCE[1], 83):
        for x in (27, 28, 29):
            grid[y][x] = GRASS

    # --- the pond and its shore, straight from the current map --------------
    for y in range(py0, py1 + 1):
        for x in range(px0, px1 + 1):
            grid[y][x] = cur(x, y)

    # --- open the goal cell into the pond's shore ---------------------------
    gx = OX + PITCH * GOAL[0]
    for y in range(py1, OY + PITCH * GOAL[1] + 2):
        for x in (gx, gx + 1):
            if not (px0 <= x <= px1 and py0 <= y <= py1):
                grid[y][x] = GRASS
    print('  pond shore opened at x %d-%d, y %d-%d' % (gx, gx + 1, py1, OY + PITCH * GOAL[1] + 1))

    # --- tall grass, for the encounters -------------------------------------
    # A few whole cells rather than scattered tiles, so patches read clearly.
    patches = [c for c in carved if c not in (ENTRANCE, GOAL)]
    rnd.shuffle(patches)
    n_tall = 0
    for c in patches[:18]:
        for (x, y) in cell_tiles(c[0], c[1]):
            if grid[y][x] == GRASS:
                grid[y][x] = TALL_GRASS
                n_tall += 1
    print('  %d tall grass tiles in 18 patches' % n_tall)

    out = b''.join(struct.pack('<H', grid[y][x]) for y in range(h) for x in range(w))
    assert len(out) == len(old)
    open(path, 'wb').write(out)
    walk = sum(1 for y in range(h) for x in range(w) if ((grid[y][x] >> 10) & 3) == 0)
    print('  written: %d walkable tiles of %d' % (walk, w * h))


if __name__ == '__main__':
    import sys
    main(int(sys.argv[1]) if len(sys.argv) > 1 else 7)
