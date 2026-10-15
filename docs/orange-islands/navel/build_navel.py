"""Author Navel's island, summit trail and reciprocal north-coast loop.

This is an explicit layout draft, not a general town generator. It changes only
the two Navel layouts/events. Facades use native FRLG motifs; all dry walking
surfaces use elevation 3 and ocean uses elevation 1.
"""
from pathlib import Path
import json
import struct
import sys
from PIL import Image, ImageDraw

HERE=Path(__file__).resolve().parent
ROOT=HERE.parents[2]
sys.path.insert(0,str(HERE.parent))
sys.path.insert(0,str(HERE.parent/'water-routes'))
from render_maps import render
from add_boundaries import seal

WATER=0x112B
SAND=0x3103
GRASS=0x3001
TABLE=json.loads((ROOT/'data/layouts/layouts.json').read_text())
LAYOUTS={l['name'].removesuffix('_Layout'):l for l in TABLE['layouts']}


def polygon(w,h,points):
    mask=Image.new('1',(w,h));ImageDraw.Draw(mask).polygon(points,fill=1)
    return {(x,y) for y in range(h) for x in range(w) if mask.getpixel((x,y))}


class Island:
    def __init__(self,name,w,h,outline):
        self.name='OrangeIslands_'+name;self.w=w;self.h=h
        self.land=polygon(w,h,outline);self.a=[[WATER]*w for _ in range(h)]
        self.signs=[];self.goals=[];self.facades=[];self.warps=[]
        # A generous sand margin keeps the grass away from the wave band.
        for x,y in self.land:
            inland=all((x+dx,y+dy) in self.land for dx,dy in [(0,-4),(0,4),(-4,0),(4,0)])
            self.a[y][x]=GRASS if inland else 0x3115
        for x,y in self.land:
            n,s,l,r=[p not in self.land for p in [(x,y-1),(x,y+1),(x-1,y),(x+1,y)]]
            mid=0x10C if n and l else 0x10E if n and r else 0x11C if s and l else 0x11E if s and r else 0x10D if n else 0x11D if s else 0x114 if l else 0x116 if r else None
            if mid is None:
                # Concave coast turns use native sand/water inset corners.
                mid=next((m for p,m in [((x-1,y-1),0x125),((x+1,y-1),0x126),((x-1,y+1),0x12D),((x+1,y+1),0x12E)] if p not in self.land),None)
            if mid is not None:self.a[y][x]=0x3000|mid

    def rect(self,x,y,w,h,v):
        for yy in range(y,y+h):
            for xx in range(x,x+w):self.a[yy][xx]=v

    def stamp(self,x,y,rows,blocked=True):
        for dy,row in enumerate(rows):
            for dx,mid in enumerate(row):self.a[y+dy][x+dx]=mid|(0x3400 if blocked else 0x3000)

    def path(self,points,width=3):
        for (x,y),(xx,yy) in zip(points,points[1:]):
            steps=max(abs(xx-x),abs(yy-y),1)
            for i in range(steps+1):
                cx=round(x+(xx-x)*i/steps);cy=round(y+(yy-y)*i/steps)
                assert all((cx+dx,cy+dy) in self.land for dy in range(-width//2+1,width//2+1) for dx in range(-width//2+1,width//2+1)),('path leaves island',cx,cy)
                self.rect(cx-width//2,cy-width//2,width,width,SAND)

    def terrace(self,points,walkable=True):
        cells=polygon(self.w,self.h,points)
        for x,y in cells:
            n,s,l,r=[p not in cells for p in [(x,y-1),(x,y+1),(x-1,y),(x+1,y)]]
            mid=0x68 if n and l else 0x6A if n and r else 0x78 if s and l else 0x7A if s and r else 0x69 if n else 0x79 if s else 0x70 if l else 0x72 if r else 0x106
            self.a[y][x]=mid|(0x3400 if n or s or l or r or not walkable else 0x3000)

    def steps(self,x,y,width=3):
        self.rect(x,y,width,1,0x3089);self.rect(x,y+1,width,1,0x3091)

    def palm(self,x,y,sand=False):
        b=0x282 if sand else 0x280
        self.stamp(x,y,[[b,b+1],[b+16,b+17],[b+32,b+33]])

    def rock(self,x,y):self.stamp(x,y,[[0x98,0x9A],[0xA8,0xAA]])

    def building(self,x,y,kind):
        rows={
            'center':[[0x48,0x49,0x4A,0x4B,0x187],[0x50,0x51,0x52,0x53,0x18F],[0x58,0x59,0x5A,0x5B,0x197],[0x60,0x61,0x62,0x186,0x19F]],
            'mart':[[0x28,0x29,0x2A,0x2B],[0x30,0x31,0x32,0x33],[0x38,0x39,0x3A,0x3B],[0x40,0x41,0x62,0x63]],
            'gym':[[0x139]+[0x13A]*5+[0x13B],[0x141]+[0x142]*5+[0x143],[0x149]+[0x14A]*5+[0x14B],[0x150,0x151,0x152,0x153,0x154,0x155,0x156],[0x158,0x159,0x15A,0x15B,0x15C,0x15D,0x15E]],
        }
        if kind in ('house','large_house'):
            # The One Island house motif, remapped through the packed Sevii set.
            mapping=json.loads((ROOT/'data/tilesets/secondary/orange_sevii_123/packing.json').read_text())['sevii_metatile_mapping']
            rows[kind]=[[int(mapping[str(mid)]) for mid in row] for row in [[0x314,0x315,0x316,0x316,0x317],[0x286,0x287,0x29E,0x29E,0x29F],[0x28E,0x28F,0x2A6,0x2A5,0x2A7],[0x296,0x297,0x2AE,0x2AD,0x2AF]]]
        if kind=='large_house':
            # Add a complete two-column window bay beneath matching roof strips.
            rows[kind]=[row[:4]+row[2:4]+row[4:] for row in rows[kind]]
        patch=rows[kind];self.stamp(x,y,patch)
        self.goals.append((x+(3 if kind=='gym' else 2),y+len(patch)))
        self.facades.append([kind,x,y,len(patch[0]),len(patch)])

    def sign(self,x,y,label,lines):
        self.a[y][x]=0x3402;self.signs.append((x,y,label,lines));self.goals.append((x,y+1))

    def warp(self,x,y,dest,index,north):
        self.a[y][x]=0x3000|(0x1F7 if north else 0x1FF)
        self.warps.append(dict(x=x,y=y,elevation=3,dest_map='MAP_ORANGE_ISLANDS_'+dest.upper(),dest_warp_id=str(index)))
        # A one-tile trail mouth makes both the warp and its direction explicit.
        self.rock(x-2,y-1 if north else y)
        self.rock(x+1,y-1 if north else y)
        self.a[y+(-1 if north else 1)][x]=0x34AF
        self.goals.append((x,y))

    def finish(self):
        # Blend sand paths and the upper beach margin into grass. Decorations
        # are stamped after this stage so complete motifs are never sliced.
        old=[r[:] for r in self.a]
        def green(x,y):return 0<=x<self.w and 0<=y<self.h and old[y][x]==GRASS
        for y in range(1,self.h-1):
            for x in range(1,self.w-1):
                if old[y][x] not in (SAND,0x3115,0x3106):continue
                n,s,l,r=green(x,y-1),green(x,y+1),green(x-1,y),green(x+1,y)
                mid=0xD3 if n and l else 0xD5 if n and r else 0x1E6 if s and l else 0x1E7 if s and r else 0xD4 if n else 0xE3 if s else 0xDC if l else 0xE2 if r else None
                if mid is not None:self.a[y][x]=0x3000|mid
        for y in range(self.h):
            for x in range(self.w):
                if self.a[y][x]==GRASS and (x*19+y*23)%47 in (0,2):self.a[y][x]=0x3000|(8 if (x+y)%2 else 9)

    def save(self):
        seal(self.a)
        layout=LAYOUTS[self.name];layout.update(width=self.w,height=self.h,primary_tileset='gTileset_OrangeGeneral',secondary_tileset='gTileset_OrangeSevii123')
        assert (self.w+15)*(self.h+14)<=10240
        (ROOT/layout['blockdata_filepath']).write_bytes(struct.pack('<'+'H'*(self.w*self.h),*(v for row in self.a for v in row)))
        (ROOT/layout['border_filepath']).write_bytes(struct.pack('<4H',0x510,0x511,0x518,0x519))
        directory=ROOT/'data/maps'/self.name;hp=directory/'map.json';header=json.loads(hp.read_text())
        header['warp_events']=self.warps
        header['bg_events']=[dict(type='sign',x=x,y=y,elevation=3,player_facing_dir='BG_EVENT_PLAYER_FACING_ANY',script=self.name+'_EventScript_'+label) for x,y,label,_ in self.signs]
        hp.write_text(json.dumps(header,indent=2)+'\n')
        scripts=self.name+'_MapScripts::\n\t.byte 0\n';texts=''
        for x,y,label,lines in self.signs:
            scripts+=f'\n{self.name}_EventScript_{label}::\n\tmsgbox {self.name}_Text_{label}, MSGBOX_SIGN\n\tend\n'
            texts+=f'{self.name}_Text_{label}::\n'
            texts+=''.join('\t.string "'+line+(('\\n' if i==0 else '\\l') if i<len(lines)-1 else '$')+'"\n' for i,line in enumerate(lines))+'\n'
        (directory/'scripts.inc').write_text(scripts);(directory/'text.inc').write_text(texts.rstrip()+'\n')
        im=render(layout);im.save(HERE/('town.png' if self.name.endswith('Navel') else 'beach.png'));im.save(HERE.parent/(self.name+'.png'))
        return dict(size=[self.w,self.h],goals=self.goals,facades=self.facades,warps=self.warps)


def build():
    town=Island('Navel',88,80,[(18,10),(29,10),(29,8),(42,8),(42,10),(54,10),(54,12),(65,12),(65,16),(74,16),(74,23),(80,23),(80,35),(79,35),(79,43),(79,54),(73,54),(73,62),(66,62),(66,67),(57,67),(57,62),(51,62),(51,68),(44,68),(44,73),(28,73),(28,70),(18,70),(18,66),(11,66),(11,59),(8,59),(8,47),(12,47),(12,37),(9,37),(9,26),(13,26),(13,16),(18,16)])
    # A broad ridge with an eastern shoulder; two smaller summit ledges give
    # the ascent three changes of direction without cloning Kindle Road.
    town.terrace([(24,15),(48,15),(48,17),(58,17),(58,20),(65,20),(65,27),(72,27),(72,41),(65,41),(65,51),(30,51),(30,49),(22,49),(22,43),(18,43),(18,27),(21,27),(21,20),(24,20)])
    town.terrace([(28,21),(43,21),(43,23),(51,23),(51,26),(59,26),(59,30),(62,30),(62,36),(58,36),(58,40),(49,40),(49,43),(30,43),(30,41),(25,41),(25,30),(28,30)])
    town.terrace([(32,25),(42,25),(42,27),(48,27),(48,29),(51,29),(51,34),(45,34),(45,36),(31,36),(31,33),(29,33),(29,29),(32,29)])
    # The northbound mouths sit on opposite headlands, as in the user's sketch.
    town.path([(19,12),(19,20),(16,23),(16,31),(16,43),(18,49),(24,53),(31,55),(47,55),(60,55),(70,55),(75,45),(75,33),(75,25),(72,22),(68,18)])
    town.path([(31,55),(31,63),(39,63),(39,68)])
    town.path([(18,49),(22,56),(19,56),(19,63)])
    town.path([(47,55),(47,61),(52,61)])
    # Long eastward first traverse, westward second traverse, final summit turn.
    town.path([(32,55),(32,51),(32,49),(58,49),(61,46),(61,42),(55,42),(55,40),(55,38),(47,38),(33,38),(33,36),(33,34),(37,34),(37,31)])
    town.steps(31,51);town.steps(54,40);town.steps(32,36)
    # Village lots cluster around a crooked lane, leaving the coast open.
    town.rect(24,53,12,8,GRASS);town.path([(32,53),(32,59),(28,59),(28,58)])
    town.rect(40,53,9,10,GRASS);town.path([(39,55),(44,55),(44,59),(47,59)])
    town.rect(14,57,12,9,GRASS);town.path([(22,56),(22,63),(19,63)])
    town.rect(32,61,13,9,GRASS);town.path([(31,63),(39,63),(39,68)])
    town.rect(63,30,9,8,GRASS)
    town.path([(61,42),(63,42),(63,39),(67,39),(67,35)])
    town.finish()
    town.steps(31,51);town.steps(54,40);town.steps(32,36)
    town.building(26,54,'center');town.building(42,54,'mart')
    town.building(16,58,'house');town.building(34,64,'house')
    town.building(34,26,'gym')
    town.building(64,31,'large_house')
    town.sign(42,32,'Gym',['NAVEL SUMMIT',"DANNY'S GYM",'The coast lies below.'])
    town.sign(24,58,'Town',['NAVEL ISLAND','Village below the summit'])
    town.sign(62,52,'Beach',['NORTH COAST LOOP','Both trails lead to the beach.'])
    town.sign(22,15,'WestTrail',['NORTH BEACH','West trail - return via the east.'])
    town.sign(71,21,'EastTrail',['NORTH BEACH','East trail - return via the west.'])
    town.warp(19,12,'Navel_Beach',0,True);town.warp(68,18,'Navel_Beach',1,True)
    for x,y in [(13,42),(12,52),(25,65),(45,65),(63,58),(72,48),(73,28),(56,14)]:town.palm(x,y)
    for x,y in [(24,23),(59,33),(27,43),(56,44),(33,17),(52,28),(35,44),(42,45)]:town.rock(x,y)
    # Small upland grass patches and flowers soften the village, not a forest.
    for x,y in [(24,59),(25,59),(40,66),(41,66),(29,59),(30,59)]:town.a[y][x]=0x3004

    beach=Island('Navel_Beach',88,38,[(11,17),(17,17),(17,13),(25,13),(25,11),(39,11),(39,14),(48,14),(48,17),(55,17),(55,12),(67,12),(67,14),(75,14),(75,19),(79,19),(79,31),(73,31),(73,34),(14,34),(14,31),(8,31),(8,22),(11,22)])
    # Backing cliffs split the north coast into three connected coves. The
    # continuous dry trail crosses all of them without requiring Surf.
    beach.rect(15,25,58,9,GRASS)
    beach.terrace([(27,27),(39,27),(39,29),(48,29),(48,33),(27,33)],False)
    beach.terrace([(56,25),(62,25),(62,27),(72,27),(72,33),(56,33)],False)
    beach.path([(19,30),(19,25),(23,22),(36,22),(43,24),(52,24),(58,21),(68,21),(73,25),(74,29),(74,30)])
    beach.finish()
    beach.warp(19,30,'Navel',0,False);beach.warp(74,30,'Navel',1,False)
    beach.sign(21,26,'Town',['WEST TRAIL','Return to NAVEL village.'])
    beach.sign(71,25,'Harbor',['EAST TRAIL','Around the mountain to town.'])
    for x,y in [(14,22),(26,18),(39,17),(51,26),(63,16),(75,22)]:beach.palm(x,y)
    for x,y in [(11,27),(31,15),(44,19),(59,15),(69,29)]:beach.rock(x,y)
    report={'town':town.save(),'beach':beach.save()}
    (ROOT/'data/layouts/layouts.json').write_text(json.dumps(TABLE,indent=2)+'\n')
    (HERE/'landmarks.json').write_text(json.dumps(report,indent=2)+'\n')
    print('Authored Navel 88x80 and north coast 88x38, with two reciprocal trail warps.')


if __name__=='__main__':build()
