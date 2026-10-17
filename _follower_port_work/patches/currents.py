#!/usr/bin/env python3
# One-way currents that gate gym progression:
#  - Route 5: a full-width band of southward current (rows 76-79), below the Route 6 junction. From Mikan you can
#    no longer surf north to Navel or Route 6; coming from Navel/Route 6 the current carries you down to Mikan.
#  - Route 14: an eastward current band in front of the Trovita exit (cols 82-85). Kumquat -> Trovita works;
#    arriving from Trovita you are pushed back out. Mossdeep has no current tiles, so Pacifidlog's eastward
#    current metatile is copied into hoenn_native_mossdeep (2 tiles + 1 metatile, appended once).
# Run from the repo root (after world_links.py, which walls Route 14's unused right edge).
import struct
from PIL import Image
def rd(m, W):
    b = open('data/layouts/OrangeIslands_%s/map.bin' % m, 'rb').read(); H = len(b) // 2 // W
    return [[struct.unpack_from('<H', b, (y * W + x) * 2)[0] for x in range(W)] for y in range(H)]
def wr(m, g): open('data/layouts/OrangeIslands_%s/map.bin' % m, 'wb').write(b''.join(struct.pack('<H', v) for r in g for v in r))
surf = lambda v: (v >> 12) == 1 and not (v & 0xc00)
# Route 5 (Pacifidlog): 0x2eb = southward current
g = rd('Route_5', 48)
for y in range(76, 80):
    for x in range(48):
        if surf(g[y][x]): g[y][x] = 0x2eb | (1 << 12)
wr('Route_5', g)
# Mossdeep: add an eastward current metatile (copy of Pacifidlog 0x2e4)
S = 'data/tilesets/secondary/'
pac_mt = open(S + 'hoenn_native_pacifidlog/metatiles.bin', 'rb').read()
mos = S + 'hoenn_native_mossdeep/'
mt = bytearray(open(mos + 'metatiles.bin', 'rb').read()); at = bytearray(open(mos + 'metatile_attributes.bin', 'rb').read())
src = list(struct.unpack_from('<8H', pac_mt, (0x2e4 - 640) * 16))
pim = Image.open(S + 'hoenn_native_pacifidlog/tiles.png'); mim = Image.open(mos + 'tiles.png')
def tile_of(im, t):
    tw = im.width // 8; return im.crop(((t % tw) * 8, (t // tw) * 8, (t % tw) * 8 + 8, (t // tw) * 8 + 8))
EAST = None
for i in range(len(mt) // 16):
    if struct.unpack_from('<I', at, i * 4)[0] == 0x20000450: EAST = 640 + i
if EAST is None:
    ntiles = (mim.width // 8) * (mim.height // 8)
    used = {(e & 0x3ff) - 640 for i in range(len(mt) // 16) for e in struct.unpack_from('<8H', mt, i * 16) if (e & 0x3ff) >= 640}
    first = max(used) + 1
    need = first + 2; rows = (need + mim.width // 8 - 1) // (mim.width // 8)
    if rows * 8 > mim.height:
        ni = Image.new('P', (mim.width, rows * 8), 0); ni.putpalette(mim.getpalette()); ni.paste(mim, (0, 0)); mim = ni
    remap = {}
    for k, e in enumerate(src[:4]):
        t = e & 0x3ff
        if t not in remap:
            dst = first + len(remap); remap[t] = dst
            mim.paste(tile_of(pim, t - 640), ((dst % (mim.width // 8)) * 8, (dst // (mim.width // 8)) * 8))
    ent = [(e & ~0x3ff) | (640 + remap[e & 0x3ff]) if (e & 0x3ff) >= 640 else e for e in src]
    mt += struct.pack('<8H', *ent); at += struct.pack('<I', 0x20000450)
    EAST = 640 + len(mt) // 16 - 1
    mim.save(mos + 'tiles.png'); open(mos + 'metatiles.bin', 'wb').write(mt); open(mos + 'metatile_attributes.bin', 'wb').write(at)
# Route 14: eastward band in front of the Trovita exit
g = rd('Route_14', 88)
for y in range(2, 16):
    for x in range(82, 86):
        if surf(g[y][x]): g[y][x] = EAST | (1 << 12)
wr('Route_14', g)
print('currents ok; east current metatile', hex(EAST))
