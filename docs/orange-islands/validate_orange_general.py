"""Verify lossless tile remapping and every retained animation frame."""
from pathlib import Path
import struct,json,subprocess,tempfile
ROOT=Path(__file__).resolve().parents[2]
source=ROOT/'data/tilesets/primary/general';dest=ROOT/'data/tilesets/primary/orange_general'
report=json.loads((dest/'packing.json').read_text())
def words(b):return [v for (v,) in struct.iter_unpack('<H',b)]
def flip(b,flags):
 rows=[[((b[y*4+x//2]>>(4*(x%2)))&15) for x in range(8)] for y in range(8)]
 if flags&0x400:rows=[r[::-1] for r in rows]
 if flags&0x800:rows=rows[::-1]
 return bytes(r[x]|(r[x+1]<<4) for r in rows for x in range(0,8,2))
with tempfile.TemporaryDirectory() as tmp:
 buffers=[]
 for i,p in enumerate([source,dest,ROOT/'data/tilesets/secondary/orange_islands']):
  target=Path(tmp)/f'{i}.4bpp'
  subprocess.run([str(ROOT/'tools/gbagfx/gbagfx'),str(p/'tiles.png'),str(target)],check=True)
  raw=target.read_bytes();buffers.append([raw[j:j+32] for j in range(0,len(raw),32)])
 old,new,secondary=buffers;assert len(old)==len(new)==640
 assert all(t==bytes(32) for t in new[report['first_free_tile']:])
 comparisons=[((source/'metatiles.bin').read_bytes(),(dest/'metatiles.bin').read_bytes()),(bytes.fromhex(report['original_secondary_metatiles_hex']),(ROOT/'data/tilesets/secondary/orange_islands/metatiles.bin').read_bytes())]
 def verify(old_tiles,new_tiles):
  for before,after in comparisons:
   assert len(before)==len(after)
   for a,b in zip(words(before),words(after)):
    assert a&0xf000==b&0xf000,'Palette bank changed during tile remapping'
    ai,bi=a&1023,b&1023
    at=old_tiles[ai] if ai<640 else secondary[ai-640]
    bt=new_tiles[bi] if bi<640 else secondary[bi-640]
    assert flip(at,a&0xc00)==flip(bt,b&0xc00),'Metatile pixel indices changed'
 verify(old,new)
 assert (source/'metatile_attributes.bin').read_bytes()==(dest/'metatile_attributes.bin').read_bytes()
 frame_count=0
 names={'water':'water_current_landwatersedge','sand':'sandwatersedge','flower':'flower'}
 for name,bank in report['animation_banks'].items():
  for frame in sorted((source/'anim'/names[name]).glob('*.png')):
   target=Path(tmp)/'frame.4bpp';subprocess.run([str(ROOT/'tools/gbagfx/gbagfx'),str(frame),str(target)],check=True)
   b=target.read_bytes();assert len(b)==bank['count']*32
   tiles=[b[j:j+32] for j in range(0,len(b),32)];o=old[:];n=new[:]
   o[bank['source_start']:bank['source_start']+bank['count']]=tiles
   n[bank['destination_start']:bank['destination_start']+bank['count']]=tiles
   verify(o,n);frame_count+=1
layouts=json.loads((ROOT/'data/layouts/layouts.json').read_text())['layouts']
sea_routes={f'OrangeIslands_Route_{n}_Layout' for n in range(4,15)}
alternate_pairs={('gTileset_General','gTileset_SeviiIslands45'),('gTileset_General','gTileset_SeviiIslands67'),
                 *{('gTileset_HoennGeneral','gTileset_Hoenn'+name) for name in ('Dewford','Mossdeep','Pacifidlog')}}
for layout in layouts:
 if not layout['name'].startswith('OrangeIslands_'):continue
 if layout['primary_tileset']=='gTileset_OrangeGeneral':continue
 assert layout['name'] in sea_routes and (layout['primary_tileset'],layout['secondary_tileset']) in alternate_pairs,'Unapproved Orange tileset pairing'
assert all(l['primary_tileset']!='gTileset_OrangeGeneral' for l in layouts if not l['name'].startswith('OrangeIslands_'))
print(f'All 736 metatile definitions are pixel-index equivalent; attributes/palette banks preserved; {frame_count} animation frames verified.')
orange_count=sum(l['name'].startswith('OrangeIslands_') and l['primary_tileset']=='gTileset_OrangeGeneral' for l in layouts)
print(f'{report["free_primary_tiles"]} free primary graphics slots; {orange_count} Orange layouts use OrangeGeneral; alternate sea-route pairings verified.')
