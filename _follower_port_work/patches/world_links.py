#!/usr/bin/env python3
# Orange Islands world wiring, generated from one geography (WORLD positions in tiles, y grows south).
# - Seamless map connections are kept for linked pairs that already have one; stale connections are dropped.
# - Every other link becomes a whole-edge sea link (edge_links.py): offsets/clamps/landings come from geometry.
# - Rock rims on sea routes are opened (2 deep) where a link needs water.
# Change a position or a link here and re-run:  python3 world_links.py   (then edge_links.py runs automatically)
import json, os, re, struct, subprocess, sys
HERE = os.path.dirname(os.path.abspath(__file__))
WORLD = {
    'Valencia': (0, 0), 'Route_1': (0, -63), 'Route_4': (-120, -80), 'Sunburst': (-184, -88),
    'Valencia_Beach': (67, 16), 'Route_3': (163, -2), 'Mikan_Beach': (303, -7), 'Mikan': (303, -79),
    'Route_5': (311, -203), 'Navel': (290, -268), 'Navel_Beach': (290, -338),
    'Route_6': (223, -170), 'Mandarin_South': (127, -180), 'Route_18': (127, -220),
    'Route_8': (91, -104), 'Kinnow': (19, -150), 'Route_10': (-85, -200),
    'Route_17': (-125, -232), 'Moro': (-189, -232), 'Route_7': (-190, -176),
    'Route_13': (131, -300), 'Kumquat': (131, -360),
    'Route_16': (95, -312), 'Pinkan': (55, -312), 'Route_15': (19, -312),
    'Route_14': (115, -432), 'Trovita': (203, -472), 'Trovita_Beach': (203, -416), 'Route_11': (210, -380),
    'Pummelo': (420, -520),
}
LINKS = [
    ('Valencia', 'up', 'Route_1'), ('Route_1', 'left', 'Route_4'), ('Route_4', 'left', 'Sunburst'),
    ('Valencia', 'right', 'Valencia_Beach'), ('Valencia_Beach', 'right', 'Route_3'), ('Route_3', 'right', 'Mikan_Beach'), ('Mikan_Beach', 'up', 'Mikan'),
    ('Mikan', 'up', 'Route_5'), ('Route_5', 'up', 'Navel'), ('Navel', 'up', 'Navel_Beach'), ('Route_5', 'left', 'Route_6'),
    ('Route_6', 'left', 'Mandarin_South'), ('Mandarin_South', 'up', 'Route_18'), ('Mandarin_South', 'down', 'Route_8'),
    ('Route_8', 'left', 'Kinnow'), ('Kinnow', 'left', 'Route_10'), ('Route_10', 'left', 'Route_17'), ('Moro', 'right', 'Route_17'),
    ('Sunburst', 'up', 'Route_7'), ('Route_7', 'up', 'Moro'),
    ('Route_18', 'up', 'Route_13'), ('Route_13', 'up', 'Kumquat'), ('Route_13', 'left', 'Route_16'),
    ('Route_16', 'left', 'Pinkan'), ('Pinkan', 'left', 'Route_15'),
    ('Kumquat', 'up', 'Route_14'), ('Route_14', 'right', 'Trovita'), ('Trovita', 'down', 'Trovita_Beach'),
    ('Trovita_Beach', 'down', 'Route_11'), ('Route_11', 'right', 'Navel_Beach'),
]
if __name__ != '__main__': raise SystemExit
OPP = {'up': 'down', 'down': 'up', 'left': 'right', 'right': 'left'}
EDGE = {'up': 'top', 'down': 'bottom', 'left': 'left', 'right': 'right'}
const = lambda m: 'MAP_ORANGE_ISLANDS_' + m.upper()
L = {l['id']: l for l in json.load(open('data/layouts/layouts.json'))['layouts'] if 'id' in l}
J = {}; LAY = {}
for m in WORLD:
    p = 'data/maps/OrangeIslands_%s/map.json' % m; J[m] = json.load(open(p)); LAY[m] = L[J[m]['layout']]
size = {m: (LAY[m]['width'], LAY[m]['height']) for m in WORLD}
grid = {}
def load(m):
    if m not in grid:
        W, H = size[m]; b = open(LAY[m]['blockdata_filepath'], 'rb').read()
        grid[m] = [[struct.unpack_from('<H', b, (y * W + x) * 2)[0] for x in range(W)] for y in range(H)]
    return grid[m]
def save(m):
    open(LAY[m]['blockdata_filepath'], 'wb').write(b''.join(struct.pack('<H', v) for r in grid[m] for v in r))
surf = lambda v: (v >> 12) == 1 and not (v & 0xc00)
def edge_cells(m, side):
    W, H = size[m]
    return {'top': [(x, 0) for x in range(W)], 'bottom': [(x, H - 1) for x in range(W)],
            'left': [(0, y) for y in range(H)], 'right': [(W - 1, y) for y in range(H)]}[side]
def axis(side): return 0 if side in ('top', 'bottom') else 1
# 1. purge every old sea link (SeaEdge / SailTo triggers + script blocks) on the Orange maps
for m in WORLD:
    j = J[m]
    j['coord_events'] = [e for e in j.get('coord_events', []) if 'SeaEdgeTo' not in e['script'] and '_SailTo' not in e['script']]
    s = 'data/maps/OrangeIslands_%s/scripts.inc' % m; t = open(s).read()
    blocks = t.split('\n\n')
    t = '\n\n'.join(b for b in blocks if not re.search(r'_EventScript_(SeaEdgeTo|SailTo)\w*::', b)).rstrip('\n') + '\n'
    open(s, 'w', newline='\n').write(t)
# 2. connections: keep linked pairs that are already seamless, drop stale ones
pairs = {(a, b) for a, _, b in LINKS} | {(b, a) for a, _, b in LINKS}
seamless = set()
for m in WORLD:
    keep = []
    for c in (J[m].get('connections') or []):
        n = next((k for k in WORLD if const(k) == c['map']), None)
        if n and (m, n) in pairs: keep.append(c); seamless.add((m, n))
        else: print('drop connection', m, '->', c['map'])
    J[m]['connections'] = keep or 0
# 3. sea links from geometry
edges = []
for a, d, b in LINKS:
    for m, side, n in ((a, d, b), (b, OPP[d], a)):
        if (m, n) in seamless: continue
        e, f = EDGE[side], EDGE[OPP[side]]; ax = axis(e)
        off = WORLD[m][ax] - WORLD[n][ax]                    # dest coord = src coord + off
        ln = size[n][ax]; lm = size[m][ax]
        lo, hi = max(0, -off), min(lm, ln - off) - 1          # source coords that land on n's edge
        if hi < lo: print('NO OVERLAP', m, side, n); continue
        g = load(m)
        has_water = any(surf(g[y][x]) and lo <= (x, y)[ax] <= hi for x, y in edge_cells(m, e))
        if m.startswith('Route_') and not has_water:          # sealed rim: open it across the overlap
            water = max(((v & 0x3ff) for r in g for v in r if surf(v)), key=lambda t: sum((v & 0x3ff) == t for r in g for v in r if surf(v)))
            W, H = size[m]
            for c in range(lo + 1, hi):
                for k in (0, 1):
                    x, y = {'top': (c, k), 'bottom': (c, H - 1 - k), 'left': (k, c), 'right': (W - 1 - k, c)}[e]
                    if not surf(g[y][x]): g[y][x] = water | (1 << 12)
            save(m)
        # clamp to where n's facing edge has water
        gn = load(n); dest_ok = [c - off for (x, y) in edge_cells(n, f) for c in [(x, y)[ax]] if surf(gn[y][x]) and lo <= c - off <= hi]
        if n.startswith('Route_') and not dest_ok: dest_ok = [lo + 1, hi - 1]
        if not dest_ok: print('NO WATER on', n, f, 'for', m); continue
        clo, chi = min(dest_ok), max(dest_ok)
        land = {'top': size[n][1] - 2, 'bottom': 1, 'left': size[n][0] - 2, 'right': 1}[e]
        edges.append((m, e, n, off, (clo, chi), land))
for m in WORLD:
    open('data/maps/OrangeIslands_%s/map.json' % m, 'w', newline='\n').write(json.dumps(J[m], indent=2) + '\n')
# 4. write the EDGES table and run edge_links.py
p = os.path.join(HERE, 'edge_links.py'); s = open(p).read()
i = s.index('EDGES = ['); k = s.index('\n]\n', i) + 3
s = s[:i] + 'EDGES = [   # generated by world_links.py\n' + ''.join('    %r,\n' % (e,) for e in edges) + ']\n' + s[k:]
open(p, 'w', newline='\n').write(s)
print(len(edges), 'sea links,', len(seamless) // 2, 'seamless joins')
subprocess.check_call([sys.executable, p] + sorted({e[0] for e in edges}))
