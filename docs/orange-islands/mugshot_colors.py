"""Render every MUGSHOT_COLOR_* as a labelled swatch strip, so a colour can be
picked by eye instead of by name.

The mugshot runs on BG palette 15. The opponent's palette is loaded whole, then
entries 10-15 are overwritten by the player palette -- so only entries 0-9 of a
MUGSHOT_COLOR_* ever reach the screen, and 10-15 are shown here greyed out to
make that obvious. Entry 0 is the transparent slot.
"""
import re
import struct
import sys

from PIL import Image, ImageDraw

REPO = '/home/claude/fr2'
SRC = REPO + '/src/battle_transition.c'

SW = 34          # swatch size
LABEL_W = 230
ROW_H = SW + 8


def load_pal(path):
    d = open(REPO + '/' + path, 'rb').read()
    out = []
    for i in range(16):
        c = struct.unpack_from('<H', d, i * 2)[0]
        out.append(((c & 31) * 8, ((c >> 5) & 31) * 8, ((c >> 10) & 31) * 8))
    return out


def main():
    s = open(SRC).read()
    # symbol -> file
    files = dict(re.findall(
        r'static const u16 (sMugshotPal_\w+)\[\] = INCBIN_U16\("([^"]+)"\);', s))
    # enum entry -> symbol, plus the author's own note on the same line
    table = s[s.index('sOpponentMugshotsPals[MUGSHOT_COLOR_COUNT]'):]
    table = table[:table.index('};')]
    rows = []
    for m in re.finditer(r'\[(MUGSHOT_COLOR_\w+)\]\s*=\s*(sMugshotPal_\w+),?\s*(//[^\n]*)?', table):
        name, sym, note = m.group(1), m.group(2), (m.group(3) or '')
        rows.append((name, files[sym], note.lstrip('/ ').strip()))

    # keep the enum's own order
    order = {}
    for m in re.finditer(r'(MUGSHOT_COLOR_\w+)', open(REPO + '/include/battle_transition.h').read()):
        order.setdefault(m.group(1), len(order))
    rows.sort(key=lambda r: order.get(r[0], 999))

    W = LABEL_W + 16 * SW + 20
    H = ROW_H * len(rows) + 46
    img = Image.new('RGB', (W, H), (24, 24, 28))
    d = ImageDraw.Draw(img)
    d.text((10, 8), 'MUGSHOT_COLOR_*  -  entries 0-9 are the opponent half; '
                    '10-15 are replaced by the player palette',
           fill=(240, 240, 240))
    for i in range(16):
        d.text((LABEL_W + i * SW + SW // 2 - 4, 26),
               '%d' % i, fill=(150, 150, 150))

    for r, (name, path, note) in enumerate(rows):
        y = 40 + r * ROW_H
        short = name[len('MUGSHOT_COLOR_'):]
        d.text((10, y + 4), short, fill=(255, 220, 120))
        if note:
            d.text((10, y + 17), note[:34], fill=(130, 130, 140))
        pal = load_pal(path)
        for i, c in enumerate(pal):
            x = LABEL_W + i * SW
            img.paste(Image.new('RGB', (SW - 2, SW - 2), c), (x, y))
            if i >= 10:
                # dim the slots the player palette overwrites
                veil = Image.new('RGBA', (SW - 2, SW - 2), (24, 24, 28, 170))
                base = img.crop((x, y, x + SW - 2, y + SW - 2)).convert('RGBA')
                img.paste(Image.alpha_composite(base, veil).convert('RGB'), (x, y))
        d.rectangle([LABEL_W - 2, y - 2, LABEL_W + 10 * SW - 2, y + SW - 2],
                    outline=(255, 255, 255))

    out = sys.argv[1] if len(sys.argv) > 1 else 'mugshot_colors.png'
    img.save(out)
    print(out, img.size, len(rows), 'colours')


if __name__ == '__main__':
    main()
