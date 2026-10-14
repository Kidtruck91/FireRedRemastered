"""Render each imported metatile picker with its recommended primary and palettes."""
from pathlib import Path
from functools import lru_cache
import json,struct
from PIL import Image,ImageDraw
ROOT=Path(__file__).resolve().parents[2]
def render(record):
 path=ROOT/record['directory'];primary=path if record['is_primary'] else ROOT/'data/tilesets/primary'/('hoenn_general' if 'gTileset_HoennGeneral' in record['recommended_primaries'] else 'hoenn_interior');images=[Image.open(p/'tiles.png') for p in (primary,path)]
 pals=[[tuple(map(int,line.split())) for line in ((primary if i<7 else path)/'palettes'/f'{i:02}.pal').read_text().splitlines()[3:19]] for i in range(16)]
 raw=(path/'metatiles.bin').read_bytes();metas=list(struct.iter_unpack('<8H',raw));count=len(record['metatile_mapping']) if record['is_primary'] else len(metas);metas=metas[:count]
 @lru_cache(None)
 def tile(mid):
  out=Image.new('RGB',(16,16))
  for k,v in enumerate(metas[mid]):
   idx=v&1023;img=images[idx>=640];idx=idx-640 if idx>=640 else idx;columns=img.width//8
   t=img.crop(((idx%columns)*8,(idx//columns)*8,(idx%columns+1)*8,(idx//columns+1)*8))
   if v&0x400:t=t.transpose(Image.Transpose.FLIP_LEFT_RIGHT)
   if v&0x800:t=t.transpose(Image.Transpose.FLIP_TOP_BOTTOM)
   for y in range(8):
    for x in range(8):
     color=t.getpixel((x,y))
     if k<4 or color:out.putpixel(((k%2)*8+x,((k%4)//2)*8+y),pals[v>>12][color])
  return out
 atlas=Image.new('RGB',(640,((len(metas)+15)//16)*44),(32,38,40));draw=ImageDraw.Draw(atlas)
 for i in range(len(metas)):
  x=(i%16)*40;y=(i//16)*44;atlas.paste(tile(i).resize((32,32),Image.Resampling.NEAREST),(x,y));draw.text((x,y+32),f'{i+(0 if record["is_primary"] else 640):03X}',fill='white')
 return atlas
if __name__=='__main__':
 report=json.loads((ROOT/'docs/hoenn-tilesets/import-report.json').read_text());destination=ROOT/'docs/hoenn-tilesets/previews';destination.mkdir(exist_ok=True)
 for record in report['tilesets']:render(record).save(destination/(record['symbol']+'.png'))
 print(f'Rendered {len(report["tilesets"])} tileset previews.')
