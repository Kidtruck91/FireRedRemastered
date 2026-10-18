# Run from the repo root, after rudy_counter.py, with the new PNGs already in place:
#   graphics/object_events/pics/people/{ivy,cissy,danny,rudy,luana}.png
#   graphics/trainers/front_pics/{cissy,danny,rudy,luana}_front_pic.png
# Gives Ivy and the Orange leaders their own image files (they used to share another trainer's
# image with a different palette). Each PNG's own palette is the one used in game, so the files
# can be edited (pixels and colours) in any indexed-colour editor.
import re

def rd(p): return open(p, encoding='utf-8').read()
def wr(p, s): open(p, 'w', newline='\n', encoding='utf-8').write(s)
def sub1(t, old, new, label):
    assert old in t, label
    return t.replace(old, new, 1)

OW = [('Ivy', 'ivy', 'Lorelei'), ('Cissy', 'cissy', 'Misty'), ('Danny', 'danny', 'Blackbelt'),
      ('Rudy', 'rudy', 'CooltrainerM'), ('Luana', 'luana', 'Beauty')]
TR = [('Cissy', 'cissy', 'LeaderMisty'), ('Danny', 'danny', 'RSBlackBelt'),
      ('Rudy', 'rudy', 'CooltrainerM'), ('Luana', 'luana', 'Beauty')]

# ---------------------------------------------------------------- overworld pics + palettes
p = 'src/data/object_events/object_event_graphics.h'; t = rd(p)
for name, f, base in OW:
    old = 'const u16 gObjectEventPalette_%s[] = INCBIN_U16("graphics/object_events/palettes/%s.gbapal");' % (name, f)
    new = ('const u16 gObjectEventPic_%s[] = INCBIN_U16("graphics/object_events/pics/people/%s.4bpp");\n'
           'const u16 gObjectEventPalette_%s[] = INCBIN_U16("graphics/object_events/pics/people/%s.gbapal");') % (name, f, name, f)
    if ('gObjectEventPic_%s[]' % name) not in t:
        t = sub1(t, old, new, 'ow ' + name)
wr(p, t)

p = 'src/data/object_events/object_event_pic_tables.h'; t = rd(p)
for name, f, base in OW:
    if ('sPicTable_%s[]' % name) in t:
        continue
    m = re.search(r'static const struct SpriteFrameImage sPicTable_%s\[\] = \{\n(.*?)\n\};\n' % base, t, re.S)
    assert m, 'pic table ' + base
    body = m.group(1).replace('gObjectEventPic_%s,' % base, 'gObjectEventPic_%s,' % name)
    t = t.rstrip('\n') + '\n\nstatic const struct SpriteFrameImage sPicTable_%s[] = {\n%s\n};\n' % (name, body)
wr(p, t)

p = 'src/data/object_events/object_event_graphics_info.h'; t = rd(p)
for name, f, base in OW:
    m = re.search(r'(const struct ObjectEventGraphicsInfo gObjectEventGraphicsInfo_%s = \{.*?\.images = )sPicTable_\w+(,)' % name, t, re.S)
    assert m, 'info ' + name
    t = t[:m.start()] + m.group(1) + 'sPicTable_' + name + m.group(2) + t[m.end():]
t = t.replace('// Prof. Ivy (recoloured Lorelei) and Cissy (recoloured Misty)', '// Prof. Ivy and Cissy (graphics/object_events/pics/people/ivy.png, cissy.png)')
t = t.replace('// Orange League leaders: Danny (recoloured Black Belt), Rudy (Cooltrainer M), Luana (Beauty)',
              '// Orange League leaders: Danny, Rudy, Luana (graphics/object_events/pics/people/*.png)')
wr(p, t)

# ---------------------------------------------------------------- trainer front pics + palettes
p = 'src/data/graphics/trainers.h'; t = rd(p)
for name, f, base in TR:
    old = 'const u32 gTrainerPalette_%s[] = INCBIN_U32("graphics/trainers/palettes/%s.gbapal.lz");' % (name, f)
    new = ('const u32 gTrainerFrontPic_%s[] = INCBIN_U32("graphics/trainers/front_pics/%s_front_pic.4bpp.lz");\n'
           'const u32 gTrainerPalette_%s[] = INCBIN_U32("graphics/trainers/front_pics/%s_front_pic.gbapal.lz");') % (name, f, name, f)
    if ('gTrainerFrontPic_%s[]' % name) not in t:
        t = sub1(t, old, new, 'tr ' + name)
wr(p, t)

p = 'include/graphics.h'; t = rd(p)
for name, f, base in TR:
    decl = 'extern const u32 gTrainerFrontPic_%s[];\n' % name
    if decl not in t:
        t = sub1(t, 'extern const u32 gTrainerPalette_%s[];\n' % name, decl + 'extern const u32 gTrainerPalette_%s[];\n' % name, 'gfx.h ' + name)
wr(p, t)

p = 'src/data/trainer_graphics/front_pic_tables.h'; t = rd(p)
for name, f, base in TR:
    t = t.replace('TRAINER_SPRITE(%s, gTrainerFrontPic_%s, 0x800)' % (name.upper(), base),
                  'TRAINER_SPRITE(%s, gTrainerFrontPic_%s, 0x800)' % (name.upper(), name))
    assert ('gTrainerFrontPic_%s, 0x800' % name) in t, 'sprite ' + name
wr(p, t)
print('orange art split ok')
