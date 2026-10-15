"""Render current Porymap block data, palettes and flip flags for the GitHub gallery."""
from pathlib import Path
from functools import lru_cache
import json,struct,re
from PIL import Image,ImageDraw
ROOT=Path(__file__).resolve().parents[2]
@lru_cache(None)
def tileset_path(symbol):
 headers=(ROOT/'src/data/tilesets/headers.h').read_text()
 body=re.search(r'const struct Tileset '+re.escape(symbol)+r'\s*=\s*\{(.*?)\};',headers,re.S).group(1)
 metatiles=re.search(r'\.metatiles\s*=\s*(\w+)',body).group(1)
 definitions=(ROOT/'src/data/tilesets/metatiles.h').read_text()
 path=re.search(re.escape(metatiles)+r'\[\]\s*=\s*INCBIN_U16\("([^"]+)"\)',definitions).group(1)
 return (ROOT/path).parent
def render(layout):
 primary=tileset_path(layout['primary_tileset']);secondary=tileset_path(layout['secondary_tileset'])
 images=[Image.open(p/'tiles.png') for p in (primary,secondary)]
 palettes=[[tuple(map(int,line.split())) for line in ((primary if bank<7 else secondary)/'palettes'/f'{bank:02}.pal').read_text().splitlines()[3:19]] for bank in range(16)]
 metas=[]
 for p in (primary,secondary):metas.extend(struct.unpack('<8H',raw) for raw in [bytes(chunk) for chunk in zip(*[iter((p/'metatiles.bin').read_bytes())]*16)])
 @lru_cache(None)
 def tile(mid):
  out=Image.new('RGB',(16,16))
  for layer,v in enumerate(metas[mid]):
   index=v&1023;img=images[index>=640];index=index-640 if index>=640 else index
   graphic=img.crop(((index%16)*8,(index//16)*8,(index%16+1)*8,(index//16+1)*8))
   if v&0x400:graphic=graphic.transpose(Image.Transpose.FLIP_LEFT_RIGHT)
   if v&0x800:graphic=graphic.transpose(Image.Transpose.FLIP_TOP_BOTTOM)
   for y in range(8):
    for x in range(8):
     color=graphic.getpixel((x,y))
     if layer<4 or color:out.putpixel(((layer%2)*8+x,((layer%4)//2)*8+y),palettes[v>>12][color])
  return out
 width=layout['width'];height=layout['height'];out=Image.new('RGB',(width*16,height*16))
 for i,(v,) in enumerate(struct.iter_unpack('<H',(ROOT/layout['blockdata_filepath']).read_bytes())):out.paste(tile(v&1023),((i%width)*16,(i//width)*16))
 return out
if __name__=='__main__':
 layouts=json.loads((ROOT/'data/layouts/layouts.json').read_text())['layouts'];towns=[]
 for layout in layouts:
  if layout['name'].startswith('OrangeIslands_'):
   name=layout['name'].removesuffix('_Layout');im=render(layout);im.save(ROOT/'docs/orange-islands'/f'{name}.png')
   if '_Route_' not in name and not name.endswith('_Beach'):towns.append((name,im))
 sheet=Image.new('RGB',(1400,((len(towns)+3)//4)*480),(30,40,35));draw=ImageDraw.Draw(sheet)
 for index,(name,im) in enumerate(towns):
  im.thumbnail((340,440),Image.Resampling.NEAREST);x=(index%4)*350;y=(index//4)*480;draw.text((x+8,y+8),name.removeprefix('OrangeIslands_').replace('_',' '),fill='white');sheet.paste(im,(x+(350-im.width)//2,y+32))
 sheet.save(ROOT/'docs/orange-islands/sevii-inspired-towns.png')
 references=json.loads((ROOT/'docs/orange-islands/sevii-references.json').read_text());names={value[key] for value in references.values() for key in ('sevii_town','sevii_coast','sevii_approach') if value.get(key)}
 destination=ROOT/'docs/orange-islands/sevii-reference-maps';destination.mkdir(exist_ok=True)
 for layout in layouts:
  if layout['name'].removesuffix('_Layout') in names:render(layout).save(destination/(layout['name'].removesuffix('_Layout')+'.png'))
 print('Rendered all current Orange maps and town overview.')
