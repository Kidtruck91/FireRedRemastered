"""Tidy up the hand-painted Mandarin South / Mandarin South Beach rework.

The new geography is good: Mandarin South's east side now runs into a cliff wall
with a three-tile sand gap, the beach map carries that same wall down its west
side, and the "right"/"left" connection pair at offset -25/+25 lines the two up
seam for seam. What is missing is everything Porymap does not paint for you.

Every cell the rework touched was left at the brush default, collision 0, so:

  * all 374 of Mandarin South's new cliff tiles are walk-through,
  * the whole beach map is walk-through -- 922 cliff cells included,
  * the beach's sand sits at elevation 0 instead of 3, and
  * the beach's 2,601 ocean cells sit at elevation 0 instead of 1, so the sea
    is not surfable.

Rather than guess, this reads the right answer off the rest of the region. For
every metatile id, it counts the (collision, elevation) pairs the author used
across every other Orange map drawn on gTileset_General or gTileset_OrangeGeneral,
and takes the one he used most. The verdicts are not close -- 071 is col1/el0 in
1481 of 1525 cells elsewhere, 07C in 500 of 500, 1CB/1CC/1D3/1D4 in every one --
so this is applying the author's own convention, not inventing one.

The beach, being new, is normalised whole. Mandarin South is not: only cells
whose metatile belongs to the cliff family AND are currently passable are
touched, which is exactly the new paint (before the rework the map held one
single 071, already solid). Sand, shore and the author's city are left alone.

The three-tile doorway survives: Mandarin South rows 7-9 at x=95 are sand
(col0/el3) and the beach rows 32-34 at x=0 are 0DC, whose regional norm is also
col0/el3.

Also fixed here: the beach's remaining sign still sat at x=101 on a map that is
now 91 wide, and Mandarin South's row-75 sea lost its Route 8 trigger at x=0
when the coast was redrawn.

    python3 docs/orange-islands/tidy_mandarin_south.py          # report only
    python3 docs/orange-islands/tidy_mandarin_south.py --write
"""
import collections
import json
import os
import re
import struct
import subprocess
import sys

REPO = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
LAYOUTS = os.path.join(REPO, 'data/layouts/layouts.json')
MS = 'LAYOUT_ORANGE_ISLANDS_MANDARIN_SOUTH'
BEACH = 'LAYOUT_ORANGE_ISLANDS_MANDARIN_SOUTH_BEACH'
SHARED_PRIMARIES = ('gTileset_General', 'gTileset_OrangeGeneral')

# The cliff / rock-wall family, the only metatiles touched in Mandarin South.
CLIFF = {0x071, 0x073, 0x078, 0x079, 0x07C, 0x098, 0x099, 0x09A, 0x0A0, 0x0A2,
         0x0A8, 0x0B3, 0x0BB, 0x0E7, 0x110, 0x111, 0x118, 0x119,
         0x1CB, 0x1CC, 0x1D3, 0x1D4, 0x1DB, 0x213, 0x21A, 0x21B, 0x212}

# Open water and its shore wash: these belong at elevation 1, never 3.
SEA = {0x12A, 0x12B, 0x12C, 0x122, 0x123, 0x124, 0x1D9, 0x1FB, 0x1F8, 0x1F9}

# How confident the region has to be before this script overrules what is there.
CONFIDENCE = 0.75


def layouts():
    doc = json.load(open(LAYOUTS))
    return doc, {l['id']: l for l in doc['layouts']}


def blocks(layout):
    raw = open(os.path.join(REPO, layout['blockdata_filepath']), 'rb').read()
    return bytearray(raw)


def baseline(layout):
    """The committed block data, so the Mandarin South pass only touches the rework.

    Without it the pass falls back to "any passable cliff metatile", which also
    catches things the author left passable on purpose -- the walkable fence
    along the top of the tree plantation at y=29, for one.
    """
    try:
        out = subprocess.run(['git', 'show', 'HEAD:' + layout['blockdata_filepath']],
                             cwd=REPO, capture_output=True, check=True).stdout
    except (OSError, subprocess.CalledProcessError):
        return None
    return out if len(out) == 2 * layout['width'] * layout['height'] else None


def norms(index):
    """The (collision, elevation) the author uses most for each metatile id."""
    seen = collections.defaultdict(collections.Counter)
    for lid, l in index.items():
        if not lid.startswith('LAYOUT_ORANGE') or lid in (MS, BEACH):
            continue
        if l['primary_tileset'] not in SHARED_PRIMARIES:
            continue
        try:
            raw = open(os.path.join(REPO, l['blockdata_filepath']), 'rb').read()
        except OSError:
            continue
        for i in range(0, len(raw), 2):
            v = struct.unpack_from('<H', raw, i)[0]
            seen[v & 0x3FF][v >> 10] += 1
    out = {}
    for mid, c in seen.items():
        hi, n = c.most_common(1)[0]
        tot = sum(c.values())
        if n / tot >= CONFIDENCE:
            out[mid] = (hi, n, tot)
            continue
        # No single pair wins, but if the author never once left this metatile
        # walkable, the collision bit is still settled -- 099 is col1/el3 six
        # times and col1/el0 six times, and either way you cannot walk into it.
        cols = {k & 3 for k in c}
        if len(cols) == 1 and cols != {0}:
            col = cols.pop()
            els = collections.Counter({k >> 2: v for k, v in c.items()})
            el, en = els.most_common(1)[0]
            out[mid] = ((el << 2) | col, en, tot)
    return out


def fmt(hi):
    return 'col%d/el%d' % (hi & 3, hi >> 2)


def fix(layout, norm, whole, label):
    """Return (patched bytes, change tally). `whole` normalises every cell."""
    data = blocks(layout)
    was = None if whole else baseline(layout)
    if not whole:
        print('  (%s)' % ('comparing against the committed map' if was else
                          'no git baseline -- falling back to the cliff-metatile rule'))
    tally = collections.Counter()
    for i in range(0, len(data), 2):
        v = struct.unpack_from('<H', data, i)[0]
        mid, hi = v & 0x3FF, v >> 10
        if mid not in norm:
            continue
        want = norm[mid][0]
        if want == hi:
            continue
        if not whole:
            # Mandarin South: the new, passable cliff paint, plus any sea tile
            # the rework left standing at walking height (1FB at (89,50) is a
            # wave the player would have walked on).
            if was is not None and struct.unpack_from('<H', was, i)[0] == v:
                continue                      # the author's, not the rework's
            cliff = mid in CLIFF and (hi & 3) == 0
            drowned = mid in SEA and (hi >> 2) == 3
            if not (cliff or drowned):
                continue
        struct.pack_into('<H', data, i, (want << 10) | mid)
        tally[(mid, hi, want)] += 1
    print('  %s: %d cells' % (label, sum(tally.values())))
    for (mid, hi, want), n in tally.most_common(12):
        print('      %03X  %s -> %-9s %5d   (author: %d of %d elsewhere)'
              % (mid, fmt(hi), fmt(want), n, norm[mid][1], norm[mid][2]))
    return bytes(data), sum(tally.values())


def check_doorway(index, msdata, bedata):
    mw = index[MS]['width']
    bw = index[BEACH]['width']
    get = lambda d, w, x, y: struct.unpack_from('<H', d, 2 * (y * w + x))[0]
    open_rows = []
    for y in range(index[MS]['height']):
        yb = y + 25                      # connection offset -25
        if not (0 <= yb < index[BEACH]['height']):
            continue
        a = get(msdata, mw, mw - 1, y)
        b = get(bedata, bw, 0, yb)
        if ((a >> 10) & 3) == 0 and ((b >> 10) & 3) == 0 and (a >> 12) == (b >> 12):
            open_rows.append((y, yb, a & 0x3FF, b & 0x3FF))
    if not open_rows:
        print('  !! the seam is now sealed -- no row is passable on both sides')
    else:
        print('  doorway: %d row(s) passable both sides -- MS y%d-%d / beach y%d-%d'
              % (len(open_rows), open_rows[0][0], open_rows[-1][0],
                 open_rows[0][1], open_rows[-1][1]))
    return open_rows


def fix_events(index, write):
    bw, bh = index[BEACH]['width'], index[BEACH]['height']
    p = os.path.join(REPO, 'data/maps/OrangeIslands_Mandarin_South_Beach/map.json')
    mj = json.load(open(p))
    # The one surviving sign still stood at x=101 on a map that is now 91 wide.
    # It reads as the town sign, so it belongs on the cliff beside the doorway
    # the player comes through from Mandarin South.
    HOME = (4, 31)
    moved = []
    for ev in mj.get('bg_events') or []:
        if ev['x'] >= bw or ev['y'] >= bh:
            moved.append((ev['x'], ev['y']))
            ev['x'], ev['y'] = HOME
    if moved:
        print('  beach: %d sign(s) were off the %dx%d map %s -> moved to %s, on the '
              'cliff beside the doorway' % (len(moved), bw, bh, moved, HOME))
        if write:
            with open(p, 'w', newline='\n') as f:
                json.dump(mj, f, indent=2)
                f.write('\n')

    # Mandarin South: every sea cell on the Route 8 row wants a trigger.
    mp = os.path.join(REPO, 'data/maps/OrangeIslands_Mandarin_South/map.json')
    mj = json.load(open(mp))
    L = index[MS]
    raw = open(os.path.join(REPO, L['blockdata_filepath']), 'rb').read()
    g = lambda x, y: struct.unpack_from('<H', raw, 2 * (y * L['width'] + x))[0]
    row = L['height'] - 1
    sea = {x for x in range(L['width'])
           if (g(x, row) & 0x3FF) in (0x12B, 0x12A, 0x12C) and ((g(x, row) >> 10) & 3) == 0}
    have = {c['x'] for c in mj['coord_events']
            if c['y'] == row and 'Route8' in (c.get('script') or '')}
    missing = sorted(sea - have)
    if missing:
        print('  Mandarin South: row %d sea with no Route 8 trigger at x=%s -- added'
              % (row, missing))
        for x in missing:
            mj['coord_events'].append({
                "type": "trigger", "x": x, "y": row, "elevation": 0,
                "var": "VAR_TEMP_0", "var_value": "0",
                "script": "OrangeIslands_Mandarin_South_EventScript_SeaEdgeToRoute8"})
        if write:
            with open(mp, 'w', newline='\n') as f:
                json.dump(mj, f, indent=2)
                f.write('\n')


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


def fix_stray_warp_tiles(index, write):
    """A walkable warp-behaviour tile with no warp event on it is a trap.

    Mandarin South has one at (37,15): a south-arrow-warp metatile the author
    used as rooftop decoration, left walkable while every roof tile around it
    is solid. It predates this rework and the player cannot reach a rooftop, but
    it stops the map checker before it gets to the new seam, so give it the
    collision its neighbours have.
    """
    for lid, mapdir in ((MS, 'OrangeIslands_Mandarin_South'),
                        (BEACH, 'OrangeIslands_Mandarin_South_Beach')):
        l = index[lid]
        pb = open(os.path.join(tileset_dir(l['primary_tileset']),
                               'metatile_attributes.bin'), 'rb').read()
        sb = open(os.path.join(tileset_dir(l['secondary_tileset']),
                               'metatile_attributes.bin'), 'rb').read()
        attrs = list(struct.unpack('<' + 'I' * ((len(pb) + len(sb)) // 4), pb + sb))
        w, h = l['width'], l['height']
        path = os.path.join(REPO, l['blockdata_filepath'])
        data = bytearray(open(path, 'rb').read())
        header = json.load(open(os.path.join(REPO, 'data/maps', mapdir, 'map.json')))
        doors = {(wp['x'], wp['y']) for wp in header['warp_events']}
        hits = []
        for y in range(h):
            for x in range(w):
                i = 2 * (y * w + x)
                v = struct.unpack_from('<H', data, i)[0]
                if v & 0xC00 or (x, y) in doors:
                    continue
                if 0x60 <= (attrs[v & 0x3FF] & 0x1FF) <= 0x7F:
                    hits.append((x, y))
                    struct.pack_into('<H', data, i, v | (1 << 10))
        if hits:
            print('  %s: %d walkable warp tile(s) with no warp on them %s -> made solid'
                  % (mapdir, len(hits), hits[:4]))
            if write:
                open(path, 'wb').write(bytes(data))


def match_seam(index, write):
    """Make every join Mandarin South has agree, cell for cell, where you can walk.

    Both maps draw the seam -- the edge column or row of each is shown beside
    the other -- so a walkable pair that is not the same block changes texture
    underfoot as you cross. Mandarin South has two: the beach on its right and
    Route 18 above it, and the rework left a handful of pairs disagreeing.

    A run of two or more walkable pairs is a way through, and the Mandarin South
    side takes the neighbour's block so the neighbour's own ground carries
    across unbroken. A run of exactly one is a hole rather than a door -- the
    single cell at the foot of the beach cliff, where 0A8 happens to be walkable
    on one side and sand on the other -- and gets sealed with the wall block
    from the cell beside it.

    Water pairs are left alone; the ocean meeting the ocean needs no agreement.
    """
    WATER = {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x1A, 0x1B,
             0x50, 0x51, 0x52, 0x53}

    def attrs_of(l):
        pb = open(os.path.join(tileset_dir(l['primary_tileset']),
                               'metatile_attributes.bin'), 'rb').read()
        sb = open(os.path.join(tileset_dir(l['secondary_tileset']),
                               'metatile_attributes.bin'), 'rb').read()
        return list(struct.unpack('<' + 'I' * ((len(pb) + len(sb)) // 4), pb + sb))

    ms = index[MS]
    header = json.load(open(os.path.join(
        REPO, 'data/maps/OrangeIslands_Mandarin_South/map.json')))
    byid = {}
    for p in os.listdir(os.path.join(REPO, 'data/maps')):
        f = os.path.join(REPO, 'data/maps', p, 'map.json')
        if os.path.exists(f):
            h = json.load(open(f))
            byid[h['id']] = h

    mp = os.path.join(REPO, ms['blockdata_filepath'])
    md = bytearray(open(mp, 'rb').read())
    ma = attrs_of(ms)

    for c in header.get('connections') or []:
        other = byid.get(c['map'])
        if not other:
            continue
        ol = index[other['layout']]
        op = os.path.join(REPO, ol['blockdata_filepath'])
        od = bytearray(open(op, 'rb').read())
        oa = attrs_of(ol)
        vertical = c['direction'] in ('up', 'down')
        span = ms['width'] if vertical else ms['height']
        olimit = ol['width'] if vertical else ol['height']

        def mine(t):
            x, y = (t, 0 if c['direction'] == 'up' else ms['height'] - 1) if vertical \
                else (0 if c['direction'] == 'left' else ms['width'] - 1, t)
            return 2 * (y * ms['width'] + x)

        def theirs(t):
            x, y = (t, ol['height'] - 1 if c['direction'] == 'up' else 0) if vertical \
                else (ol['width'] - 1 if c['direction'] == 'left' else 0, t)
            return 2 * (y * ol['width'] + x)

        pairs = []
        for t in range(span):
            ot = t - c['offset']
            if not 0 <= ot < olimit:
                continue
            v = struct.unpack_from('<H', md, mine(t))[0]
            ov = struct.unpack_from('<H', od, theirs(ot))[0]
            if v & 0xC00 or ov & 0xC00:
                continue
            if (ma[v & 0x3FF] & 0x1FF) in WATER or (oa[ov & 0x3FF] & 0x1FF) in WATER:
                continue
            pairs.append((t, ot, v, ov))

        runs, run = [], []
        for item in pairs:
            if run and item[0] != run[-1][0] + 1:
                runs.append(run)
                run = []
            run.append(item)
        if run:
            runs.append(run)

        doors, holes = [], []
        for r in runs:
            if len(r) >= 2:
                for t, ot, v, ov in r:
                    if v != ov:
                        struct.pack_into('<H', md, mine(t), ov)
                        doors.append((t, '%04X' % v, '%04X' % ov))
            else:
                t, ot, v, ov = r[0]
                wall = next((struct.unpack_from('<H', od, theirs(n))[0]
                             for n in (ot - 1, ot + 1) if 0 <= n < olimit
                             and struct.unpack_from('<H', od, theirs(n))[0] & 0xC00), None)
                if wall is None:
                    print('  seam with %s: lone gap at %d, nothing solid to seal it with'
                          % (c['map'], t))
                    continue
                struct.pack_into('<H', od, theirs(ot), wall)
                holes.append((t, '%04X' % ov, '%04X' % wall))

        short = c['map'].replace('MAP_ORANGE_ISLANDS_', '')
        if doors:
            print('  seam with %s: %d crossing cell(s) retextured so the ground '
                  'matches %s' % (short, len(doors), doors))
        if holes:
            print('  seam with %s: %d one-tile hole(s) sealed on the far side %s'
                  % (short, len(holes), holes))
        if write and holes:
            open(op, 'wb').write(bytes(od))
    if write:
        open(mp, 'wb').write(bytes(md))


def fix_sea_crossings(write):
    """The reworked coast moved the water, so the inbound sea crossings miss.

    Route 6 lands on Mandarin South's column 94 at the player's row + 10, and
    Route 8 lands on row 74 at the player's column - 36. Column 94 is now cliff
    above row 68, and row 74 is cliff at x 1-2 and 93, so both clamps have to
    come in to the water that is actually left.
    """
    edits = [
        ('data/maps/OrangeIslands_Route_6/scripts.inc',
         'SeaEdgeToMandarinSouth',
         # MS column 94 is ocean on rows 68-73; row 74 is dry ground sitting at
         # water height, which is no place to surface. R6 row + 10 = MS row.
         [('\tcompare VAR_0x8005, 0\n', '\tcompare VAR_0x8005, 58\n'),
          ('\tcompare VAR_0x8005, 65\n', '\tcompare VAR_0x8005, 63\n'),
          ('\tcompare VAR_0x8005, 64\n', '\tcompare VAR_0x8005, 63\n'),
          ('\tsetvar VAR_0x8005, 0\n', '\tsetvar VAR_0x8005, 58\n'),
          ('\tsetvar VAR_0x8005, 65\n', '\tsetvar VAR_0x8005, 63\n'),
          ('\tsetvar VAR_0x8005, 64\n', '\tsetvar VAR_0x8005, 63\n')]),
        ('data/maps/OrangeIslands_Route_8/scripts.inc',
         'SeaEdgeToMandarinSouth',
         # MS row 74 is open water on columns 3-92; R8 column - 36 = MS column.
         [('\tcompare VAR_0x8004, 36\n', '\tcompare VAR_0x8004, 39\n'),
          ('\tcompare VAR_0x8004, 131\n', '\tcompare VAR_0x8004, 128\n'),
          ('\tsetvar VAR_0x8004, 36\n', '\tsetvar VAR_0x8004, 39\n'),
          ('\tsetvar VAR_0x8004, 131\n', '\tsetvar VAR_0x8004, 128\n')]),
    ]
    for rel, label, pairs in edits:
        p = os.path.join(REPO, rel)
        s = open(p, encoding='utf-8').read()
        start = s.index(label)
        end = s.find('\n\n@', start)
        end = len(s) if end < 0 else end
        block, done = s[start:end], 0
        for old, new in pairs:
            if old in block:
                block = block.replace(old, new)
                done += 1
        if done:
            print('  %s: %d clamp(s) pulled in to the water that is left'
                  % (os.path.basename(os.path.dirname(rel)), done))
            if write:
                open(p, 'w', encoding='utf-8', newline='\n').write(s[:start] + block + s[end:])


def fix_route12_crossing(write):
    """Route 12 still rows across into what is now a cliff.

    It landed on Mandarin South's column 94 at the player's row + 16, which was
    open water from row 20 down. The rework walled that column from row 0 to 67,
    leaving rows 68-73, and Route 12's own open edge (rows 4-31) cannot reach
    them by carrying the row across. So it lands at one spot in the channel that
    is left -- the same answer the author already reached for this file's
    Trovita crossing, whose comment explains the reasoning.
    """
    p = os.path.join(REPO, 'data/maps/OrangeIslands_Route_12/scripts.inc')
    s = open(p, encoding='utf-8').read()
    label = 'OrangeIslands_Route_12_EventScript_SeaEdgeToMandarinSouth'
    if label + '::' not in s or 'only rows' in s:
        return     # already landed mid-channel
    start = s.index(label + '::')
    end = s.index('\n\n@', start)
    block = ('@ Mandarin South\'s east coast is now cliff from row 0 to 67; only rows\n'
             '@ 68-73 are open water, and this edge (rows 4-31) cannot reach them by\n'
             '@ carrying the row across, so the crossing lands mid-channel instead.\n'
             '%s::\n'
             '\tsetvar VAR_0x8004, 94\n'
             '\tsetvar VAR_0x8005, 71\n'
             '\twarp MAP_ORANGE_ISLANDS_MANDARIN_SOUTH, VAR_0x8004, VAR_0x8005\n'
             '\twaitstate\n'
             '\tend' % label)
    print('  Route 12: the Mandarin South crossing now lands at (94,71), the only '
          'open water left on that coast')
    if write:
        open(p, 'w', encoding='utf-8', newline='\n').write(s[:start] + block + s[end:])


def fix_dead_warp(write):
    """The beach is an edge connection now, so its old warp partner is dangling."""
    p = os.path.join(REPO, 'data/maps/OrangeIslands_Mandarin_South/map.json')
    mj = json.load(open(p))
    keep = [wp for wp in mj['warp_events']
            if wp['dest_map'] != 'MAP_ORANGE_ISLANDS_MANDARIN_SOUTH_BEACH']
    if len(keep) != len(mj['warp_events']):
        gone = [wp for wp in mj['warp_events'] if wp not in keep]
        print('  Mandarin South: dropped the dead beach warp at %s -- the beach has '
              'no warps left to land on' % [(w['x'], w['y']) for w in gone])
        mj['warp_events'] = keep
        if write:
            with open(p, 'w', newline='\n') as f:
                json.dump(mj, f, indent=2)
                f.write('\n')


def fix_landmarks(write):
    """Redraw the two reachability fixtures for the maps as they are now.

    The beach one still described the old 132x50 layout. Mandarin South's
    pointed at (43,36), a tree inside the fenced plantation that was never
    walkable -- the checker simply never got that far, because the map tripped
    an earlier assertion first. Its replacement is (95,8), the cell on the east
    edge the player steps through to reach the beach, so from now on the
    fixture fails the moment that crossing is walled up again.
    """
    p = os.path.join(REPO, 'docs/orange-islands/island_landmarks.json')
    doc = json.load(open(p))
    fresh = {
        'Mandarin_South_Beach': [[45, 45], [[0, 33], [30, 4], [60, 19], [35, 73],
                                            [20, 60]], False],
        'Mandarin_South': [[34, 63], [[28, 33], [22, 26], [35, 27], [48, 25],
                                      [95, 8], [35, 39], [32, 39]], False],
        # Route 18 had no fixture at all, which is why Mandarin South's seam
        # check gave up: the checker only holds data for maps it has landmarks
        # for, and Route 18 is Mandarin South's other neighbour. Giving it one
        # lets both of its seams be checked, which matters now the route is two
        # columns wider than it was.
        'Route_18': [[48, 39], [[46, 8], [22, 28], [73, 20], [56, 39]], False],
    }
    hit = [k for k, v in fresh.items() if doc.get(k) != v]
    if hit:
        print('  landmarks: redrawn for %s' % ', '.join(hit))
        doc.update(fresh)
        if write:
            with open(p, 'w', newline='\n') as f:
                json.dump(doc, f, indent=1)
                f.write('\n')


def main():
    write = '--write' in sys.argv
    doc, index = layouts()
    norm = norms(index)
    print('learned a collision/elevation norm for %d metatiles' % len(norm))

    bedata, nb = fix(index[BEACH], norm, True, 'Mandarin South Beach (whole map)')
    msdata, nm = fix(index[MS], norm, False, 'Mandarin South (new cliff paint only)')
    check_doorway(index, msdata, bedata)
    # The block data goes down first; the stray-warp pass reads it back.
    if write:
        open(os.path.join(REPO, index[BEACH]['blockdata_filepath']), 'wb').write(bedata)
        open(os.path.join(REPO, index[MS]['blockdata_filepath']), 'wb').write(msdata)
        print('  block data written: %d + %d cells' % (nb, nm))

    fix_events(index, write)
    fix_stray_warp_tiles(index, write)
    match_seam(index, write)
    fix_sea_crossings(write)
    fix_route12_crossing(write)
    fix_dead_warp(write)
    fix_landmarks(write)
    if not write:
        print('(report only -- pass --write to apply)')


if __name__ == '__main__':
    main()
