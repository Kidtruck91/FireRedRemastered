"""Check that every warp event sits on a tile the engine will actually warp on,
that every bg_event can be reached and read, and that arriving on a warp does
not put the map's border on screen.

A warp event does nothing on its own: field_control_avatar.c only acts on it
when the metatile underneath carries a behaviour one of its three paths
recognises -- IsWarpMetatileBehavior (doors, ladders, warp pads...),
IsArrowWarpMetatileBehavior (the directional arrows) or
IsDirectionalStairWarpMetatileBehavior (the four stair corners). A warp placed
on plain floor is silent: the map looks right and the door simply does nothing.
A bg_event has the same kind of silent failure -- a sign with no walkable tile
next to it can never be faced, so its script is unreachable.

A third thing goes wrong quietly. The player lands on the warp tile and the
camera centres on them, so a warp close to the edge of an outdoor map puts
the repeating border in view -- open sea beyond the end of a beach, say. The
engine pads its view by MAP_OFFSET, which is 7, so the safe range is 7 tiles
in from every edge horizontally. Vertically the screen is shorter: 10 rows
with the player near the middle, so 5 above and 6 below.

    python3 docs/orange-islands/validate_warps.py [MapName ...]

With no arguments it checks every map. Exit status is 1 if anything failed.
"""
import json
import os
import struct
import sys

REPO = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..')

# Beaches that are deliberately not wired to their island. The maps exist and
# their island still has a warp pointing at them, but nothing fires it and
# that is the intention -- they are empty shells kept in case the island ever
# wants a shore. Listed so the dead-door report stays worth reading; take an
# entry out the day the beach is given a way in.
#
# For contrast, PUMMELO's beach is not here: it is reached through
# PUMMELO_SECRET_CAVE, whose ladders connect the island to the shore.
UNWIRED = {
    'OrangeIslands_Kinnow_Beach': 'not needed; KINNOW is entered from the sea',
    'OrangeIslands_Moro_Beach': 'not needed',
    'OrangeIslands_Pinkan_Beach': 'not needed',
}
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

# How much map has to sit outside a warp before the border stops showing when
# the player lands on it. MAP_OFFSET is 7 (include/fieldmap.h), and the screen
# is 15 tiles wide by 10 tall with the player near the centre.
EDGE_MARGIN_X = 7
EDGE_MARGIN_UP = 5
EDGE_MARGIN_DOWN = 6
# For blending, every kind of water counts as the same thing: a FAST_WATER
# border below an OCEAN_WATER edge still just reads as more sea.
def family(behaviour):
    if behaviour and 'WATER' in behaviour:
        return 'water'
    return behaviour


# Only outdoors: an indoor map's border is black, and seeing it is normal.
OUTDOOR_TYPES = {'MAP_TYPE_TOWN', 'MAP_TYPE_CITY', 'MAP_TYPE_ROUTE',
                 'MAP_TYPE_OCEAN_ROUTE'}


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


def border_behaviours(layout, ts, _cache={}):
    """The set of behaviours in a layout's border block."""
    key = layout['id']
    if key in _cache:
        return _cache[key]
    bw, bh = layout.get('border_width', 2), layout.get('border_height', 2)
    p = os.path.join(REPO, layout['border_filepath'])
    out = set()
    if os.path.exists(p):
        raw = open(p, 'rb').read()
        for i in range(min(bw * bh, len(raw) // 2)):
            m = struct.unpack_from('<H', raw, i * 2)[0] & 0x3FF
            b = family(ts.behaviour(layout, m))
            if b:
                out.add(b)
    _cache[key] = out
    return out


def main(argv):
    layouts = {l['id']: l for l in json.load(
        open(os.path.join(REPO, 'data/layouts/layouts.json')))['layouts'] if 'id' in l}
    ts = Tilesets()
    maps_dir = os.path.join(REPO, 'data/maps')
    wanted = set(argv) if argv else None

    problems = []
    notes = []
    suspect = []
    dead = []
    exits = {}
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
                if any(c['x'] == warp['x'] and c['y'] == warp['y']
                       for c in m.get('coord_events') or []):
                    continue  # a trigger on the tile does the warping instead
                notes.append(
                    '%s warp %d (%d,%d): metatile 0x%03X is %s -- cannot fire, so '
                    'it is arrival-only (or a dead door)'
                    % (name, i, warp['x'], warp['y'], mid, beh))
                dead.append((name, i, warp['x'], warp['y'], warp['dest_map'], beh))
            else:
                exits.setdefault(name, set()).add(warp['dest_map'])
        # Walking off the edge counts: a beach joined to its island by a map
        # connection does not need its warps to fire.
        for conn in (m.get('connections') or []):
            exits.setdefault(name, set()).add(conn['map'])
            if beh in WARPABLE and coll != 0 and beh in WARP_ON_STEP:
                problems.append(
                    '%s warp %d (%d,%d): %s warps fire on stepping on the tile, but '
                    'it is not walkable' % (name, i, warp['x'], warp['y'], beh))

        def blends(direction, wx, wy):
            """True when the border looks like more of what the map ends on,
            judged only over the stretch of edge that is actually on screen
            when the player stands at (wx, wy)."""
            bb = border_behaviours(layout, ts)
            if not bb:
                return True
            if direction in ('west', 'east'):
                ex = 0 if direction == 'west' else w - 1
                span = range(max(0, wy - EDGE_MARGIN_UP), min(h, wy + EDGE_MARGIN_DOWN + 1))
                edge = [cell(ex, yy) for yy in span]
            else:
                ey = 0 if direction == 'north' else h - 1
                span = range(max(0, wx - EDGE_MARGIN_X), min(w, wx + EDGE_MARGIN_X + 1))
                edge = [cell(xx, ey) for xx in span]
            seen = {family(ts.behaviour(layout, c[0])) for c in edge if c}
            seen.discard(None)
            return bool(seen) and seen <= bb

        if m.get('map_type') in OUTDOOR_TYPES:
            for i, warp in enumerate(warps):
                x, y = warp['x'], warp['y']
                short = []
                if x < EDGE_MARGIN_X and not blends('west', x, y):
                    short.append('%d short on the west' % (EDGE_MARGIN_X - x))
                if x > w - 1 - EDGE_MARGIN_X and not blends('east', x, y):
                    short.append('%d short on the east' % (x - (w - 1 - EDGE_MARGIN_X)))
                if y < EDGE_MARGIN_UP and not blends('north', x, y):
                    short.append('%d short on the north' % (EDGE_MARGIN_UP - y))
                if y > h - 1 - EDGE_MARGIN_DOWN and not blends('south', x, y):
                    short.append('%d short on the south' % (y - (h - 1 - EDGE_MARGIN_DOWN)))
                if short:
                    problems.append(
                        '%s warp %d (%d,%d): the player lands here and the camera '
                        'centres on them, so the border shows -- %s (map is %dx%d)'
                        % (name, i, x, y, ', '.join(short), w, h))

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

    # A warp that cannot fire is usually an arrival anchor: the foot of a
    # ladder, the cave mouth you step out of. It is a dead door only when the
    # map it sits on has no other way to reach that destination. Scripts count
    # as a way -- the sea crossings and the KINNOW gangway all warp from a
    # coord_event -- so the map's own scripts.inc is read for `warp MAP_...`.
    WARPCMD = ('warp ', 'warpsilent ', 'warpteleport ', 'warpdoor ',
               'warphole ', 'setwarp ', 'setdynamicwarp ')

    def warps_in(path):
        found = set()
        if os.path.exists(path):
            for line in open(path, encoding='utf-8', errors='replace'):
                line = line.strip()
                if line.startswith(WARPCMD):
                    found.add(line.split()[1].rstrip(','))
        return found

    # The shared scripts under data/scripts are reachable from any map, so
    # anything they warp to counts for every map. That is what drives the
    # ELITE FOUR's doors, the TRAINER TOWER lift and the MANSION's holes.
    shared = set()
    scripts_dir = os.path.join(REPO, 'data/scripts')
    if os.path.isdir(scripts_dir):
        for f in os.listdir(scripts_dir):
            shared |= warps_in(os.path.join(scripts_dir, f))

    script_exits = {}
    for mapname, _i, _x, _y, _dest, _beh in dead:
        if mapname in script_exits:
            continue
        script_exits[mapname] = (
            warps_in(os.path.join(maps_dir, mapname, 'scripts.inc')) | shared)
    def unwired(mapname, dest):
        if mapname in UNWIRED:
            return True
        # ... and the island's half of the same pair.
        for beach in UNWIRED:
            if dest == 'MAP_' + beach.replace('OrangeIslands_', 'ORANGE_ISLANDS_').upper():
                return True
        return False

    for mapname, i, x, y, dest, beh in dead:
        if dest in exits.get(mapname, ()) or dest in script_exits.get(mapname, ()):
            continue
        if unwired(mapname, dest):
            continue
        suspect.append(
            '%s warp %d (%d,%d) -> %s: the tile is %s, which no warp path fires '
            'on, and no warp or script on this map reaches that destination'
            % (mapname, i, x, y, dest.replace('MAP_', ''), beh))

    # Reported, not failed on. A map can also be left by C -- the ELITE FOUR's
    # doors, the TRAINER TOWER lift, the MANSION's holes -- which this cannot
    # see, so the list over-reports. It is still the only thing that catches a
    # door that silently does nothing, which is otherwise invisible until
    # someone walks into it.
    if UNWIRED:
        print('beaches left unwired on purpose (see UNWIRED in this script):')
        for k in sorted(UNWIRED):
            print('  - %s: %s' % (k.replace('OrangeIslands_', ''), UNWIRED[k]))
    if suspect:
        print('possible dead doors (a warp that cannot fire, with no other way '
              'to its destination -- over-reports, see the source):')
        for d in suspect:
            print('  ? ' + d)

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
