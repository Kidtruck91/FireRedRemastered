import json,re,os
FR='/home/claude/fr'
d={int(k):v for k,v in json.load(open('/home/claude/gen/followers.json'))['species'].items()}
spc={}
for m in re.finditer(r'#define (SPECIES_\w+) (\d+)', open(FR+'/include/constants/species.h').read()):
    spc.setdefault(int(m.group(2)), m.group(1))
def camel(sp):
    n=spc[sp][len('SPECIES_'):]
    return ''.join(w.capitalize() for w in n.split('_'))
L=[]
L.append('// Follower Pokemon overworld graphics. Generated from the FollowerEm data (non-expansion species list).')
L.append('// Indexed by species: the follower graphics id for a species is OBJ_EVENT_GFX_FOLLOWER(species).\n')
pics_written=set()
for sp,v in sorted(d.items()):
    for sym,path in v['pics'].items():
        fn=os.path.basename(path)
        L.append(f'static const u32 sFollowerPic_{camel(sp)}[] = INCBIN_U32("graphics/object_events/pics/followers/{fn}");')
    pal=os.path.basename(v['palpath'])
    L.append(f'static const u16 sFollowerPal_{camel(sp)}[] = INCBIN_U16("graphics/object_events/palettes/followers/{pal}");')
L.append('')
for sp,v in sorted(d.items()):
    (sym,)=list(v['pics'].keys())
    L.append(f'static const struct SpriteFrameImage sFollowerPicTable_{camel(sp)}[] = {{')
    for fr in v['frames']:
        m=re.match(r'overworld_frame\((\w+), (\d+), (\d+), (\d+)\)', fr)
        L.append(f'    overworld_frame(sFollowerPic_{camel(sp)}, {m.group(2)}, {m.group(3)}, {m.group(4)}),')
    L.append('};')
L.append('')
oammap={'sOamTables_32x32':'gObjectEventSpriteOamTables_32x32','sOamTables_64x64':'gObjectEventSpriteOamTables_64x64'}
for sp,v in sorted(d.items()):
    f=v['fields']
    L.append(f'static const struct ObjectEventGraphicsInfo sFollowerGraphicsInfo_{camel(sp)} = {{')
    L.append('    .tileTag = TAG_NONE,')
    L.append(f'    .paletteTag = FOLLOWER_PAL_TAG({spc[sp]}),')
    L.append('    .reflectionPaletteTag = OBJ_EVENT_PAL_TAG_NONE,')
    L.append(f'    .size = {f[3]},')
    L.append(f'    .width = {f[4]},')
    L.append(f'    .height = {f[5]},')
    L.append('    .paletteSlot = PALSLOT_DYNAMIC,')
    L.append(f'    .shadowSize = {f[7]},')
    L.append(f'    .inanimate = {f[8]},')
    L.append('    .disableReflectionPaletteLoad = TRUE,')
    L.append(f'    .tracks = {f[10]},')
    L.append(f'    .oam = {f[11]},')
    L.append(f'    .subspriteTables = {oammap[f[12]]},')
    L.append(f'    .anims = {f[13]},')
    L.append(f'    .images = sFollowerPicTable_{camel(sp)},')
    L.append(f'    .affineAnims = sFollowerAffineAnimTable,')
    L.append('};')
L.append('')
L.append('const struct ObjectEventGraphicsInfo *const gFollowerGraphicsInfoPointers[FOLLOWER_GFX_COUNT] = {')
L.append(f'    [SPECIES_NONE] = &sFollowerGraphicsInfo_{camel(25)},')
for sp in range(1,412):
    src = sp if sp in d else 201  # Old Unown placeholders -> Unown
    L.append(f'    [{spc[sp]}] = &sFollowerGraphicsInfo_{camel(src)},')
L.append('};\n')
L.append('static const u16 *const sFollowerPalettes[FOLLOWER_GFX_COUNT] = {')
L.append(f'    [SPECIES_NONE] = sFollowerPal_{camel(25)},')
for sp in range(1,412):
    src = sp if sp in d else 201
    L.append(f'    [{spc[sp]}] = sFollowerPal_{camel(src)},')
L.append('};')
open(FR+'/src/data/object_events/follower_graphics.h','w').write('\n'.join(L)+'\n')

# spritesheet rules
R=['','# Follower Pokemon overworld sprites']
seen=set()
for sp,v in sorted(d.items()):
    (path,)=list(v['pics'].values())
    fn=os.path.basename(path)
    if fn in seen: continue
    seen.add(fn)
    big = v['fields'][4]=='64'
    R.append(f'$(OBJEVENTGFXDIR)/followers/{fn}: %.4bpp: %.png')
    R.append(f'\t$(GFX) $< $@ -mwidth {8 if big else 4} -mheight {8 if big else 4}\n')
open('/home/claude/gen/follower_rules.mk','w').write('\n'.join(R)+'\n')
# palettes list
open('/home/claude/gen/pals.txt','w').write('\n'.join(sorted(set(v['palpath'].replace('.gbapal','.pal') for v in d.values())))+'\n')
print('ok', len(d))
