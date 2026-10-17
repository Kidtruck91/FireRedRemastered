# Whole-edge sea links: every surfable cell along a map edge is a trigger, and one script per link
# lands the player at the matching position on the other map (getplayerxy + clamp + offset), so open
# water needs no funnel. Run from the repo root; optional args restrict to those maps.
import json, struct, sys
ONLY = set(sys.argv[1:])
EDGES = [
    # (map, side, dest map, offset added to the coordinate along the edge, source clamp (lo, hi), landing coordinate across the edge)
    ('Mikan', 'top', 'Route_5', -8, (8, 55), 122),
    ('Route_5', 'bottom', 'Mikan', 8, (0, 47), 1),
    ('Sunburst', 'top', 'Route_6', 12, (8, 55), 69),
    ('Route_6', 'bottom', 'Sunburst', -12, (20, 67), 1),
    ('Sunburst', 'left', 'Route_7', 20, (0, 63), 63),
    ('Route_7', 'right', 'Sunburst', -20, (20, 83), 1),
    ('Mandarin_South', 'top', 'Route_13', -4, (24, 71), 77),
    ('Route_13', 'bottom', 'Mandarin_South', 4, (20, 67), 1),
    ('Mandarin_South', 'bottom', 'Route_9', -22, (30, 65), 2),
    ('Route_9', 'top', 'Mandarin_South', 22, (8, 43), 74),
    ('Mandarin_South', 'left', 'Route_10', -4, (14, 61), 101),
    ('Route_10', 'right', 'Mandarin_South', 4, (10, 57), 1),
    ('Mandarin_South', 'right', 'Route_12', -16, (20, 55), 2),
    ('Route_12', 'left', 'Mandarin_South', 16, (4, 39), 94),
    ('Kinnow', 'left', 'Route_8', -10, (16, 49), 129),
    ('Route_8', 'right', 'Kinnow', 10, (6, 39), 1),
    ('Kinnow', 'top', 'Route_11', 4, (16, 55), 77),
    ('Route_11', 'bottom', 'Kinnow', -4, (20, 59), 1),
    ('Trovita', 'left', 'Route_12', -12, (16, 43), 137),
    ('Route_12', 'right', 'Trovita', 12, (4, 31), 1),
    ('Trovita_Beach', 'bottom', 'Route_11', 4, (16, 55), 2),
    ('Route_11', 'top', 'Trovita_Beach', -4, (20, 59), 34),
    ('Trovita', 'top', 'Route_14', 8, (12, 59), 69),
    ('Route_14', 'bottom', 'Trovita', -8, (20, 67), 1),
    ('Kumquat', 'bottom', 'Route_13', 8, (12, 59), 2),
    ('Route_13', 'top', 'Kumquat', -8, (20, 67), 58),
    ('Kumquat', 'right', 'Route_14', 6, (6, 49), 2),
    ('Route_14', 'left', 'Kumquat', -6, (12, 55), 70),
    ('Moro', 'bottom', 'Route_7', 4, (16, 55), 2),
    ('Route_7', 'top', 'Moro', -4, (20, 59), 54),
    ('Moro', 'right', 'Route_9', 30, (4, 49), 2),
    ('Route_9', 'left', 'Moro', -30, (34, 79), 62),
    ('Pinkan', 'right', 'Route_13', 6, (24, 43), 2),
    ('Route_13', 'left', 'Pinkan', -6, (30, 49), 58),
]
def const(m): return 'MAP_ORANGE_ISLANDS_' + m.upper()
L = {l['name']: l for l in json.load(open('data/layouts/layouts.json'))['layouts'] if 'name' in l}
for m in sorted({e[0] for e in EDGES}):
    if ONLY and m not in ONLY: continue
    p = 'data/maps/OrangeIslands_%s/map.json' % m; j = json.load(open(p))
    l = next(v for v in L.values() if v['id'] == j['layout'])
    W, H = l['width'], l['height']; b = open(l['blockdata_filepath'], 'rb').read()
    tile = lambda x, y: struct.unpack_from('<H', b, (y * W + x) * 2)[0]
    sc = open('data/maps/OrangeIslands_%s/scripts.inc' % m).read()
    for a, side, d, off, (lo, hi), across in EDGES:
        if a != m: continue
        label = 'OrangeIslands_%s_EventScript_SeaEdgeTo%s' % (m, d.replace('_', ''))
        old = 'OrangeIslands_%s_EventScript_SailTo%s' % (m, d.replace('_', ''))
        j['coord_events'] = [e for e in j.get('coord_events', []) if e['script'] not in (label, old)]
        sc = '\n\n'.join(blk for blk in sc.split('\n\n') if label + '::' not in blk and old + '::' not in blk).rstrip('\n') + '\n'
        cells = {'top': [(x, 0) for x in range(W)], 'bottom': [(x, H - 1) for x in range(W)],
                 'left': [(0, y) for y in range(H)], 'right': [(W - 1, y) for y in range(H)]}[side]
        n = 0
        for x, y in cells:
            v = tile(x, y)
            if v >> 12 == 1 and not v & 0xc00:          # surfable, passable
                j['coord_events'].append({'type': 'trigger', 'x': x, 'y': y, 'elevation': 0, 'var': 'VAR_TEMP_0', 'var_value': '0', 'script': label})
                n += 1
        along, fixed = ('VAR_0x8004', 'VAR_0x8005') if side in ('top', 'bottom') else ('VAR_0x8005', 'VAR_0x8004')
        add = '\taddvar %s, %d\n' % (along, off) if off > 0 else ('\tsubvar %s, %d\n' % (along, -off) if off < 0 else '')
        sc += ('\n@ Sea edge: any surfable cell on this edge, landing at the matching spot on %s\n'
               '%s::\n\tgetplayerxy VAR_0x8004, VAR_0x8005\n'
               '\tcompare %s, %d\n\tgoto_if_lt %s_Lo\n\tcompare %s, %d\n\tgoto_if_gt %s_Hi\n'
               '%s_Go:\n%s\tsetvar %s, %d\n\twarp %s, %s, %s\n\twaitstate\n\tend\n'
               '%s_Lo:\n\tsetvar %s, %d\n\tgoto %s_Go\n%s_Hi:\n\tsetvar %s, %d\n\tgoto %s_Go\n') % (
            d.replace('_', ' '), label, along, lo, label, along, hi, label,
            label, add, fixed, across, const(d), 'VAR_0x8004', 'VAR_0x8005',
            label, along, lo, label, label, along, hi, label)
        print(m, side, '->', d, n, 'trigger cells')
    open('data/maps/OrangeIslands_%s/scripts.inc' % m, 'w', newline='\n').write(sc)
    open(p, 'w', newline='\n').write(json.dumps(j, indent=2) + '\n')
