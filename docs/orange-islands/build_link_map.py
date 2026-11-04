"""Extract how every Orange Islands map connects, and lay the result out for a viewer.

Four different mechanisms join maps in this hack, and only one of them is visible in
Porymap's connection editor, which is why a broken link is easy to miss:

    edge        a map connection in map.json
    warp        a warp_event's dest_map
    trigger     a coord_event whose script runs a warp -- most Orange sea crossings
    dive        a dive/emerge connection, or a setdivewarp in an ON_RESUME map script

This reads all four, works out which links are answered from the far side and which are
one-way, finds the maps nothing leads into, and runs a small spring layout so the viewer
can draw the region without a physics library. The result is written as JSON for
docs/orange-islands/link_map.html.

    python3 docs/orange-islands/build_link_map.py
"""
import json
import math
import os
import random
import re
import struct
from collections import defaultdict, OrderedDict

REPO = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
OUT = os.path.join(REPO, 'docs/orange-islands/link_map.json')

WARP_CMD = re.compile(r'^\s*warp\w*\s+(MAP_\w+)', re.M)
DIVE_CMD = re.compile(r'^\s*setdivewarp\s+(MAP_\w+)', re.M)
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


def main():
    layouts = {l['id']: l for l in json.load(
        open(os.path.join(REPO, 'data/layouts/layouts.json')))['layouts'] if 'id' in l}
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

    # --- spring layout, islands pulled toward their own centre ----------------------
    rnd = random.Random(5)
    ids = list(nodes)
    pos = {m: [rnd.uniform(-1, 1), rnd.uniform(-1, 1)] for m in ids}
    groups = ISLANDS + [SEA]
    anchor = {g: (math.cos(2 * math.pi * i / len(groups)) * 1.6,
                  math.sin(2 * math.pi * i / len(groups)) * 1.6)
              for i, g in enumerate(groups)}
    anchor[SEA] = (0.0, 0.0)
    adj = defaultdict(set)
    for e in edges:
        adj[e['from']].add(e['to']); adj[e['to']].add(e['from'])
    for step in range(600):
        k = 0.06 * (1 - step / 600) + 0.004
        for m in ids:
            fx = fy = 0.0
            for o in ids:
                if o == m:
                    continue
                dx, dy = pos[m][0] - pos[o][0], pos[m][1] - pos[o][1]
                d2 = dx * dx + dy * dy + 1e-4
                f = 0.010 / d2
                fx += dx * f; fy += dy * f
            for o in adj[m]:
                dx, dy = pos[o][0] - pos[m][0], pos[o][1] - pos[m][1]
                fx += dx * 0.45; fy += dy * 0.45
            ax, ay = anchor.get(nodes[m]['island'], (0, 0))
            fx += (ax - pos[m][0]) * 0.22; fy += (ay - pos[m][1]) * 0.22
            pos[m][0] += max(-0.4, min(0.4, fx)) * k
            pos[m][1] += max(-0.4, min(0.4, fy)) * k
    xs = [p[0] for p in pos.values()]; ys = [p[1] for p in pos.values()]
    lo_x, hi_x, lo_y, hi_y = min(xs), max(xs), min(ys), max(ys)
    for m in ids:
        nodes[m]['x'] = round(60 + 880 * (pos[m][0] - lo_x) / (hi_x - lo_x or 1), 1)
        nodes[m]['y'] = round(60 + 740 * (pos[m][1] - lo_y) / (hi_y - lo_y or 1), 1)

    doc = {'nodes': list(nodes.values()), 'edges': edges,
           'islands': groups,
           'oneWay': [[e['from'], e['to']] for e in edges if not e['twoWay']],
           'unreachable': [m for m in ids if not nodes[m]['reachable']]}
    with open(OUT, 'w', newline='\n') as f:
        json.dump(doc, f, indent=1); f.write('\n')
    print('  %d maps, %d links' % (len(nodes), len(edges)))
    print('  one-way links: %d' % len(doc['oneWay']))
    print('  unreachable from Valencia: %d  %s'
          % (len(doc['unreachable']), [nodes[m]['name'] for m in doc['unreachable']][:8]))
    print('  wrote %s' % OUT)


if __name__ == '__main__':
    main()
