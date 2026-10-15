"""Seal unused sea-map edges with whole native reef objects.

The two existing Route 3 beach joins remain open. Run after terrain generation;
the operation is idempotent and does not regenerate any route interiors.
"""
import json
import struct
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]


def seal(grid, hoenn=False, openings=frozenset()):
    h, w = len(grid), len(grid[0])
    motifs = [(0x150,0x151,0x158,0x159),(0x175,0x176,0x17D,0x17E)] if hoenn else [(0x110,0x111,0x118,0x119),(0x1DB,0x1DC,0x1E3,0x1E4)]
    stamps = {(x,y) for x in range(0,w,2) for y in (0,h-2)} | {(x,y) for y in range(2,h-2,2) for x in (0,w-2)}
    stamps = {(x,y) for x,y in stamps if not any((x+dx,y+dy) in openings for dx in range(2) for dy in range(2))}
    changed = {(x+dx,y+dy) for x,y in stamps for dx in range(2) for dy in range(2)}
    # Clear entire old primitive objects if their anchor grid differs. This
    # prevents half boulders where the new perimeter crosses an existing reef.
    clear = set()
    for x,y in changed:
        mid=grid[y][x]&1023
        for motif in motifs:
            if mid not in motif: continue
            k=motif.index(mid); ax,ay=x-k%2,y-k//2
            if 0<=ax<w-1 and 0<=ay<h-1:
                clear.update((ax+dx,ay+dy) for dx in range(2) for dy in range(2))
    water = 0x1170 if hoenn else 0x112B
    for x,y in clear: grid[y][x]=water
    for x,y in sorted(stamps):
        motif=motifs[(x//2+y//2)%7==0]
        for k,mid in enumerate(motif): grid[y+k//2][x+k%2]=0x400|mid
    return grid


def build():
    layouts=json.loads((ROOT/'data/layouts/layouts.json').read_text())['layouts']
    for number in range(3,15):
        name=f'OrangeIslands_Route_{number}'
        layout=next(l for l in layouts if l['name']==name+'_Layout')
        if number==11: continue  # The authored maze already has a closed reef rim.
        w,h=layout['width'],layout['height'];path=ROOT/layout['blockdata_filepath']
        values=struct.unpack('<'+'H'*(w*h),path.read_bytes())
        grid=[list(values[y*w:(y+1)*w]) for y in range(h)]
        openings={(x,y) for x in (0,1) for y in range(22,34)} | {(x,y) for x in (w-2,w-1) for y in range(2,h-2)} if number==3 else set()
        seal(grid,layout['primary_tileset']=='gTileset_HoennGeneral',openings)
        path.write_bytes(struct.pack('<'+'H'*(w*h),*(v for row in grid for v in row)))
    print('Sealed unused edges on all twelve sea routes; Route 3 beach joins retained.')


if __name__=='__main__':build()
