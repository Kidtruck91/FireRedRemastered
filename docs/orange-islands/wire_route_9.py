"""Give Route 9 a home on the region map and make all three of its crossings two-way.

Route 9 was a 52x116 sea route that nothing led into. It had three exits, every
one of them one-way, and no square on the region map at all -- so the link map
listed it among the maps you could leave but never reach.

Its terrain says exactly what the author intended. The only open water on its
border is in three places:

    north   row 0,   x 8-43     a wide channel at the top
    west    col 0,   y 34-79    a tall channel on the left, two thirds up
    east    col 51,  y 106-113  a narrow channel at the bottom right

and its south edge has no water at all. So: come in from the north, branch west
partway down, leave east at the bottom.

The trouble was where those three went. North led to Mandarin South, west to
Route 17 and east to Navel Beach -- three maps spread across eleven columns and
two rows of the region map, with Route 10, Kinnow and Route 8 filling the row
between them. No single square can be south of Mandarin South, east of Route 17
and west of Navel at once.

The southern band of the region map is empty, so Route 9 goes there, from (4,10)
to (13,10), running under Route 10, Kinnow and Route 8. That also settles it
numerically: along row 9 the chain reads Route 10, Kinnow, Route 8, and Route 9
is the sea beneath them.

Which frees the north channel to do something better than Mandarin South, whose
south coast is Route 8's anyway. Kinnow sits at (8,9), directly above the lane,
and its entire south coast -- fifty-two tiles of open water from x 11 to x 62 --
is unused. So the north channel lands on Kinnow, and Route 10 -> Kinnow ->
Route 8 gains the sea route numbered between them.

The three crossings, each a matched band so the player's position carries over:

    Route 9 row 0,   x 8-43     <->  Kinnow row 65,  x 13-48   (offset +5)
    Route 9 col 0,   y 34-72    <->  Route 17 col 39, y 4-42   (offset -30)
    Route 9 col 51,  y 106-113  <->  Navel row 64,   x 2-9     (offset -104)

Route 17's east edge already gives rows 43-55 to Route 10, so Route 9 takes the
rows above it and Route 9's own band is pulled in from 79 to 72 to match; the
triggers below that still work and clamp to the bottom of the band. Navel's
south coast already gives x 23-66 to Route 5, so Route 9 takes the south-west
corner, which is the side it approaches from.

    python3 docs/orange-islands/wire_route_9.py
"""
import json
import os
import re
import struct

REPO = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))

# The southern sea lane: row 10, under Route 10, Kinnow and Route 8.
REGION_ROW = 10
REGION_COLS = range(4, 14)

# (map, script suffix, destination map, the band, how to carry the position)
CROSSINGS = [
    # Route 9's side
    dict(src='OrangeIslands_Route_9', name='SeaEdgeToKinnow',
         dest='MAP_ORANGE_ISLANDS_KINNOW', axis='x', at=0, lo=8, hi=43,
         shift=+5, fixed=('y', 64),
         note="Kinnow's whole south coast was unused; this is the sea below it."),
    dict(src='OrangeIslands_Route_9', name='SeaEdgeToRoute17',
         dest='MAP_ORANGE_ISLANDS_ROUTE_17', axis='y', at=0, lo=34, hi=72,
         shift=-30, fixed=('x', 38),
         note="Route 10 holds Route 17's east edge from row 43 down."),
    dict(src='OrangeIslands_Route_9', name='SeaEdgeToNavel',
         dest='MAP_ORANGE_ISLANDS_NAVEL', axis='y', at=51, lo=106, hi=113,
         shift=-104, fixed=('y', 63), swap=True,
         note="Navel's south-west corner; Route 5 has x 23-66 of that coast."),
    # and the legs back, which never existed
    dict(src='OrangeIslands_Kinnow', name='SeaEdgeToRoute9',
         dest='MAP_ORANGE_ISLANDS_ROUTE_9', axis='x', at=65, lo=13, hi=48,
         shift=-5, fixed=('y', 1),
         note='the return of the crossing above'),
    dict(src='OrangeIslands_Route_17', name='SeaEdgeToRoute9',
         dest='MAP_ORANGE_ISLANDS_ROUTE_9', axis='y', at=39, lo=4, hi=42,
         shift=+30, fixed=('x', 1),
         note='above the rows Route 10 already uses'),
    dict(src='OrangeIslands_Navel', name='SeaEdgeToRoute9',
         dest='MAP_ORANGE_ISLANDS_ROUTE_9', axis='x', at=64, lo=2, hi=9,
         shift=+104, fixed=('x', 50), swap=True,
         note='west of the stretch Route 5 uses'),
]


def script_for(c):
    """One sea crossing, in the shape the rest of the region already uses."""
    label = 'OrangeIslands_%s_EventScript_%s' % (c['src'].replace('OrangeIslands_', ''), c['name'])
    label = '%s_EventScript_%s' % (c['src'], c['name'])
    # The Navel crossing turns a corner: a column of water on one side meets a
    # row of it on the other, so the carried position has to change axis. The
    # trick is to read the player's position into the registers the other way
    # round -- getplayerxy takes the two variables as arguments -- rather than
    # shuffling it afterwards with copyvar, which is both clearer and avoids
    # ever holding a coordinate in the register the warp will read as the other
    # axis.
    swap = c.get('swap', False)
    if swap:
        read = 'getplayerxy VAR_0x8005, VAR_0x8004'
        var = 'VAR_0x8005' if c['axis'] == 'x' else 'VAR_0x8004'
    else:
        read = 'getplayerxy VAR_0x8004, VAR_0x8005'
        var = 'VAR_0x8004' if c['axis'] == 'x' else 'VAR_0x8005'
    out_var = var
    fixed_axis, fixed_val = c['fixed']
    fixed_var = 'VAR_0x8004' if fixed_axis == 'x' else 'VAR_0x8005'

    L = []
    L.append('@ %s' % c['note'])
    L.append('%s::' % label)
    L.append('\t%s' % read)
    L.append('\tcompare %s, %d' % (var, c['lo']))
    L.append('\tgoto_if_lt %s_Lo' % label)
    L.append('\tcompare %s, %d' % (var, c['hi']))
    L.append('\tgoto_if_gt %s_Hi' % label)
    L.append('%s_Go:' % label)
    if c['shift'] > 0:
        L.append('\taddvar %s, %d' % (out_var, c['shift']))
    elif c['shift'] < 0:
        L.append('\tsubvar %s, %d' % (out_var, -c['shift']))
    L.append('\tsetvar %s, %d' % (fixed_var, fixed_val))
    L.append('\twarp %s, VAR_0x8004, VAR_0x8005' % c['dest'])
    L.append('\twaitstate')
    L.append('\tend')
    L.append('%s_Lo:' % label)
    L.append('\tsetvar %s, %d' % (var, c['lo']))
    L.append('\tgoto %s_Go' % label)
    L.append('%s_Hi:' % label)
    L.append('\tsetvar %s, %d' % (var, c['hi']))
    L.append('\tgoto %s_Go' % label)
    L.append('')
    return label, '\n'.join(L)


# A walkable warp-behaviour tile with no warp event on it: a door on a
# decorative building in the south of Kinnow, inside the stretch of coast Route
# 9 now arrives at. The player walks into it and nothing happens. It predates
# this work and nothing led to that coast before, so it was never noticed.
#
# Route 9's cave mouth at (9,27) looked like the same fault and is not. A
# warp-behaviour tile fires when the player steps onto it, so walkable is what
# it should be -- 142 of vanilla's 147 are, including this very metatile on
# Route 2, Route 4, Route 10 and Route 11. The map checker was reading the
# blocks without the warp events beside them; it now reads both.
SEAL = [('LAYOUT_ORANGE_ISLANDS_KINNOW', 33, 48)]


def layout_index():
    return {l['id']: l for l in
            json.load(open(os.path.join(REPO, 'data/layouts/layouts.json')))['layouts']}


def seal_stray_warps(layouts):
    """Give each stray warp tile the collision its neighbours have."""
    for layout_id, x, y in SEAL:
        layout = layouts[layout_id]
        p = os.path.join(REPO, layout['blockdata_filepath'])
        raw = bytearray(open(p, 'rb').read())
        at = 2 * (y * layout['width'] + x)
        v, = struct.unpack_from('<H', raw, at)
        if v & 0xC00:
            continue
        struct.pack_into('<H', raw, at, v | 0x400)
        open(p, 'wb').write(raw)
        print('  %s: sealed the warp tile at (%d,%d), %04X -> %04X'
              % (layout_id.replace('LAYOUT_ORANGE_ISLANDS_', ''), x, y, v, v | 0x400))


def main():
    # --- the region map ----------------------------------------------------
    p = os.path.join(REPO, 'src/data/region_map/region_map_layout_orange.h')
    src = open(p, encoding='utf-8').read()
    rows = re.findall(r'\{[^{}]*MAPSEC[^{}]*\}', src)
    cells = [c.strip() for c in rows[REGION_ROW].strip('{}').split(',') if c.strip()]
    changed = 0
    for x in REGION_COLS:
        if cells[x] == 'MAPSEC_NONE':
            cells[x] = 'MAPSEC_ORANGE_ROUTE_9'
            changed += 1
    if changed:
        width = max(len(c) for c in cells)
        rebuilt = '{' + ', '.join('%-*s' % (width, c) for c in cells).rstrip() + '}'
        src = src.replace(rows[REGION_ROW], rebuilt, 1)
        open(p, 'w', encoding='utf-8', newline='\n').write(src)
    print('  region map: Route 9 given %d squares on row %d, columns %d-%d'
          % (changed, REGION_ROW, REGION_COLS[0], REGION_COLS[-1]))

    # --- the crossings -----------------------------------------------------
    by_map = {}
    for c in CROSSINGS:
        by_map.setdefault(c['src'], []).append(c)

    for mapname, crossings in by_map.items():
        d = os.path.join(REPO, 'data/maps', mapname)
        sp = os.path.join(d, 'scripts.inc')
        s = open(sp, encoding='utf-8').read()
        mj = json.load(open(os.path.join(d, 'map.json')))

        # Route 9's old crossings are replaced wholesale: the Mandarin South one
        # is gone and the Navel one stops being a single fixed landing spot.
        for stale in ('SeaEdgeToMandarinSouth', 'SailToNavelBeach'):
            label = '%s_EventScript_%s' % (mapname, stale)
            if label + '::' not in s:
                continue
            start = s.index(label + '::')
            # take the comment line above it too, if there is one
            head = s.rfind('\n@', 0, start)
            if head != -1 and s.count('\n', head, start) == 1:
                start = head + 1
            end = s.find('\n\n@', start)
            end = len(s) if end < 0 else end + 1
            s = s[:start] + s[end:]
            mj['coord_events'] = [e for e in mj['coord_events']
                                  if stale not in (e.get('script') or '')]
            print('  %s: dropped %s' % (mapname, stale))

        added = []
        for c in crossings:
            label, block = script_for(c)
            if label + '::' in s:
                continue
            s = s.rstrip('\n') + '\n\n' + block
            band = [(c['at'], i) if c['axis'] == 'y' else (i, c['at'])
                    for i in range(c['lo'], c['hi'] + 1)]
            for (x, y) in band:
                mj['coord_events'].append({
                    "type": "trigger", "x": x, "y": y, "elevation": 0,
                    "var": "VAR_TEMP_0", "var_value": "0", "script": label})
            added.append((c['name'], len(band)))

        # Route 9's west band was 34-79 and landed on Route 17 rows 4-49, which
        # runs into the rows Route 10 holds from 43 down. The band's top stays
        # where it is and its clamp comes in to 72, so everything below that
        # still crosses and simply arrives at the bottom of Route 17's stretch.
        if mapname == 'OrangeIslands_Route_9':
            s = s.replace('\tcompare VAR_0x8005, 79\n'
                          '\tgoto_if_gt OrangeIslands_Route_9_EventScript_SeaEdgeToRoute17_Hi',
                          '\tcompare VAR_0x8005, 72\n'
                          '\tgoto_if_gt OrangeIslands_Route_9_EventScript_SeaEdgeToRoute17_Hi')
            s = s.replace('OrangeIslands_Route_9_EventScript_SeaEdgeToRoute17_Hi:\n'
                          '\tsetvar VAR_0x8005, 79',
                          'OrangeIslands_Route_9_EventScript_SeaEdgeToRoute17_Hi:\n'
                          '\tsetvar VAR_0x8005, 72')
        # Route 9's west band was 34-79; Route 17 only has room to row 42, so the
        # triggers below 72 stay and clamp rather than being thrown away.
        if mapname == 'OrangeIslands_Route_9':
            label = 'OrangeIslands_Route_9_EventScript_SeaEdgeToRoute17'
            have = {e['y'] for e in mj['coord_events'] if e.get('script') == label}
            for y in range(73, 80):
                if y not in have:
                    mj['coord_events'].append({
                        "type": "trigger", "x": 0, "y": y, "elevation": 0,
                        "var": "VAR_TEMP_0", "var_value": "0", "script": label})

        open(sp, 'w', encoding='utf-8', newline='\n').write(s)
        with open(os.path.join(d, 'map.json'), 'w', newline='\n') as f:
            json.dump(mj, f, indent=2)
            f.write('\n')
        if added:
            print('  %s: %s' % (mapname,
                                ', '.join('%s (%d tiles)' % a for a in added)))

    # --- the fault the crossing uncovered ----------------------------------
    seal_stray_warps(layout_index())


if __name__ == '__main__':
    main()
