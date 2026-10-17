#!/usr/bin/env python3
# Scaffolding for the island land routes: Orange Routes 15-18 (map sections 0x19-0x1C, taken from FireRed's
# unused Hoenn ROUTE_110-113 slots), map_groups / event_scripts / layouts entries and empty map files.
# Layout data is written by the per-area patches (pinkan3.py, moro17.py, mandarin18.py). Idempotent.
import json, os, re, struct
NEW = {15: ('gTileset_OrangeGeneral', 'gTileset_OrangeSevii67', 36, 92, 'MUS_SEVII_67'),
       16: ('gTileset_OrangeGeneral', 'gTileset_OrangeSevii67', 36, 92, 'MUS_SEVII_67'),
       17: ('gTileset_OrangeGeneral', 'gTileset_OrangeSevii45', 40, 56, 'MUS_LAVENDER'),
       18: ('gTileset_General', 'gTileset_CeladonCity', 96, 40, 'MUS_ROUTE1')}
h = 'include/constants/region_map_sections.h'; s = open(h).read()
for n, old in zip(range(15, 19), range(110, 114)):
    s = s.replace('#define MAPSEC_ROUTE_%d                    0x%02X' % (old, 0x19 + n - 15),
                  '#define MAPSEC_ORANGE_ROUTE_%d              0x%02X' % (n, 0x19 + n - 15))
s = re.sub(r'#define MAPSEC_ORANGE_END\s+0x19', '#define MAPSEC_ORANGE_END                   0x1D', s)
open(h, 'w', newline='\n').write(s)
c = 'src/region_map.c'; s = open(c).read()
if 'sMapsecName_ORANGE_ROUTE_15' not in s:
    a = 'static const u8 sMapsecName_ORANGE_ROUTE_14[] = _("ORANGE ROUTE 14");\n'
    s = s.replace(a, a + ''.join('static const u8 sMapsecName_ORANGE_ROUTE_%d[] = _("ORANGE ROUTE %d");\n' % (n, n) for n in NEW))
    a = '    [MAPSEC_ORANGE_ROUTE_14 - MAPSECS_ORANGE] = sMapsecName_ORANGE_ROUTE_14,\n'
    s = s.replace(a, a + ''.join('    [MAPSEC_ORANGE_ROUTE_%d - MAPSECS_ORANGE] = sMapsecName_ORANGE_ROUTE_%d,\n' % (n, n) for n in NEW))
    open(c, 'w', newline='\n').write(s)
g = open('data/maps/map_groups.json').read()
e = open('data/event_scripts.s').read()
L = open('data/layouts/layouts.json').read()
for n, (prim, sec, w, hh, mus) in NEW.items():
    name = 'OrangeIslands_Route_%d' % n
    if '"%s"' % name not in g:
        prev = 'OrangeIslands_Route_%d' % (n - 1) if n > 15 else 'OrangeIslands_Moro_GhostShip'
        g = re.sub(r'"%s"(,?)\n' % prev, lambda m: '"%s",\n    "%s"%s\n' % (prev, name, m.group(1)), g, count=1) if n > 15 else \
            g.replace('"%s"' % prev, '"%s",\n    "%s"' % (prev, name), 1)
    if 'maps/%s/scripts.inc' % name not in e:
        a = '\t.include "data/maps/OrangeIslands_Moro_GhostShip/text.inc"\n'
        e = e.replace(a, a + '\t.include "data/maps/%s/scripts.inc"\n\t.include "data/maps/%s/text.inc"\n' % (name, name), 1)
    lid = 'LAYOUT_ORANGE_ISLANDS_ROUTE_%d' % n
    if lid not in L:
        i = L.index('"id": "LAYOUT_ORANGE_ISLANDS_ROUTE_14"'); j = L.index('}', i) + 1
        L = L[:j] + ''',
    {
      "id": "%s",
      "name": "%s_Layout",
      "width": %d,
      "height": %d,
      "border_width": 2,
      "border_height": 2,
      "primary_tileset": "%s",
      "secondary_tileset": "%s",
      "border_filepath": "data/layouts/%s/border.bin",
      "blockdata_filepath": "data/layouts/%s/map.bin"
    }''' % (lid, name, w, hh, prim, sec, name, name) + L[j:]
    os.makedirs('data/layouts/' + name, exist_ok=True)
    open('data/layouts/%s/border.bin' % name, 'wb').write(struct.pack('<4H', *[0x12b | (1 << 12)] * 4))
    if not os.path.exists('data/layouts/%s/map.bin' % name):
        open('data/layouts/%s/map.bin' % name, 'wb').write(struct.pack('<H', 0x12b | (1 << 12)) * (w * hh))
    d = 'data/maps/' + name; os.makedirs(d, exist_ok=True)
    if not os.path.exists(d + '/map.json'):
        json.dump({"id": "MAP_ORANGE_ISLANDS_ROUTE_%d" % n, "name": name, "layout": lid, "music": mus,
                   "region_map_section": "MAPSEC_ORANGE_ROUTE_%d" % n, "requires_flash": False, "weather": "WEATHER_SUNNY",
                   "map_type": "MAP_TYPE_ROUTE", "allow_cycling": True, "allow_escaping": False, "allow_running": True,
                   "show_map_name": True, "floor_number": 0, "battle_scene": "MAP_BATTLE_SCENE_NORMAL", "connections": 0,
                   "object_events": [], "warp_events": [], "coord_events": [], "bg_events": []},
                  open(d + '/map.json', 'w', newline='\n'), indent=2)
        open(d + '/scripts.inc', 'w', newline='\n').write('%s_MapScripts::\n\t.byte 0\n' % name)
        open(d + '/text.inc', 'w', newline='\n').write('')
open('data/maps/map_groups.json', 'w', newline='\n').write(g)
open('data/event_scripts.s', 'w', newline='\n').write(e)
open('data/layouts/layouts.json', 'w', newline='\n').write(L)
print('scaffold ok')
