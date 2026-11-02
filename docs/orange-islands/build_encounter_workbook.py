"""Build the Orange Islands encounter workbook: every map, its picture, and its wild
Pokemon split by the kind of encounter.

Everything is read from the repo, so re-running this after editing
src/data/wild_encounters.json or any map rebuilds the book to match. Nothing here is
typed in by hand except the island themes and habitat pools, which are copied from
docs/orange-islands/encounter-plan.md and marked as such on the Reference sheet.

    python3 docs/orange-islands/render_maps.py            # pictures first, if maps changed
    python3 docs/orange-islands/build_encounter_workbook.py [out.xlsx]

A species' real rate is the sum of the engine slots it holds, so the rows are one per
species per table with the slots it occupies listed. The slot rates are fixed by the
engine and read out of src/data/wild_encounters.h rather than assumed.
"""
import json
import os
import re
import sys
from collections import OrderedDict, defaultdict

from PIL import Image
from openpyxl import Workbook
from openpyxl.drawing.image import Image as XLImage
from openpyxl.styles import Alignment, Border, Font, PatternFill, Side
from openpyxl.utils import get_column_letter

REPO = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
DOCS = os.path.join(REPO, 'docs/orange-islands')
THUMBS = os.path.join(DOCS, '__thumbs')
THUMB_W = 460          # px; the full renders are far too big to embed as they are

FONT = 'Arial'
HDR_FILL = PatternFill('solid', fgColor='1F3864')
BAND_FILL = PatternFill('solid', fgColor='EEF1F7')
EMPTY_FILL = PatternFill('solid', fgColor='FFF2CC')
TYPE_FILL = {
    'Grass':   'E2EFDA',
    'Beach':   'FFF2CC',
    'Cave':    'E7E6E6',
    'Surfing': 'DDEBF7',
    'Fishing': 'FCE4D6',
}
THIN = Side(style='thin', color='BFBFBF')
BOX = Border(left=THIN, right=THIN, top=THIN, bottom=THIN)

# Islands, in the order the player meets them. Everything else falls to Sea Routes.
ISLANDS = OrderedDict([
    ('Valencia', 'Valencia'),
    ('Mikan', 'Mikan'),
    ('Sunburst', 'Sunburst'),
    ('Navel', 'Navel'),
    ('Moro', 'Moro'),
    ('Kinnow', 'Kinnow'),
    ('Mandarin', 'Mandarin'),
    ('Trovita', 'Trovita'),
    ('Kumquat', 'Kumquat'),
    ('Pinkan', 'Pinkan'),
    ('Pummelo', 'Pummelo'),
])
SEA = 'Sea Routes'

# Copied from docs/orange-islands/encounter-plan.md.
THEMES = {
    'Valencia': ('welcoming tropical habitat', 'Oddish, Wingull, Aipom, Exeggcute'),
    'Mikan': ('warm beaches and reefs', 'Krabby, Staryu, Corsola, Horsea'),
    'Navel': ('mountain island', 'Geodude, Machop, Nosepass, Spheal/Snorunt up at the summit'),
    'Pinkan': ('protected wildlife sanctuary', 'Nidoran, Rhyhorn, Scyther'),
    'Moro': ('ruins and ghost ship', 'Gastly, Duskull, Shuppet, Sableye'),
    'Kinnow': ('lush woodland and fruit groves', 'Bellsprout, Aipom, Shroomish, Tropius'),
    'Kumquat': ('busy resort island', 'Meowth, Snubbull, Wingull, Luvdisc in the water'),
    'Trovita': ('wooded streams and waterfalls', 'Surskit, Marill, Lotad, Goldeen'),
    'Sunburst': ('glass-blowers and the crystal cave', 'cave pool: Zubat, Geodude, Whismur'),
    'Mandarin': ('city island', 'not planned yet'),
    'Pummelo': ('stadium island', 'not planned yet'),
    SEA: ('open sea between the islands', 'Tentacool, Wingull, Magikarp; Mantine and Lapras rare'),
}
POOLS = [
    ('Island woods', 'Oddish, Caterpie, Wurmple, Spinarak', 'Bellsprout, Exeggcute, Aipom, Shroomish', 'Heracross, Tropius, Kecleon, Pinsir'),
    ('Beach and shoreline', 'Wingull, Krabby, Zigzagoon', 'Slowpoke, Corphish, Taillow', "Shuckle, Farfetch'd, Exeggutor"),
    ('Open sea', 'Tentacool, Wingull, Magikarp', 'Horsea, Remoraid, Wailmer', 'Mantine, Lapras, Sharpedo'),
    ('Coastal caves', 'Zubat, Geodude, Whismur', 'Machop, Aron, Golbat', 'Onix, Nosepass, Dunsparce'),
]


# --------------------------------------------------------------------------------------
# the engine's slot rates, read from the header rather than assumed
# --------------------------------------------------------------------------------------
def slot_rates():
    """{'land': [...], 'water': [...], 'old'/'good'/'super': {slot: rate}} from the header."""
    text = open(os.path.join(REPO, 'src/data/wild_encounters.h')).read()
    cum = {}
    for name, body in re.findall(r'#define\s+(ENCOUNTER_CHANCE_\w+)\s+(.+)', text):
        body = body.strip()
        m = re.match(r'^(ENCOUNTER_CHANCE_\w+)\s*\+\s*(\d+)$', body)
        if m:
            cum[name] = cum[m.group(1)] + int(m.group(2))
        elif body.isdigit():
            cum[name] = int(body)

    def run(prefix, slots):
        out, prev = {}, 0
        for s in slots:
            total = cum['%s_SLOT_%d' % (prefix, s)]
            out[s] = total - prev
            prev = total
        return out

    return {
        'land': run('ENCOUNTER_CHANCE_LAND_MONS', range(12)),
        'water': run('ENCOUNTER_CHANCE_WATER_MONS', range(5)),
        'old': run('ENCOUNTER_CHANCE_FISHING_MONS_OLD_ROD', [0, 1]),
        'good': run('ENCOUNTER_CHANCE_FISHING_MONS_GOOD_ROD', [2, 3, 4]),
        'super': run('ENCOUNTER_CHANCE_FISHING_MONS_SUPER_ROD', [5, 6, 7, 8, 9]),
    }


# --------------------------------------------------------------------------------------
# the maps
# --------------------------------------------------------------------------------------
OUTDOOR = {'MAP_TYPE_TOWN', 'MAP_TYPE_CITY', 'MAP_TYPE_ROUTE', 'MAP_TYPE_OCEAN_ROUTE',
           'MAP_TYPE_UNDERGROUND', 'MAP_TYPE_UNDERWATER'}


def pretty(dirname):
    return dirname.removeprefix('OrangeIslands_').replace('_', ' ')


def island_of(dirname):
    stem = dirname.removeprefix('OrangeIslands_')
    head = stem.split('_')[0]
    if head in ('Route', 'Underwater'):
        return SEA
    return ISLANDS.get(head, SEA)


def land_kind(dirname, map_type):
    """Which bucket a map's land_mons table belongs in."""
    if dirname.endswith('_Beach'):
        return 'Beach'
    if map_type in ('MAP_TYPE_UNDERGROUND', 'MAP_TYPE_UNDERWATER', 'MAP_TYPE_INDOOR'):
        return 'Cave'
    if 'Cave' in dirname or 'Grotto' in dirname or 'GhostShip' in dirname:
        return 'Cave'
    return 'Grass'


def collect():
    layouts = {l['id']: l for l in json.load(
        open(os.path.join(REPO, 'data/layouts/layouts.json')))['layouts'] if 'id' in l}
    enc = {e['map']: e for e in json.load(
        open(os.path.join(REPO, 'src/data/wild_encounters.json')))
        ['wild_encounter_groups'][0]['encounters']}
    maps = []
    for name in sorted(os.listdir(os.path.join(REPO, 'data/maps'))):
        p = os.path.join(REPO, 'data/maps', name, 'map.json')
        if not os.path.exists(p):
            continue
        mj = json.load(open(p))
        if not mj['id'].startswith('MAP_ORANGE_ISLANDS'):
            continue
        has_table = mj['id'] in enc
        if mj.get('map_type') not in OUTDOOR and not has_table:
            continue
        layout = layouts.get(mj['layout'], {})
        lname = layout.get('name', '').removesuffix('_Layout')
        img = os.path.join(DOCS, lname + '.png')
        maps.append({
            'id': mj['id'],
            'dir': name,
            'name': pretty(name),
            'island': island_of(name),
            'map_type': mj.get('map_type', '').removeprefix('MAP_TYPE_').title(),
            'w': layout.get('width'), 'h': layout.get('height'),
            'image': img if os.path.exists(img) else None,
            'enc': enc.get(mj['id']),
        })
    return maps


def tables_for(m, rates):
    """[(type, group, step_rate, [(species, pct, [slots], minLv, maxLv)])] for one map."""
    out = []
    e = m['enc']
    if not e:
        return out

    def group(mons, slot_rate, label, grp, step, slot_offset=0):
        agg = OrderedDict()
        for i, mon in enumerate(mons):
            slot = i + slot_offset
            if slot not in slot_rate:
                continue
            sp = mon['species'].removeprefix('SPECIES_').title()
            a = agg.setdefault(sp, {'pct': 0, 'slots': [], 'lo': 99, 'hi': 0})
            a['pct'] += slot_rate[slot]
            a['slots'].append(slot)
            a['lo'] = min(a['lo'], mon['min_level'])
            a['hi'] = max(a['hi'], mon['max_level'])
        rows = [(sp, a['pct'], a['slots'], a['lo'], a['hi']) for sp, a in agg.items()]
        rows.sort(key=lambda r: (-r[1], r[0]))
        if rows:
            out.append((label, grp, step, rows))

    if 'land_mons' in e:
        kind = land_kind(m['dir'], 'MAP_TYPE_' + m['map_type'].upper())
        group(e['land_mons']['mons'], rates['land'], kind, kind,
              e['land_mons']['encounter_rate'])
    if 'water_mons' in e:
        group(e['water_mons']['mons'], rates['water'], 'Surfing', 'Surfing',
              e['water_mons']['encounter_rate'])
    if 'fishing_mons' in e:
        mons = e['fishing_mons']['mons']
        step = e['fishing_mons']['encounter_rate']
        group(mons[0:2], rates['old'], 'Fishing - Old Rod', 'Fishing', step, 0)
        group(mons[2:5], rates['good'], 'Fishing - Good Rod', 'Fishing', step, 2)
        group(mons[5:10], rates['super'], 'Fishing - Super Rod', 'Fishing', step, 5)
    return out


# --------------------------------------------------------------------------------------
# pictures
# --------------------------------------------------------------------------------------
def thumb(path):
    """A width-limited copy, so the book does not carry full-size renders."""
    os.makedirs(THUMBS, exist_ok=True)
    dest = os.path.join(THUMBS, os.path.basename(path))
    if not os.path.exists(dest) or os.path.getmtime(dest) < os.path.getmtime(path):
        im = Image.open(path)
        if im.width > THUMB_W:
            im = im.resize((THUMB_W, max(1, round(im.height * THUMB_W / im.width))),
                           Image.Resampling.NEAREST)
        im.save(dest)
    return dest


# --------------------------------------------------------------------------------------
# writing
# --------------------------------------------------------------------------------------
def head(ws, row, labels, widths=None):
    for i, text in enumerate(labels, start=1):
        c = ws.cell(row=row, column=i, value=text)
        c.font = Font(name=FONT, bold=True, color='FFFFFF', size=10)
        c.fill = HDR_FILL
        c.alignment = Alignment(vertical='center', wrap_text=True)
        c.border = BOX
    ws.row_dimensions[row].height = 28
    if widths:
        for i, w in enumerate(widths, start=1):
            ws.column_dimensions[get_column_letter(i)].width = w


def cell(ws, row, col, value, bold=False, fill=None, fmt=None, align=None, italic=False):
    c = ws.cell(row=row, column=col, value=value)
    c.font = Font(name=FONT, size=10, bold=bold, italic=italic)
    c.border = BOX
    if fill:
        c.fill = PatternFill('solid', fgColor=fill)
    if fmt:
        c.number_format = fmt
    if align:
        c.alignment = Alignment(horizontal=align)
    return c


def build(out_path):
    rates = slot_rates()
    maps = collect()
    wb = Workbook()

    # ---------------- Encounters: one row per species per table ----------------------
    ws = wb.active
    ws.title = 'Encounters'
    head(ws, 1,
         ['Island', 'Map', 'Map type', 'Encounter type', 'Type group', 'Species',
          'Rate %', 'Slots', 'Min Lv', 'Max Lv', 'Step rate'],
         [12, 26, 13, 19, 11, 15, 9, 11, 8, 8, 10])
    ws.freeze_panes = 'A2'
    r = 2
    first_data_row = r
    for m in maps:
        for label, grp, step, rows in tables_for(m, rates):
            for sp, pct, slots, lo, hi in rows:
                fill = TYPE_FILL.get(grp)
                cell(ws, r, 1, m['island'])
                cell(ws, r, 2, m['name'])
                cell(ws, r, 3, m['map_type'])
                cell(ws, r, 4, label, fill=fill)
                cell(ws, r, 5, grp, fill=fill)
                cell(ws, r, 6, sp, bold=True)
                cell(ws, r, 7, pct / 100.0, fmt='0.0%', align='center')
                cell(ws, r, 8, ', '.join(str(s) for s in slots), align='center')
                cell(ws, r, 9, lo, align='center')
                cell(ws, r, 10, hi, align='center')
                cell(ws, r, 11, step, align='center')
                r += 1
    last_data_row = r - 1
    ws.auto_filter.ref = 'A1:K%d' % last_data_row

    # ---------------- Overview: every map x type, implemented or not -----------------
    ov = wb.create_sheet('Overview')
    cell(ov, 1, 1, 'Orange Islands wild encounters', bold=True).font = Font(
        name=FONT, bold=True, size=14)
    cell(ov, 2, 1, 'Every Orange map that can hold wild Pokemon. A shaded row is a table '
                   'that does not exist yet. Rate totals are formulas over the Encounters '
                   'sheet and must each come to 100%.', italic=True)
    ov.merge_cells('A1:J1')
    ov.merge_cells('A2:J2')
    head(ov, 4,
         ['Island', 'Map', 'Map type', 'Size', 'Encounter type', 'Status',
          'Species', 'Rate total', 'Levels', 'Step rate'],
         [12, 26, 13, 10, 19, 14, 9, 11, 10, 10])
    ov.freeze_panes = 'A5'
    r = 5
    for m in maps:
        built = tables_for(m, rates)
        present = {lbl for lbl, _, _, _ in built}
        for label, grp, step, rows in built:
            fill = TYPE_FILL.get(grp)
            cell(ov, r, 1, m['island'])
            cell(ov, r, 2, m['name'])
            cell(ov, r, 3, m['map_type'])
            cell(ov, r, 4, '%sx%s' % (m['w'], m['h']), align='center')
            cell(ov, r, 5, label, fill=fill)
            cell(ov, r, 6, 'built')
            cell(ov, r, 7, len(rows), align='center')
            cell(ov, r, 8, '=SUMIFS(Encounters!$G$%d:$G$%d,Encounters!$B$%d:$B$%d,$B%d,'
                           'Encounters!$D$%d:$D$%d,$E%d)'
                 % (first_data_row, last_data_row, first_data_row, last_data_row, r,
                    first_data_row, last_data_row, r),
                 fmt='0%', align='center')
            cell(ov, r, 9, '%d-%d' % (min(x[3] for x in rows), max(x[4] for x in rows)),
                 align='center')
            cell(ov, r, 10, step, align='center')
            r += 1
        # the tables this map could have and has not
        wants = []
        kind = land_kind(m['dir'], 'MAP_TYPE_' + m['map_type'].upper())
        wants.append(kind)
        if m['map_type'] not in ('Underground', 'Underwater', 'Indoor'):
            wants += ['Surfing', 'Fishing - Old Rod', 'Fishing - Good Rod',
                      'Fishing - Super Rod']
        for label in wants:
            if label in present:
                continue
            cell(ov, r, 1, m['island'], fill='FFF2CC')
            cell(ov, r, 2, m['name'], fill='FFF2CC')
            cell(ov, r, 3, m['map_type'], fill='FFF2CC')
            cell(ov, r, 4, '%sx%s' % (m['w'], m['h']), fill='FFF2CC', align='center')
            cell(ov, r, 5, label, fill='FFF2CC')
            cell(ov, r, 6, 'not written yet', fill='FFF2CC', italic=True)
            for col in (7, 8, 9, 10):
                cell(ov, r, col, None, fill='FFF2CC')
            r += 1
    ov.auto_filter.ref = 'A4:J%d' % (r - 1)
    ov_last = r - 1

    # summary, as formulas over the rows just written
    r += 1
    cell(ov, r, 1, 'Summary', bold=True)
    r += 1
    head(ov, r, ['Encounter type', 'Built', 'Not written yet', 'Maps covered'])
    sr = r + 1
    for i, label in enumerate(['Grass', 'Beach', 'Cave', 'Surfing',
                               'Fishing - Old Rod', 'Fishing - Good Rod',
                               'Fishing - Super Rod']):
        rr = sr + i
        cell(ov, rr, 1, label, fill=TYPE_FILL.get(label.split(' -')[0]))
        cell(ov, rr, 2, '=COUNTIFS($E$5:$E$%d,$A%d,$F$5:$F$%d,"built")'
             % (ov_last, rr, ov_last), align='center')
        cell(ov, rr, 3, '=COUNTIFS($E$5:$E$%d,$A%d,$F$5:$F$%d,"not written yet")'
             % (ov_last, rr, ov_last), align='center')
        cell(ov, rr, 4, '=COUNTIF($E$5:$E$%d,$A%d)' % (ov_last, rr), align='center')
    tot = sr + 7
    cell(ov, tot, 1, 'Total', bold=True)
    for col in (2, 3, 4):
        L = get_column_letter(col)
        cell(ov, tot, col, '=SUM(%s%d:%s%d)' % (L, sr, L, tot - 1), bold=True,
             align='center')

    # ---------------- one sheet per island -------------------------------------------
    by_island = defaultdict(list)
    for m in maps:
        by_island[m['island']].append(m)
    for island in list(ISLANDS.values()) + [SEA]:
        group = by_island.get(island)
        if not group:
            continue
        sh = wb.create_sheet(island[:31])
        theme, sig = THEMES.get(island, ('', ''))
        cell(sh, 1, 1, island, bold=True).font = Font(name=FONT, bold=True, size=14)
        cell(sh, 2, 1, 'Theme: %s' % theme, italic=True)
        cell(sh, 3, 1, 'Signature: %s' % sig, italic=True)
        for i, w in enumerate([16, 16, 11, 11, 10, 10, 10], start=1):
            sh.column_dimensions[get_column_letter(i)].width = w
        sh.column_dimensions['A'].width = 66   # the pictures sit in column A
        r = 5
        for m in group:
            cell(sh, r, 1, m['name'], bold=True).font = Font(name=FONT, bold=True, size=12)
            cell(sh, r, 2, '%s  %sx%s blocks' % (m['map_type'], m['w'], m['h']), italic=True)
            r += 1
            img_bottom = r
            if m['image']:
                pic = XLImage(thumb(m['image']))
                sh.add_image(pic, 'A%d' % r)
                img_bottom = r + max(2, round(pic.height / 19.0)) + 1
            built = tables_for(m, rates)
            tr = r
            if not built:
                cell(sh, tr, 2, 'no wild encounters written for this map yet',
                     italic=True, fill='FFF2CC')
                tr += 1
            for label, grp, step, rows in built:
                cell(sh, tr, 2, label, bold=True, fill=TYPE_FILL.get(grp))
                cell(sh, tr, 3, 'step rate %d' % step, italic=True)
                tr += 1
                head(sh, tr, [''] * 1 + ['Species', 'Rate %', 'Slots', 'Min Lv', 'Max Lv'])
                sh.cell(row=tr, column=1).fill = PatternFill()
                sh.cell(row=tr, column=1).border = Border()
                tr += 1
                for sp, pct, slots, lo, hi in rows:
                    cell(sh, tr, 2, sp)
                    cell(sh, tr, 3, pct / 100.0, fmt='0.0%', align='center')
                    cell(sh, tr, 4, ', '.join(str(s) for s in slots), align='center')
                    cell(sh, tr, 5, lo, align='center')
                    cell(sh, tr, 6, hi, align='center')
                    tr += 1
                tr += 1
            r = max(img_bottom, tr) + 2

    # ---------------- Reference ------------------------------------------------------
    rf = wb.create_sheet('Reference')
    cell(rf, 1, 1, 'How to read this', bold=True).font = Font(name=FONT, bold=True, size=14)
    notes = [
        'Built by docs/orange-islands/build_encounter_workbook.py, straight from the repo. '
        'Re-run it after editing src/data/wild_encounters.json or any map and the book matches again.',
        'Slot rates are fixed by the engine, so a species real rate is the sum of the slots it '
        'holds. The rates below are read from src/data/wild_encounters.h, not assumed.',
        'Step rate (encounter_rate) is how often walking rolls at all, not which species comes up.',
        'Grass / Beach / Cave are all the same land_mons table in the ROM. Which bucket a map '
        'lands in is decided here: a *_Beach map is Beach, an underground, underwater or ship '
        'interior is Cave, anything else is Grass.',
        'The beaches are overworld-only maps (sOverworldOnlyMaps in src/data/overworld_mons.h): '
        'their land table feeds the wandering Pokemon you can see, and stepping never rolls one.',
        'Island themes and habitat pools are copied from docs/orange-islands/encounter-plan.md '
        'and are the plan, not what is in the ROM.',
    ]
    r = 2
    for n in notes:
        c = cell(rf, r, 1, '- ' + n)
        c.alignment = Alignment(wrap_text=True, vertical='top')
        rf.merge_cells(start_row=r, start_column=1, end_row=r, end_column=6)
        rf.row_dimensions[r].height = 30
        r += 1
    rf.column_dimensions['A'].width = 30
    for col in 'BCDEF':
        rf.column_dimensions[col].width = 26

    r += 1
    cell(rf, r, 1, 'Engine slot rates', bold=True)
    r += 1
    head(rf, r, ['Table', 'Slots', 'Rates %'])
    r += 1
    for label, key in [('land (Grass / Beach / Cave)', 'land'), ('water (Surf)', 'water'),
                       ('fishing - Old Rod', 'old'), ('fishing - Good Rod', 'good'),
                       ('fishing - Super Rod', 'super')]:
        d = rates[key]
        cell(rf, r, 1, label)
        cell(rf, r, 2, ', '.join(str(s) for s in sorted(d)))
        cell(rf, r, 3, ', '.join(str(d[s]) for s in sorted(d)))
        r += 1

    r += 1
    cell(rf, r, 1, 'Habitat pools (planned)', bold=True)
    r += 1
    head(rf, r, ['Habitat', 'Common', 'Uncommon', 'Rare'])
    r += 1
    for row in POOLS:
        for i, v in enumerate(row, start=1):
            cell(rf, r, i, v)
        r += 1

    r += 1
    cell(rf, r, 1, 'Island themes (planned)', bold=True)
    r += 1
    head(rf, r, ['Island', 'Theme', 'Signature'])
    r += 1
    for island in list(ISLANDS.values()) + [SEA]:
        theme, sig = THEMES.get(island, ('', ''))
        cell(rf, r, 1, island)
        cell(rf, r, 2, theme)
        cell(rf, r, 3, sig)
        r += 1

    wb.move_sheet('Overview', offset=-1)   # Overview first, Encounters behind it
    wb.save(out_path)
    print('wrote %s' % out_path)
    print('  %d maps, %d species rows' % (len(maps), last_data_row - first_data_row + 1))


if __name__ == '__main__':
    build(sys.argv[1] if len(sys.argv) > 1 else
          os.path.join(DOCS, 'orange_encounters.xlsx'))
