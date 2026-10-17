#!/usr/bin/env python3
# Orange Islands Town Map: a fifth region-map page used whenever the player is in an Orange mapsec.
# Tilemap reuses existing region-map tiles (BG0 has room for only 16 more). Sections, corners and dimensions
# come from ORANGE_GRID below (22x15 cells), which mirrors world_links.py's geography. Run from repo root.
import os, re, struct, shutil
HERE = os.path.dirname(os.path.abspath(__file__))
TOWNS = {'VALENCIA_ISLAND': (7, 13), 'SUNBURST_ISLAND': (2, 11), 'MORO_ISLAND': (2, 9), 'KINNOW_ISLAND': (8, 9),
         'MANDARIN_SOUTH': (11, 8), 'MIKAN_ISLAND': (14, 12), 'NAVEL_ISLAND': (14, 7), 'PINKAN_ISLAND': (9, 5),
         'KUMQUAT_ISLAND': (11, 4), 'TROVITA_ISLAND': (13, 2), 'PUMMELO_ISLAND': (17, 1)}
ROUTES = {1: [(7, 12), (7, 11)], 3: [(x, 13) for x in range(8, 15)], 4: [(x, 11) for x in range(3, 7)], 7: [(2, 10)],
          17: [(3, 9)], 10: [(x, 9) for x in range(4, 8)], 8: [(9, 9), (10, 9), (11, 9)], 6: [(12, 8), (13, 8)],
          5: [(14, y) for y in range(8, 12)], 18: [(11, 7)], 13: [(11, 6), (11, 5)], 16: [(10, 5)], 15: [(8, 5)],
          14: [(11, 3), (11, 2), (12, 2)], 11: [(13, y) for y in range(3, 8)]}
SPARE = {2: 'VALENCIA_ISLAND', 9: 'MANDARIN_SOUTH', 12: 'KUMQUAT_ISLAND'}   # not on the grid yet: show at this island
CELLS = {}
for n, cs in ROUTES.items():
    for c in cs: CELLS[c] = 'MAPSEC_ORANGE_ROUTE_%d' % n
for t, c in TOWNS.items(): CELLS[c] = 'MAPSEC_' + t
# ---- tilemap ------------------------------------------------------------------------------------
G = 'graphics/region_map/'
b = open(G + 'sevii_45.bin', 'rb').read(); tm = [struct.unpack_from('<H', b, i * 2)[0] for i in range(600)]
BUTTON = {(x, y) for x in (24, 25, 26) for y in (16, 17, 18)}
for y in range(2, 20):
    for x in range(3, 27):
        if (x, y) not in BUTTON: tm[y * 30 + x] = 0x0003                       # open sea
for (cx, cy), ms in CELLS.items():
    tm[(cy + 4) * 30 + cx + 4] = 0x1001 if ms[7:] in TOWNS else 0x0004          # island town / sea route
open(G + 'orange.bin', 'wb').write(b''.join(struct.pack('<H', e) for e in tm))
# ---- section layout -----------------------------------------------------------------------------
rows = lambda layer: '\n'.join('        {' + ', '.join((CELLS.get((x, y), 'MAPSEC_NONE') if layer == 0 else 'MAPSEC_NONE') for x in range(22)) + '},' for y in range(15))
open('src/data/region_map/region_map_layout_orange.h', 'w', newline='\n').write(
    'static const u8 sRegionMapSections_Orange[LAYER_COUNT][MAP_HEIGHT][MAP_WIDTH] = {\n    [LAYER_MAP] =\n    {\n' + rows(0) +
    '\n    },\n    [LAYER_DUNGEON] =\n    {\n' + rows(1) + '\n    }\n};\n')
boxes = {}
for (x, y), ms in CELLS.items():
    bx = boxes.setdefault(ms, [x, y, x, y]); bx[0] = min(bx[0], x); bx[1] = min(bx[1], y); bx[2] = max(bx[2], x); bx[3] = max(bx[3], y)
for n, isl in SPARE.items(): boxes['MAPSEC_ORANGE_ROUTE_%d' % n] = boxes['MAPSEC_' + isl]
corner = ''.join('    [%s - MAPSECS_ORANGE] = { %d, %d },\n' % (ms, bx[0], bx[1]) for ms, bx in sorted(boxes.items()))
dims = ''.join('    [%s - MAPSECS_ORANGE] = { %d, %d },\n' % (ms, bx[2] - bx[0] + 1, bx[3] - bx[1] + 1) for ms, bx in sorted(boxes.items()))
# ---- region_map.c -------------------------------------------------------------------------------
p = 'src/region_map.c'; s = open(p).read()
def sub(old, new):
    global s
    if new in s: return
    assert old in s, old; s = s.replace(old, new, 1)
sub('    REGIONMAP_JOHTO,     // right-hand column of the switch menu (room below it for more regions)\n',
    '    REGIONMAP_JOHTO,     // right-hand column of the switch menu (room below it for more regions)\n'
    '    REGIONMAP_ORANGE,    // Orange Islands: shown on its own while the player is there (no switch menu entry)\n')
sub('static const u32 sJohto_Tilemap[] = INCBIN_U32("graphics/region_map/johto.bin.lz");\n',
    'static const u32 sJohto_Tilemap[] = INCBIN_U32("graphics/region_map/johto.bin.lz");\n'
    'static const u32 sOrange_Tilemap[] = INCBIN_U32("graphics/region_map/orange.bin.lz");\n')
sub('#include "data/region_map/region_map_layout_johto.h"\n',
    '#include "data/region_map/region_map_layout_johto.h"\n#include "data/region_map/region_map_layout_orange.h"\n'
    '\n// Orange Islands section boxes (cells on the Orange page), indexed from MAPSECS_ORANGE\n'
    'static const u16 sOrangeMapSectionTopLeftCorners[MAPSEC_ORANGE_END - MAPSECS_ORANGE][2] = {\n' + corner + '};\n'
    'static const u16 sOrangeMapSectionDimensions[MAPSEC_ORANGE_END - MAPSECS_ORANGE][2] = {\n' + dims + '};\n')
sub('''    case 9:
        LZ77UnCompWram(sJohto_Tilemap, sRegionMap->layouts[REGIONMAP_JOHTO]);
        break;
''', '''    case 9:
        LZ77UnCompWram(sJohto_Tilemap, sRegionMap->layouts[REGIONMAP_JOHTO]);
        break;
    case 10:
        LZ77UnCompWram(sOrange_Tilemap, sRegionMap->layouts[REGIONMAP_ORANGE]);
        break;
''')
sub('''    if (gMapHeader.regionMapSectionId >= MAPSECS_JOHTO_TOWNS && gMapHeader.regionMapSectionId != MAPSEC_NONE)
    {
        region = REGIONMAP_JOHTO;
    }''', '''    if (gMapHeader.regionMapSectionId < MAPSEC_ORANGE_END)
    {
        region = REGIONMAP_ORANGE;
        sRegionMap->permissions[MAPPERM_HAS_SWITCH_BUTTON] = FALSE;
    }
    else if (gMapHeader.regionMapSectionId >= MAPSECS_JOHTO_TOWNS && gMapHeader.regionMapSectionId != MAPSEC_NONE)
    {
        region = REGIONMAP_JOHTO;
    }''')
sub('''    case REGIONMAP_JOHTO:
        return sRegionMapSections_Johto[layer][y][x];
''', '''    case REGIONMAP_JOHTO:
        return sRegionMapSections_Johto[layer][y][x];
    case REGIONMAP_ORANGE:
        return sRegionMapSections_Orange[layer][y][x];
''')
sub('''    sMapCursor->selectedMapsec -= MAPSECS_KANTO;
    divisor = width''', '''    if (sMapCursor->selectedMapsec < MAPSEC_ORANGE_END)
    {
        const u16 (*dim)[2] = sOrangeMapSectionDimensions;
        const u16 (*tl)[2] = sOrangeMapSectionTopLeftCorners;
        u16 ms = sMapCursor->selectedMapsec - MAPSECS_ORANGE;
        divisor = width / dim[ms][0];
        if (divisor == 0)
            divisor = 1;
        x /= divisor;
        if (x >= dim[ms][0])
            x = dim[ms][0] - 1;
        divisor = height / dim[ms][1];
        if (divisor == 0)
            divisor = 1;
        y /= divisor;
        if (y >= dim[ms][1])
            y = dim[ms][1] - 1;
        sMapCursor->x = x + tl[ms][0];
        sMapCursor->y = y + tl[ms][1];
        return;
    }
    sMapCursor->selectedMapsec -= MAPSECS_KANTO;
    divisor = width''')
open(p, 'w', newline='\n').write(s)
print('orange town map installed:', len(boxes), 'sections')
