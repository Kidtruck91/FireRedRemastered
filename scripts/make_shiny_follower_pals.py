#!/usr/bin/env python3
"""Generates shiny overworld (follower) palettes from each species' battle palettes.

For every species: each colour of its follower palette is matched to the nearest colour of the
normal battle palette, and replaced by the matching shiny battle colour, keeping the follower
colour's own shading offset. Outputs graphics/object_events/palettes/followers_shiny/<name>.pal
and src/data/object_events/follower_shiny_palettes.h.
Run from the repo root:  python3 scripts/make_shiny_follower_pals.py  (needs Pillow: pip install pillow)
Re-run it after changing a follower palette or a battle shiny palette; hand-edit a
followers_shiny/*.pal afterwards if a generated one looks off.
"""
import os, re

def read_pal(path):
    lines = open(path).read().split('\n')
    n = int(lines[2])
    return [tuple(int(v) for v in lines[3 + i].split()) for i in range(n)]

def write_pal(path, cols):
    with open(path, 'w', newline='\r\n') as f:
        f.write('JASC-PAL\n0100\n%d\n' % len(cols))
        for c in cols:
            f.write('%d %d %d\n' % c)

def quant(c):  # GBA colours are 5-bit
    return tuple(min(255, max(0, (v // 8) * 8)) for v in c)

def dist(a, b):
    return sum(abs(x - y) for x, y in zip(a, b))

src = open('src/data/object_events/follower_graphics.h').read()
pal_path = dict(re.findall(r'static const u16 (sFollowerPal_\w+)\[\] = INCBIN_U16\("([^"]+)\.gbapal"\)', src))
table = re.search(r'sFollowerPalettes\[FOLLOWER_GFX_COUNT\] = \{(.*?)\};', src, re.S).group(1)
species_to_fpal = re.findall(r'\[(SPECIES_\w+)\] = (sFollowerPal_\w+)', table)

def table_map(path, macro):
    t = open(path).read()
    return dict(('SPECIES_' + a, b) for a, b in re.findall(macro + r'\((\w+), (\w+)\)', t))
normal_sym = table_map('src/data/pokemon_graphics/palette_table.h', 'SPECIES_PAL')
shiny_sym = table_map('src/data/pokemon_graphics/shiny_palette_table.h', 'SPECIES_SHINY_PAL')
gfx = open('src/data/graphics/pokemon.h').read()
sym_path = dict(re.findall(r'const u32 (gMon\w*Palette_\w+)\[\] = INCBIN_U32\("([^"]+)\.gbapal\.lz"\)', gfx))

os.makedirs('graphics/object_events/palettes/followers_shiny', exist_ok=True)
out_decl, out_table, done = [], [], {}
for species, fpal in species_to_fpal:
    if species == 'SPECIES_NONE' or species not in normal_sym or species not in shiny_sym:
        out_table.append('    [%s] = NULL,' % species)
        continue
    fpath = pal_path[fpal] + '.pal'
    npath = sym_path[normal_sym[species]] + '.pal'
    spath = sym_path[shiny_sym[species]] + '.pal'
    name = os.path.basename(pal_path[fpal])
    if not (os.path.exists(fpath) and os.path.exists(npath) and os.path.exists(spath)):
        out_table.append('    [%s] = NULL, // battle palette not found' % species)
        continue
    if name not in done:
        follower = read_pal(fpath)
        normal, shiny = read_pal(npath), read_pal(spath)
        usage = {}
        front = os.path.join(os.path.dirname(npath), 'front.png')
        if os.path.exists(front):
            from PIL import Image
            for idx in Image.open(front).getdata():
                usage[idx] = usage.get(idx, 0) + 1
        usable = [i for i in range(1, 16) if normal[i] != (255, 0, 255)]
        changing = [i for i in usable if normal[i] != shiny[i]]
        new = [follower[0]]
        for c in follower[1:]:
            if not changing or c == (0, 0, 0):
                new.append(c)
                continue
            # close to a colour the shiny doesn't change (outlines, greys, eyes...): keep it
            i_all = min(usable, key=lambda k: dist(c, normal[k]))
            if normal[i_all] == shiny[i_all] and dist(c, normal[i_all]) < 40:
                new.append(c)
                continue
            # otherwise follow the nearest colour that does change in the shiny; when two are
            # about equally close, prefer the one covering more of the battle sprite (the body,
            # not a small detail like a flame)
            best = min(dist(c, normal[k]) for k in changing)
            near = [k for k in changing if dist(c, normal[k]) <= best + 30]
            i = max(near, key=lambda k: (usage.get(k, 0), -dist(c, normal[k])))
            if dist(c, normal[i]) > 160:
                new.append(c)
                continue
            new.append(quant(tuple(s + (f - n) for s, f, n in zip(shiny[i], c, normal[i]))))
        write_pal('graphics/object_events/palettes/followers_shiny/%s.pal' % name, new)
        sym = 'sFollowerShinyPal_' + fpal[len('sFollowerPal_'):]
        out_decl.append('static const u16 %s[] = INCBIN_U16("graphics/object_events/palettes/followers_shiny/%s.gbapal");' % (sym, name))
        done[name] = sym
    out_table.append('    [%s] = %s,' % (species, done[name]))

with open('src/data/object_events/follower_shiny_palettes.h', 'w') as f:
    f.write('// Shiny overworld palettes for follower / overworld Pokemon sprites.\n')
    f.write('// Generated from the battle palettes by scripts/make_shiny_follower_pals.py; NULL = use the normal one.\n\n')
    f.write('\n'.join(out_decl) + '\n\n')
    f.write('static const u16 *const sFollowerShinyPalettes[FOLLOWER_GFX_COUNT] = {\n' + '\n'.join(out_table) + '\n};\n')
print(len(done), 'shiny palettes')
