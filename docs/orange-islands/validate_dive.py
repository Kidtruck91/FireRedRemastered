"""Check the dive spots against the two things the engine assumes.

Diving hands the player their current x,y on the map below; surfacing hands
them their current x,y on the map above. Neither end checks that the square
it drops you on is somewhere you can be, so both have to be true by
construction:

  1. every swimmable underwater cell lies under open ocean, or surfacing
     there puts the player on land while in surf state;
  2. every cell of a dive pool is swimmable directly below, or diving drops
     the player inside rock.

It also checks that the only diveable tiles on a route are the pools (stock
FireRed counts MB_FAST_WATER as diveable too, which the Orange routes use for
currents), that every pool actually reaches the seafloor's open water, and
that the dive/emerge connections pair up.

    python3 -B docs/orange-islands/validate_dive.py
"""
import json
import os
import re
import struct
import sys

REPO = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
sys.path.insert(0, os.path.join(REPO, 'docs/orange-islands'))

WATER = {0x10, 0x11, 0x12, 0x15, 0x16}
MB_DEEP = 0x12
MB_FAST = 0x11
MB_BLOCKED_ABOVE = 0x19


def symbol_dirs():
    out = {}
    src = open(os.path.join(REPO, 'src/data/tilesets/metatiles.h')).read()
    for sym, path in re.findall(
            r'gMetatileAttributes_(\w+)\[\]\s*=\s*INCBIN_U32\("([^"]+)"\)', src):
        out['gTileset_' + sym] = os.path.dirname(path)
    return out


def behaviours(primary, secondary, dirs):
    def load(sym):
        p = os.path.join(REPO, dirs[sym], 'metatile_attributes.bin')
        return [v & 0x1FF for (v,) in struct.iter_unpack('<I', open(p, 'rb').read())]
    P, S = load(primary), load(secondary)
    def beh(m):
        if m < 640:
            return P[m] if m < len(P) else 0
        i = m - 640
        return S[i] if i < len(S) else 0
    return beh


def grid_of(layout, dirs):
    w, h = layout['width'], layout['height']
    b = open(os.path.join(REPO, layout['blockdata_filepath']), 'rb').read()
    beh = behaviours(layout['primary_tileset'], layout['secondary_tileset'], dirs)
    cells = []
    for y in range(h):
        row = []
        for x in range(w):
            (v,) = struct.unpack_from('<H', b, 2 * (y * w + x))
            row.append((v & 0x3FF, (v >> 10) & 3, beh(v & 0x3FF)))
        cells.append(row)
    return w, h, cells


def main():
    dirs = symbol_dirs()
    L = {l['name']: l for l in json.load(open(os.path.join(
        REPO, 'data/layouts/layouts.json')))['layouts']}
    maps = {}
    for d in sorted(os.listdir(os.path.join(REPO, 'data/maps'))):
        p = os.path.join(REPO, 'data/maps', d, 'map.json')
        if os.path.exists(p):
            j = json.load(open(p))
            maps[j['id']] = (d, j)

    problems = 0
    pairs = []
    for cid, (d, j) in maps.items():
        for c in (j.get('connections') or []):
            if c['direction'] == 'dive':
                pairs.append((d, c['map']))

    if not pairs:
        print('no dive connections found')
        return 1

    for above_name, below_id in pairs:
        below_name = maps[below_id][0]
        A = L[above_name + '_Layout']
        B = L[below_name + '_Layout']
        if (A['width'], A['height']) != (B['width'], B['height']):
            print('!! %s %dx%d and %s %dx%d must be the same size for a dive '
                  'connection' % (above_name, A['width'], A['height'],
                                  below_name, B['width'], B['height']))
            problems += 1
            continue
        w, h, up = grid_of(A, dirs)
        _, _, dn = grid_of(B, dirs)

        ocean = [[up[y][x][2] in WATER and up[y][x][1] == 0 for x in range(w)]
                 for y in range(h)]
        swim = {(x, y) for y in range(h) for x in range(w) if dn[y][x][1] == 0}

        # 1. swimmable only under open ocean
        dry = [c for c in swim if not ocean[c[1]][c[0]]]
        # 2. pools swimmable below, and nothing else diveable
        pool = {(x, y) for y in range(h) for x in range(w)
                if up[y][x][2] == MB_DEEP}
        fast = sum(1 for y in range(h) for x in range(w) if up[y][x][2] == MB_FAST)
        solid_under_pool = [c for c in pool if c not in swim]
        # 3. every pool reaches open water
        seen, q = set(), []
        for c in pool:
            if c in swim and c not in seen:
                seen.add(c)
                q.append(c)
        while q:
            x, y = q.pop()
            for dx, dy in ((1, 0), (-1, 0), (0, 1), (0, -1)):
                n = (x + dx, y + dy)
                if n in swim and n not in seen:
                    seen.add(n)
                    q.append(n)
        stranded = [c for c in pool if c in swim and c not in seen]
        # 4. the seafloor points back
        back = [c for c in (maps[below_id][1].get('connections') or [])
                if c['direction'] == 'emerge']
        reciprocal = len(back) == 1 and back[0]['map'] == maps[
            [k for k, v in maps.items() if v[0] == above_name][0]][1]['id']
        # 5. items are reachable
        loot = [(o['x'], o['y']) for o in maps[below_id][1]['object_events']]
        lost = [c for c in loot if c not in seen]

        print('%-34s %3dx%-3d  pool %3d  seafloor %4d swimmable %4d reached (%3d%%)'
              % (below_name, w, h, len(pool), len(swim), len(seen),
                 100 * len(seen) // max(1, len(swim))))
        for label, bad in (('swimmable cell not under open ocean', dry),
                           ('pool cell with rock below', solid_under_pool),
                           ('pool cut off from the seafloor', stranded),
                           ('item out of reach', lost)):
            if bad:
                print('   !! %d %s: %s%s' % (len(bad), label,
                      ' '.join('%d,%d' % c for c in bad[:6]),
                      ' ...' if len(bad) > 6 else ''))
                problems += len(bad)
        if fast:
            print('   !! %d MB_FAST_WATER cells on %s would also be diveable'
                  % (fast, above_name))
            problems += 1
        if not reciprocal:
            print('   !! %s has no matching emerge connection' % below_name)
            problems += 1
    print('problems: %d' % problems)
    return 1 if problems else 0


if __name__ == '__main__':
    sys.exit(main())
