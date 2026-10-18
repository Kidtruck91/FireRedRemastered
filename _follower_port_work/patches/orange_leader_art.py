# Run from the repo root, after orange_gyms.py.
# Leader art, recoloured from existing sprites (same pixels, new palettes):
#  - Overworld: Danny (Black Belt), Rudy (Cooltrainer M), Luana (Beauty) in the special NPC slot.
#  - Battle front pics: Cissy (Misty), Danny (RS Black Belt), Rudy (Cooltrainer M), Luana (Beauty).
# The leaders and their gym objects are switched over to the new art.
import json, re

def rd(p): return open(p, encoding='utf-8').read()
def wr(p, s): open(p, 'w', newline='\n', encoding='utf-8').write(s)
def sub1(t, old, new, label):
    assert old in t, label
    return t.replace(old, new, 1)
def write_pal(path, pal):
    with open(path, 'w', newline='\r\n') as f:
        f.write('JASC-PAL\n0100\n16\n' + ''.join('{} {} {}\n'.format(*c) for c in pal))

OW = {'danny': [(115, 197, 164), (232, 184, 140), (200, 148, 104), (156, 108, 72), (123, 65, 65), (205, 172, 98), (52, 52, 74), (24, 24, 36), (230, 106, 74), (148, 57, 41), (82, 16, 0), (150, 170, 226), (108, 128, 196), (72, 88, 156), (214, 226, 250), (0, 0, 0)],
      'rudy': [(115, 197, 164), (255, 213, 180), (246, 189, 148), (222, 148, 115), (123, 65, 65), (255, 222, 74), (213, 172, 32), (131, 98, 0), (244, 208, 128), (204, 156, 82), (142, 98, 52), (128, 214, 204), (62, 164, 164), (30, 112, 122), (255, 255, 255), (0, 0, 0)],
      'luana': [(115, 197, 164), (255, 213, 180), (246, 189, 148), (222, 148, 115), (123, 65, 65), (244, 160, 208), (206, 96, 166), (134, 52, 112), (206, 168, 238), (106, 82, 189), (110, 80, 150), (176, 126, 214), (120, 80, 170), (74, 49, 49), (255, 255, 255), (0, 0, 0)]}
TP = {'cissy': [(213, 222, 238), (255, 222, 205), (238, 180, 148), (213, 148, 115), (123, 90, 82), (24, 70, 120), (48, 128, 176), (110, 200, 220), (170, 180, 210), (240, 240, 250), (90, 100, 140), (222, 222, 246), (255, 0, 255), (232, 112, 48), (255, 255, 255), (0, 0, 0)],
      'danny': [(115, 197, 164), (232, 184, 140), (200, 148, 104), (156, 108, 72), (90, 65, 49), (148, 131, 123), (106, 90, 82), (58, 58, 78), (30, 30, 44), (150, 170, 226), (108, 128, 196), (72, 88, 156), (255, 214, 74), (214, 160, 32), (214, 226, 250), (0, 0, 0)],
      'rudy': [(115, 197, 164), (255, 222, 205), (238, 180, 148), (205, 139, 115), (123, 90, 82), (222, 222, 230), (164, 180, 189), (74, 98, 123), (244, 208, 128), (204, 156, 82), (142, 98, 52), (128, 214, 204), (62, 164, 164), (30, 112, 122), (255, 255, 255), (0, 0, 0)],
      'luana': [(115, 197, 164), (255, 222, 205), (238, 180, 148), (197, 139, 106), (106, 74, 74), (244, 160, 208), (206, 96, 166), (134, 52, 112), (230, 123, 115), (213, 74, 65), (74, 74, 90), (176, 126, 214), (206, 168, 238), (213, 222, 246), (255, 255, 255), (0, 0, 0)]}
OW_BASE = {'danny': 'Blackbelt', 'rudy': 'CooltrainerM', 'luana': 'Beauty'}
TP_BASE = {'cissy': 'LeaderMisty', 'danny': 'RSBlackBelt', 'rudy': 'CooltrainerM', 'luana': 'Beauty'}
TP_ANIMS = {'cissy': 'sAnims_LeaderMisty', 'danny': 'sAnims_RSBlackBelt', 'rudy': 'sAnims_CooltrainerM', 'luana': 'sAnims_Beauty'}
ORDER = ['cissy', 'danny', 'rudy', 'luana']

# ---------------------------------------------------------------- overworld
for n, pal in OW.items():
    write_pal('graphics/object_events/palettes/{}.pal'.format(n), pal)
p = 'include/constants/event_objects.h'; t = rd(p)
if 'OBJ_EVENT_GFX_DANNY' not in t:
    t = sub1(t, '#define NUM_OBJ_EVENT_GFX     170\n',
             '#define OBJ_EVENT_GFX_DANNY 170\n#define OBJ_EVENT_GFX_RUDY 171\n#define OBJ_EVENT_GFX_LUANA 172\n#define NUM_OBJ_EVENT_GFX     173\n', 'ids')
    wr(p, t)
p = 'src/data/object_events/object_event_graphics_info_pointers.h'; t = rd(p)
if 'GraphicsInfo_Danny' not in t:
    t = sub1(t, 'const struct ObjectEventGraphicsInfo gObjectEventGraphicsInfo_Cissy;',
             'const struct ObjectEventGraphicsInfo gObjectEventGraphicsInfo_Cissy;\n'
             'const struct ObjectEventGraphicsInfo gObjectEventGraphicsInfo_Danny;\n'
             'const struct ObjectEventGraphicsInfo gObjectEventGraphicsInfo_Rudy;\n'
             'const struct ObjectEventGraphicsInfo gObjectEventGraphicsInfo_Luana;', 'decl')
    old = '    [OBJ_EVENT_GFX_CISSY]                    = &gObjectEventGraphicsInfo_Cissy,\n'
    t = sub1(t, old, old + '    [OBJ_EVENT_GFX_DANNY]                    = &gObjectEventGraphicsInfo_Danny,\n'
                         '    [OBJ_EVENT_GFX_RUDY]                     = &gObjectEventGraphicsInfo_Rudy,\n'
                         '    [OBJ_EVENT_GFX_LUANA]                    = &gObjectEventGraphicsInfo_Luana,\n', 'ptr')
    wr(p, t)
p = 'src/data/object_events/object_event_graphics.h'; t = rd(p)
if 'gObjectEventPalette_Danny' not in t:
    old = 'const u16 gObjectEventPalette_Cissy[] = INCBIN_U16("graphics/object_events/palettes/cissy.gbapal");\n'
    t = sub1(t, old, old + ''.join('const u16 gObjectEventPalette_{}[] = INCBIN_U16("graphics/object_events/palettes/{}.gbapal");\n'.format(n.capitalize(), n)
                                   for n in ('danny', 'rudy', 'luana')), 'pal')
    wr(p, t)
p = 'src/event_object_movement.c'; t = rd(p)
if 'OBJ_EVENT_PAL_DANNY' not in t:
    old = '#define OBJ_EVENT_PAL_CISSY                         0x1126\n'
    t = sub1(t, old, old + '#define OBJ_EVENT_PAL_DANNY                         0x1127\n'
                         '#define OBJ_EVENT_PAL_RUDY                          0x1128\n'
                         '#define OBJ_EVENT_PAL_LUANA                         0x1129\n', 'tag')
    old = '    {gObjectEventPalette_Cissy,             OBJ_EVENT_PAL_CISSY},\n'
    t = sub1(t, old, old + '    {gObjectEventPalette_Danny,             OBJ_EVENT_PAL_DANNY},\n'
                         '    {gObjectEventPalette_Rudy,              OBJ_EVENT_PAL_RUDY},\n'
                         '    {gObjectEventPalette_Luana,             OBJ_EVENT_PAL_LUANA},\n', 'tbl')
    wr(p, t)
p = 'src/data/object_events/object_event_graphics_info.h'; t = rd(p)
if 'gObjectEventGraphicsInfo_Danny' not in t:
    def clone(src, dst, tag):
        m = re.search(r'const struct ObjectEventGraphicsInfo gObjectEventGraphicsInfo_{} = \{{.*?\n\}};\n'.format(src), t, re.S)
        s = m.group(0).replace('gObjectEventGraphicsInfo_{} '.format(src), 'gObjectEventGraphicsInfo_{} '.format(dst))
        s = re.sub(r'\.paletteTag = \w+', '.paletteTag = ' + tag, s)
        s = re.sub(r'\.paletteSlot = \w+', '.paletteSlot = PALSLOT_NPC_SPECIAL', s)
        s = re.sub(r'\.reflectionPaletteTag\s*=\s*\w+', '.reflectionPaletteTag = OBJ_EVENT_PAL_TAG_NONE', s)
        return s
    t = t.rstrip('\n') + '\n\n// Orange League leaders: Danny (recoloured Black Belt), Rudy (Cooltrainer M), Luana (Beauty)\n' \
        + '\n'.join(clone(OW_BASE[n], n.capitalize(), 'OBJ_EVENT_PAL_' + n.upper()) for n in ('danny', 'rudy', 'luana'))
    wr(p, t)

# ---------------------------------------------------------------- battle pics
for n, pal in TP.items():
    write_pal('graphics/trainers/palettes/{}.pal'.format(n), pal)
p = 'include/constants/trainers.h'; t = rd(p)
if 'TRAINER_PIC_CISSY' not in t:
    t = sub1(t, '#define TRAINER_PIC_COUNT                 156\n',
             '#define TRAINER_PIC_CISSY                 156\n#define TRAINER_PIC_DANNY                 157\n'
             '#define TRAINER_PIC_RUDY                  158\n#define TRAINER_PIC_LUANA                 159\n'
             '#define TRAINER_PIC_COUNT                 160\n', 'pics')
    wr(p, t)
p = 'src/data/graphics/trainers.h'; t = rd(p)
if 'gTrainerPalette_Cissy' not in t:
    t = t.rstrip('\n') + '\n\n// Orange League leaders (recoloured: the front pics are shared with their base trainers)\n' + \
        ''.join('const u32 gTrainerPalette_{}[] = INCBIN_U32("graphics/trainers/palettes/{}.gbapal.lz");\n'.format(n.capitalize(), n) for n in ORDER) + '\n'
    wr(p, t)
p = 'include/graphics.h'; t = rd(p)
if 'gTrainerPalette_Cissy' not in t:
    t = sub1(t, 'extern const u32 gTrainerPalette_LeaderClair[];\n', 'extern const u32 gTrainerPalette_LeaderClair[];\n' +
             ''.join('extern const u32 gTrainerPalette_{}[];\n'.format(n.capitalize()) for n in ORDER), 'ext')
    wr(p, t)
p = 'src/data/trainer_graphics/front_pic_tables.h'; t = rd(p)
if 'TRAINER_SPRITE(CISSY' not in t:
    t = sub1(t, '    TRAINER_SPRITE(LEADER_CLAIR, gTrainerFrontPic_LeaderClair, 0x800),\n',
             '    TRAINER_SPRITE(LEADER_CLAIR, gTrainerFrontPic_LeaderClair, 0x800),\n' +
             ''.join('    TRAINER_SPRITE({}, gTrainerFrontPic_{}, 0x800),\n'.format(n.upper(), TP_BASE[n]) for n in ORDER), 'sprite')
    t = sub1(t, '    TRAINER_PAL(LEADER_CLAIR, gTrainerPalette_LeaderClair)',
             '    TRAINER_PAL(LEADER_CLAIR, gTrainerPalette_LeaderClair),\n' +
             ',\n'.join('    TRAINER_PAL({}, gTrainerPalette_{})'.format(n.upper(), n.capitalize()) for n in ORDER), 'pal')
    # gTrainerFrontPicCoords is positional: one entry per new pic (all four bases use size 8, y 1)
    i = t.index('gTrainerFrontPicCoords[] ='); j = t.index('};', i)
    t = t[:j] + ''.join('    {{.size = 8, .y_offset = 1}}, // {}\n'.format(n.upper()) for n in ORDER) + t[j:]
    wr(p, t)
p = 'src/data/trainer_graphics/front_pic_anims.h'; t = rd(p)
if 'TRAINER_PIC_CISSY' not in t:
    t = sub1(t, '    [TRAINER_PIC_LEADER_CLAIR]         = sAnims_LeaderClair,\n',
             '    [TRAINER_PIC_LEADER_CLAIR]         = sAnims_LeaderClair,\n' +
             ''.join('    [TRAINER_PIC_{}]{}= {},\n'.format(n.upper(), ' ' * (18 - len(n)), TP_ANIMS[n]) for n in ORDER), 'anims')
    wr(p, t)
for p, val in (('src/data/battle_transitions/mugshot_coords.h', '{ 0, 0}'), ('src/data/battle_transitions/mugshot_rotation_scales.h', '0x200')):
    t = rd(p)
    if 'TRAINER_PIC_CISSY' not in t:
        old = re.search(r' \[TRAINER_PIC_LEADER_CLAIR\]\s+= [^\n]*\n', t).group(0)
        t = sub1(t, old, old + ''.join(' [TRAINER_PIC_{}]{}= {},\n'.format(n.upper(), ' ' * (32 - len(n)), val) for n in ORDER), p)
        wr(p, t)

# ---------------------------------------------------------------- use the art
p = 'src/data/trainers.h'; t = rd(p)
for const, pic in (('TRAINER_LEADER_CISSY', 'TRAINER_PIC_CISSY'), ('TRAINER_LEADER_DANNY', 'TRAINER_PIC_DANNY'),
                   ('TRAINER_LEADER_RUDY', 'TRAINER_PIC_RUDY'), ('TRAINER_LEADER_LUANA', 'TRAINER_PIC_LUANA')):
    i = t.index('    [{}] = {{'.format(const)); j = t.index('    },\n', i)
    t = t[:i] + re.sub(r'\.trainerPic = \w+', '.trainerPic = ' + pic, t[i:j], count=1) + t[j:]
wr(p, t)
for m, script, gfx in (('OrangeIslands_Navel_Gym', 'OrangeIslands_Navel_Gym_EventScript_Danny', 'OBJ_EVENT_GFX_DANNY'),
                       ('OrangeIslands_Trovita_Gym', 'OrangeIslands_Trovita_Gym_EventScript_Rudy', 'OBJ_EVENT_GFX_RUDY'),
                       ('OrangeIslands_Kumquat_Gym', 'OrangeIslands_Kumquat_Gym_EventScript_Luana', 'OBJ_EVENT_GFX_LUANA')):
    p = 'data/maps/{}/map.json'.format(m); d = json.loads(rd(p))
    for o in d['object_events']:
        if o['script'] == script:
            o['graphics_id'] = gfx
    wr(p, json.dumps(d, indent=2) + '\n')
print('orange leader art ok')
