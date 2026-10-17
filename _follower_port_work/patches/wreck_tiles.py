# Adds the Route 108 Abandoned Ship hull (Emerald Slateport 849-875) to orange_sevii_45, palette slot 12.
import sys,struct,json
sys.path.insert(0,'/tmp/claude-0/hoenn')
from erender import Pair
from PIL import Image
S='/home/claude/fr2/data/tilesets/secondary/orange_sevii_45/'
PAL=12
p=Pair('primary/general','secondary/slateport')
GRID=[[None,849,850,851,852,None],[856,857,858,859,860,861],[864,865,866,867,868,869],[None,873,874,875,None,None]]
WATER=[0x4001,0x4002,0x4003,0x4004]
# gather RGB top-layer tiles (8x8, None=transparent)
def top_tile(e):
    t=p.tile_idx(e&0x3ff); pal=p.pal[e>>12]
    out=[]
    for yy in range(8):
        row=[]
        for xx in range(8):
            sx=7-xx if e&0x400 else xx; sy=7-yy if e&0x800 else yy
            c=t[sy][sx]; row.append(None if c==0 else pal[c])
        out.append(row)
    return out
def mt_top(m):
    es=p.entries(m); tops=[]
    for q in range(4):
        # emerald: bottom layer = water (entries 0-3); some (866,874) have wreck pieces in bottom too
        b=es[q]; t=es[4+q]
        tb=top_tile(b) if (b&0x3ff) not in (448,449) else None
        tt=top_tile(t) if (t&0x3ff)!=0 else None
        if tb and tt:
            tb=[[tt[y][x] if tt[y][x] is not None else tb[y][x] for x in range(8)] for y in range(8)]
        tops.append(tt if tb is None else tb)
    return tops
cells={}
for gy,row in enumerate(GRID):
    for gx,m in enumerate(row):
        if m: cells[(gx,gy)]=mt_top(m)
cols=set()
for v in cells.values():
    for t in v:
        if t:
            for r in t:
                for c in r:
                    if c is not None: cols.add(c)
print('colors',len(cols))
cols=sorted(cols)
if len(cols)>15:
    # quantize with PIL
    im=Image.new('RGB',(len(cols),1)); [im.putpixel((i,0),c) for i,c in enumerate(cols)]
    q=im.quantize(15,method=Image.MEDIANCUT); qp=q.getpalette()[:45]
    palette=[tuple(qp[i*3:i*3+3]) for i in range(15)]
else: palette=cols
def nearest(c): return min(range(len(palette)),key=lambda i:sum((a-b)**2 for a,b in zip(c,palette[i])))+1
cmap={c:nearest(c) for c in cols}
# tiles image
timg=Image.open(S+'tiles.png'); W=timg.width//8
pk=json.load(open(S+'packing.json')); used=pk['occupied_tiles']
mtb=bytearray(open(S+'metatiles.bin','rb').read()); atb=bytearray(open(S+'metatile_attributes.bin','rb').read())
n0=len(mtb)//16
tiles=[]; tindex={}
def add_tile(t):
    key=tuple(tuple(cmap[c] if c is not None else 0 for c in r) for r in t)
    for fl,k in ((0,key),(0x400,tuple(tuple(reversed(r)) for r in key)),(0x800,tuple(reversed(key))),(0xc00,tuple(tuple(reversed(r)) for r in reversed(key)))):
        if k in tindex: return tindex[k]|fl
    idx=640+used+len(tiles); tiles.append(key); tindex[key]=idx; return idx
ids={}
for (gx,gy),tops in sorted(cells.items(),key=lambda kv:(kv[0][1],kv[0][0])):
    ent=list(WATER)+[ (add_tile(t)|(PAL<<12)) if t else 0 for t in tops]
    mtb+=struct.pack('<8H',*ent)
    m=GRID[gy][gx]
    attr=0x22000415 if m==874 else 0x20000000
    atb+=struct.pack('<I',attr)
    ids[(gx,gy)]=0x280+n0+len(ids)
print('new tiles',len(tiles),'metatiles',len(ids), 'first id',hex(0x280+n0))
need=used+len(tiles); H=max(timg.height,((need+W-1)//W)*8)
ni=Image.new('P',(timg.width,H),0); ni.putpalette(timg.getpalette()); ni.paste(timg,(0,0))
for i,k in enumerate(tiles):
    s=used+i; x=(s%W)*8; y=(s//W)*8
    for yy in range(8):
        for xx in range(8): ni.putpixel((x+xx,y+yy),k[yy][xx])
ni.save(S+'tiles.png'); open(S+'metatiles.bin','wb').write(mtb); open(S+'metatile_attributes.bin','wb').write(atb)
pal=[(0,0,0)]+palette+[(0,0,0)]*(15-len(palette))
open(S+'palettes/%02d.pal'%PAL,'w').write('JASC-PAL\r\n0100\r\n16\r\n'+''.join('%d %d %d\r\n'%c for c in pal))
pk['occupied_tiles']=need; pk['free_graphics_slots']=384-need; pk['metatiles']=len(mtb)//16; pk['free_metatile_slots']=384-len(mtb)//16
pk['wreck_metatiles']={'%d,%d'%k:v for k,v in ids.items()}
json.dump(pk,open(S+'packing.json','w'),indent=2)
json.dump({'%d,%d'%k:v for k,v in ids.items()},open('/tmp/claude-0/wreck/ids.json','w'))
