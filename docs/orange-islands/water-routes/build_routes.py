"""Rebuild only the twelve Orange sea layouts; retain all connection records.

Macro placements are authored in designs.json. Tiny complete beach motifs come
from native FRLG maps, with straight sections extended rather than redrawing
corners. Hoenn references inform composition, not tileset IDs or copied maps.
"""
from pathlib import Path
import json
import struct
import sys
from PIL import Image, ImageDraw

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[2]
sys.path.insert(0, str(HERE.parent))
from render_maps import render

WATER = 0x112B
BLUE = ((0x0510, 0x0511), (0x0518, 0x0519))
BROWN = ((0x05DB, 0x05DC), (0x05E3, 0x05E4))
TEMPLATES = json.loads((HERE/'native-beaches.json').read_text())
DESIGNS = json.loads((HERE/'designs.json').read_text())


def extended_patch(name, extra_x, extra_y):
    patch = [row[:] for row in TEMPLATES[name]['blocks']]
    if extra_x:
        candidates = [x for x in range(len(patch[0]))
                      if all(row[x] & 1023 in {0x10D, 0x115, 0x11D, 0x12B} for row in patch)]
        assert candidates, (name, 'no straight column')
        x = min(candidates, key=lambda x: abs(x-len(patch[0])/2))
        patch = [row[:x]+[row[x]]*extra_x+row[x:] for row in patch]
    if extra_y:
        candidates = [y for y, row in enumerate(patch)
                      if all(v & 1023 in {0x114, 0x115, 0x116, 0x12B} for v in row)]
        assert candidates, (name, 'no straight row')
        y = min(candidates, key=lambda y: abs(y-len(patch)/2))
        patch = patch[:y]+[patch[y][:] for _ in range(extra_y)]+patch[y:]
    return patch


def build():
    path = ROOT/'data/layouts/layouts.json'
    table = json.loads(path.read_text())
    layouts = {l['id']: l for l in table['layouts']}
    report = {}
    for number, design in DESIGNS.items():
        name = 'OrangeIslands_Route_'+number
        hp = ROOT/'data/maps'/name/'map.json'
        header = json.loads(hp.read_text())
        connections = header['connections']
        layout = layouts[header['layout']]
        w, h = design['size']
        assert (w+15)*(h+14) <= 10240
        grid = [[WATER]*w for _ in range(h)]
        vertical = h > w
        length, breadth = (h, w) if vertical else (w, h)

        def stamp(x, y, patch):
            for dy, row in enumerate(patch):
                for dx, value in enumerate(row):
                    if 0 <= x+dx < w and 0 <= y+dy < h:
                        grid[y+dy][x+dx] = value

        def rock(u, v, brown=False):
            x, y = (v, u) if vertical else (u, v)
            x,y = round(x),round(y)
            cells = [(x+dx,y+dy) for dy in range(2) for dx in range(2)]
            if not all(0<=cx<w and 0<=cy<h and grid[cy][cx]==WATER for cx,cy in cells):
                return
            if number == '3' and any((cx<7 and 22<=cy<34) or
                                     (cx>=w-9 and 8<=cy<21) for cx,cy in cells):
                return
            stamp(x, y, BROWN if brown else BLUE)

        def chain(points, spacing, salt):
            index = 0
            for (u0, v0), (u1, v1) in zip(points, points[1:]):
                steps = max(1, round(max(abs(u1-u0), abs(v1-v0))/spacing))
                for i in range(steps):
                    t = i/steps
                    rock(u0+(u1-u0)*t, v0+(v1-v0)*t, (index+salt)%3 == 1)
                    index += 1
            rock(*points[-1], brown=(index+salt)%3 == 1)

        # One irregular reef chain on each side, with alternating large brown
        # boulders and low grey reefs. The out-of-map border blocks departure;
        # no second straight parallel fence is needed inside the map.
        islands = []
        for template, x, y, ex, ey in design['islets']:
            patch = extended_patch(template, ex, ey)
            stamp(x, y, patch)
            islands.append((x, y, patch))

        for i, points in enumerate(design['banks']):
            chain(points, 2.5, i+int(number))
        for i, points in enumerate(design['reefs']):
            chain(points, 3, i+int(number)+2)

        # Selected islets have a small native sand-backed cliff outcrop.
        # Its beach approach remains visible and walkable on every side.
        for island_index in design.get('outcrops', []):
            ix, iy, patch = islands[island_index]
            candidates = [(x,y) for y in range(1,len(patch)-3)
                          for x in range(1,len(patch[0])-3)
                          if all(patch[y+dy][x+dx]&1023 == 0x115
                                 for dy in range(3) for dx in range(3))]
            assert candidates, (name, 'outcrop needs a dry 3x3 base')
            x,y = candidates[0]
            rock_patch = ((0x3498,0x3499,0x349A),
                          (0x34A0,0x34A1,0x34A2),
                          (0x34A8,0x34A9,0x34AA))
            stamp(ix+x,iy+y,rock_patch)
            for dy,row in enumerate(rock_patch):
                for dx,value in enumerate(row):
                    patch[y+dy][x+dx] = value

        # Keep both of the user's Route 3 links and their offsets. The west
        # opening aligns to Valencia Beach; the east sand tongue meets Mikan.
        if number == '3':
            for y in range(0, 22, 2):
                stamp(0, y, BLUE)
            for y in range(22, 34):
                for x in range(7):
                    grid[y][x] = WATER
            for y in range(8, 21):
                for x in range(w-9, w):
                    grid[y][x] = WATER
            for x in range(w-6, w):
                grid[11][x] = 0x310D
                grid[17][x] = 0x311D
                for y in range(12, 17):
                    grid[y][x] = 0x3103
            grid[11][w-7] = 0x310C
            grid[17][w-7] = 0x311C
            for y in range(12, 17):
                grid[y][w-7] = 0x3114

        for x, y, width, height in design.get('currents', []):
            for dx in range(width):
                for cy in range(y+dx%3, y+height-dx%2):
                    assert grid[cy][x+dx] == WATER, (name, 'current overlaps a feature')
                    grid[cy][x+dx] = 0x11EC

        # Rehome the existing signs onto dry sand, preserving their script IDs.
        signs = []
        for event, (ix, iy, patch) in zip(header['bg_events'], islands):
            spots = [(x, y) for y in range(1, len(patch)-1)
                     for x in range(1, len(patch[0])-1)
                     if patch[y][x] & 1023 == 0x115
                     and any(patch[ny][nx] & 1023 == 0x115
                             for nx, ny in [(x,y+1),(x-1,y),(x+1,y),(x,y-1)])]
            assert spots, (name, 'no dry sign approach')
            x, y = min(spots, key=lambda p: abs(p[0]-len(patch[0])/2)+abs(p[1]-len(patch)/2))
            event['x'], event['y'], event['elevation'] = ix+x, iy+y, 3
            grid[iy+y][ix+x] = 0x3402
            ax, ay = next((nx,ny) for nx,ny in [(x,y+1),(x-1,y),(x+1,y),(x,y-1)]
                          if patch[ny][nx] & 1023 == 0x115)
            signs.append([ix+ax, iy+ay])
        assert len(signs) == len(header['bg_events'])
        assert header['connections'] == connections
        layout['width'], layout['height'] = w, h
        (ROOT/layout['blockdata_filepath']).write_bytes(struct.pack('<'+'H'*(w*h), *(v for row in grid for v in row)))
        (ROOT/layout['border_filepath']).write_bytes(struct.pack('<4H', *sum(BLUE, ())))
        hp.write_text(json.dumps(header, indent=2)+'\n')
        # All sign text describes local navigation; disconnected endpoints do
        # not advertise island links that the user will arrange later.
        texts = []
        for index, event in enumerate(header['bg_events']):
            symbol = event['script'].replace('_EventScript_', '_Text_')
            if number == '3' and index == 0:
                message = ['ROUTE 3 - REEF PASSAGE', 'West: VALENCIA BEACH', 'East: MIKAN BEACH']
            elif number == '5' and index == 0:
                message = ['ROUTE 5 - CURRENT CHANNEL', 'Currents flow north.', 'Calm water passes on either side.']
            else:
                message = ['ROUTE '+number+' - '+design['title'].upper(),
                           ['Follow the gaps between reefs.', 'A small beach to rest on.', 'Keep the open channel clear.'][index%3]]
            body = '\n'.join('\t.string '+json.dumps(line+('\\n' if i<len(message)-1 else '$')) for i, line in enumerate(message))
            # Assembly expects literal \n, rather than a doubly escaped slash.
            body = body.replace('\\\\n', '\\n')
            texts.append(symbol+'::\n'+body+'\n')
        (hp.parent/'text.inc').write_text('\n'.join(texts))
        # The renderer operates directly on the changed data, with no
        # generated binary or manual image edits between maps and previews.
        render(layout).save(HERE.parent/(name+'.png'))
        report[number] = {'size':[w,h], 'reference':design['reference'], 'sign_approaches':signs}
    path.write_text(json.dumps(table, indent=2)+'\n')
    (HERE/'landmarks.json').write_text(json.dumps(report, indent=2)+'\n')
    overview()
    print('Rebuilt Routes 3-14; connection records preserved.')


def overview():
    out = Image.new('RGB',(1600,1440),(29,38,43))
    draw = ImageDraw.Draw(out)
    for i,(number,design) in enumerate(DESIGNS.items()):
        im = Image.open(HERE.parent/f'OrangeIslands_Route_{number}.png')
        vertical = im.height>im.width
        if vertical:
            im = im.transpose(Image.Transpose.ROTATE_90)
        im.thumbnail((770,192),Image.Resampling.NEAREST)
        x,y = i%2*800,i//2*240
        draw.text((x+10,y+5),f'Route {number}: {design["title"]}',fill='white')
        draw.text((x+10,y+20),f'{design["size"][0]} x {design["size"][1]} / Hoenn {design["reference"]}'+(' / rotated for overview' if vertical else ''),fill=(190,210,215))
        out.paste(im,(x+10,y+42))
    out.save(HERE/'overview.png')


if __name__ == '__main__':
    build()
