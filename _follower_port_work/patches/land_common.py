# Helpers shared by the island land-route patches.
import json, struct
def strip_link(m, dest):
    """Remove a whole-edge sea link (coord triggers + script block) from map m toward dest."""
    label = 'OrangeIslands_%s_EventScript_SeaEdgeTo%s' % (m, dest.replace('_', ''))
    p = 'data/maps/OrangeIslands_%s/map.json' % m; j = json.load(open(p))
    j['coord_events'] = [e for e in j.get('coord_events', []) if e['script'] != label]
    open(p, 'w', newline='\n').write(json.dumps(j, indent=2) + '\n')
    s = 'data/maps/OrangeIslands_%s/scripts.inc' % m; t = open(s).read()
    t = '\n\n'.join(b for b in t.split('\n\n') if label + '::' not in b).rstrip('\n') + '\n'
    open(s, 'w', newline='\n').write(t)
def connect(a, b, direction, offset=0):
    """Seamless map connection a -> b (and the reverse)."""
    opp = {'up': 'down', 'down': 'up', 'left': 'right', 'right': 'left'}
    for m, n, d, o in ((a, b, direction, offset), (b, a, opp[direction], -offset)):
        p = 'data/maps/OrangeIslands_%s/map.json' % m; j = json.load(open(p))
        cs = [c for c in (j.get('connections') or []) if c['map'] != 'MAP_ORANGE_ISLANDS_' + n.upper()]
        cs.append({'map': 'MAP_ORANGE_ISLANDS_' + n.upper(), 'offset': o, 'direction': d})
        j['connections'] = cs
        open(p, 'w', newline='\n').write(json.dumps(j, indent=2) + '\n')
def write_bin(path, rows):
    open(path, 'wb').write(b''.join(struct.pack('<H', v) for r in rows for v in r))
def read_bin(path, w):
    b = open(path, 'rb').read(); h = len(b) // 2 // w
    return [[struct.unpack_from('<H', b, (y * w + x) * 2)[0] for x in range(w)] for y in range(h)]
def edge_entries(add, remove=()):
    """Edit the EDGES table in edge_links.py: drop (map, side, dest) triples in remove, append new tuples."""
    p = '_follower_port_work/patches/edge_links.py'; s = open(p).read()
    lines = s.split('\n'); out = []
    for l in lines:
        if any(l.strip().startswith("('%s', '%s', '%s'," % r) for r in remove): continue
        out.append(l)
    s = '\n'.join(out)
    i = s.index('\n]\n', s.index('EDGES = ['))
    new = ''.join('\n    %r,' % (e,) for e in add if repr(e) not in s)
    s = s[:i] + new + s[i:]
    open(p, 'w', newline='\n').write(s)
