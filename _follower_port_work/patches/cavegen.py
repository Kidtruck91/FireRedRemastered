#!/usr/bin/env python3
"""Generate the missing Orange Islands cave floors.

What was there: SUNBURST CAVE and PUMMELO CAVE each had one drawn floor with
ladder tiles that led nowhere, and the "upper floors" were a byte-for-byte copy
of the ground floor (Sunburst 2F) or blank 20x20 rectangles (both Pummelo 2Fs,
which are the same file under two names). Two route cave mouths on Route 4 and
one on Route 9 opened onto nothing at all.

The drawn ground floors are the user's and are not touched -- only their ladders
get wired. Everything else here is generated: rooms placed on a grid, joined by
L-corridors as a spanning tree plus a few extra loops so a floor is not a single
line, with the required ladders dropped into rooms and the whole thing checked
for connectivity before it is written.

Seeded, so re-running reproduces the same caves.
"""
import json, os, random, struct, sys
from collections import deque

FLOOR, FILL, WALL_H, WALL_V, LADDER = 0x281, 0x291, 0x283, 0x282, 0x285
E_FLOOR, E_WALL = 3, 0


class Floor:
    def __init__(self, w, h, seed):
        self.w, self.h = w, h
        self.rng = random.Random(seed)
        self.open = set()
        self.rooms = []

    # -- carving -----------------------------------------------------------
    def room(self, x, y, rw, rh):
        for a in range(x, x + rw):
            for b in range(y, y + rh):
                if 2 <= a < self.w - 2 and 2 <= b < self.h - 2:
                    self.open.add((a, b))
        self.rooms.append((x + rw // 2, y + rh // 2))

    def corridor(self, p, q, width=1):
        (x1, y1), (x2, y2) = p, q
        horiz_first = self.rng.random() < 0.5
        pts = []
        if horiz_first:
            pts += [(x, y1) for x in range(min(x1, x2), max(x1, x2) + 1)]
            pts += [(x2, y) for y in range(min(y1, y2), max(y1, y2) + 1)]
        else:
            pts += [(x1, y) for y in range(min(y1, y2), max(y1, y2) + 1)]
            pts += [(x, y2) for x in range(min(x1, x2), max(x1, x2) + 1)]
        for (x, y) in pts:
            for dx in range(width):
                for dy in range(width):
                    a, b = x + dx, y + dy
                    if 2 <= a < self.w - 2 and 2 <= b < self.h - 2:
                        self.open.add((a, b))

    def scatter(self, n, lo=4, hi=9):
        """Place n rooms without letting them drift off the map."""
        tries = 0
        while len(self.rooms) < n and tries < n * 60:
            tries += 1
            rw = self.rng.randint(lo, hi)
            rh = self.rng.randint(lo - 1, hi - 2)
            x = self.rng.randint(3, max(4, self.w - rw - 4))
            y = self.rng.randint(3, max(4, self.h - rh - 4))
            c = (x + rw // 2, y + rh // 2)
            if any(abs(c[0] - o[0]) < 7 and abs(c[1] - o[1]) < 6 for o in self.rooms):
                continue
            self.room(x, y, rw, rh)

    def join(self, extra_loops=3):
        """Spanning tree over the rooms, then a few extra edges for loops."""
        if not self.rooms:
            return
        linked = [self.rooms[0]]
        rest = list(self.rooms[1:])
        while rest:
            best = min(((a, b) for a in linked for b in rest),
                       key=lambda e: abs(e[0][0] - e[1][0]) + abs(e[0][1] - e[1][1]))
            self.corridor(best[0], best[1], width=self.rng.choice([1, 1, 2]))
            linked.append(best[1])
            rest.remove(best[1])
        for _ in range(extra_loops):
            a, b = self.rng.sample(self.rooms, 2)
            self.corridor(a, b, width=1)

    # -- checks ------------------------------------------------------------
    def reachable(self, start):
        seen = {start}
        q = deque([start])
        while q:
            x, y = q.popleft()
            for d in ((0, 1), (0, -1), (1, 0), (-1, 0)):
                n = (x + d[0], y + d[1])
                if n in self.open and n not in seen:
                    seen.add(n)
                    q.append(n)
        return seen

    def nearest_open(self, p):
        return min(self.open, key=lambda q: abs(q[0] - p[0]) + abs(q[1] - p[1]))

    # -- output ------------------------------------------------------------
    def blocks(self, ladders):
        g = {}
        for y in range(self.h):
            for x in range(self.w):
                g[(x, y)] = (FILL, 1, E_WALL)
        for p in self.open:
            g[p] = (FLOOR, 0, E_FLOOR)
        for p in ladders:
            g[p] = (LADDER, 0, E_FLOOR)
        faces = {}
        for (x, y), (m, c, e) in g.items():
            if not c:
                continue
            for dx, dy in ((0, 1), (0, -1), (-1, 0), (1, 0)):
                if g.get((x + dx, y + dy), (0, 1, 0))[1] == 0:
                    faces[(x, y)] = (WALL_H if dy else WALL_V, 1, E_WALL)
                    break
        g.update(faces)
        return b''.join(struct.pack('<H', m | (c << 10) | (e << 12))
                        for y in range(self.h) for x in range(self.w)
                        for (m, c, e) in [g[(x, y)]])


def build(w, h, seed, n_rooms, features):
    """features: list of (name, preferred (x,y)). Returns (floor, {name: pos})."""
    for attempt in range(40):
        f = Floor(w, h, seed + attempt * 977)
        f.scatter(n_rooms)
        f.join()
        if len(f.open) < w * h * 0.12:
            continue
        placed, used = {}, set()
        for name, pref in features:
            cands = sorted(f.open, key=lambda q: abs(q[0] - pref[0]) + abs(q[1] - pref[1]))
            spot = next((c for c in cands
                         if c not in used
                         and all(abs(c[0] - u[0]) + abs(c[1] - u[1]) > 8 for u in used)), None)
            if spot is None:
                break
            placed[name] = spot
            used.add(spot)
        if len(placed) != len(features):
            continue
        reach = f.reachable(next(iter(placed.values())))
        if all(p in reach for p in placed.values()):
            return f, placed
    raise SystemExit('could not lay out a %dx%d floor with %d features' % (w, h, len(features)))
