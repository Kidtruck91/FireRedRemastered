"""Simulate the mugshot battle intro and write it out as an animated GIF, so a
MUGSHOT_COLOR_* can be judged in motion instead of from a swatch.

This follows src/battle_transition.c rather than approximating it: the same
banner tiles and tilemap, the same per-scanline window and the same frame
counts, so the timing matches what the GBA does.

    BG0 holds graphics/battle_transitions/mugshot_banner.4bpp arranged by
    vsbar_tilemap.bin, every tile on palette 15. HBlankCB_Mugshots gives the
    top half of the screen one horizontal scroll and the bottom half another,
    moving 8px per frame in opposite directions -- that is the diagonal drift.
    WIN0 is what reveals it: winOut does not include BG0, so the banner is
    only drawn inside the window, and Mugshot_ShowBanner walks the window open
    with a sine wobble down its edge.

    Palette 15 is the opponent's MUGSHOT_COLOR_*, except entries 10-15, which
    Mugshot_SetGfx overwrites with the player palette. Only entries 0-9 of a
    mugshot colour ever reach the screen, and in practice only 5-9 differ
    between colours -- 0-4 are the same structural greys in every one of them
    except STARS.

The trainer sprites are not drawn. They sit on top of the banner and use their
own palettes, so they tell you nothing about the colour being tuned -- but
their slide is still simulated, because its length decides how long the banner
drifts for.

Examples
--------
    # one colour, as the game plays it
    python3 docs/orange-islands/mugshot_preview.py SILVER_YELLOW

    # against a female player, and stop before the white-out
    python3 docs/orange-islands/mugshot_preview.py RED_ORANGE --player FEMALE --skip-fade

    # what happens if two of the five colours trade places
    python3 docs/orange-islands/mugshot_preview.py SILVER_YELLOW --swap 6,8

    # try a colour without editing the .pal first
    python3 docs/orange-islands/mugshot_preview.py SILVER_YELLOW --entry 7=F8C000

    # a still of every colour at the same moment, for comparing
    python3 docs/orange-islands/mugshot_preview.py all --sheet sheet.png
"""
import argparse
import math
import os
import re
import struct
import sys

from PIL import Image

REPO = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
SRC = os.path.join(REPO, 'src/battle_transition.c')

WIDTH, HEIGHT = 240, 160
BG_W = 256          # BG0 is 32 tiles wide, and the scroll wraps at 256


# --------------------------------------------------------------------------
# the game's own data
# --------------------------------------------------------------------------

def sine_table():
    """gSineTable: sin scaled to 8 bits, which is what Sin() indexes."""
    return [int(round(math.sin(i * math.pi / 128) * 256)) for i in range(256)]


SINE = sine_table()


def Sin(index, amplitude):
    return (SINE[index & 0xFF] * amplitude) >> 8


def read_palettes():
    """MUGSHOT_COLOR_* -> 16 RGB tuples, plus the player palettes."""
    s = open(SRC).read()
    files = dict(re.findall(
        r'static const u16 (sMugshotPal_\w+)\[\] = INCBIN_U16\("([^"]+)"\);', s))

    def load(path):
        d = open(os.path.join(REPO, path), 'rb').read()
        out = []
        for i in range(16):
            c = struct.unpack_from('<H', d, i * 2)[0]
            out.append(((c & 31) * 8, ((c >> 5) & 31) * 8, ((c >> 10) & 31) * 8))
        return out

    def table(decl):
        body = s[s.index(decl):]
        body = body[:body.index('};')]
        return {m.group(1): load(files[m.group(2)]) for m in
                re.finditer(r'\[(\w+)\]\s*=\s*(sMugshotPal_\w+)', body)}

    opponents = table('sOpponentMugshotsPals[MUGSHOT_COLOR_COUNT]')
    players = table('sPlayerPalettes[PLAYER_PALETTES_COUNT]')
    return opponents, players


def read_banner():
    """The 4bpp tiles and the tilemap BG0 is filled with."""
    gfx = open(os.path.join(REPO, 'graphics/battle_transitions/mugshot_banner.4bpp'), 'rb').read()
    tiles = []
    for t in range(len(gfx) // 32):
        px = []
        for row in range(8):
            for b in range(4):
                byte = gfx[t * 32 + row * 4 + b]
                px.append(byte & 0xF)
                px.append(byte >> 4)
        tiles.append(px)
    raw = open(os.path.join(REPO, 'graphics/battle_transitions/vsbar_tilemap.bin'), 'rb').read()
    tilemap = [struct.unpack_from('<H', raw, i * 2)[0] for i in range(len(raw) // 2)]
    return tiles, tilemap


def build_bg_indices(tiles, tilemap):
    """Flatten BG0 into a 256x160 grid of palette indices, once."""
    grid = [[0] * BG_W for _ in range(HEIGHT)]
    for row in range(20):
        for col in range(32):
            entry = tilemap[row * 32 + col]
            tile, hflip, vflip = entry & 0x3FF, entry & 0x400, entry & 0x800
            if tile >= len(tiles):
                continue
            px = tiles[tile]
            for y in range(8):
                sy = (7 - y) if vflip else y
                for x in range(8):
                    sx = (7 - x) if hflip else x
                    grid[row * 8 + y][col * 8 + x] = px[sy * 8 + sx]
    return grid


# --------------------------------------------------------------------------
# the transition, frame by frame
# --------------------------------------------------------------------------

class Slide:
    """One trainer pic's slide. Only its duration is used."""

    def __init__(self, direction):
        self.dir = direction
        self.speed = 12 if direction == 0 else -12
        self.accel = -1 if direction == 0 else 1
        self.x = -32 if direction == 0 else WIDTH + 32
        self.state = 'slide'
        self.done = False

    def step(self):
        if self.state == 'slide':
            self.x += self.speed
            if (self.dir and self.x < WIDTH - 107) or (not self.dir and self.x > 103):
                self.state = 'slow'
        elif self.state == 'slow':
            self.speed += self.accel
            self.x += self.speed
            if self.speed == 0:
                self.state = 'done'
                self.done = True


def simulate(skip_fade=False):
    """Yields one (win, hofs_top, hofs_bottom, bldy, mode) per frame.

    win     per-scanline (left, right) of WIN0
    bldy    per-scanline blend strength, 0-16
    mode    'lighten' while the banner plays, 'darken' for the final fade
    """
    hofs_top = 0        # bg0HOfsOpponent
    hofs_bottom = 0     # bg0HOfsPlayer
    full = [(0, WIDTH)] * HEIGHT
    nofade = [0] * HEIGHT

    # --- Mugshot_ShowBanner: walk WIN0 open from both edges -----------------
    sin_index = 0
    top_x, bottom_x = 1, WIDTH - 1
    while True:
        win = []
        idx = sin_index
        sin_index = (sin_index + 16) & 0xFFFF
        for i in range(HEIGHT):
            idx = (idx + 16) & 0xFFFF
            if i < HEIGHT // 2:
                x = top_x + Sin(idx, 16)
                x = 1 if x < 0 else (WIDTH if x > WIDTH else x)
                win.append((0, x))
            else:
                x = bottom_x - Sin(idx, 16)
                x = 0 if x < 0 else (WIDTH - 1 if x > WIDTH - 1 else x)
                win.append((x, WIDTH))
        top_x = min(top_x + 8, WIDTH)
        bottom_x = max(bottom_x - 8, 0)
        hofs_top -= 8
        hofs_bottom += 8
        yield win, hofs_top, hofs_bottom, nofade, 'lighten'
        if top_x == WIDTH and bottom_x == 0:
            break

    # --- the slides: window wide open, banner still drifting ----------------
    hofs_top -= 8
    hofs_bottom += 8
    opponent, player = Slide(0), Slide(1)
    yield full, hofs_top, hofs_bottom, nofade, 'lighten'
    while not opponent.done:
        opponent.step()
        hofs_top -= 8
        hofs_bottom += 8
        yield full, hofs_top, hofs_bottom, nofade, 'lighten'
    while not player.done:
        player.step()
        hofs_top -= 8
        hofs_bottom += 8
        yield full, hofs_top, hofs_bottom, nofade, 'lighten'

    if skip_fade:
        return

    # --- Mugshot_GradualWhiteFade: whiten outwards from the middle ----------
    bldy = [0] * HEIGHT
    timer = 0
    spread = 0
    while True:
        hofs_top -= 8
        hofs_bottom += 8
        spread = min(spread + 2, HEIGHT // 2)
        timer += 1
        active = True
        if timer & 1:
            active = False
            for i in range(spread + 1):
                for y in (HEIGHT // 2 - i, HEIGHT // 2 + i):
                    if 0 <= y < HEIGHT and bldy[y] <= 15:
                        active = True
                        bldy[y] += 1
        yield full, hofs_top, hofs_bottom, list(bldy), 'lighten'
        if spread == HEIGHT // 2 and not active:
            break

    # --- white, then down to black ------------------------------------------
    for timer in range(1, 17):
        yield full, hofs_top, hofs_bottom, [timer] * HEIGHT, 'darken'


# --------------------------------------------------------------------------
# drawing
# --------------------------------------------------------------------------

BACKDROP = (40, 40, 48)   # stands in for the overworld still behind the banner


def render(frame, bg, palette, backdrop=BACKDROP, bgimg=None):
    win, hofs_top, hofs_bottom, bldy, mode = frame
    img = Image.new('RGB', (WIDTH, HEIGHT))
    px = img.load()
    behind = bgimg.load() if bgimg is not None else None
    for y in range(HEIGHT):
        left, right = win[y]
        hofs = hofs_top if y < HEIGHT // 2 else hofs_bottom
        row = bg[y]
        b = bldy[y]
        for x in range(WIDTH):
            under = behind[x, y] if behind is not None else backdrop
            if left <= x < right:
                i = row[(x + hofs) % BG_W]
                # colour 0 of a 4bpp BG tile is transparent, which is why
                # entry 0 is the same unused red in every mugshot palette
                c = under if i == 0 else palette[i]
            else:
                c = under
            if b:
                if mode == 'lighten':
                    c = tuple(v + ((255 - v) * b) // 16 for v in c)
                else:
                    # the palette was already blended to white at this point
                    c = tuple(255 - (255 * b) // 16 for _ in c)
            px[x, y] = c
    return img


def apply_edits(pal, swaps, order, entries):
    pal = list(pal)
    if order:
        if sorted(order) != list(range(16)):
            sys.exit('--order needs all 16 indices exactly once')
        pal = [pal[i] for i in order]
    for a, b in swaps:
        pal[a], pal[b] = pal[b], pal[a]
    for i, rgb in entries:
        pal[i] = rgb
    return pal


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('color', help="a MUGSHOT_COLOR_* (the prefix is optional), "
                                  "'all', or 'list' to see them")
    ap.add_argument('--player', default='MALE',
                    help='PLAYER_PALETTE_* for entries 10-15 (default MALE)')
    ap.add_argument('--swap', action='append', default=[], metavar='A,B',
                    help='swap two palette entries; may be repeated')
    ap.add_argument('--order', metavar='LIST',
                    help='full 16-entry permutation, e.g. 0,1,2,3,4,9,8,7,6,5,...')
    ap.add_argument('--entry', action='append', default=[], metavar='I=RRGGBB',
                    help='override one entry with a hex colour; may be repeated')
    ap.add_argument('--out', help='output file (default <color>.gif)')
    ap.add_argument('--scale', type=int, default=3)
    ap.add_argument('--skip-fade', action='store_true',
                    help='stop after the slides, before the white-out')
    ap.add_argument('--sheet', metavar='FILE',
                    help='write one still per colour instead of an animation')
    ap.add_argument('--at', type=int, default=45,
                    help='frame to use for --sheet (default 45)')
    ap.add_argument('--bg', metavar='FILE',
                    help='a 240x160 screenshot to show through the transparent '
                         'parts, instead of a flat backdrop')
    args = ap.parse_args()

    opponents, players = read_palettes()
    if args.color == 'list':
        for k in opponents:
            print(k)
        return

    tiles, tilemap = read_banner()
    bg = build_bg_indices(tiles, tilemap)
    bgimg = None
    if args.bg:
        bgimg = Image.open(args.bg).convert('RGB').resize((WIDTH, HEIGHT))

    pkey = args.player if args.player.startswith('PLAYER_PALETTE_') else 'PLAYER_PALETTE_' + args.player
    if pkey not in players:
        sys.exit('unknown player palette %s; have %s' % (pkey, ', '.join(players)))
    player_pal = players[pkey]

    swaps = []
    for s in args.swap:
        a, b = (int(v) for v in s.split(','))
        swaps.append((a, b))
    order = [int(v) for v in args.order.split(',')] if args.order else None
    entries = []
    for e in args.entry:
        i, hexv = e.split('=')
        hexv = hexv.lstrip('#')
        entries.append((int(i), tuple(int(hexv[k:k + 2], 16) for k in (0, 2, 4))))

    for a, b in swaps:
        if a >= 10 or b >= 10:
            print('  note: entries 10-15 are overwritten by the player palette, '
                  'so swapping %d,%d will not change anything' % (a, b))
    for i, _ in entries:
        if i >= 10:
            print('  note: entry %d is overwritten by the player palette' % i)

    def palette_for(name):
        # Mugshot_SetGfx loads the opponent palette whole, then overwrites
        # 10-15 with the first six colours of the player palette.
        pal = apply_edits(opponents[name], swaps, order, entries)
        return pal[:10] + player_pal[:6]

    if args.sheet:
        names = list(opponents) if args.color == 'all' else [resolve(args.color, opponents)]
        frames = {}
        for n in names:
            seq = list(simulate(skip_fade=True))
            f = seq[min(args.at, len(seq) - 1)]
            frames[n] = render(f, bg, palette_for(n), bgimg=bgimg)
        cols = 4
        rows = (len(frames) + cols - 1) // cols
        pad, lab = 6, 14
        W = cols * (WIDTH + pad) + pad
        H = rows * (HEIGHT + pad + lab) + pad
        sheet = Image.new('RGB', (W, H), (20, 20, 24))
        from PIL import ImageDraw
        d = ImageDraw.Draw(sheet)
        for i, (n, im) in enumerate(frames.items()):
            x = pad + (i % cols) * (WIDTH + pad)
            y = pad + (i // cols) * (HEIGHT + pad + lab)
            d.text((x, y), n[len('MUGSHOT_COLOR_'):], fill=(255, 220, 120))
            sheet.paste(im, (x, y + lab))
        sheet.save(args.sheet)
        print('%s  %dx%d  %d colours at frame %d' % (args.sheet, W, H, len(frames), args.at))
        return

    name = resolve(args.color, opponents)
    pal = palette_for(name)
    imgs = [render(f, bg, pal, bgimg=bgimg) for f in simulate(skip_fade=args.skip_fade)]
    if args.scale != 1:
        imgs = [im.resize((WIDTH * args.scale, HEIGHT * args.scale), Image.NEAREST)
                for im in imgs]
    out = args.out or (name[len('MUGSHOT_COLOR_'):].lower() + '.gif')
    imgs[0].save(out, save_all=True, append_images=imgs[1:], duration=17, loop=0)
    print('%s  %d frames  %dx%d  (%.1f s at 60fps)'
          % (out, len(imgs), imgs[0].width, imgs[0].height, len(imgs) / 60.0))


def resolve(name, opponents):
    key = name if name.startswith('MUGSHOT_COLOR_') else 'MUGSHOT_COLOR_' + name.upper()
    if key not in opponents:
        sys.exit('unknown colour %s; run with "list" to see them all' % name)
    return key


if __name__ == '__main__':
    main()
