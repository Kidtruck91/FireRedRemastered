"""Check that every warp event sits on a tile the engine will actually warp on,
and that every bg_event can be reached and read.

A warp event does nothing on its own: field_control_avatar.c only acts on it
when the metatile underneath carries a behaviour one of its three paths
recognises -- IsWarpMetatileBehavior (doors, ladders, warp pads...),
IsArrowWarpMetatileBehavior (the directional arrows) or
IsDirectionalStairWarpMetatileBehavior (the four stair corners). A warp placed
on plain floor is silent: the map looks right and the door simply does nothing.
A bg_event has the same kind of silent failure -- a sign with no walkable tile
next to it can never be faced, so its script is unreachable.

    python3 docs/orange-islands/validate_warps.py [MapName ...]

With no arguments it checks every map. Exit status is 1 if anything failed.
"""
import json
import os
import struct
import sys

REPO = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..')
REPO = os.path.normpath(REPO)

# Behaviours the engine will warp on, by which path handles them.
#
# WARP_ON_STEP fires from TryStartWarpEventScript when the player steps onto the
# tile, so the tile has to be walkable. MB_WARP_DOOR is deliberately not in
# here: it goes through TryDoorWarp, which fires when the player walks north
# *into* it, which is why every outdoor building door is a blocked tile.
WARP_ON_STEP = {
    'MB_CAVE_DOOR', 'MB_LADDER', 'MB_ESCALATOR', 'MB_UP_ESCALATOR',
    'MB_DOWN_ESCALATOR', 'MB_REGULAR_WARP', 'MB_FALL_WARP',
    'MB_UNION_ROOM_WARP', 'MB_LAVARIDGE_1F_WARP',
}
WARP_ON_WALK_INTO = {
    'MB_WARP_DOOR',
    'MB_NORTH_ARROW_WARP', 'MB_SOUTH_ARROW_WARP',
    'MB_EAST_ARROW_WARP', 'MB_WEST_ARROW_WARP',
}
WARP_STAIRS = {
    'MB_UP_LEFT_STAIR_WARP', 'MB_UP_RIGHT_STAIR_WARP',
    'MB_DOWN_LEFT_STAIR_WARP', 'MB_DOWN_RIGHT_STAIR_WARP',
}
WARPABLE = WARP_ON_STEP | WARP_ON_WALK_INTO | WARP_STAIRS


def behaviour_names():
    names = {}
    for line in open(os.path.join(REPO, 'include/constants/metatile_behaviors.h')):
        p = line.split()
        if len(p) >= 3 and p[0] == '#define' and p[1].startswith('MB_'):
            try:
                names[int(p[2], 0)] = p[1]
            except ValueError:
                pass
    return names


class Tilesets:
    """Resolves a metatile id to its behaviour, for a given layout's pair."""

    def __init__(self):
        self.cache = {}
        self.names = behaviour_names()

    def attrs(self, label):
        if label in self.cache:
            return self.cache[label]
        # gTileset_HoennGeneral -> primary/hoenn_general, searched in both dirs
        base = ''
        for c in label[len('gTileset_'):]:
            base += ('_' + c.lower()) if (c.isupper() and base) else c.lower()
        data = None
        for sub in ('primary', 'secondary'):
            p = os.path.join(REPO, 'data/tilesets', sub, base, 'metatile_attributes.bin')
            if os.path.exists(p):
                data = open(p, 'rb').read()
                break
        self.cache[label] = data
        return data

    def behaviour(self, layout, mid):
        primary = mid < 640
        label = layout['primary_tileset'] if primary else layout['secondary_tileset']
        a = self.attrs(label)
        if a is None:
            return None
        i = mid if primary else mid - 640
        if (i + 1) * 4 > len(a):
            return None
        return self.names.get(struct.unpack_from('<I', a, i * 4)[0] & 0x1FF)


def main(argv):
    layouts = {l['id']: l for l in json.load(
        open(os.path.join(REPO, 'data/layouts/layouts.json')))['layouts'] if 'id' in l}
    ts = Tilesets()
    maps_dir = os.path.join(REPO, 'data/maps')
    wanted = set(argv) if argv else None

    problems = []
    notes = []
    checked = 0
    for name in sorted(os.listdir(maps_dir)):
        mj = os.path.join(maps_dir, name, 'map.json')
        if not os.path.exists(mj):
            continue
        if wanted is not None and name not in wanted:
            continue
        m = json.load(open(mj))
        layout = layouts.get(m['layout'])
        if layout is None:
            problems.append('%s: unknown layout %s' % (name, m['layout']))
            continue
        w, h = layout['width'], layout['height']
        blockpath = os.path.join(REPO, layout['blockdata_filepath'])
        if not os.path.exists(blockpath):
            problems.append('%s: missing %s' % (name, layout['blockdata_filepath']))
            continue
        b = open(blockpath, 'rb').read()
        checked += 1

        def cell(x, y):
            if not (0 <= x < w and 0 <= y < h):
                return None
            off = 2 * (y * w + x)
            if off + 2 > len(b):
                return None
            v = struct.unpack_from('<H', b, off)[0]
            return v & 0x3FF, (v >> 10) & 3

        warps = m.get('warp_events', [])
        # A wide door is several warp events side by side with only the middle
        # tile carrying the behaviour: the outer ones exist so the player can
        # arrive on them, and are never meant to fire. Treat a non-warpable
        # warp as one of those when a warpable warp sits next to it.
        warpable_at = set()
        for warp in warps:
            c = cell(warp['x'], warp['y'])
            if c and ts.behaviour(layout, c[0]) in WARPABLE:
                warpable_at.add((warp['x'], warp['y']))

        def has_warpable_neighbour(x, y):
            return any((x + dx, y + dy) in warpable_at
                       for dx, dy in ((-1, 0), (1, 0), (0, -1), (0, 1)))

        for i, warp in enumerate(warps):
            c = cell(warp['x'], warp['y'])
            if c is None:
                problems.append('%s warp %d (%d,%d): outside the layout'
                                % (name, i, warp['x'], warp['y']))
                continue
            mid, coll = c
            beh = ts.behaviour(layout, mid)
            if beh is None:
                continue  # tileset not readable; nothing to say
            if beh not in WARPABLE:
                if has_warpable_neighbour(warp['x'], warp['y']):
                    continue  # arrival-only half of a wide door
                # A warp on a tile with no warp behaviour cannot fire, but that
                # is a normal thing to find: vanilla is full of warps that exist
                # only to be arrived on (the foot of a ladder, a cave mouth you
                # step out of). Telling those apart from a genuinely dead door
                # needs to know whether anything else reaches the destination,
                # which this does not, so it is reported and not failed on.
                notes.append(
                    '%s warp %d (%d,%d): metatile 0x%03X is %s -- cannot fire, so '
                    'it is arrival-only (or a dead door)'
                    % (name, i, warp['x'], warp['y'], mid, beh))
            elif coll != 0 and beh in WARP_ON_STEP:
                problems.append(
                    '%s warp %d (%d,%d): %s warps fire on stepping on the tile, but '
                    'it is not walkable' % (name, i, warp['x'], warp['y'], beh))

        for bg in m.get('bg_events', []):
            if bg.get('type') not in (None, 'sign'):
                continue
            here = cell(bg['x'], bg['y'])
            if here is None:
                problems.append('%s sign (%d,%d): outside the layout'
                                % (name, bg['x'], bg['y']))
                continue
            if here[1] == 0:
                continue  # standing on it works
            if not any((c and c[1] == 0) for c in (
                    cell(bg['x'], bg['y'] + 1), cell(bg['x'], bg['y'] - 1),
                    cell(bg['x'] - 1, bg['y']), cell(bg['x'] + 1, bg['y']))):
                problems.append(
                    '%s sign (%d,%d): no walkable tile beside it, so it can never '
                    'be read' % (name, bg['x'], bg['y']))

    print('checked %d maps' % checked)
    for p in problems:
        print('  ' + p)
    print('problems: %d' % len(problems))
    # Informational, and only worth listing when looking at specific maps.
    if notes and wanted is not None:
        for n in notes:
            print('  note: ' + n)
    print('arrival-only warps: %d' % len(notes))
    return 1 if problems else 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
