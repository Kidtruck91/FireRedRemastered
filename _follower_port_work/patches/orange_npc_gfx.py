# Run from the repo root.
# Overworld sprites for Prof. Ivy and Cissy: recolours of the Lorelei and Misty sprites
# (same pixels, new palettes) in the "special" NPC palette slot, like the Johto NPCs.
#   OBJ_EVENT_GFX_IVY   = 168   (walks: 9 frames)
#   OBJ_EVENT_GFX_CISSY = 169   (stands: 3 frames, like Misty)
import re
PALS = {
 'ivy':   [(115,197,164),(255,213,180),(246,189,148),(222,148,115),(123,65,65),(0,0,0),(0,0,0),(60,40,70),
           (176,92,60),(126,58,38),(70,28,16),(200,220,236),(236,236,244),(172,176,196),(255,255,255),(0,0,0)],
 'cissy': [(115,197,164),(255,213,180),(246,189,148),(222,148,115),(123,65,65),(110,200,220),(48,128,176),(24,70,120),
           (0,0,0),(248,248,252),(96,120,170),(0,0,0),(232,112,48),(16,34,70),(0,0,0),(0,0,0)],
}
for name, pal in PALS.items():
    with open('graphics/object_events/palettes/%s.pal' % name, 'w', newline='\r\n') as f:
        f.write('JASC-PAL\n0100\n16\n' + ''.join('%d %d %d\n' % c for c in pal))

p = 'include/constants/event_objects.h'; t = open(p).read()
if 'OBJ_EVENT_GFX_IVY' not in t:
    t = t.replace('#define NUM_OBJ_EVENT_GFX     168\n',
                  '#define OBJ_EVENT_GFX_IVY 168\n#define OBJ_EVENT_GFX_CISSY 169\n#define NUM_OBJ_EVENT_GFX     170\n', 1)
    assert 'OBJ_EVENT_GFX_IVY' in t
    open(p, 'w', newline='\n').write(t)

p = 'src/data/object_events/object_event_graphics_info_pointers.h'; t = open(p).read()
if 'GraphicsInfo_Ivy' not in t:
    t = t.replace('const struct ObjectEventGraphicsInfo gObjectEventGraphicsInfo_Sudowoodo;',
                  'const struct ObjectEventGraphicsInfo gObjectEventGraphicsInfo_Sudowoodo;\n'
                  'const struct ObjectEventGraphicsInfo gObjectEventGraphicsInfo_Ivy;\n'
                  'const struct ObjectEventGraphicsInfo gObjectEventGraphicsInfo_Cissy;', 1)
    old = '    [OBJ_EVENT_GFX_SUDOWOODO]                = &gObjectEventGraphicsInfo_Sudowoodo,\n'
    assert old in t
    t = t.replace(old, old + '    [OBJ_EVENT_GFX_IVY]                      = &gObjectEventGraphicsInfo_Ivy,\n'
                             '    [OBJ_EVENT_GFX_CISSY]                    = &gObjectEventGraphicsInfo_Cissy,\n', 1)
    assert 'GraphicsInfo_Cissy;' in t
    open(p, 'w', newline='\n').write(t)

p = 'src/data/object_events/object_event_graphics.h'; t = open(p).read()
if 'gObjectEventPalette_Ivy' not in t:
    old = 'const u16 gObjectEventPalette_Elm[] = INCBIN_U16("graphics/object_events/palettes/elm.gbapal");\n'
    assert old in t
    t = t.replace(old, old + 'const u16 gObjectEventPalette_Ivy[] = INCBIN_U16("graphics/object_events/palettes/ivy.gbapal");\n'
                             'const u16 gObjectEventPalette_Cissy[] = INCBIN_U16("graphics/object_events/palettes/cissy.gbapal");\n', 1)
    open(p, 'w', newline='\n').write(t)

p = 'src/event_object_movement.c'; t = open(p).read()
if 'OBJ_EVENT_PAL_IVY' not in t:
    old = '#define OBJ_EVENT_PAL_ELM                           0x1124\n'
    assert old in t
    t = t.replace(old, old + '#define OBJ_EVENT_PAL_IVY                           0x1125\n'
                             '#define OBJ_EVENT_PAL_CISSY                         0x1126\n', 1)
    old = '    {gObjectEventPalette_Elm,               OBJ_EVENT_PAL_ELM},\n'
    assert old in t
    t = t.replace(old, old + '    {gObjectEventPalette_Ivy,               OBJ_EVENT_PAL_IVY},\n'
                             '    {gObjectEventPalette_Cissy,             OBJ_EVENT_PAL_CISSY},\n', 1)
    open(p, 'w', newline='\n').write(t)

p = 'src/data/object_events/object_event_graphics_info.h'; t = open(p).read()
if 'gObjectEventGraphicsInfo_Ivy' not in t:
    def clone(src, dst, tag):
        m = re.search(r'const struct ObjectEventGraphicsInfo gObjectEventGraphicsInfo_%s = \{.*?\n\};\n' % src, t, re.S)
        s = m.group(0).replace('gObjectEventGraphicsInfo_%s ' % src, 'gObjectEventGraphicsInfo_%s ' % dst)
        s = re.sub(r'\.paletteTag = \w+', '.paletteTag = %s' % tag, s)
        s = re.sub(r'\.paletteSlot = \w+', '.paletteSlot = PALSLOT_NPC_SPECIAL', s)
        s = re.sub(r'\.reflectionPaletteTag\s*=\s*\w+', '.reflectionPaletteTag = OBJ_EVENT_PAL_TAG_NONE', s)
        return s
    t = t.rstrip('\n') + '\n\n// Prof. Ivy (recoloured Lorelei) and Cissy (recoloured Misty)\n' \
        + clone('Lorelei', 'Ivy', 'OBJ_EVENT_PAL_IVY') + '\n' + clone('Misty', 'Cissy', 'OBJ_EVENT_PAL_CISSY')
    open(p, 'w', newline='\n').write(t)
print('orange npc gfx ok')
