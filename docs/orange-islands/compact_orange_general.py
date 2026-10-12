"""Losslessly repack General graphics; preserve metatile IDs and animation banks.
Run from any directory. Existing generated outputs are checked before replacement.
"""
from pathlib import Path
import json,struct,shutil,subprocess
ROOT=Path(__file__).resolve().parents[2]
SOURCE=ROOT/'data/tilesets/primary/general'
DEST=ROOT/'data/tilesets/primary/orange_general'
SECONDARY=ROOT/'data/tilesets/secondary/orange_islands/metatiles.bin'
MANIFEST=DEST/'packing.json'
GFX=ROOT/'tools/gbagfx/gbagfx'
# Use the repository converter rather than interpreting PNG palette colors.
subprocess.run([str(GFX),str(SOURCE/'tiles.png'),str(SOURCE/'tiles.4bpp'),'-num_tiles','640'],check=True)
raw=(SOURCE/'tiles.4bpp').read_bytes();tiles=[raw[i:i+32] for i in range(0,len(raw),32)]
def flip(tile,h,v):
 pixels=[[((tile[y*4+x//2] >> (4*(x%2)))&15) for x in range(8)] for y in range(8)]
 if h:pixels=[row[::-1] for row in pixels]
 if v:pixels=pixels[::-1]
 return bytes(row[x]|(row[x+1]<<4) for row in pixels for x in range(0,8,2))
packed=[tiles[0]];mapping={0:(0,0)};banks={}
for name,start,count in [('water',416,48),('sand',464,18),('flower',508,4)]:
 banks[name]=dict(source_start=start,destination_start=len(packed),count=count)
 for old in range(start,start+count):mapping[old]=(len(packed),0);packed.append(tiles[old])
# Animated slots are deliberately excluded from static deduplication.
lookup={}
for flags in (0,0x400,0x800,0xc00):lookup.setdefault(flip(tiles[0],flags&0x400,flags&0x800),(0,flags))
for old,tile in enumerate(tiles):
 if old in mapping:continue
 if tile in lookup:mapping[old]=lookup[tile];continue
 new=len(packed);packed.append(tile);mapping[old]=(new,0)
 for flags in (0,0x400,0x800,0xc00):lookup.setdefault(flip(tile,flags&0x400,flags&0x800),(new,flags))
assert len(packed)<=640
used=len(packed);binary=b''.join(packed)+bytes((640-used)*32)
def remap(data):
 result=[]
 for (entry,) in struct.iter_unpack('<H',data):
  old=entry&1023
  if old<640:
   new,flags=mapping[old];entry=(entry&0xf000)|((entry&0xc00)^flags)|new
  result.append(entry)
 return struct.pack('<'+'H'*len(result),*result)
primary=remap((SOURCE/'metatiles.bin').read_bytes())
if MANIFEST.exists():
 previous=json.loads(MANIFEST.read_text());secondary_source=bytes.fromhex(previous['original_secondary_metatiles_hex'])
 assert SECONDARY.read_bytes()==remap(secondary_source),'Secondary metatiles changed; review before regenerating.'
else:secondary_source=SECONDARY.read_bytes()
secondary=remap(secondary_source)
DEST.mkdir(parents=True,exist_ok=True)
(DEST/'palettes').mkdir(exist_ok=True)
for p in (SOURCE/'palettes').glob('*.pal'):
 if not (DEST/'palettes'/p.name).exists():shutil.copyfile(p,DEST/'palettes'/p.name)
(DEST/'tiles.4bpp').write_bytes(binary)
subprocess.run([str(GFX),str(DEST/'tiles.4bpp'),str(DEST/'tiles.png'),'-width','16','-palette',str(DEST/'palettes/00.pal')],check=True)
(DEST/'metatiles.bin').write_bytes(primary)
shutil.copyfile(SOURCE/'metatile_attributes.bin',DEST/'metatile_attributes.bin')
SECONDARY.write_bytes(secondary)
report={'used_tiles':used,'free_primary_tiles':640-used,'first_free_tile':used,'animation_banks':banks,'mapping':{str(k):list(v) for k,v in mapping.items()},'original_secondary_metatiles_hex':secondary_source.hex()}
MANIFEST.write_text(json.dumps(report,indent=2)+'\n')
header='#ifndef GUARD_ORANGE_GENERAL_TILES_H\n#define GUARD_ORANGE_GENERAL_TILES_H\n\n'
for name,bank in banks.items():header+=f'#define ORANGE_GENERAL_{name.upper()}_TILE_START {bank["destination_start"]}\n'
header+=f'#define ORANGE_GENERAL_FIRST_FREE_TILE {used}\n#define ORANGE_GENERAL_FREE_TILES {640-used}\n\n#endif\n'
(ROOT/'include/constants/orange_general_tiles.h').write_text(header)
print(f'Orange General: {used}/640 primary tiles occupied; {640-used} free slots. All 640 metatiles retained.')
