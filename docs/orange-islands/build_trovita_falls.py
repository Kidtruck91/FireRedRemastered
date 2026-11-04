"""Extend Trovita Falls to 48x96 and put three waterfall tiers in the climb.

The falls were 48x56: a channel from the Trovita entrance at the bottom up to the Gym
at (31,19), with nothing in the way. This adds forty rows below the old map -- the old
rows 0-55 are untouched -- so the entrance moves to the new bottom and the player climbs
north through three waterfalls to reach the Gym.

Nothing scripts the lock. A metatile whose behaviour is MB_WATERFALL is gated by the
engine itself: field_control_avatar.c only lets the player up it when FLAG_BADGE07_GET
is set and they are surfing north, and otherwise runs EventScript_CantUseWaterfall.
Standing on one pushes the player back south (ForcedMovement_PushedSouthByCurrent), so
coming back down is free.

The channel and its cliff walls are not invented: rows 54 and 55 of the existing map are
taken as the even/odd templates and repeated, which keeps the alternating cliff tiles in
phase. Forty is even, so that phase is preserved. A waterfall row is the same template
with the channel columns replaced; the stack reads 127 at the top (repeatable), then
12F, then 137 at the base, which is how TOJO_FALLS builds one on the same tileset.

    python3 docs/orange-islands/build_trovita_falls.py
"""
import json
import os
import struct

REPO = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
LAYOUT = 'LAYOUT_ORANGE_ISLANDS_TROVITA_FALLS'
OLD_H, NEW_H = 56, 96
CHANNEL = range(19, 26)          # the water columns, x=19..25
WF_TOP, WF_MID, WF_BASE = 0x127, 0x12F, 0x137
WATER = 0x1 << 12                # collision 0, elevation 1

# (first row, last row) of each tier, north to south. Between them are open pools.
TIERS = [(64, 67), (76, 79), (84, 87)]
ENTRY_ROW = 89                   # where Trovita lands the player: last row with no border
EXIT_ROW = NEW_H - 1             # the return trigger, on the bottom edge


def main():
    lp = os.path.join(REPO, 'data/layouts/layouts.json')
    doc = json.load(open(lp))
    layout = next(l for l in doc['layouts'] if l.get('id') == LAYOUT)
    w = layout['width']
    assert layout['height'] == OLD_H, 'expected the old %d-row map, found %d' % (OLD_H, layout['height'])

    path = os.path.join(REPO, layout['blockdata_filepath'])
    old = open(path, 'rb').read()
    assert len(old) == w * OLD_H * 2
    cell = lambda x, y: struct.unpack_from('<H', old, 2 * (y * w + x))[0]

    # the two template rows, taken from the plain stretch of channel at the old bottom
    template = {p: [cell(x, OLD_H - 2 + p) for x in range(w)] for p in (0, 1)}
    assert (OLD_H - 2) % 2 == 0, 'template parity'

    rows = [[cell(x, y) for x in range(w)] for y in range(OLD_H)]

    def tier_tile(y, a, b):
        """127 for every row but the last two, then 12F, then 137 at the base."""
        if y == b:
            return WF_BASE
        if y == b - 1:
            return WF_MID
        return WF_TOP

    for y in range(OLD_H, NEW_H):
        row = list(template[y % 2])
        for a, b in TIERS:
            if a <= y <= b:
                for x in CHANNEL:
                    row[x] = tier_tile(y, a, b) | WATER
                break
        rows.append(row)

    out = b''.join(struct.pack('<H', rows[y][x]) for y in range(NEW_H) for x in range(w))
    open(path, 'wb').write(out)
    layout['height'] = NEW_H
    with open(lp, 'w', newline='\n') as f:
        json.dump(doc, f, indent=2); f.write('\n')
    print('  %s is now %dx%d (%d bytes)' % (LAYOUT, w, NEW_H, len(out)))
    print('  tiers at rows %s' % ', '.join('%d-%d' % t for t in TIERS))

    # --- move the way out to the new bottom ----------------------------------------
    mp = os.path.join(REPO, 'data/maps/OrangeIslands_Trovita_Falls/map.json')
    mj = json.load(open(mp))
    kept = [c for c in mj['coord_events'] if 'WalkToTrovita' not in (c.get('script') or '')]
    for x in CHANNEL:
        kept.append({"type": "trigger", "x": x, "y": EXIT_ROW, "elevation": 0,
                     "var": "VAR_TEMP_0", "var_value": "0",
                     "script": "OrangeIslands_Trovita_Falls_EventScript_WalkToTrovita"})
    mj['coord_events'] = kept
    with open(mp, 'w', newline='\n') as f:
        json.dump(mj, f, indent=2); f.write('\n')
    print('  return triggers moved to row %d (x %d-%d)' % (EXIT_ROW, CHANNEL[0], CHANNEL[-1]))

    # --- and land the player below every tier ---------------------------------------
    sp = os.path.join(REPO, 'data/maps/OrangeIslands_Trovita/scripts.inc')
    s = open(sp, encoding='utf-8').read()
    old_warp = 'warp MAP_ORANGE_ISLANDS_TROVITA_FALLS, 22, 48'
    new_warp = 'warp MAP_ORANGE_ISLANDS_TROVITA_FALLS, 22, %d' % ENTRY_ROW
    if old_warp in s:
        s = s.replace(old_warp, new_warp)
        open(sp, 'w', encoding='utf-8', newline='\n').write(s)
    print('  Trovita now lands the player at (22,%d), south of every tier' % ENTRY_ROW)


if __name__ == '__main__':
    main()
