"""Verify source artwork, palettes, layers, budgets, and packed animation banks."""
from pathlib import Path
from functools import lru_cache
import argparse,json,struct,subprocess
from PIL import Image
from import_tilesets import ROOT

def pixels(im,index,flags):
 width=im.width//8;t=im.crop(((index%width)*8,(index//width)*8,(index%width+1)*8,(index//width+1)*8))
 if flags&1024:t=t.transpose(Image.Transpose.FLIP_LEFT_RIGHT)
 if flags&2048:t=t.transpose(Image.Transpose.FLIP_TOP_BOTTOM)
 return bytes(t.tobytes())
def validate(source,rom=False):
 report=json.loads((ROOT/'docs/hoenn-tilesets/import-report.json').read_text());assert subprocess.check_output(['git','rev-parse','HEAD'],cwd=source,text=True).strip()==report['source_commit'],'Source version differs'
 count=0;art=0;rom_bytes=(ROOT/'pokefirered.gba').read_bytes() if rom else None;frames=json.loads((ROOT/'docs/hoenn-tilesets/animation-frames.json').read_text())
 for record in report['tilesets']:
  directory=ROOT/record['directory'];src=source/record['source'];im=Image.open(directory/'tiles.png');native=Image.open(src/'tiles.png');raw=(directory/'metatiles.bin').read_bytes();attrs=(directory/'metatile_attributes.bin').read_bytes()
  metas=list(struct.iter_unpack('<8H',raw));at=[v[0] for v in struct.iter_unpack('<I',attrs)];original=list(struct.iter_unpack('<8H',(src/'metatiles.bin').read_bytes()));original_at=[v[0] for v in struct.iter_unpack('<H',(src/'metatile_attributes.bin').read_bytes())]
  limit=640 if record['is_primary'] else 384;assert len(metas)<=limit and im.width*im.height//64<=limit
  assert len(at)==len(metas);assert len(metas)==record['metatiles'];assert all(v>>12<=12 for row in metas for v in row),'Unloaded palette'
  assert all(a&511<240 and a>>29<3 and (a>>24&7)<3 for a in at),'Invalid FireRed attribute'
  palette_map={int(k):v for k,v in record['palette_mapping'].items()};mapping={int(k):v for k,v in record['metatile_mapping'].items()}
  primary_name='general' if record['is_primary'] and record['symbol']=='HoennGeneral' else 'building'
  # Pixel indices are independent of which supported primary palette is chosen.
  source_primary=Image.open(source/'data/tilesets/primary'/primary_name/'tiles.png')
  target_primary=Image.open(ROOT/'data/tilesets/primary'/('hoenn_general' if primary_name=='general' else 'hoenn_interior')/'tiles.png')
  if not record['is_primary']:
   for primary in record['recommended_primaries']:assert primary in ('gTileset_HoennGeneral','gTileset_HoennInterior')
   # General and Interior both preserve the same source tile indices; compare both pairings.
   primary_name='general' if 'gTileset_HoennGeneral' in record['recommended_primaries'] else 'building'
   source_primary=Image.open(source/'data/tilesets/primary'/primary_name/'tiles.png');target_primary=Image.open(ROOT/'data/tilesets/primary'/('hoenn_general' if primary_name=='general' else 'hoenn_interior')/'tiles.png')
  for bank,target in palette_map.items():assert (src/'palettes'/f'{bank:02}.pal').read_bytes()==(directory/'palettes'/f'{target:02}.pal').read_bytes(),'Palette colors changed'
  for source_id,target_id in mapping.items():
   old_index=source_id if record['is_primary'] else source_id-512;new_index=target_id if record['is_primary'] else target_id-640
   old=original[old_index];new=metas[new_index];assert at[new_index]>>29==original_at[old_index]>>12,'Layer type changed'
   for ov,nv in zip(old,new):
    oi=ov&1023;ni=nv&1023
    old_im=source_primary if oi<512 else native;old_tile=oi if oi<512 else oi-512
    new_im=target_primary if ni<640 else im;new_tile=ni if ni<640 else ni-640
    # Native files omit unreferenced padding tiles. Such tiles are zero-filled in VRAM.
    op=bytes(64) if old_tile>=old_im.width*old_im.height//64 else pixels(old_im,old_tile,ov&3072)
    assert op==pixels(new_im,new_tile,nv&3072),f'{record["symbol"]}: changed graphic in source metatile {source_id:#x}'
    assert nv>>12==palette_map.get(ov>>12,ov>>12),'Palette bank points to wrong colors'
   art+=1
  gm=record['graphics_mapping']
  for start,size in record['animation_ranges']:
   target=gm[str(start)][0]
   assert target>=640 and target+size<=1024
   assert all(gm[str(start+i)]==[target+i,0] for i in range(size)),'Animation lost contiguous slots or gained flip flags'
  if rom:assert raw in rom_bytes,record['symbol']+' missing from ROM'
  count+=1
 # Every source ID, including collapsed duplicates and all partitioned sets, remains available.
 grouped={}
 for record in report['tilesets']:
  if not record['is_primary']:grouped.setdefault(record['source'],set()).update(map(int,record['metatile_mapping']))
 for name,ids in grouped.items():
  length=(source/name/'metatiles.bin').stat().st_size//16;assert ids==set(range(512,512+length)),name+' lost source metatiles'
 for frame in frames:
  sources=frame.get('sources',[frame.get('source')]);expected=b''
  for name in sources:
   im=Image.open(source/name)
   expected+=b''.join(pixels(im,i,0) for i in range(im.width*im.height//64))
  target=Image.open(ROOT/frame['target']);actual=b''.join(pixels(target,i,0) for i in range(target.width*target.height//64));assert actual[:len(expected)]==expected and not any(actual[len(expected):]),frame['target']
 print(f'{count} tilesets passed; {art} Emerald metatile IDs preserve graphics, palettes and layer types.')
 print(f'{len(frames)} animation frames preserve their pixel indices; packed animation banks fit and stay contiguous.')
 if rom:print('The ROM contains every imported metatile definition.')
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--source',type=Path,required=True);p.add_argument('--rom',action='store_true');a=p.parse_args();validate(a.source.resolve(),a.rom)
