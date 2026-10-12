"""Pack native Sevii scenery with the shared Orange coast; leave source tilesets intact.

Secondary metatile IDs 640-735 and the first 112 graphics remain tropical, so
neighboring sea routes can render the same seven-tile connection strips.
"""
from pathlib import Path
import json, struct, subprocess, tempfile, shutil
ROOT = Path(__file__).resolve().parents[2]
GFX = ROOT/'tools/gbagfx/gbagfx'
BASE = ROOT/'data/tilesets/secondary/orange_islands'
FAMILIES = {
 '123': ['OneIsland','TwoIsland','ThreeIsland','OneIsland_KindleRoad','OneIsland_TreasureBeach','TwoIsland_CapeBrink','ThreeIsland_BondBridge','ThreeIsland_Port'],
 '45': ['FourIsland','FiveIsland','FiveIsland_Meadow','FiveIsland_MemorialPillar','FiveIsland_ResortGorgeous','FiveIsland_WaterLabyrinth'],
 '67': ['SixIsland','SevenIsland','SixIsland_WaterPath','SixIsland_OutcastIsland','SixIsland_GreenPath','SixIsland_RuinValley','SevenIsland_TrainerTower','SevenIsland_SevaultCanyon','SevenIsland_SevaultCanyon_Entrance','SevenIsland_TanobyRuins'],
}
LAB = [0x2a8,0x2a9,0x2bd,0x2b5,0x2b0,0x2b1,0x2b3,0x2b4,0x2b8,0x2b9,0x2bb,0x2bc,0x2c0,0x2c1,0x2d0,0x2c2,0x2c3,0x2c4,0x2c5,0x2c8,0x2c9,0x2d8,0x2ac,0x2cb,0x2cc,0x2cd]
def flip(b, flags):
 rows=[[b[y*4+x//2]>>(4*(x%2))&15 for x in range(8)] for y in range(8)]
 if flags&0x400: rows=[row[::-1] for row in rows]
 if flags&0x800: rows=rows[::-1]
 return bytes(row[x]|row[x+1]<<4 for row in rows for x in range(0,8,2))
def build():
 layouts={l['id']:l for l in json.loads((ROOT/'data/layouts/layouts.json').read_text())['layouts']}
 reports={}
 with tempfile.TemporaryDirectory() as tmp:
  tmp=Path(tmp)
  def graphics(p):
   out=tmp/'tiles.4bpp';subprocess.run([str(GFX),str(p/'tiles.png'),str(out)],check=True)
   raw=out.read_bytes();return [raw[i:i+32] for i in range(0,len(raw),32)]
  for family,names in FAMILIES.items():
   donor=ROOT/f'data/tilesets/secondary/sevii_islands_{family}_condensed'
   dest=ROOT/f'data/tilesets/secondary/orange_sevii_{family}'
   selected=set()
   for name in names:
    header=json.loads((ROOT/'data/maps'/name/'map.json').read_text());l=layouts[header['layout']]
    assert l['secondary_tileset']==f'gTileset_SeviiIslands{family}'
    raw=(ROOT/l['blockdata_filepath']).read_bytes()
    selected.update(v&1023 for (v,) in struct.iter_unpack('<H',raw) if v&1023>=640)
   packed=graphics(BASE);lookup={}
   def remember(t,index):
    for flags in (0,0x400,0x800,0xc00):lookup.setdefault(flip(t,flags),(index,flags))
   for index,t in enumerate(packed):remember(t,index)
   metas=bytearray((BASE/'metatiles.bin').read_bytes());attrs=bytearray((BASE/'metatile_attributes.bin').read_bytes())
   def append(source,ids,palette_map=None):
    source_tiles=graphics(source);sm=(source/'metatiles.bin').read_bytes();sa=(source/'metatile_attributes.bin').read_bytes();mapping={}
    for mid in sorted(ids):
     mapping[str(mid)]=640+len(metas)//16;entries=[]
     for (v,) in struct.iter_unpack('<H',sm[(mid-640)*16:(mid-639)*16]):
      before=v
      if v&1023>=640:
       t=source_tiles[(v&1023)-640]
       if t not in lookup:
        index=len(packed);packed.append(t);remember(t,index)
       index,flags=lookup[t];v=(v&0xf000)|((v&0xc00)^flags)|(640+index)
       assert flip(t,before&0xc00)==flip(packed[index],v&0xc00)
      if palette_map and v>>12>=7:
       assert v>>12 in palette_map;v=(v&0xfff)|(palette_map[v>>12]<<12)
      elif family=='123':assert v>>12<13,'Lab reserved palette banks'
      entries.append(v)
     metas.extend(struct.pack('<8H',*entries));attrs.extend(sa[(mid-640)*4:(mid-639)*4])
    return mapping
   mapping=append(donor,selected);lab={}
   if family=='123':lab=append(ROOT/'data/tilesets/secondary/pallet_town_condensed',LAB,{9:13,10:14,11:15})
   assert len(packed)<=384 and len(metas)//16<=384,(family,len(packed),len(metas)//16)
   dest.mkdir(exist_ok=True);(dest/'palettes').mkdir(exist_ok=True)
   for index in range(16):
    source=BASE if index<7 else donor;bank=index
    if family=='123' and index>=13:source=ROOT/'data/tilesets/secondary/pallet_town_condensed';bank=index-4
    target=dest/'palettes'/f'{index:02}.pal'
    if not target.exists():shutil.copyfile(source/'palettes'/f'{bank:02}.pal',target)
   # Refuse to silently overwrite a subsequent Porymap edit.
   if (dest/'metatiles.bin').exists():
    assert (dest/'metatiles.bin').read_bytes()==metas,'Edited metatiles: review before regenerating'
    assert (dest/'metatile_attributes.bin').read_bytes()==attrs,'Edited attributes: review before regenerating'
    assert b''.join(graphics(dest))==b''.join(packed)+bytes((-len(packed)%16)*32),'Edited graphics: review before regenerating'
   (dest/'metatiles.bin').write_bytes(metas);(dest/'metatile_attributes.bin').write_bytes(attrs)
   raw=b''.join(packed)+bytes((-len(packed)%16)*32);out=tmp/'mixed.4bpp';out.write_bytes(raw)
   subprocess.run([str(GFX),str(out),str(dest/'tiles.png'),'-width','16','-palette',str(dest/'palettes/07.pal')],check=True)
   assert b''.join(graphics(dest))==raw
   report=dict(primary='gTileset_OrangeGeneral',source_maps=names,occupied_tiles=len(packed),free_graphics_slots=384-len(packed),metatiles=len(metas)//16,free_metatile_slots=384-len(metas)//16,sevii_metatile_mapping=mapping,pallet_lab_mapping=lab)
   (dest/'packing.json').write_text(json.dumps(report,indent=2)+'\n');reports[family]=report
   print(f'Orange Sevii {family}: {len(packed)}/384 graphics, {len(metas)//16}/384 metatiles')
 return reports
if __name__=='__main__':build()
