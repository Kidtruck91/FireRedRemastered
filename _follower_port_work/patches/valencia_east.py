#!/usr/bin/env python3
# Valencia Beach joins Valencia's east side seamlessly (offset +16: rock, water, sand rows 20-53 line up).
# Valencia Beach's west 8 columns (cliff block) become the same beach pattern as column 8; Valencia's blocked
# east-edge sand is opened. Route 5's east edge gets a rock rim. Run from the repo root.
import json, os, struct, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from land_common import read_bin, write_bin, connect
vb = read_bin('data/layouts/OrangeIslands_Valencia_Beach/map.bin', 96)
for y in range(len(vb)):
    src = vb[y][8]; t = src & 0x3ff
    for x in range(8):
        if t in (0x110, 0x111): v = (0x110 + (x & 1)) | (src & ~0x3ff)
        elif t in (0x118, 0x119): v = (0x118 + (x & 1)) | (src & ~0x3ff)
        else: v = src
        vb[y][x] = v
write_bin('data/layouts/OrangeIslands_Valencia_Beach/map.bin', vb)
va = read_bin('data/layouts/OrangeIslands_Valencia/map.bin', 67)
for y in range(20, 54):
    for x in range(55, 67):
        v = va[y][x]
        if (v & 0x3ff) in (0xdc, 0x115) and (v >> 10) == 13: va[y][x] = (v & 0x3ff) | (3 << 12)
write_bin('data/layouts/OrangeIslands_Valencia/map.bin', va)
connect('Valencia', 'Valencia_Beach', 'right', 16)
r5 = read_bin('data/layouts/OrangeIslands_Route_5/map.bin', 48)
for y in range(len(r5)):
    for x in (46, 47):
        r5[y][x] = (0x150 + (x & 1) + (8 if y & 1 else 0)) | (1 << 10)
write_bin('data/layouts/OrangeIslands_Route_5/map.bin', r5)
print('valencia_east ok')
