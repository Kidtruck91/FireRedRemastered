"""Extract how every Orange Islands map connects, and place it on the game's own region map.

Four different mechanisms join maps in this hack, and only one of them is visible in
Porymap's connection editor, which is why a broken link is easy to miss:

    edge        a map connection in map.json
    warp        a warp_event's dest_map
    trigger     a coord_event whose script runs a warp -- most Orange sea crossings
    dive        a dive/emerge connection, or a setdivewarp in an ON_RESUME map script
    ferry       a Seagallop sailing: the script sets VAR_0x8006 to a SEAGALLOP_ id and
                hands off to EventScript_SetSail, and src/seagallop.c says which map
                that id lands on. Pummelo is reached this way, from Kumquat, so a tool
                that only reads the first four kinds calls the Orange League cut off.

This reads all four, works out which links are answered from the far side and which are
one-way, finds the maps nothing leads into, and places every map where the game itself puts it.

The positions are not invented. src/data/region_map/region_map_layout_orange.h is the
in-game region map: a 22x15 grid of MAPSEC values. Each map carries a
region_map_section, so a map sits on the cells its own section occupies, and maps that
share a section are spread across them. A section missing from that grid is reported --
a map with nowhere on the region map is usually one that was never finished.

It writes two things: link_map.json, which docs/orange-islands/link_map.html reads, and
link_map.png, the same graph drawn straight from Python so you can look at it without a
browser.

    python3 docs/orange-islands/build_link_map.py           # json + png
    python3 docs/orange-islands/build_link_map.py --no-png   # json only
"""
import json
import math
import os
import sys
import re
import struct
from collections import defaultdict, OrderedDict

REPO = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
OUT = os.path.join(REPO, 'docs/orange-islands/link_map.json')

WARP_CMD = re.compile(r'^\s*warp\w*\s+(MAP_\w+)', re.M)
DIVE_CMD = re.compile(r'^\s*setdivewarp\s+(MAP_\w+)', re.M)
FERRY_CMD = re.compile(r'^\s*setvar\s+VAR_0x8006,\s*(SEAGALLOP_\w+)', re.M)
JUMP_CMD = re.compile(r'^\s*(?:goto|call)\s+(\w+)', re.M)
COMPASS = {'left': 'West', 'right': 'East', 'up': 'North', 'down': 'South',
           'dive': 'Dive', 'emerge': 'Emerge'}

ISLANDS = ['Valencia', 'Mikan', 'Sunburst', 'Navel', 'Moro', 'Kinnow',
           'Mandarin', 'Trovita', 'Kumquat', 'Pinkan', 'Pummelo']
SEA = 'Sea routes'
OUTDOOR = {'MAP_TYPE_TOWN', 'MAP_TYPE_CITY', 'MAP_TYPE_ROUTE', 'MAP_TYPE_OCEAN_ROUTE',
           'MAP_TYPE_UNDERGROUND', 'MAP_TYPE_UNDERWATER'}


def island_of(dirname):
    head = dirname.removeprefix('OrangeIslands_').split('_')[0]
    return SEA if head in ('Route', 'Underwater') else (head if head in ISLANDS else SEA)


def block(text, label, depth=2):
    m = re.search(r'^%s::\s*$' % re.escape(label), text, re.M)
    if not m:
        return ''
    end = text.find('\n\n', m.end())
    body = text[m.end():end if end != -1 else len(text)]
    if depth > 0:
        for nxt in JUMP_CMD.findall(body):
            m2 = re.search(r'^%s:\s*$' % re.escape(nxt), text, re.M)
            if m2:
                e2 = text.find('\n\n', m2.end())
                body += '\n' + text[m2.end():e2 if e2 != -1 else len(text)]
    return body


def seagallop_destinations():
    """SEAGALLOP_* -> the MAP_ constant it puts the player on, from src/seagallop.c."""
    src = open(os.path.join(REPO, 'src/seagallop.c')).read()
    out = {}
    for sid, dest in re.findall(r'\[(SEAGALLOP_\w+)\]\s*=\s*\{MAP\((\w+)\)', src):
        out[sid] = 'MAP_' + dest
    return out


# --------------------------------------------------------------------------------------
# the same graph, drawn from Python
# --------------------------------------------------------------------------------------
FONT_DIR = '/usr/share/fonts/truetype/dejavu'
PALETTE = {
    'bg': (12, 23, 30), 'sea': (18, 43, 58), 'route': (23, 57, 76), 'land': (59, 53, 36),
    'cellEdge': (43, 71, 89), 'ink': (227, 237, 243), 'soft': (140, 166, 181),
    'two': (70, 98, 117), 'one': (226, 166, 71), 'ferry': (92, 196, 150),
    'bad': (232, 117, 92), 'tray': (232, 117, 92),
}


def render_png(doc, path, scale=2):
    from PIL import Image, ImageDraw, ImageFont

    def font(size, bold=False):
        name = 'DejaVuSans-Bold.ttf' if bold else 'DejaVuSans.ttf'
        try:
            return ImageFont.truetype(os.path.join(FONT_DIR, name), size)
        except OSError:
            return ImageFont.load_default()

    G = doc['grid']
    C = G['cell']
    W = int((G['w'] + 7.0) * C)
    H = int(G['h'] * C) + 54
    img = Image.new('RGB', (W * scale, H * scale), PALETTE['bg'])
    d = ImageDraw.Draw(img)
    S = lambda v: int(round(v * scale))
    TOP = 46

    f_sec = font(7 * scale)
    f_node = font(9 * scale)
    f_title = font(15 * scale, True)
    f_key = font(10 * scale)

    d.text((S(10), S(12)), 'Orange Islands link map', font=f_title, fill=PALETTE['ink'])
    key = ('%d maps   %d links   %d one-way   %d nothing leads in'
           % (len(doc['nodes']), len(doc['edges']), len(doc['oneWay']), len(doc['unreachable'])))
    d.text((S(10), S(32)), key, font=f_key, fill=PALETTE['soft'])

    # the region plate
    d.rectangle([S(0), S(TOP), S(G['w'] * C), S(TOP + G['h'] * C)], fill=PALETTE['sea'])
    for sec, spots in doc['grid']['cells'].items():
        route = sec.startswith('ORANGE_ROUTE')
        for cx, cy in spots:
            d.rectangle([S(cx * C), S(TOP + cy * C), S((cx + 1) * C), S(TOP + (cy + 1) * C)],
                        fill=PALETTE['route'] if route else PALETTE['land'],
                        outline=PALETTE['cellEdge'])
        mx = sum(p[0] for p in spots) / len(spots)
        my = sum(p[1] for p in spots) / len(spots)
        label = sec.replace('ORANGE_', '').replace('_', ' ')
        d.text((S((mx + 0.5) * C), S(TOP + (my + 0.5) * C + 15)), label,
               font=f_sec, fill=PALETTE['soft'], anchor='mm')

    nodes = {n['id']: n for n in doc['nodes']}
    # the tray for maps with nowhere on the region map
    if doc['unplaced']:
        ys = [nodes[m]['y'] for m in doc['unplaced']]
        d.rounded_rectangle([S((G['w'] + 0.55) * C), S(TOP + min(ys) - 26),
                             S((G['w'] + 5.75) * C), S(TOP + max(ys) + 26)],
                            radius=S(8), outline=PALETTE['tray'], width=max(1, scale))
        d.text((S((G['w'] + 0.75) * C), S(TOP + min(ys) - 38)), 'Not on the region map',
               font=f_key, fill=PALETTE['tray'])

    drawn = set()
    for e in doc['edges']:
        a, b = nodes.get(e['from']), nodes.get(e['to'])
        if not a or not b:
            continue
        key2 = tuple(sorted((e['from'], e['to']))) + (e['kind'],)
        if key2 in drawn:
            continue
        drawn.add(key2)
        col = PALETTE['ferry'] if e['kind'] == 'ferry' else (
            PALETTE['two'] if e['twoWay'] else PALETTE['one'])
        wid = max(1, int(scale * (2.2 if not e['twoWay'] else 1.5)))
        d.line([S(a['x']), S(TOP + a['y']), S(b['x']), S(TOP + b['y'])], fill=col, width=wid)

    # Labels are nudged off each other: in a section like Moro's ghost ship, eight maps
    # share one region cell, and stacked names are unreadable.
    placed = []

    def slot(cx, cy, w, h):
        for step in range(0, 9):
            for dy in ((-step, step) if step else (0,)):
                box = (cx - w / 2, cy + dy - h / 2, cx + w / 2, cy + dy + h / 2)
                if all(box[2] < q[0] or box[0] > q[2] or box[3] < q[1] or box[1] > q[3]
                       for q in placed):
                    placed.append(box)
                    return cy + dy
            step += 0
        placed.append((cx - w / 2, cy - h / 2, cx + w / 2, cy + h / 2))
        return cy

    for n in sorted(doc['nodes'], key=lambda n: n['y']):
        r = 5 if n['type'] in ('Route', 'Underwater') else 6.8
        hue = (doc['islands'].index(n['island']) / max(1, len(doc['islands']))) if n['island'] in doc['islands'] else 0.55
        import colorsys
        rgb = tuple(int(v * 255) for v in colorsys.hls_to_rgb(hue, 0.55, 0.45))
        box = [S(n['x'] - r), S(TOP + n['y'] - r), S(n['x'] + r), S(TOP + n['y'] + r)]
        d.ellipse(box, fill=rgb,
                  outline=PALETTE['bad'] if not n['reachable'] else PALETTE['bg'],
                  width=max(1, int(scale * (2.4 if not n['reachable'] else 1.4))))
        tw = d.textlength(n['name'], font=f_node) / scale
        ly = slot(n['x'], n['y'] - r - 8, tw + 4, 13)
        if abs(ly - (n['y'] - r - 8)) > 2:
            d.line([S(n['x']), S(TOP + n['y'] - r), S(n['x']), S(TOP + ly + 6)],
                   fill=PALETTE['cellEdge'], width=max(1, scale))
        d.text((S(n['x']), S(TOP + ly)), n['name'], font=f_node,
               fill=PALETTE['ink'], anchor='mm',
               stroke_width=max(1, int(scale * 1.6)), stroke_fill=PALETTE['sea'])

    img.save(path)
    print('  wrote %s  (%dx%d)' % (path, img.width, img.height))


def main():
    layouts = {l['id']: l for l in json.load(
        open(os.path.join(REPO, 'data/layouts/layouts.json')))['layouts'] if 'id' in l}
    ferry_dest = seagallop_destinations()
    every = {}
    for name in sorted(os.listdir(os.path.join(REPO, 'data/maps'))):
        p = os.path.join(REPO, 'data/maps', name, 'map.json')
        if os.path.exists(p):
            j = json.load(open(p))
            every[j['id']] = (name, j)

    scope = {mid for mid, (n, j) in every.items()
             if mid.startswith('MAP_ORANGE_ISLANDS')
             and (j.get('map_type') in OUTDOOR or j.get('map_type') == 'MAP_TYPE_INDOOR')}
    # keep interiors out of the region picture unless they hold encounters (the ghost ship)
    scope = {mid for mid in scope
             if every[mid][1].get('map_type') in OUTDOOR or 'GhostShip' in every[mid][0]}

    nodes, edges = OrderedDict(), []
    for mid in sorted(scope):
        name, j = every[mid]
        lay = layouts.get(j['layout'], {})
        nodes[mid] = {
            'id': mid, 'name': name.removeprefix('OrangeIslands_').replace('_', ' '),
            'island': island_of(name),
            'type': (j.get('map_type') or '').removeprefix('MAP_TYPE_').title(),
            'w': lay.get('width'), 'h': lay.get('height'), 'exits': [],
        }

    for mid in nodes:
        name, j = every[mid]
        seen = set()

        def add(kind, where, dest, count=None):
            if dest in seen and kind == 'warp':
                return
            seen.add(dest)
            edges.append({'from': mid, 'to': dest, 'kind': kind,
                          'where': where, 'count': count})

        for c in (j.get('connections') or []):
            add('edge', COMPASS.get(c.get('direction'), c.get('direction', '')), c['map'])

        sp = os.path.join(REPO, 'data/maps', name, 'scripts.inc')
        text = open(sp, encoding='utf-8', errors='replace').read() if os.path.exists(sp) else ''
        bands = defaultdict(list)
        for e in j.get('coord_events', []):
            if e.get('script'):
                bands[e['script']].append((e['x'], e['y']))
        # a sailing is reachable from anywhere its script is, so read the whole file
        for sid in sorted(set(FERRY_CMD.findall(text))):
            dest = ferry_dest.get(sid)
            if dest and dest in nodes and dest != mid:
                add('ferry', 'Seagallop', dest)

        for label, pts in bands.items():
            body = block(text, label)
            xs = sorted({x for x, _ in pts}); ys = sorted({y for _, y in pts})
            where = ('x %d' % xs[0] if len(xs) == 1 else 'x %d-%d' % (xs[0], xs[-1])) + \
                    (', y %d' % ys[0] if len(ys) == 1 else ', y %d-%d' % (ys[0], ys[-1]))
            for dest in sorted(set(WARP_CMD.findall(body))):
                add('trigger', where, dest, len(pts))
        under = j.get('map_type') == 'MAP_TYPE_UNDERWATER'
        for dest in sorted(set(DIVE_CMD.findall(text))):
            add('dive', 'Emerge' if under else 'Dive', dest)
        for wv in j.get('warp_events', []):
            d = wv.get('dest_map')
            if d and d in nodes and d not in seen:
                add('warp', '(%d,%d)' % (wv['x'], wv['y']), d)

    edges = [e for e in edges if e['to'] in nodes]
    out_of = defaultdict(set)
    for e in edges:
        out_of[e['from']].add(e['to'])
    for e in edges:
        e['twoWay'] = e['from'] in out_of.get(e['to'], set())
    for mid, n in nodes.items():
        n['exits'] = [{'to': e['to'], 'kind': e['kind'], 'where': e['where'],
                       'count': e['count'], 'twoWay': e['twoWay']}
                      for e in edges if e['from'] == mid]
        n['inbound'] = sorted({e['from'] for e in edges if e['to'] == mid})

    start = 'MAP_ORANGE_ISLANDS_VALENCIA'
    seen, stack = {start}, [start]
    while stack:
        for nxt in out_of.get(stack.pop(), ()):
            if nxt not in seen:
                seen.add(nxt); stack.append(nxt)
    for mid, n in nodes.items():
        n['reachable'] = mid in seen

    # --- place every map where the region map puts its section ----------------------
    ids = list(nodes)
    layout_src = open(os.path.join(REPO, 'src/data/region_map/region_map_layout_orange.h')).read()
    grid = []
    lm = re.search(r'\[LAYER_MAP\]\s*=\s*\{(.*?)\n    \},', layout_src, re.S)
    for rm in re.finditer(r'\{([^{}]*)\}', lm.group(1)):
        grid.append([c.strip() for c in rm.group(1).split(',') if c.strip()])
    gh, gw = len(grid), len(grid[0])
    cells = defaultdict(list)
    for yy, row in enumerate(grid):
        for xx, sec in enumerate(row):
            if sec != 'MAPSEC_NONE':
                cells[sec].append((xx, yy))

    CELL = 44.0
    by_sec = defaultdict(list)
    for mid in ids:
        by_sec[every[mid][1].get('region_map_section')].append(mid)

    unplaced = []
    for sec, members in by_sec.items():
        spots = cells.get(sec)
        if not spots:
            unplaced.extend(members)
            continue
        # Members of one section fan out on a golden-angle spiral whose reach grows with
        # how many share it, so six Pummelo maps do not stack their labels on one cell.
        for i, mid in enumerate(sorted(members)):
            cx, cy = spots[i % len(spots)]
            ring = i // len(spots)
            ang = (i * 2.39996) + ring * 0.7
            per = max(1, len(members) / len(spots))
            rad = 0.0 if len(members) == 1 else min(1.25, 0.30 + 0.17 * (per - 1) + 0.22 * ring)
            nodes[mid]['x'] = round((cx + 0.5 + math.cos(ang) * rad) * CELL, 1)
            nodes[mid]['y'] = round((cy + 0.5 + math.sin(ang) * rad) * CELL, 1)
            nodes[mid]['sec'] = sec.removeprefix('MAPSEC_')
            nodes[mid]['onMap'] = True

    # maps whose section is nowhere on the region map get a tray down the right edge
    for i, mid in enumerate(sorted(unplaced)):
        nodes[mid]['x'] = round((gw + 1.1) * CELL, 1)
        nodes[mid]['y'] = round((1.0 + i * 0.78) * CELL, 1)
        nodes[mid]['sec'] = (every[mid][1].get('region_map_section') or '').removeprefix('MAPSEC_')
        nodes[mid]['onMap'] = False

    doc = {'nodes': list(nodes.values()), 'edges': edges,
           'islands': ISLANDS + [SEA],
           'grid': {'w': gw, 'h': gh, 'cell': CELL,
                    'cells': {k.removeprefix('MAPSEC_'): v for k, v in cells.items()}},
           'unplaced': sorted(unplaced),
           'oneWay': [[e['from'], e['to']] for e in edges if not e['twoWay']],
           'unreachable': [m for m in ids if not nodes[m]['reachable']]}
    with open(OUT, 'w', newline='\n') as f:
        json.dump(doc, f, indent=1); f.write('\n')
    print('  %d maps, %d links' % (len(nodes), len(edges)))
    print('  one-way links: %d' % len(doc['oneWay']))
    print('  unreachable from Valencia: %d  %s'
          % (len(doc['unreachable']), [nodes[m]['name'] for m in doc['unreachable']][:8]))
    print('  not on the region map at all: %d  %s'
          % (len(unplaced), [nodes[m]['name'] for m in unplaced]))
    print('  wrote %s' % OUT)
    if '--no-png' not in sys.argv:
        render_png(doc, os.path.join(REPO, 'docs/orange-islands/link_map.png'))


if __name__ == '__main__':
    main()
