"""Repair the landmark fixtures the map checker walks.

A fixture is a start and a handful of goals: the checker flood-fills from the
start and asserts every goal is reachable. The goals are the valuable part --
they are hand-picked, and each one says "the player must still be able to get
here", which is exactly what a map edit can quietly break. The start is not:
it is only somewhere the player stands, and any walkable cell in the same
region does the job.

Which is why the fixtures rot the way they do. Edit a map and the start can
land inside a cliff, and then the checker stops before it walks anything at
all -- so the goals, the part worth checking, go unchecked. Thirteen of the
region's maps were in that state, and five more were worse: Navel started at
(22,109) on a map 65 tiles tall, Navel Beach's first goal sat north of its own
beach, Kinnow started at (22,75) on a map 66 tall, and Route 9 and Route 17 had
no fixture at all.

So two passes, and the difference between them is deliberate:

    --repair   move a start that is no longer walkable, and leave every goal
               alone. An unreachable goal is a finding, not rot: it is the
               checker telling you a map edit cut something off, and quietly
               moving the goal would throw that away.

    --redraw   derive a whole fixture from the terrain, for the handful whose
               goals describe a layout that no longer exists. Takes the largest
               walkable region, starts near its centre of mass and puts the
               goals at its four extremes -- which on Route 9 lands all three
               of them on the mouths of its sea crossings, so the fixture now
               fails the moment one is walled up.

    python3 docs/orange-islands/repair_landmark_fixtures.py --repair --redraw
"""
import json
import os
import re
import struct
import sys
from collections import deque

REPO = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))

# The maps whose goals no longer describe their own terrain, and whether the
# player can surf there -- a sea route's water is walkable to the checker.
REDRAW = {'Navel': False, 'Navel_Beach': False, 'Kinnow': False,
          'Route_9': True, 'Route_17': True}

WATER = {0x10, 0x11, 0x12, 0x13, 0x15, 0x17, 0x50, 0x51, 0x52, 0x53}

FIXTURES = os.path.join(REPO, 'docs/orange-islands/island_landmarks.json')


def tileset_dir(symbol):
    """Where a tileset's binaries live, read out of the C headers.

    render_maps.py has the same lookup, but importing it would drag in Pillow
    for a script that only ever touches bytes.
    """
    headers = open(os.path.join(REPO, 'src/data/tilesets/headers.h')).read()
    body = re.search(r'const struct Tileset ' + re.escape(symbol) + r'\s*=\s*\{(.*?)\};',
                     headers, re.S).group(1)
    name = re.search(r'\.metatiles\s*=\s*(\w+)', body).group(1)
    defs = open(os.path.join(REPO, 'src/data/tilesets/metatiles.h')).read()
    path = re.search(re.escape(name) + r'\[\]\s*=\s*INCBIN_U16\("([^"]+)"\)', defs).group(1)
    return os.path.dirname(os.path.join(REPO, path))


def behaviours(layout):
    """Metatile behaviours for a layout's pair of tilesets, primary first."""
    out = []
    for key in ('primary_tileset', 'secondary_tileset'):
        b = open(os.path.join(tileset_dir(layout[key]), 'metatile_attributes.bin'), 'rb').read()
        out.extend(struct.unpack('<' + 'I' * (len(b) // 4), b))
    return [a & 0x1FF for a in out]


def terrain(layout, surf):
    """The checker's own walkability rule, and the regions it produces."""
    w, h = layout['width'], layout['height']
    beh = behaviours(layout)
    raw = open(os.path.join(REPO, layout['blockdata_filepath']), 'rb').read()
    blocks = struct.unpack('<' + 'H' * (len(raw) // 2), raw)

    def walk(x, y):
        if not (0 <= x < w and 0 <= y < h):
            return False
        v = blocks[y * w + x]
        return not (v & 0xC00) and (surf or beh[v & 0x3FF] not in WATER)

    regions, seen = [], set()
    for y in range(h):
        for x in range(w):
            if (x, y) in seen or not walk(x, y):
                continue
            region, q = {(x, y)}, deque([(x, y)])
            while q:
                cx, cy = q.popleft()
                for n in ((cx - 1, cy), (cx + 1, cy), (cx, cy - 1), (cx, cy + 1)):
                    if n not in region and walk(*n):
                        region.add(n)
                        q.append(n)
            seen |= region
            regions.append(region)
    return walk, regions


def middle(region):
    """The cell nearest a region's centre of mass -- the centre itself may be
    a wall, and a start wants to be well inside."""
    cx = sum(p[0] for p in region) / len(region)
    cy = sum(p[1] for p in region) / len(region)
    return min(region, key=lambda p: (p[0] - cx) ** 2 + (p[1] - cy) ** 2)


def layouts_and_maps():
    layouts = {l['id']: l for l in
               json.load(open(os.path.join(REPO, 'data/layouts/layouts.json')))['layouts']}
    maps = {}
    for d in sorted(os.listdir(os.path.join(REPO, 'data/maps'))):
        p = os.path.join(REPO, 'data/maps', d, 'map.json')
        if os.path.exists(p):
            maps[d] = json.load(open(p))
    return layouts, maps


def redraw(layout, surf):
    _, regions = terrain(layout, surf)
    best = max(regions, key=len)
    start = middle(best)
    goals = [min(best, key=lambda p: (p[0], p[1])),      # west
             max(best, key=lambda p: (p[0], -p[1])),     # east
             min(best, key=lambda p: (p[1], p[0])),      # north
             max(best, key=lambda p: (p[1], -p[0]))]     # south
    goals = [g for g in dict.fromkeys(goals) if g != start]
    # Two extremes can be the same cell -- Route 17's westmost tile is also its
    # northmost -- so top the list back up with whatever cell lies farthest
    # from everything already chosen.
    while len(goals) < 4:
        chosen = goals + [start]
        goals.append(max(best, key=lambda p: min((p[0] - q[0]) ** 2 + (p[1] - q[1]) ** 2
                                                 for q in chosen)))
    return list(start), [list(g) for g in goals], len(best)


def repair(layout, surf, start, goals):
    """Move an unwalkable start, keeping every goal exactly where it is.

    The new start goes in whichever region holds the most of the existing
    goals, so the fixture keeps checking what its author meant it to check.
    """
    walk, regions = terrain(layout, surf)
    if walk(*start):
        return None
    want = {tuple(g) for g in goals}
    best = max(regions, key=lambda r: (len(r & want), len(r)))
    return list(middle(best)), len(best & want), len(want)


def main():
    do_repair = '--repair' in sys.argv
    do_redraw = '--redraw' in sys.argv
    if not (do_repair or do_redraw):
        sys.exit(__doc__)
    layouts, maps = layouts_and_maps()
    book = json.load(open(FIXTURES))
    changed = False

    if do_redraw:
        for suffix, surf in sorted(REDRAW.items()):
            layout = layouts[maps['OrangeIslands_' + suffix]['layout']]
            start, goals, size = redraw(layout, surf)
            if book.get(suffix) == [start, goals, surf]:
                continue
            book[suffix] = [start, goals, surf]
            changed = True
            print('  redrawn  %-16s start %-10s goals %s  (region of %d cells)'
                  % (suffix, '(%d,%d)' % tuple(start),
                     ' '.join('(%d,%d)' % tuple(g) for g in goals), size))

    if do_repair:
        for suffix in sorted(book):
            if do_redraw and suffix in REDRAW:
                continue
            name = 'OrangeIslands_' + suffix
            if name not in maps:
                print('  skipped  %-16s no such map' % suffix)
                continue
            start, goals, surf = book[suffix]
            moved = repair(layouts[maps[name]['layout']], surf, tuple(start), goals)
            if moved is None:
                continue
            fresh, held, total = moved
            book[suffix] = [fresh, goals, surf]
            changed = True
            print('  moved    %-16s start (%d,%d) -> (%d,%d), the region holding %d of its %d goals'
                  % (suffix, start[0], start[1], fresh[0], fresh[1], held, total))

    if changed:
        with open(FIXTURES, 'w', newline='\n') as f:
            json.dump(book, f, indent=1)
            f.write('\n')
    else:
        print('  every fixture already stands on its own terrain')


if __name__ == '__main__':
    main()
