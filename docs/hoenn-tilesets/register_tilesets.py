"""Register imported sets and adapt Emerald animation callbacks to packed graphics."""
from pathlib import Path
import argparse,json,re,shutil
from import_tilesets import ROOT,load_tiles,save_tiles
BEGIN='// BEGIN IMPORTED HOENN TILESETS\n';END='// END IMPORTED HOENN TILESETS\n'
def replace_block(path,text,before_guard=False):
 s=path.read_text()
 if BEGIN in s:s=s[:s.index(BEGIN)]+s[s.index(END)+len(END):]
 block=BEGIN+text+END
 if before_guard:
  index=s.rfind('#endif');s=s[:index]+block+'\n'+s[index:]
 else:s=s.rstrip()+'\n\n'+block
 path.write_text(s)
def functions(text):
 out={}
 for match in re.finditer(r'^(?:static )?void (\w+)\(([^;]*?)\)\s*\n\{',text,re.M):
  depth=1;end=match.end()
  while depth:
   depth+=(text[end]=='{')-(text[end]=='}');end+=1
  out[match.group(1)]=(match.group(2),text[match.end():end-1])
 return out

def register(source):
 report=json.loads((ROOT/'docs/hoenn-tilesets/import-report.json').read_text());records=report['tilesets'];graphics='';metatiles='';headers='';decl=''
 native=(source/'src/tileset_anims.c').read_text();funcs=functions(native)
 for record in records:
  symbol=record['symbol'];directory=record['directory'];cb=record['callback']
  graphics+=f'const u32 gTilesetTiles_{symbol}[] = INCBIN_U32("{directory}/tiles.4bpp.lz");\nconst u16 gTilesetPalettes_{symbol}[][16] =\n{{\n'+''.join(f'    INCBIN_U16("{directory}/palettes/{bank:02}.gbapal"),\n' for bank in range(16))+'};\n\n'
  metatiles+=f'const u16 gMetatiles_{symbol}[] = INCBIN_U16("{directory}/metatiles.bin");\nconst u32 gMetatileAttributes_{symbol}[] = INCBIN_U32("{directory}/metatile_attributes.bin");\n\n'
  headers+=f'const struct Tileset gTileset_{symbol} =\n{{\n    .isCompressed = TRUE,\n    .isSecondary = '+('FALSE' if record['is_primary'] else 'TRUE')+f',\n    .tiles = gTilesetTiles_{symbol},\n    .palettes = gTilesetPalettes_{symbol},\n    .metatiles = gMetatiles_{symbol},\n    .metatileAttributes = gMetatileAttributes_{symbol},\n    .callback = {cb},\n}};\n\n'
  if cb!='NULL':decl+=f'void {cb}(void);\n'
 replace_block(ROOT/'src/data/tilesets/graphics.h',graphics);replace_block(ROOT/'src/data/tilesets/metatiles.h',metatiles);replace_block(ROOT/'src/data/tilesets/headers.h',headers);replace_block(ROOT/'include/tileset_anims.h',decl,True)
 # Namespace the original arrays, schedulers, and frame-selection functions.
 first=native.index('const u16 gTilesetAnims_General_Flower_Frame1');last=native.index('static void ResetTilesetAnimBuffer(void)\n{')
 data=native[first:last]
 entities=data.split(';')
 data=';\n'.join(entity.strip() for entity in entities if entity.strip() and not re.search(r'tileset_anims_space_|gTilesetAnims_Unused',entity))+';\n'
 animroot=ROOT/'data/tilesets/hoenn_animations';animroot.mkdir(exist_ok=True)
 frames=[]
 def asset(match):
  filename,extension=match.group(1),match.group(2);native_path=source/filename;relative=Path(filename).relative_to('data/tilesets');target=animroot/relative;target.parent.mkdir(parents=True,exist_ok=True);shutil.copyfile(native_path,target)
  frames.append(dict(source=filename,target=target.relative_to(ROOT).as_posix()))
  return 'INCBIN_U16("'+target.with_suffix(extension).relative_to(ROOT).as_posix()+'")'
 data=re.sub(r'INCGFX_U16\("([^"]+)", "(\.[^"]+)"\)',asset,data)
 # The storm animation combines the Kyogre and Groudon strips into one 96-tile frame.
 def storm(match):
  one,two=match.group(1),match.group(2);a=(source/one).with_suffix('.png');b=(source/two).with_suffix('.png');target=animroot/'secondary/sootopolis/anim/stormy_water'/a.name.replace('_kyogre','');target.parent.mkdir(parents=True,exist_ok=True)
  palette=ROOT/'data/tilesets/secondary/hoenn_native_sootopolis/palettes/07.pal';save_tiles(target,load_tiles(a)+load_tiles(b),palette)
  frames.append(dict(sources=[a.relative_to(source).as_posix(),b.relative_to(source).as_posix()],target=target.relative_to(ROOT).as_posix()))
  return 'INCBIN_U16("'+target.with_suffix('.4bpp').relative_to(ROOT).as_posix()+'")'
 data=re.sub(r'INCBIN_U16\("([^"]+_kyogre\.4bpp)", "([^"]+_groudon\.4bpp)"\)',storm,data)
 domedata=''
 for index in range(4):
  target=animroot/f'dome_anim{index+1}.pal';shutil.copyfile(source/f'graphics/battle_frontier/dome_anim{index+1}.pal',target)
  domedata+=f'const u16 gTilesetAnims_BattleDomePals0_{index}[] = INCBIN_U16("{target.with_suffix(".gbapal").relative_to(ROOT).as_posix()}");\n'
 data=domedata+data
 helpers={name for name in funcs if name.startswith(('TilesetAnim_','QueueAnimTiles_','BlendAnimPalette_'))}
 helpers-={'TilesetAnim_BattleDome2','BlendAnimPalette_BattleDome_FloorLightsNoBlend'}
 def adapt(text):
  text=re.sub(r'\bgTilesetAnims_', 'sHoennTilesetAnims_',text)
  text=re.sub(r'\bsTilesetAnims_', 'sHoennTilesetAnims_',text)
  for name in sorted(helpers,key=len,reverse=True):text=re.sub(r'\b'+name+r'\b','Hoenn'+name,text)
  text=text.replace('AppendTilesetAnimToBuffer(', 'HoennAppendTilesetAnim(').replace('NUM_TILES_IN_PRIMARY','512')
  return text
 data=adapt(data)
 # Arrays are private to the existing engine's animation translation unit.
 data=re.sub(r'(?m)^(const u16|u16 \*const)',r'static \1',data)
 declarations=''.join(f'static void Hoenn{name}({funcs[name][0]});\n' for name in sorted(helpers))
 lookup='static const u16 *sHoennAnimationMap;\n\n'
 for record in records:
  if record['is_primary'] or record['callback']=='NULL':continue
  mapping=record['graphics_mapping'];values=[mapping.get(str(index),[65535,0])[0] for index in range(512)]
  lookup+=f'static const u16 sHoennGraphicsMap_{record["symbol"]}[512] =\n{{\n'+''.join('    '+', '.join(map(str,values[i:i+16]))+',\n' for i in range(0,512,16))+'};\n\n'
 wrapper='''// Emerald callbacks address 512 primary tiles. Translate whole animation banks
// to the reserved, contiguous slots of the currently selected FireRed secondary.
static void HoennAppendTilesetAnim(const u16 *src, u16 *nativeDest, u16 size)
{
    u32 index = ((u32)nativeDest - BG_VRAM) / TILE_SIZE_4BPP;
    if (index >= 512)
    {
        if (sHoennAnimationMap == NULL || index >= 1024)
            return;
        index = sHoennAnimationMap[index - 512];
        if (index == 0xFFFF)
            return;
    }
    AppendTilesetAnimToBuffer(src, (u16 *)(BG_VRAM + TILE_OFFSET_4BPP(index)), size);
}

'''
 callbacks=''
 for name in sorted(helpers):
  args,body=funcs[name]
  if name=='BlendAnimPalette_BattleDome_FloorLights':
   # FireRed's battle transition tasks differ. Keep the floor palette animation
   # within its assigned bank and respect the current palette fade.
   dome=next(r for r in records if r['symbol']=='HoennBattleDome');bank=dome['palette_mapping']['8']
   body=f'\n    CpuCopy16(sTilesetAnims_BattleDomeFloorLightPals[timer % ARRAY_COUNT(sTilesetAnims_BattleDomeFloorLightPals)], &gPlttBufferUnfaded[BG_PLTT_ID({bank})], 32);\n    BlendPalette(BG_PLTT_ID({bank}), 16, gPaletteFade.y, gPaletteFade.blendColor & 0x7FFF);\n'
  callbacks+=f'static void Hoenn{name}({args})\n{{'+adapt(body)+'}\n\n'
 for record in records:
  if record['callback']=='NULL':continue
  name=record['callback'];original='InitTilesetAnim_'+('General' if record['symbol']=='HoennGeneral' else 'Building' if record['symbol']=='HoennInterior' else record['source_callback'].removeprefix('InitTilesetAnim_'))
  body=adapt(funcs[original][1]);prefix='' if record['is_primary'] else '\n    sHoennAnimationMap = sHoennGraphicsMap_'+record['symbol']+';'
  callbacks+=f'void {name}(void)\n{{'+prefix+body+'}\n\n'
 out='// Adapted from pret/pokeemerald at '+report['source_commit']+'\n// Generated by docs/hoenn-tilesets/register_tilesets.py.\n\n'+declarations+'\n'+data+'\n'+lookup+wrapper+callbacks
 (ROOT/'src/data/tilesets/hoenn_anims.h').write_text(out.rstrip()+'\n')
 c=ROOT/'src/tileset_anims.c';text=c.read_text()
 for header in ('palette.h','util.h'):
  if '#include "'+header+'"' not in text:text=text.replace('#include "global.h"','#include "global.h"\n#include "'+header+'"')
 if '#include "data/tilesets/hoenn_anims.h"' not in text:text=text.rstrip()+'\n\n#include "data/tilesets/hoenn_anims.h"\n'
 c.write_text(text)
 (ROOT/'docs/hoenn-tilesets/animation-frames.json').write_text(json.dumps(frames,indent=2)+'\n')
 print(f'Registered {len(records)} sets; imported {len(frames)} graphic animation frames.')
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--source',type=Path,required=True);a=p.parse_args();register(a.source.resolve())
