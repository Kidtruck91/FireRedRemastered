"""Draw the VOLTORB FLIP tiles.

VOLTORB FLIP is a DS game. There is no GBA decomp to take graphics from, so
these are drawn here rather than ported, which is also why this script lives in
the repo: the PNGs it writes are committed, but they are generated, and this is
the source they are generated from.

Everything is one indexed PNG per sheet with an explicit sixteen-colour palette,
because gbagfx reads the palette straight off the file.

    cards.png   16px wide, nine cards tall. Each card is 16x16, which gbagfx
                splits into four 8x8 tiles in the order top-left, top-right,
                bottom-left, bottom-right -- so card n occupies tiles 4n to
                4n+3 and a 2x2 tilemap entry is (4n, 4n+1) over (4n+2, 4n+3).
                Order: back, 1, 2, 3, VOLTORB, then the four memo marks.
    cursor.png  one 16x16 sprite.

    python3 docs/orange-islands/make_voltorb_flip_gfx.py
"""
import os

from PIL import Image

REPO = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
OUT = os.path.join(REPO, 'graphics/voltorb_flip')

# index 0 is transparent to gbagfx, so nothing is ever drawn in it
PALETTE = [
    (255, 0, 255),      # 0  transparent
    (16, 16, 24),       # 1  outline
    (56, 80, 144),      # 2  card back, body
    (96, 128, 200),     # 3  card back, highlight
    (248, 248, 248),    # 4  white
    (248, 240, 208),    # 5  card face, body
    (200, 184, 144),    # 6  card face, shade
    (216, 72, 56),      # 7  VOLTORB red
    (152, 40, 32),      # 8  VOLTORB red, dark
    (160, 160, 168),    # 9  grey
    (72, 120, 216),     # 10 the 1
    (240, 176, 48),     # 11 the 2
    (88, 184, 112),     # 12 the 3
    (128, 144, 184),    # 13 memo ink
    (248, 232, 96),     # 14 cursor
    (64, 64, 80),       # 15 shade
]

# Five-by-seven glyphs, drawn big. '.' is the card body, '#' the ink.
DIGITS = {
    1: ["..#..",
        ".##..",
        "..#..",
        "..#..",
        "..#..",
        "..#..",
        ".###."],
    2: [".###.",
        "#...#",
        "....#",
        "..##.",
        ".#...",
        "#....",
        "#####"],
    3: [".###.",
        "#...#",
        "....#",
        "..##.",
        "....#",
        "#...#",
        ".###."],
}


def new_card():
    """A blank 16x16 card in the face colours, with an outline and a shade."""
    im = Image.new('P', (16, 16), 5)
    px = im.load()
    for x in range(16):
        px[x, 0] = px[x, 15] = 1
    for y in range(16):
        px[0, y] = px[15, y] = 1
    for x in range(2, 15):
        px[x, 14] = 6
    for y in range(2, 15):
        px[14, y] = 6
    for c in ((0, 0), (15, 0), (0, 15), (15, 15)):
        px[c] = 0                      # clipped corners, so cards read as cards
    return im


def card_back():
    im = Image.new('P', (16, 16), 2)
    px = im.load()
    for x in range(16):
        px[x, 0] = px[x, 15] = 1
    for y in range(16):
        px[0, y] = px[15, y] = 1
    for x in range(3, 13):
        px[x, 3] = px[x, 12] = 3
    for y in range(3, 13):
        px[3, y] = px[12, y] = 3
    px[7, 7] = px[8, 7] = px[7, 8] = px[8, 8] = 3
    for c in ((0, 0), (15, 0), (0, 15), (15, 15)):
        px[c] = 0
    return im


def stamp(im, glyph, x0, y0, ink):
    px = im.load()
    for dy, row in enumerate(glyph):
        for dx, ch in enumerate(row):
            if ch == '#':
                px[x0 + dx, y0 + dy] = ink


def digit_card(n):
    im = new_card()
    # a 5x7 glyph doubled to 10x14 reads clearly at this size
    big = [''.join(ch * 2 for ch in row) for row in DIGITS[n] for _ in (0, 1)]
    ink = {1: 10, 2: 11, 3: 12}[n]
    px = im.load()
    for dy, row in enumerate(big):
        for dx, ch in enumerate(row):
            if ch == '#':
                px[3 + dx, 1 + dy] = ink
    return im


def voltorb_card():
    """A VOLTORB: red top, white bottom, a black band and one highlight."""
    im = new_card()
    px = im.load()
    cx, cy, r = 7.5, 8.0, 5.6
    for y in range(16):
        for x in range(16):
            dx, dy = x - cx, y - cy
            if dx * dx + dy * dy > r * r:
                continue
            if abs(dy) < 0.9:
                px[x, y] = 1                       # the band
            elif dy < 0:
                px[x, y] = 7 if dy > -3.4 else 8   # red, darker at the crown
            else:
                px[x, y] = 4
    px[5, 5] = px[6, 5] = 4                        # the glint
    return im


def memo_card(kind):
    """A face-down card with a pencil mark on it."""
    im = card_back()
    px = im.load()
    if kind == 'voltorb':
        for y in range(5, 11):
            for x in range(5, 11):
                dx, dy = x - 7.5, y - 7.5
                if dx * dx + dy * dy <= 7.5:
                    px[x, y] = 13
    else:
        stamp(im, DIGITS[kind], 5, 4, 13)
    return im


def cursor():
    """A bracket frame, so it reads over any card."""
    im = Image.new('P', (16, 16), 0)
    px = im.load()
    for i in range(6):
        for (x, y) in ((i, 0), (0, i), (15 - i, 0), (15, i),
                       (i, 15), (0, 15 - i), (15 - i, 15), (15, 15 - i)):
            px[x, y] = 14
    for i in range(5):
        for (x, y) in ((i, 1), (1, i), (15 - i, 1), (14, i),
                       (i, 14), (1, 15 - i), (15 - i, 14), (14, 15 - i)):
            px[x, y] = 1
    return im


def main():
    os.makedirs(OUT, exist_ok=True)
    flat = [c for rgb in PALETTE for c in rgb] + [0] * (768 - 48)

    cards = [card_back(), digit_card(1), digit_card(2), digit_card(3), voltorb_card(),
             memo_card(1), memo_card(2), memo_card(3), memo_card('voltorb')]
    sheet = Image.new('P', (16, 16 * len(cards)), 0)
    for i, c in enumerate(cards):
        sheet.paste(c, (0, i * 16))
    sheet.putpalette(flat)
    sheet.save(os.path.join(OUT, 'cards.png'))

    cur = cursor()
    cur.putpalette(flat)
    cur.save(os.path.join(OUT, 'cursor.png'))
    print('  wrote cards.png (%d cards, %d tiles) and cursor.png'
          % (len(cards), len(cards) * 4))


if __name__ == '__main__':
    main()
