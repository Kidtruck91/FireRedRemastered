"""Create and verify independent secondary copies paired with OrangeGeneral."""
from pathlib import Path
import json,struct,subprocess,tempfile,shutil,re
ROOT=Path(__file__).resolve().parents[2]
SETS={'pallet_town':'PalletTown','vermilion_city':'VermilionCity','sevii_islands_123':'SeviiIslands123','sevii_islands_45':'SeviiIslands45','sevii_islands_67':'SeviiIslands67'}
GFX=ROOT/'tools/gbagfx/gbagfx'
primary=json.loads((ROOT/'data/tilesets/primary/orange_general/packing.json').read_text())['mapping']
def flip(b,flags):
 r=[[b[y*4+x//2]>>(4*(x%2))&15 for x in range(8)] for y in range(8)]
 if flags&0x400:r=[row[::-1] for row in r]
 if flags&0x800:r=r[::-1]
 return bytes(row[x]|row[x+1]<<4 for row in r for x in range(0,8,2))
reports=[]
with tempfile.TemporaryDirectory() as tmp:
 for name,symbol in SETS.items():
  source=ROOT/'data/tilesets/secondary'/name;dest=ROOT/'data/tilesets/secondary'/(name+'_condensed')
  headers=(ROOT/'src/data/tilesets/headers.h').read_text()
  block=re.search(r'const struct Tileset gTileset_'+symbol+r' =\s*\{(.*?)\};',headers,re.S)
  assert block and '.callback = NULL' in block.group(1),name+': animated or unknown tileset; static packing is unsafe'
  target=Path(tmp)/'source.4bpp';subprocess.run([str(GFX),str(source/'tiles.png'),str(target)],check=True)
  raw=target.read_bytes();tiles=[raw[i:i+32] for i in range(0,len(raw),32)]
  old=(source/'metatiles.bin').read_bytes();entries=[v for (v,) in struct.iter_unpack('<H',old)]
  referenced=sorted({(v&1023)-640 for v in entries if v&1023>=640})
  packed=[bytes(32)];lookup={bytes(32):(0,0)};mapping={}
  for idx in referenced:
   assert idx<len(tiles),name+': graphics reference out of range'
   t=tiles[idx]
   if t in lookup:mapping[idx]=lookup[t];continue
   new=len(packed);packed.append(t);mapping[idx]=(new,0)
   for flags in (0,0x400,0x800,0xc00):lookup.setdefault(flip(t,flags),(new,flags))
  assert len(packed)<=384
  new_entries=[]
  for v in entries:
   idx=v&1023
   new,flags=primary[str(idx)] if idx<640 else mapping[idx-640]
   new_entries.append((v&0xf000)|((v&0xc00)^flags)|(new if idx<640 else new+640))
  result=struct.pack('<'+'H'*len(new_entries),*new_entries)
  # Every secondary graphic remains pixel-index equivalent; primary mappings are separately validated.
  for before,after in zip(entries,new_entries):
   assert before&0xf000==after&0xf000
   if before&1023>=640:
    assert flip(tiles[(before&1023)-640],before&0xc00)==flip(packed[(after&1023)-640],after&0xc00)
  if dest.exists():
   assert (dest/'metatiles.bin').read_bytes()==result,'Edited condensed metatiles: review before regeneration'
  dest.mkdir(exist_ok=True);(dest/'palettes').mkdir(exist_ok=True)
  for p in (source/'palettes').glob('*.pal'):
   if not (dest/'palettes'/p.name).exists():shutil.copyfile(p,dest/'palettes'/p.name)
  (dest/'metatiles.bin').write_bytes(result);shutil.copyfile(source/'metatile_attributes.bin',dest/'metatile_attributes.bin')
  assert (source/'metatile_attributes.bin').read_bytes()==(dest/'metatile_attributes.bin').read_bytes()
  count=len(packed);padding=(-count)%16;out=Path(tmp)/'packed.4bpp';out.write_bytes(b''.join(packed)+bytes(padding*32))
  subprocess.run([str(GFX),str(out),str(dest/'tiles.png'),'-width','16','-palette',str(dest/'palettes/07.pal')],check=True)
  roundtrip=Path(tmp)/'check.4bpp';subprocess.run([str(GFX),str(dest/'tiles.png'),str(roundtrip)],check=True);assert roundtrip.read_bytes()==out.read_bytes()
  report=dict(source=name,symbol='gTileset_'+symbol+'Condensed',primary='gTileset_OrangeGeneral',source_png_tiles=len(tiles),referenced_source_tiles=len(referenced),occupied_tiles=count,free_secondary_slots=384-count,metatiles=len(entries)//8)
  (dest/'packing.json').write_text(json.dumps(report,indent=2)+'\n');reports.append(report)
  print(name,':',len(tiles),'source PNG tiles ->',count,'occupied;',384-count,'free slots;',len(entries)//8,'metatiles verified')
(ROOT/'docs/orange-islands/secondary_packing_report.json').write_text(json.dumps(reports,indent=2)+'\n')
