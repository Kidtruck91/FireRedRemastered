"""Check the actual Navel blocks, dry routes, reciprocal warps and ROM data."""
from collections import deque
from pathlib import Path
import argparse
import json
import struct
import sys

HERE=Path(__file__).resolve().parent
ROOT=HERE.parents[2]
sys.path.insert(0,str(HERE.parent))
from render_maps import tileset_path
SURF={0x10,0x11,0x12,0x13,0x15,0x1A,0x1B,0x50,0x51,0x52,0x53}


def flood(start,cells,w,h):
    seen={start};queue=deque(seen)
    while queue:
        i=queue.popleft();x,y=i%w,i//w
        for nx,ny in [(x-1,y),(x+1,y),(x,y-1),(x,y+1)]:
            j=ny*w+nx
            if 0<=nx<w and 0<=ny<h and j in cells and j not in seen:
                seen.add(j);queue.append(j)
    return seen


def main():
    parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('--rom',action='store_true');args=parser.parse_args()
    layouts={l['id']:l for l in json.loads((ROOT/'data/layouts/layouts.json').read_text())['layouts']}
    landmarks=json.loads((HERE/'landmarks.json').read_text());maps={};rom=(ROOT/'pokefirered.gba').read_bytes() if args.rom else None
    for name,key in [('OrangeIslands_Navel','town'),('OrangeIslands_Navel_Beach','beach')]:
        header=json.loads((ROOT/'data/maps'/name/'map.json').read_text());layout=layouts[header['layout']]
        w,h=layout['width'],layout['height'];raw=(ROOT/layout['blockdata_filepath']).read_bytes()
        assert len(raw)==w*h*2 and (w+15)*(h+14)<=10240,(name,'map buffer')
        blocks=struct.unpack('<'+'H'*(w*h),raw);attrs=[];metatiles=[]
        for symbol in [layout['primary_tileset'],layout['secondary_tileset']]:
            directory=tileset_path(symbol)
            attrs.extend(v[0] for v in struct.iter_unpack('<I',(directory/'metatile_attributes.bin').read_bytes()))
            metatiles.extend(struct.iter_unpack('<8H',(directory/'metatiles.bin').read_bytes()))
        assert all(v&1023<len(attrs) for v in blocks),(name,'missing metatile')
        assert all(tile>>12<=12 for mid in {v&1023 for v in blocks} for tile in metatiles[mid]),(name,'unloaded palette bank')
        dry=set();water=set()
        for i,v in enumerate(blocks):
            if v&0xC00:continue
            behavior=attrs[v&1023]&511
            assert v>>12==(1 if behavior in SURF else 3),(name,'incorrect elevation',i%w,i//w)
            (water if behavior in SURF else dry).add(i)
        edge={x for x in range(w)}|{(h-1)*w+x for x in range(w)}|{y*w for y in range(h)}|{y*w+w-1 for y in range(h)}
        assert not (dry|water)&edge,(name,'unsealed map edge')
        start=header['warp_events'][0]['y']*w+header['warp_events'][0]['x']
        seen=flood(start,dry,w,h)
        assert dry<=seen,(name,'unreachable dry terrain',[(i%w,i//w) for i in dry-seen][:12])
        if key=='town':
            gym=next(f for f in landmarks[key]['facades'] if f[0]=='gym')
            _,gx,gy,gw,gh=gym;goal=(gy+gh)*w+gx+3
            for sx,sy in [(31,51),(54,40),(32,36)]:
                cut={(sy+dy)*w+sx+dx for dy in range(2) for dx in range(3)}
                assert goal not in flood(start,dry-cut,w,h),(name,'mountain stairs can be bypassed',sx,sy)
        assert len(flood(next(iter(water)),water,w,h))==len(water),(name,'isolated water pocket')
        for x,y in landmarks[key]['goals']:assert y*w+x in seen,(name,'unreachable landmark',x,y)
        for kind,x,y,fw,fh in landmarks[key]['facades']:
            assert all(blocks[(y+dy)*w+x+dx]&0xC00 for dy in range(fh) for dx in range(fw)),(name,'unfinished interior door is passable',kind)
        for event in header['bg_events']:
            x,y=event['x'],event['y'];assert blocks[y*w+x]==0x3402 and event['elevation']==3
            assert (y+1)*w+x in seen,(name,'unreadable sign')
        warps=header['warp_events'];assert len(warps)==2
        warp_positions={(event['x'],event['y']) for event in warps}
        for i in dry:
            if 0x60<=attrs[blocks[i]&1023]&511<=0x7F:assert (i%w,i//w) in warp_positions,(name,'unconfigured warp behavior')
        for event in warps:
            x,y=event['x'],event['y'];north=key=='town';dy=-1 if north else 1
            assert attrs[blocks[y*w+x]&1023]&511==(0x64 if north else 0x65),(name,'wrong trail exit direction')
            assert blocks[(y+dy)*w+x]&0xC00,(name,'walk past trail warp')
            assert (y-dy)*w+x in seen,(name,'no safe arrival step')
        if rom is not None:assert raw in rom,(name,'stale ROM map')
        maps[header['id']]=header
        print(f'{name}: {w}x{h}; {len(dry)} connected dry tiles, all facade/sign approaches, two directional trail warps and sealed edges passed.')
    for header in maps.values():
        for index,event in enumerate(header['warp_events']):
            other=maps[event['dest_map']]['warp_events'][int(event['dest_warp_id'])]
            assert other['dest_map']==header['id'] and int(other['dest_warp_id'])==index,'Nonreciprocal beach loop'
    print('Four reciprocal warp records form the north-beach loop. No Surf required for either trail.')
    if rom is not None:print('Exact Navel town and beach blocks are included in the built ROM.')


if __name__=='__main__':main()
