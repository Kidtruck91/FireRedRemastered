"""Check that water can actually be surfed onto from the land beside it.

Nothing about surfing is driven by the collision mark on its own, and nothing
by the behaviour alone. IsPlayerFacingSurfableFishableWater() in
src/field_player_avatar.c wants all of these at once:

    GetCollisionAtCoords(...) == COLLISION_ELEVATION_MISMATCH
    && PlayerGetElevation() == 3
    && MetatileAtCoordsIsWaterTile(x, y)

Unpacked against GetCollisionAtCoords and IsElevationMismatchAt:

  the water tile's collision must be 0.
        Collision is tested before elevation and any non-zero value returns
        COLLISION_IMPASSABLE, so a water tile marked impassable is a wall. It
        gives no surf prompt -- the player just bumps into it.

  the water tile's elevation must not be 3, 0 or 15.
        0 and 15 are "matches anything", so they produce no mismatch and the
        player simply walks out onto the water as though it were ground. The
        convention everywhere in the game is elevation 1.

  the land tile's elevation must be exactly 3.
        Both for mounting and, in CanStopSurfing, for getting off again.

  the water metatile's TERRAIN attribute must be TILE_TERRAIN_WATER.
        That is the attribute field, not the behaviour. MetatileAtCoordsIsWaterTile
        reads METATILE_ATTRIBUTE_TERRAIN, so a tile can behave like water,
        animate like water and still not be surfable if its terrain says
        otherwise.

So water that is meant to be swum on is collision 0, elevation 1; water that
is meant to be scenery is collision 1, and this does not complain about it.
What it reports is water a player can walk up to and would expect to surf on,
that will refuse.

    python3 docs/orange-islands/validate_surf.py              # every layout
    python3 docs/orange-islands/validate_surf.py LAYOUT_...   # just these

Exit status is 1 if anything failed.
"""
import collections
import json
import os
import struct
import sys

REPO = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
TILE_TERRAIN_WATER = 2
MAX_REPORTED = 6


def tileset_attrs(label, _cache={}):
    """metatile_attributes.bin for a gTileset_ label, or b'' if not found."""
    if label in _cache:
        return _cache[label]
    key = label[len('gTileset_'):].lower() if label else ''
    data = b''
    for sub in ('primary', 'secondary'):
        d = os.path.join(REPO, 'data/tilesets', sub)
        if not os.path.isdir(d):
            continue
        for name in os.listdir(d):
            # directory names split on digits too: SeviiIslands45Condensed
            # is sevii_islands_45_condensed
            if name.replace('_', '') == key:
                p = os.path.join(d, name, 'metatile_attributes.bin')
                if os.path.exists(p):
                    data = open(p, 'rb').read()
    _cache[label] = data
    return data


def check(l):
    w, h = l['width'], l['height']
    p = os.path.join(REPO, l['blockdata_filepath'])
    if not os.path.exists(p):
        return None
    prim = tileset_attrs(l.get('primary_tileset'))
    sec = tileset_attrs(l.get('secondary_tileset'))
    if not prim:
        return None
    raw = open(p, 'rb').read()
    if len(raw) < w * h * 2:
        return None

    def terrain(m):
        a, i = (prim, m) if m < 640 else (sec, m - 640)
        if (i + 1) * 4 > len(a):
            return 0
        return (struct.unpack_from('<I', a, i * 4)[0] >> 9) & 0x1F

    def block(x, y):
        return struct.unpack_from('<H', raw, 2 * (y * w + x))[0]

    is_water = lambda x, y: terrain(block(x, y) & 0x3FF) == TILE_TERRAIN_WATER
    coll = lambda x, y: (block(x, y) >> 10) & 3
    elev = lambda x, y: (block(x, y) >> 12) & 15
    # land the player can stand on and surf from
    land = lambda x, y: not is_water(x, y) and coll(x, y) == 0 and elev(x, y) == 3

    blocked, walkable = [], []
    for y in range(h):
        for x in range(w):
            if not is_water(x, y):
                continue
            if not any(0 <= x + dx < w and 0 <= y + dy < h and land(x + dx, y + dy)
                       for dx, dy in ((1, 0), (-1, 0), (0, 1), (0, -1))):
                continue  # nothing can reach it; scenery, and fine
            if coll(x, y) != 0:
                blocked.append((x, y, coll(x, y), elev(x, y)))
            elif elev(x, y) in (0, 3, 15):
                walkable.append((x, y, coll(x, y), elev(x, y)))
    return blocked, walkable


def main(argv):
    wanted = {a for a in argv if not a.startswith('--')}
    layouts = [l for l in json.load(
        open(os.path.join(REPO, 'data/layouts/layouts.json')))['layouts'] if 'id' in l]
    n_blocked = n_walk = 0
    checked = 0
    for l in layouts:
        if wanted and l['id'] not in wanted:
            continue
        r = check(l)
        if r is None:
            continue
        checked += 1
        blocked, walkable = r
        n_blocked += len(blocked)
        n_walk += len(walkable)
        if not blocked and not walkable:
            continue
        print('%s' % l['id'])
        if blocked:
            print('    %d water tiles beside walkable land are marked impassable '
                  '(collision %s) -- the player bumps into them instead of being '
                  'offered Surf'
                  % (len(blocked), sorted({b[2] for b in blocked})))
            for x, y, c, e in blocked[:MAX_REPORTED]:
                print('        (%d,%d) collision=%d elevation=%d' % (x, y, c, e))
            if len(blocked) > MAX_REPORTED:
                print('        ... and %d more' % (len(blocked) - MAX_REPORTED))
        if walkable:
            print('    %d water tiles beside walkable land have elevation %s -- no '
                  'mismatch, so the player walks straight out onto the water'
                  % (len(walkable), sorted({t[3] for t in walkable})))
            for x, y, c, e in walkable[:MAX_REPORTED]:
                print('        (%d,%d) collision=%d elevation=%d' % (x, y, c, e))
            if len(walkable) > MAX_REPORTED:
                print('        ... and %d more' % (len(walkable) - MAX_REPORTED))

    print('\nchecked %d layouts' % checked)
    print('shoreline water that refuses Surf: %d' % n_blocked)
    print('shoreline water the player would walk onto: %d' % n_walk)
    return 1 if (n_blocked or n_walk) else 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
