import re, os, sys, json
FEM='/home/claude/fem'
def rd(p): return open(os.path.join(FEM,p), encoding='latin-1').read().replace('\r','')
consts=rd('include/constants/event_objects.h')
# non-expansion branch only
ne = consts[consts.index('#ifndef POKEMON_EXPANSION'):consts.index('#else', consts.index('#ifndef POKEMON_EXPANSION'))]
pre = consts[:consts.index('#ifndef POKEMON_EXPANSION')]
vals={}
for m in re.finditer(r'#define\s+(OBJ_EVENT_GFX_\w+)\s+(\d+)', pre+ne):
    vals[m.group(1)]=int(m.group(2))
ptrs=rd('src/data/object_events/object_event_graphics_info_pointers.h')
pmap={}
for m in re.finditer(r'\[(OBJ_EVENT_GFX_\w+)\]\s*=\s*&(gObjectEventGraphicsInfo_\w+)', ptrs):
    pmap.setdefault(m.group(1), m.group(2))
species={}
for name,v in vals.items():
    if 239<=v<=649 and not name.endswith('_BALL'):
        sp=v-238
        if name in pmap:
            species[sp]=(name,pmap[name])
print(len(species), min(species), max(species), file=sys.stderr)
missing=[s for s in range(1,412) if s not in species and not (252<=s<=276)]
print('missing',missing, file=sys.stderr)
info_src=rd('src/data/object_events/object_event_graphics_info.h')
infos={}
for m in re.finditer(r'const struct ObjectEventGraphicsInfo (gObjectEventGraphicsInfo_\w+) = \{([^}]*)\};', info_src):
    infos[m.group(1)]=[x.strip() for x in m.group(2).split(',')]
pics_src=rd('src/data/object_events/object_event_pic_tables.h')
pictabs={}
for m in re.finditer(r'(?:static )?const struct SpriteFrameImage (\w+)\[\] = \{(.*?)\};', pics_src, re.S):
    pictabs[m.group(1)]=[l.strip() for l in m.group(2).strip().split('\n') if l.strip()]
gfx_src=rd('src/data/object_events/object_event_graphics.h')
gfx={}
for m in re.finditer(r'const (u16|u32) (\w+)\[\] = INCBIN_U(16|32)\("([^"]+)"\);', gfx_src):
    gfx[m.group(2)]=(m.group(1),m.group(4))
emv=rd('src/event_object_movement.c')
paltag2sym={}
for m in re.finditer(r'\{(gObjectEventPal_\w+),\s*(OBJ_EVENT_PAL_TAG_\w+)\}', emv):
    paltag2sym.setdefault(m.group(2), m.group(1))
out={'species':{}}
for sp,(cname,iname) in sorted(species.items()):
    f=infos[iname]
    pictab=f[14]; paltag=f[1]
    frames=pictabs[pictab]
    picsyms=sorted(set(re.findall(r'overworld_frame\((\w+),', ' '.join(frames))))
    palsym=paltag2sym[paltag]
    out['species'][sp]=dict(const=cname, info=iname, fields=f, pictab=pictab, frames=frames,
        pics={p:gfx[p][1] for p in picsyms}, palsym=palsym, palpath=gfx[palsym][1], paltag=paltag)
json.dump(out, open('/home/claude/gen/followers.json','w'), indent=1)
