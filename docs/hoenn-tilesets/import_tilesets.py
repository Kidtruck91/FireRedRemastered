"""Import Emerald's static Hoenn tilesets into FireRed's 640/384 layout.

Run with --source /path/to/pret/pokeemerald. Originals and existing maps are
never reassigned. Output mappings record every Emerald ID and graphics slot.
"""
from pathlib import Path
import argparse, json, re, struct, subprocess, shutil
from collections import defaultdict
from functools import lru_cache
from PIL import Image
ROOT=Path(__file__).resolve().parents[2]
ANIMATED={
 'rustboro':[(128,32),(448,4)], 'dewford':[(170,6)],
 'slateport':[(224,4)], 'mauville':[(96,64)],
 'lavaridge':[(160,4),(288,8)], 'ever_grande':[(224,32)],
 'pacifidlog':[(464,30),(496,8)], 'sootopolis':[(240,96)],
 'underwater':[(496,4)], 'cave':[(416,4)],
 'battle_frontier_outside_east':[(218,6)], 'battle_frontier_outside_west':[(218,6)],
 'sootopolis_gym':[(464,20),(496,12)], 'elite_four':[(480,4),(504,1)],
 'mauville_gym':[(144,16)], 'bike_shop':[(496,9)],
 'battle_pyramid':[(135,8),(151,8)],
}
ALIASES={
 'MB_NO_RUNNING':'MB_RUNNING_DISALLOWED','MB_NON_ANIMATED_DOOR':'MB_CAVE_DOOR',
 'MB_ANIMATED_DOOR':'MB_WARP_DOOR','MB_CRACKED_FLOOR_HOLE':'MB_FALL_WARP',
 'MB_AQUA_HIDEOUT_WARP':'MB_REGULAR_WARP','MB_LAVARIDGE_GYM_1F_WARP':'MB_LAVARIDGE_1F_WARP',
 'MB_LAVARIDGE_GYM_B1F_WARP':'MB_FALL_WARP','MB_INTERIOR_DEEP_WATER':'MB_DEEP_WATER',
 'MB_NO_SURFACING':'MB_DEEP_WATER','MB_SOOTOPOLIS_DEEP_WATER':'MB_DEEP_WATER',
 'MB_SEAWEED_NO_SURFACING':'MB_DEEP_WATER','MB_WATER_DOOR':'MB_CAVE_DOOR',
 'MB_WATER_SOUTH_ARROW_WARP':'MB_SOUTH_ARROW_WARP','MB_DEEP_SOUTH_WARP':'MB_SOUTH_ARROW_WARP',
 'MB_MT_PYRE_HOLE':'MB_FALL_WARP','MB_SHOAL_CAVE_ENTRANCE':'MB_CAVE_DOOR',
 'MB_LONG_GRASS':'MB_TALL_GRASS','MB_LONG_GRASS_SOUTH_EDGE':'MB_TALL_GRASS',
 'MB_ASHGRASS':'MB_TALL_GRASS','MB_DEEP_SAND':'MB_SAND','MB_FOOTPRINTS':'MB_SAND',
 'MB_BLUEPRINT':'MB_BLUEPRINTS','MB_TRASH_CAN':'MB_TRASH_BIN',
 'MB_PICTURE_BOOK_SHELF':'MB_BOOKSHELF','MB_POKEMON_CENTER_BOOKSHELF':'MB_BOOKSHELF',
 'MB_PLAYER_ROOM_PC_ON':'MB_PC','MB_RUNNING_SHOES_INSTRUCTION':'MB_SIGNPOST',
 'MB_SHOP_SHELF':'MB_POKEMART_SHELF','MB_SECRET_BASE_PC':'MB_PC',
 'MB_SECRET_BASE_SIGN':'MB_SIGNPOST','MB_SECRET_BASE_TV_SHIELD':'MB_TELEVISION',
 'MB_POKEBLOCK_FEEDER':'MB_COUNTER','MB_POKEBLOCK_CASE':'MB_COUNTER',
 'MB_JUMP_NORTHEAST':'MB_JUMP_EAST','MB_JUMP_NORTHWEST':'MB_JUMP_WEST',
 'MB_JUMP_SOUTHEAST':'MB_JUMP_EAST','MB_JUMP_SOUTHWEST':'MB_JUMP_WEST',
 'MB_BATTLE_PYRAMID_WARP':'MB_REGULAR_WARP','MB_MOSSDEEP_GYM_WARP':'MB_REGULAR_WARP',
}
def camel(s):return ''.join(w.capitalize() for w in s.split('_'))
def load_tiles(path):
 im=Image.open(path);assert im.mode=='P',path;pixels=list(im.tobytes());assert max(pixels)<16,path
 return [bytes(im.crop((x,y,x+8,y+8)).tobytes()) for y in range(0,im.height,8) for x in range(0,im.width,8)]
@lru_cache(None)
def flip(tile,flags):
 rows=[tile[i:i+8] for i in range(0,64,8)]
 if flags&0x400:rows=[row[::-1] for row in rows]
 if flags&0x800:rows=rows[::-1]
 return b''.join(rows)
def save_tiles(path,tiles,palette):
 tiles=tiles+[bytes(64)]*((-len(tiles))%16);im=Image.new('P',(128,len(tiles)//16*8))
 colors=[tuple(map(int,line.split())) for line in palette.read_text().splitlines()[3:19]]
 im.putpalette([v for color in colors for v in color]+[0]*(768-48))
 for i,t in enumerate(tiles):
  graphic=Image.new('P',(8,8));graphic.putdata(t);im.paste(graphic,((i%16)*8,(i//16)*8))
 im.save(path);assert load_tiles(path)==tiles
class Importer:
 def __init__(self,source):
  self.source=source;self.records=[];self.fallbacks=defaultdict(set)
  text=re.sub(r'//[^\n]*','',(source/'include/constants/metatile_behaviors.h').read_text())
  self.names=re.findall(r'\bMB_[A-Z0-9_]+\b',text[text.index('enum {'):text.index('};')])
  self.fr={k:int(v,16) for k,v in re.findall(r'#define (MB_\w+) (0x[0-9A-Fa-f]+)',(ROOT/'include/constants/metatile_behaviors.h').read_text())}
  attrs=(source/'src/metatile_behavior.c').read_text();self.flags={}
  for name,expression in re.findall(r'\[(MB_\w+)\]\s*=\s*([^,]+),',attrs):self.flags[name]=expression
  self.source_headers=(source/'src/data/tilesets/headers.h').read_text()
  self.layouts=json.loads((source/'data/layouts/layouts.json').read_text())['layouts']
 def attr(self,raw,setname):
  name=self.names[raw&255];target=name if name in self.fr else ALIASES.get(name,'MB_NORMAL')
  if name not in self.fr:self.fallbacks[setname].add((name,target))
  flags=self.flags.get(name,'');water='TILE_FLAG_SURFABLE' in flags;encounter='TILE_FLAG_HAS_ENCOUNTERS' in flags
  terrain=3 if target=='MB_WATERFALL' else 2 if water else 1 if target=='MB_TALL_GRASS' else 0
  encounter=2 if water and encounter else 1 if encounter else 0
  return self.fr[target]|(terrain<<9)|(encounter<<24)|((raw>>12)<<29)
 def palette_map(self,metas):
  used={v>>12 for row in metas for v in row if v>>12>=6};assert len(used)<=6
  # Retain native slots 7-12 whenever possible; move 6/14 into unused loaded banks.
  mapping={b:b for b in used if 7<=b<=12};free=iter(b for b in range(7,13) if b not in mapping.values())
  for b in sorted(used-mapping.keys()):mapping[b]=next(free)
  return mapping
 def palettes(self,src,dest,mapping,primary=False):
  (dest/'palettes').mkdir(exist_ok=True)
  blank='JASC-PAL\n0100\n16\n'+'0 0 0\n'*16
  for bank in range(16):
   source_bank=bank if bank<6 else next((s for s,t in mapping.items() if t==bank),None)
   data=(src/'palettes'/f'{source_bank:02}.pal').read_bytes() if source_bank is not None else blank.replace('\n','\r\n').encode()
   target=dest/'palettes'/f'{bank:02}.pal'
   if target.exists():assert target.read_bytes()==data, 'Edited palette: '+str(target)
   target.write_bytes(data)
 def write(self,src,dest,symbol,primary,metas,attrs,tiles,palette_map,meta_map,gfx_map,callback,part=1):
  dest.mkdir(parents=True,exist_ok=True);self.palettes(src,dest,palette_map,primary)
  raw=struct.pack('<'+'H'*(len(metas)*8),*(v for row in metas for v in row));at=struct.pack('<'+'I'*len(attrs),*attrs)
  for filename,data in [('metatiles.bin',raw),('metatile_attributes.bin',at)]:
   target=dest/filename
   if target.exists():assert target.read_bytes()==data,'Edited output: '+str(target)
   target.write_bytes(data)
  if (dest/'tiles.png').exists():assert load_tiles(dest/'tiles.png')==tiles+[bytes(64)]*((-len(tiles))%16),'Edited graphics'
  save_tiles(dest/'tiles.png',tiles,dest/'palettes/00.pal')
  source_kind=src.parent.name;source_name=src.name;recommended=set()
  if not primary:
   for layout in self.layouts:
    if layout['secondary_tileset']=='gTileset_'+callback['source_symbol']:recommended.add({'gTileset_General':'gTileset_HoennGeneral','gTileset_Building':'gTileset_HoennInterior','gTileset_SecretBase':'gTileset_HoennSecretBase'}.get(layout['primary_tileset'],layout['primary_tileset']))
  report=dict(symbol=symbol,source=src.relative_to(self.source).as_posix(),is_primary=primary,part=part,graphics_used=len(tiles),metatiles=len(metas),recommended_primaries=sorted(recommended),palette_mapping={str(k):v for k,v in palette_map.items()},metatile_mapping={str(k):v for k,v in meta_map.items()},graphics_mapping={str(k):list(v) for k,v in gfx_map.items()},animation_ranges=ANIMATED.get(source_name,[]),callback=callback['target'])
  (dest/'import.json').write_text(json.dumps(report,indent=2)+'\n');report['directory']=dest.relative_to(ROOT).as_posix();self.records.append(report)
 def primary(self,name,target):
  src=self.source/'data/tilesets/primary'/name;dest=ROOT/'data/tilesets/primary'/target;symbol='HoennGeneral' if name=='general' else 'HoennInterior'
  metas=[r for r in struct.iter_unpack('<8H',(src/'metatiles.bin').read_bytes())];attrs=[self.attr(v,name) for (v,) in struct.iter_unpack('<H',(src/'metatile_attributes.bin').read_bytes())];tiles=load_tiles(src/'tiles.png');assert len(tiles)<=512
  self.write(src,dest,symbol,True,metas+[(512,)*8]*(640-len(metas)),attrs+[0]*(640-len(attrs)),tiles+[bytes(64)]*(640-len(tiles)),{},dict(enumerate(range(len(metas)))),{i:(i,0) for i in range(512)},dict(target='InitTilesetAnim_'+symbol,source_symbol=camel(name)))
 def secondary(self,src):
  name=src.name;source_symbol=re.search(r'const struct Tileset gTileset_(\w+)\s*=\s*\{[^}]*\.tiles = gTilesetTiles_\w+,\s*\.palettes = gTilesetPalettes_\w+,\s*\.metatiles = gMetatiles_'+re.escape(camel(name))+r',',self.source_headers)
  if not source_symbol:
   # Names such as BrendansMaysHouse are already normalized by camel().
   source_symbol=camel(name)
  else:source_symbol=source_symbol.group(1)
  header=re.search(r'const struct Tileset gTileset_'+source_symbol+r'\s*=\s*\{(.*?)\};',self.source_headers,re.S);assert header,name
  init=re.search(r'\.callback = (\w+)',header.group(1)).group(1)
  srcmetas=list(struct.iter_unpack('<8H',(src/'metatiles.bin').read_bytes()));srcattrs=[v[0] for v in struct.iter_unpack('<H',(src/'metatile_attributes.bin').read_bytes())];assert len(srcmetas)==len(srcattrs),name
  native_tiles=load_tiles(src/'tiles.png');native_tiles+= [bytes(64)]*(512-len(native_tiles));pm=self.palette_map(srcmetas)
  # Collapse visually identical metatiles with identical attributes. Keep all source IDs in the lookup.
  representatives=[];equivalent={};keys={}
  reserved=sorted({i for start,count in ANIMATED.get(name,[]) for i in range(start,start+count)})
  for index,(meta,attr) in enumerate(zip(srcmetas,srcattrs)):
   pieces=[]
   for v in meta:
    idx=v&1023
    if idx<512:pieces.append(('primary',idx,v&0xc00,v>>12))
    elif idx-512 in reserved:pieces.append(('animated',idx,v&0xc00,v>>12))
    else:pieces.append((flip(native_tiles[idx-512],v&0xc00),v>>12))
   key=(tuple(pieces),attr)
   if key not in keys:keys[key]=index;representatives.append(index)
   equivalent[index]=keys[key]
  reserved=sorted({i for start,count in ANIMATED.get(name,[]) for i in range(start,start+count)})
  def pack(ids):
   tiles=[native_tiles[i] for i in reserved];mapping={i:(640+j,0) for j,i in enumerate(reserved)};lookup={}
   out=[];attrs=[]
   for index in ids:
    row=[]
    for v in srcmetas[index]:
     idx=v&1023;flags=v&0xc00
     if idx>=512:
      original=idx-512
      if original not in mapping:
       t=native_tiles[original]
       if t not in lookup:
        target=len(tiles);tiles.append(t)
        for f in (0,0x400,0x800,0xc00):lookup.setdefault(flip(t,f),(640+target,f))
       mapping[original]=lookup[t]
      idx,extra=mapping[original];flags^=extra
     bank=v>>12;bank=pm.get(bank,bank);row.append(idx|flags|(bank<<12))
    out.append(tuple(row));attrs.append(self.attr(srcattrs[index],name))
   return tiles,mapping,out,attrs
  # Partition only sets whose metatile/graphics budgets actually need it.
  groups=[];batch=[]
  for index in representatives:
   trial=batch+[index];tiles,gm,ms,ats=pack(trial)
   if len(trial)>384 or len(tiles)>384:
    assert batch,(name,index);groups.append(batch);batch=[index]
   else:batch=trial
  if batch:groups.append(batch)
  for part,ids in enumerate(groups,1):
   tiles,gm,ms,ats=pack(ids);assert len(tiles)<=384 and len(ms)<=384
   suffix='' if len(groups)==1 else '_part_'+str(part);symbol='Hoenn'+source_symbol+('' if len(groups)==1 else 'Part'+str(part));dest=ROOT/'data/tilesets/secondary'/('hoenn_native_'+name+suffix)
   targets={index:640+i for i,index in enumerate(ids)};mm={512+i:targets[rep] for i,rep in equivalent.items() if rep in targets}
   target_init='InitTilesetAnim_'+symbol if init!='NULL' else 'NULL'
   self.write(src,dest,symbol,False,ms,ats,tiles,pm,mm,gm,dict(target=target_init,source_symbol=source_symbol),part)
   self.records[-1]['source_callback']=init
  print(name,len(groups),'part(s)',len(representatives),'unique metatiles',flush=True)
 def run(self):
  self.primary('general','hoenn_general');self.primary('building','hoenn_interior')
  for src in sorted((self.source/'data/tilesets/secondary').iterdir()):
   if (src/'tiles.png').exists() and src.name not in ('unused_1','unused_2'):self.secondary(src)
  commit=subprocess.check_output(['git','rev-parse','HEAD'],cwd=self.source,text=True).strip()
  result=dict(source_repository='https://github.com/pret/pokeemerald',source_commit=commit,tilesets=self.records,behavior_conversions={name:[dict(source=s,target=t) for s,t in sorted(rows)] for name,rows in self.fallbacks.items()})
  (ROOT/'docs/hoenn-tilesets/import-report.json').write_text(json.dumps(result,indent=2)+'\n')
  return result
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--source',type=Path,required=True);a=p.parse_args();Importer(a.source.resolve()).run()
