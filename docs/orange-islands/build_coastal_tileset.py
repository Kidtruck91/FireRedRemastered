"""Combine tropical assets and selected condensed Sevii buildings without changing tropical slots."""
from pathlib import Path
import json,struct,subprocess,tempfile,shutil
ROOT=Path(__file__).resolve().parents[2];GFX=ROOT/'tools/gbagfx/gbagfx'
base=ROOT/'data/tilesets/secondary/orange_islands';donor=ROOT/'data/tilesets/secondary/sevii_islands_123_condensed';dest=ROOT/'data/tilesets/secondary/orange_coastal_town'
selected=sorted(set(range(0x280,0x2b0))|set(range(0x2e0,0x2f0))|set(range(0x310,0x318))|set(range(0x31c,0x32a))|set(range(0x380,0x384))|{0x388,0x38a,0x38b})
def flip(b,f):
 r=[[b[y*4+x//2]>>(4*(x%2))&15 for x in range(8)] for y in range(8)]
 if f&0x400:r=[row[::-1] for row in r]
 if f&0x800:r=r[::-1]
 return bytes(row[x]|row[x+1]<<4 for row in r for x in range(0,8,2))
with tempfile.TemporaryDirectory() as tmp:
 buffers=[]
 for i,p in enumerate((base,donor)):
  out=Path(tmp)/f'{i}.4bpp';subprocess.run([str(GFX),str(p/'tiles.png'),str(out)],check=True);b=out.read_bytes();buffers.append([b[j:j+32] for j in range(0,len(b),32)])
 packed=buffers[0][:];lookup={}
 for i,t in enumerate(packed):
  for f in (0,0x400,0x800,0xc00):lookup.setdefault(flip(t,f),(i,f))
 metas=bytearray((base/'metatiles.bin').read_bytes());attrs=bytearray((base/'metatile_attributes.bin').read_bytes());dm=(donor/'metatiles.bin').read_bytes();da=(donor/'metatile_attributes.bin').read_bytes();mapping={}
 for id in selected:
  new_id=640+len(metas)//16;mapping[str(id)]=new_id;entries=[]
  for (v,) in struct.iter_unpack('<H',dm[(id-640)*16:(id-640+1)*16]):
   before=v
   if v&1023>=640:
    t=buffers[1][(v&1023)-640]
    if t not in lookup:
     new=len(packed);packed.append(t)
     for f in (0,0x400,0x800,0xc00):lookup.setdefault(flip(t,f),(new,f))
    idx,f=lookup[t];v=(v&0xf000)|((v&0xc00)^f)|(640+idx)
    assert flip(t,before&0xc00)==flip(packed[(v&1023)-640],v&0xc00)
   assert before&0xf000==v&0xf000;entries.append(v)
  metas.extend(struct.pack('<8H',*entries));attrs.extend(da[(id-640)*4:(id-640+1)*4])
 # Append Ivy's lab from condensed Pallet; its three private palette banks use 13-15.
 pallet=ROOT/'data/tilesets/secondary/pallet_town_condensed'
 out=Path(tmp)/'pallet.4bpp';subprocess.run([str(GFX),str(pallet/'tiles.png'),str(out)],check=True)
 b=out.read_bytes();pt=[b[j:j+32] for j in range(0,len(b),32)]
 pm=(pallet/'metatiles.bin').read_bytes();pa=(pallet/'metatile_attributes.bin').read_bytes()
 lab_ids=[0x2a8,0x2a9,0x2bd,0x2b5,0x2b0,0x2b1,0x2b3,0x2b4,0x2b8,0x2b9,0x2bb,0x2bc,0x2c0,0x2c1,0x2d0,0x2c2,0x2c3,0x2c4,0x2c5,0x2c8,0x2c9,0x2d8,0x2ac,0x2cb,0x2cc,0x2cd]
 pallet_mapping={};palette_mapping={9:13,10:14,11:15}
 for id in sorted(lab_ids):
  pallet_mapping[str(id)]=640+len(metas)//16;entries=[]
  for (v,) in struct.iter_unpack('<H',pm[(id-640)*16:(id-639)*16]):
   before=v
   if v&1023>=640:
    t=pt[(v&1023)-640]
    if t not in lookup:
     new=len(packed);packed.append(t)
     for f in (0,0x400,0x800,0xc00):lookup.setdefault(flip(t,f),(new,f))
    idx,f=lookup[t];v=(v&0xf000)|((v&0xc00)^f)|(640+idx)
    assert flip(t,before&0xc00)==flip(packed[(v&1023)-640],v&0xc00)
   bank=v>>12
   assert bank<7 or bank in palette_mapping
   if bank>=7:v=(v&0xfff)|(palette_mapping[bank]<<12)
   entries.append(v)
  metas.extend(struct.pack('<8H',*entries));attrs.extend(pa[(id-640)*4:(id-639)*4])
 assert len(packed)<=384 and len(metas)//16<=384
 if dest.exists():
  existing=(dest/'metatiles.bin').read_bytes();previous=json.loads((dest/'packing.json').read_text())
  assert existing==metas or ('pallet_lab_mapping' not in previous and existing==metas[:len(existing)]),'Edited output: review before regenerating'
 if not dest.exists():previous={}
 dest.mkdir(exist_ok=True);(dest/'palettes').mkdir(exist_ok=True)
 for i in range(16):
  if not (dest/'palettes'/f'{i:02}.pal').exists():shutil.copyfile((base if i<7 else donor)/'palettes'/f'{i:02}.pal',dest/'palettes'/f'{i:02}.pal')
 for source_bank,target_bank in palette_mapping.items():
  target=dest/'palettes'/f'{target_bank:02}.pal'
  if dest.exists() and target.exists() and 'pallet_lab_mapping' not in previous:
   assert target.read_bytes()==(donor/'palettes'/f'{target_bank:02}.pal').read_bytes(),'Edited palette: review before extension'
  if 'pallet_lab_mapping' not in previous:shutil.copyfile(pallet/'palettes'/f'{source_bank:02}.pal',target)
 (dest/'metatiles.bin').write_bytes(metas);(dest/'metatile_attributes.bin').write_bytes(attrs)
 raw=b''.join(packed)+bytes((-len(packed)%16)*32);out=Path(tmp)/'mixed.4bpp';out.write_bytes(raw)
 subprocess.run([str(GFX),str(out),str(dest/'tiles.png'),'-width','16','-palette',str(dest/'palettes/07.pal')],check=True)
 check=Path(tmp)/'check.4bpp';subprocess.run([str(GFX),str(dest/'tiles.png'),str(check)],check=True);assert check.read_bytes()==raw
 assert metas[:(base/'metatiles.bin').stat().st_size]==(base/'metatiles.bin').read_bytes()
 report=dict(primary='gTileset_OrangeGeneral',occupied_tiles=len(packed),free_graphics_slots=384-len(packed),metatiles=len(metas)//16,free_metatile_slots=384-len(metas)//16,preserved_tropical_tiles=len(buffers[0]),sevii_metatile_mapping=mapping,pallet_lab_mapping=pallet_mapping,pallet_palette_mapping=palette_mapping)
 (dest/'packing.json').write_text(json.dumps(report,indent=2)+'\n')
 print(f'Coastal town: {len(packed)} graphics slots, {len(metas)//16} metatiles; tropical graphics/IDs preserved.')
