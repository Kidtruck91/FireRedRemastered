#!/usr/bin/env python3
"""Moro: replace the SS_ANNE sprite with the real Abandoned Ship hull tiles (ported from
Emerald Route 108 into orange_sevii_45, palette 12). Ship beached on the south shore;
board by surfing onto the gangway at (32,51); leaving drops you surfing at (32,52).
Run from repo root after the tileset files from moro_wreck/ are in place."""
import json,struct,re,os
R='.'
IDS=json.load(open(os.path.join(os.path.dirname(__file__),'moro_wreck_ids.json')))
X0,Y0=30,48; GANG=(32,51); OUT=(32,52); W=64
mb='data/layouts/OrangeIslands_Moro/map.bin'; b=bytearray(open(mb,'rb').read())
for k,mid in IDS.items():
    gx,gy=map(int,k.split(',')); x,y=X0+gx,Y0+gy
    v=mid|(1<<12)
    if (x,y)!=GANG: v|=1<<10
    struct.pack_into('<H',b,(y*W+x)*2,v)
open(mb,'wb').write(b)
mj='data/maps/OrangeIslands_Moro/map.json'; j=json.load(open(mj))
j['object_events']=[o for o in j['object_events'] if o.get('graphics_id')!='OBJ_EVENT_GFX_SS_ANNE']
for w in j['warp_events']:
    if w['dest_map']=='MAP_ORANGE_ISLANDS_MORO_GHOST_SHIP': w['x'],w['y'],w['elevation']=GANG[0],GANG[1],1
for c in j['coord_events']:
    if c['script']=='OrangeIslands_Moro_EventScript_BoardGhostShip': c['x'],c['y']=GANG
json.dump(j,open(mj,'w'),indent=2); open(mj,'a').write('\n')
s='data/maps/OrangeIslands_Moro_GhostShip/scripts.inc'; t=open(s).read()
t=re.sub(r'warp MAP_ORANGE_ISLANDS_MORO, \d+, \d+','warp MAP_ORANGE_ISLANDS_MORO, %d, %d'%OUT,t); open(s,'w').write(t)
print('wreck placed')
