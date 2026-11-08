"""Survey what is actually finished in the Orange region, island by island.

Read off the repo rather than kept by hand, because a checklist someone has to
remember to tick is a checklist that goes stale -- which is the same way the
landmark fixtures rotted. Every column here is counted from the map data at the
moment you run it, so it cannot drift from the thing it describes.

What the columns mean:

    size        the exterior map, in tiles
    npc         object events with a script that are not trainers or items
    trn         trainers, by trainer_type
    item        visible item balls, plus hidden items in brackets
    door        doors leading into this island's interior maps, and how many
                distinct interiors that reaches
    inside      how many of those interiors have anyone standing in them. A
                door into an empty room counts as open, not as finished
    wild        which encounter tables are filled: L land, W water, R rock
                smash, F fishing. A dash means the map has no entry at all
    map         whether the place has a square on the region map
    check       what validate_maps.py makes of it

    python3 docs/orange-islands/region_status.py           # print the table
    python3 docs/orange-islands/region_status.py --write   # and save the page
"""
import io
import json
import os
import re
import subprocess
import sys
from collections import defaultdict

REPO = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
OUT = os.path.join(REPO, 'docs/orange-islands/REGION_STATUS.md')


def load():
    groups = json.load(open(os.path.join(REPO, 'data/maps/map_groups.json')))
    layouts = {l['id']: l for l in
               json.load(open(os.path.join(REPO, 'data/layouts/layouts.json')))['layouts']}
    maps = {}
    for d in groups['group_order']:
        for name in groups[d]:
            p = os.path.join(REPO, 'data/maps', name, 'map.json')
            if os.path.exists(p):
                maps[name] = json.load(open(p))
    return groups, layouts, maps


def encounters():
    d = json.load(open(os.path.join(REPO, 'src/data/wild_encounters.json')))
    out = {}
    for g in d['wild_encounter_groups']:
        for e in g.get('encounters', []):
            flags = ''
            for key, letter in (('land_mons', 'L'), ('water_mons', 'W'),
                                ('rock_smash_mons', 'R'), ('fishing_mons', 'F')):
                if e.get(key) and e[key].get('mons'):
                    flags += letter
            out[e['map']] = flags or 'empty'
    return out


def region_squares():
    src = open(os.path.join(REPO, 'src/data/region_map/region_map_layout_orange.h')).read()
    return set(re.findall(r'MAPSEC_\w+', src))


def checker():
    """What validate_maps.py says, one line per map."""
    try:
        r = subprocess.run([sys.executable, os.path.join(REPO, 'docs/orange-islands/validate_maps.py')],
                           cwd=REPO, capture_output=True, text=True, timeout=600)
    except Exception:
        return {}
    out = {}
    for line in (r.stdout + r.stderr).splitlines():
        m = re.match(r'\s*(OrangeIslands_\w+): (.*)', line)
        if m:
            name, rest = m.groups()
            out[name] = 'ok' if rest.endswith('passed') else rest
    return out


def survey():
    groups, layouts, maps = load()
    enc, squares, says = encounters(), region_squares(), checker()
    interiors = {}            # island -> [interior map names]
    for g in groups['group_order']:
        m = re.match(r'gMapGroup_Orange_(\w+)_Interior$', g)
        if m:
            interiors[m.group(1)] = groups[g]
    inside_group = {n: g for g in groups['group_order'] for n in groups[g]
                    if g != 'gMapGroup_OrangeIslands'}

    rows = []
    for name in groups['gMapGroup_OrangeIslands']:
        mj = maps.get(name)
        if not mj:
            continue
        short = name.replace('OrangeIslands_', '')
        l = layouts.get(mj['layout'], {})
        objs = mj.get('object_events', [])
        trn = [o for o in objs if o.get('trainer_type', 'TRAINER_TYPE_NONE')
               not in ('TRAINER_TYPE_NONE', '0', 0)]
        items = [o for o in objs if 'ITEM_BALL' in (o.get('graphics_id') or '')]
        hidden = [b for b in mj.get('bg_events', []) if 'hidden' in (b.get('type') or '')]
        signs = [b for b in mj.get('bg_events', []) if b.get('script')]
        npc = [o for o in objs if o.get('script') and o not in trn and o not in items]

        doors = [w for w in mj.get('warp_events', [])
                 if inside_group.get(_dir_of(w['dest_map']), '').endswith('_Interior')]
        reached = {_dir_of(w['dest_map']) for w in doors}
        peopled = sum(1 for r in reached
                      if maps.get(r) and maps[r].get('object_events'))

        rows.append(dict(
            name=short, island=_island_of(short, interiors),
            size='%dx%d' % (l.get('width', 0), l.get('height', 0)),
            npc=len(npc), trn=len(trn), item=len(items), hidden=len(hidden),
            signs=len(signs), doors=len(doors), reached=len(reached), peopled=peopled,
            wild=enc.get(mj['id'], '--'),
            square='yes' if _mapsec_of(mj) in squares else 'NO',
            check=says.get(name, 'no fixture, so never walked')))
    return rows, interiors, maps


def _dir_of(map_id):
    """MAP_ORANGE_ISLANDS_MIKAN_HOUSE1 -> OrangeIslands_Mikan_House1, as the
    folder is actually spelled. Matched case-insensitively against the real
    folders, since the constant loses the capitalisation."""
    want = map_id.replace('MAP_', '').replace('_', '').lower()
    return _FOLDERS.get(want, map_id)


_FOLDERS = {}


def _mapsec_of(mj):
    return mj.get('region_map_section', '')


def _island_of(short, interiors):
    for island in interiors:
        if short == island or short.startswith(island + '_'):
            return island
    return '-'


def table(rows):
    cols = [('name', 'map', '<'), ('size', 'size', '>'), ('npc', 'npc', '>'),
            ('trn', 'trn', '>'), ('item', 'item', '>'), ('signs', 'sign', '>'),
            ('doors', 'door', '>'), ('reached', 'into', '>'), ('peopled', 'inside', '>'),
            ('wild', 'wild', '<'), ('square', 'map', '<'), ('check', 'check', '<')]
    w = {k: max(len(h), max((len(str(r[k])) for r in rows), default=0)) for k, h, _ in cols}
    buf = io.StringIO()
    line = lambda cells: buf.write('| ' + ' | '.join(cells) + ' |\n')
    line(['%-*s' % (w[k], h) for k, h, _ in cols])
    line(['-' * w[k] for k, _, _ in cols])
    for r in rows:
        line(['%*s' % (w[k] if a == '>' else -w[k], r[k]) for k, _, a in cols])
    return buf.getvalue()


def main():
    global _FOLDERS
    _FOLDERS = {d.replace('_', '').lower(): d
                for d in os.listdir(os.path.join(REPO, 'data/maps'))
                if os.path.isdir(os.path.join(REPO, 'data/maps', d))}
    rows, interiors, maps = survey()
    towns = [r for r in rows if '_Route_' not in r['name'] and not r['name'].startswith('Route_')]
    routes = [r for r in rows if r not in towns]

    out = io.StringIO()
    out.write('# Orange region: what is finished\n\n')
    out.write('Generated by `docs/orange-islands/region_status.py` -- counted off the\n'
              'map data, not kept by hand. Re-run it rather than editing this file.\n\n')
    out.write('## Islands and their coasts\n\n' + table(towns))
    out.write('\n## Routes\n\n' + table(routes))

    out.write('\n## Interiors\n\n')
    out.write('| island | interiors | with people | empty |\n|---|---|---|---|\n')
    for island in sorted(interiors):
        inner = interiors[island]
        peopled = [n for n in inner if maps.get(n) and maps[n].get('object_events')]
        empty = [n.replace('OrangeIslands_', '') for n in inner if n not in peopled]
        out.write('| %s | %d | %d | %s |\n' % (island, len(inner), len(peopled),
                                               ', '.join(empty) or '--'))
    text = out.getvalue()
    print(text)
    if '--write' in sys.argv:
        open(OUT, 'w', newline='\n').write(text)
        print('wrote ' + os.path.relpath(OUT, REPO))
    if '--csv' in sys.argv:
        import csv
        d = sys.argv[sys.argv.index('--csv') + 1]
        os.makedirs(d, exist_ok=True)
        cols = ['name', 'kind', 'island', 'size', 'npc', 'trn', 'item', 'hidden',
                'signs', 'doors', 'reached', 'peopled', 'wild', 'square', 'check']
        with open(os.path.join(d, 'maps.csv'), 'w', newline='') as f:
            w = csv.DictWriter(f, cols, extrasaction='ignore')
            w.writeheader()
            for r in rows:
                r = dict(r, kind=('route' if r in routes else
                                  'coast' if r['name'].endswith(('_Beach', '_Falls')) else 'island'))
                w.writerow(r)
        with open(os.path.join(d, 'interiors.csv'), 'w', newline='') as f:
            w = csv.writer(f)
            w.writerow(['island', 'interiors', 'peopled', 'empty'])
            for island in sorted(interiors):
                inner = interiors[island]
                peopled = [n for n in inner if maps.get(n) and maps[n].get('object_events')]
                w.writerow([island, len(inner), len(peopled), len(inner) - len(peopled)])
        print('wrote maps.csv and interiors.csv to ' + d)


if __name__ == '__main__':
    main()
