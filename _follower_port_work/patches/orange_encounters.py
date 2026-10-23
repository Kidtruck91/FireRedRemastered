#!/usr/bin/env python3
# C09: the first playable Orange Islands encounter tables -- Valencia, its coast, the
# sea route to Mikan, and the Sunburst branch that hangs off it. Everything further
# south is deliberately left empty until these have been played.
#
# Built from the habitat pools and island themes in the encounter plan:
#   island woods      Oddish/Caterpie/Wurmple/Spinarak .. Bellsprout/Exeggcute/Aipom
#                     /Shroomish .. Heracross/Tropius/Kecleon/Pinsir
#   beach, shoreline  Wingull/Krabby/Zigzagoon .. Slowpoke/Corphish/Taillow
#                     .. Shuckle/Farfetch'd/Exeggutor
#   open sea          Tentacool/Wingull/Magikarp .. Horsea/Remoraid/Wailmer
#                     .. Mantine/Lapras/Sharpedo
#   coastal caves     Zubat/Geodude/Whismur .. Machop/Aron/Golbat
#                     .. Onix/Nosepass/Dunsparce
#   Valencia          gentle tropical intro, signature Aipom
#   Mikan             warm beaches and reefs, signature Corsola
#
# Slot rates are fixed by the engine, so a species' real rate is the sum of the slots
# it holds. This script prints that sum per table; the printout is the record of what
# each table actually rolls. Day/night preferences in the plan are not implemented and
# nothing here depends on them.
#
# Levels: the first Orange cap is 20 and Cissy's team is 18-20, so this stretch takes a
# starter from about 4 to about 18 and nothing exceeds the cap.
#
# Run from the repo root.
import json

PATH = 'src/data/wild_encounters.json'

LAND_RATES = [20, 20, 10, 10, 10, 10, 5, 5, 4, 4, 1, 1]
WATER_RATES = [60, 30, 5, 4, 1]
FISH_RATES = [70, 30, 60, 20, 20, 40, 40, 15, 4, 1]
RODS = (('old rod', 0, 2), ('good rod', 2, 5), ('super rod', 5, 10))


def mons(entries):
    return [{'min_level': lo, 'max_level': hi, 'species': 'SPECIES_' + s}
            for s, lo, hi in entries]


# Each table is listed in slot order; the comment on each line is that slot's rate.
TABLES = {}


def table(name, land=None, land_rate=20, water=None, water_rate=4, fishing=None):
    t = {}
    if land:
        assert len(land) == 12, (name, 'land needs 12 slots', len(land))
        t['land_mons'] = {'encounter_rate': land_rate, 'mons': mons(land)}
    if water:
        assert len(water) == 5, (name, 'water needs 5 slots', len(water))
        t['water_mons'] = {'encounter_rate': water_rate, 'mons': mons(water)}
    if fishing:
        assert len(fishing) == 10, (name, 'fishing needs 10 slots', len(fishing))
        t['fishing_mons'] = {'encounter_rate': 30, 'mons': mons(fishing)}
    TABLES[name] = t


# --- Valencia: palm groves and gentle woods. Signature Aipom; Heracross is the chase.
table('Valencia',
      land=[('ODDISH', 3, 5), ('CATERPIE', 3, 5), ('ODDISH', 4, 6), ('BELLSPROUT', 4, 6),
            ('EXEGGCUTE', 4, 6), ('AIPOM', 4, 6), ('AIPOM', 5, 7), ('BELLSPROUT', 5, 7),
            ('EXEGGCUTE', 5, 7), ('CATERPIE', 5, 7), ('HERACROSS', 6, 8), ('TROPIUS', 6, 8)],
      water=[('TENTACOOL', 5, 10), ('WINGULL', 5, 10), ('HORSEA', 8, 12),
             ('REMORAID', 8, 12), ('LAPRAS', 10, 14)],
      fishing=[('MAGIKARP', 5, 10), ('TENTACOOL', 5, 10),
               ('TENTACOOL', 10, 14), ('MAGIKARP', 10, 14), ('HORSEA', 10, 14),
               ('HORSEA', 12, 16), ('REMORAID', 12, 16), ('CHINCHOU', 12, 16),
               ('WAILMER', 14, 18), ('LAPRAS', 14, 18)])

# --- Valencia Beach: shoreline. Farfetch'd and Shuckle are the unusual finds.
table('Valencia_Beach',
      land=[('WINGULL', 4, 6), ('ZIGZAGOON', 4, 6), ('WINGULL', 5, 7), ('ZIGZAGOON', 5, 7),
            ('TAILLOW', 5, 7), ('KRABBY', 5, 7), ('TAILLOW', 6, 8), ('SLOWPOKE', 6, 8),
            ('KRABBY', 6, 8), ('SLOWPOKE', 6, 8), ('FARFETCHD', 7, 9), ('SHUCKLE', 7, 9)],
      water=[('TENTACOOL', 5, 10), ('WINGULL', 5, 10), ('KRABBY', 8, 12),
             ('STARYU', 8, 12), ('LAPRAS', 10, 14)],
      fishing=[('MAGIKARP', 5, 10), ('TENTACOOL', 5, 10),
               ('TENTACOOL', 10, 14), ('KRABBY', 10, 14), ('STARYU', 10, 14),
               ('SHELLDER', 12, 16), ('HORSEA', 12, 16), ('CORSOLA', 12, 16),
               ('WAILMER', 14, 18), ('LAPRAS', 14, 18)])

# --- Route 1: first sea crossing, with islets of woods/shore. Pinsir is the rarity.
table('Route_1',
      land=[('WINGULL', 5, 7), ('ODDISH', 5, 7), ('WINGULL', 6, 8), ('ZIGZAGOON', 6, 8),
            ('TAILLOW', 6, 8), ('EXEGGCUTE', 6, 8), ('TAILLOW', 7, 9), ('ODDISH', 7, 9),
            ('EXEGGCUTE', 7, 9), ('ZIGZAGOON', 7, 9), ('PINSIR', 8, 10), ('HERACROSS', 8, 10)],
      water=[('TENTACOOL', 8, 13), ('WINGULL', 8, 13), ('HORSEA', 10, 15),
             ('REMORAID', 10, 15), ('MANTINE', 12, 17)],
      fishing=[('MAGIKARP', 5, 10), ('TENTACOOL', 5, 10),
               ('TENTACOOL', 12, 16), ('MAGIKARP', 12, 16), ('HORSEA', 12, 16),
               ('REMORAID', 14, 18), ('HORSEA', 14, 18), ('CHINCHOU', 14, 18),
               ('WAILMER', 16, 20), ('MANTINE', 16, 20)])

# --- Routes 3 and 4: open sea, no land at all. Route 4 is the rougher of the two.
table('Route_3',
      water=[('TENTACOOL', 10, 15), ('WINGULL', 10, 15), ('HORSEA', 12, 17),
             ('REMORAID', 12, 17), ('LAPRAS', 14, 19)],
      fishing=[('MAGIKARP', 5, 10), ('TENTACOOL', 5, 10),
               ('TENTACOOL', 12, 16), ('MAGIKARP', 12, 16), ('HORSEA', 12, 16),
               ('HORSEA', 15, 19), ('REMORAID', 15, 19), ('CHINCHOU', 15, 19),
               ('WAILMER', 16, 20), ('LAPRAS', 16, 20)])

table('Route_4',
      water=[('TENTACOOL', 10, 15), ('WINGULL', 10, 15), ('WAILMER', 12, 17),
             ('CARVANHA', 12, 17), ('SHARPEDO', 15, 19)],
      fishing=[('MAGIKARP', 5, 10), ('TENTACOOL', 5, 10),
               ('TENTACOOL', 12, 16), ('CARVANHA', 12, 16), ('REMORAID', 12, 16),
               ('WAILMER', 15, 19), ('HORSEA', 15, 19), ('CLAMPERL', 15, 19),
               ('SHARPEDO', 16, 20), ('MANTINE', 16, 20)])

# --- Sunburst: the branch off Route 4. Shore above, cave below.
table('Sunburst',
      land=[('WINGULL', 8, 11), ('ZIGZAGOON', 8, 11), ('WINGULL', 9, 12),
            ('TAILLOW', 9, 12), ('KRABBY', 9, 12), ('SLOWPOKE', 9, 12),
            ('TAILLOW', 10, 13), ('KRABBY', 10, 13), ('SLOWPOKE', 10, 13),
            ('ZIGZAGOON', 10, 13), ('SHUCKLE', 11, 14), ('FARFETCHD', 11, 14)],
      water=[('TENTACOOL', 10, 15), ('WINGULL', 10, 15), ('HORSEA', 12, 17),
             ('REMORAID', 12, 17), ('MANTINE', 14, 19)],
      fishing=[('MAGIKARP', 5, 10), ('TENTACOOL', 5, 10),
               ('TENTACOOL', 12, 16), ('MAGIKARP', 12, 16), ('HORSEA', 12, 16),
               ('HORSEA', 15, 19), ('SHELLDER', 15, 19), ('CHINCHOU', 15, 19),
               ('WAILMER', 16, 20), ('LAPRAS', 16, 20)])

table('Sunburst_Cave', land_rate=10,
      land=[('ZUBAT', 10, 13), ('GEODUDE', 10, 13), ('ZUBAT', 11, 14),
            ('GEODUDE', 11, 14), ('WHISMUR', 11, 14), ('MACHOP', 11, 14),
            ('WHISMUR', 12, 15), ('ARON', 12, 15), ('MACHOP', 12, 15),
            ('ARON', 12, 15), ('ONIX', 13, 16), ('NOSEPASS', 13, 16)],
      water=[('TENTACOOL', 10, 15), ('ZUBAT', 10, 15), ('GOLBAT', 14, 18),
             ('HORSEA', 12, 17), ('CHINCHOU', 14, 18)])

# --- Mikan: warm beaches and reefs, signature Corsola. Cissy is Lv 18-20.
table('Mikan_Beach',
      land=[('WINGULL', 10, 13), ('KRABBY', 10, 13), ('WINGULL', 11, 14),
            ('ZIGZAGOON', 11, 14), ('TAILLOW', 11, 14), ('SLOWPOKE', 11, 14),
            ('KRABBY', 12, 15), ('TAILLOW', 12, 15), ('SLOWPOKE', 12, 15),
            ('ZIGZAGOON', 12, 15), ('EXEGGUTOR', 13, 16), ('SHUCKLE', 13, 16)],
      water=[('TENTACOOL', 10, 15), ('KRABBY', 10, 15), ('STARYU', 12, 17),
             ('CORSOLA', 12, 17), ('LAPRAS', 15, 19)],
      fishing=[('MAGIKARP', 5, 10), ('TENTACOOL', 5, 10),
               ('TENTACOOL', 12, 16), ('KRABBY', 12, 16), ('STARYU', 12, 16),
               ('CORSOLA', 15, 19), ('HORSEA', 15, 19), ('SHELLDER', 15, 19),
               ('LUVDISC', 16, 20), ('MANTINE', 16, 20)])

table('Mikan',
      land=[('WINGULL', 11, 14), ('ZIGZAGOON', 11, 14), ('WINGULL', 12, 15),
            ('KRABBY', 12, 15), ('TAILLOW', 12, 15), ('SLOWPOKE', 12, 15),
            ('KRABBY', 13, 16), ('TAILLOW', 13, 16), ('SLOWPOKE', 13, 16),
            ('MEOWTH', 13, 16), ('EXEGGUTOR', 14, 17), ('FARFETCHD', 14, 17)],
      water=[('TENTACOOL', 10, 15), ('KRABBY', 10, 15), ('STARYU', 12, 17),
             ('CORSOLA', 12, 17), ('LAPRAS', 15, 19)],
      fishing=[('MAGIKARP', 5, 10), ('TENTACOOL', 5, 10),
               ('TENTACOOL', 12, 16), ('KRABBY', 12, 16), ('STARYU', 12, 16),
               ('CORSOLA', 15, 19), ('HORSEA', 15, 19), ('LUVDISC', 15, 19),
               ('SHELLDER', 16, 20), ('LAPRAS', 16, 20)])


def camel(name):
    return ''.join(p.capitalize() if not p.isdigit() else p for p in name.split('_'))


def report():
    """Print each table's real per-species rate and level span."""
    for name in TABLES:
        print('\n%s' % name)
        for kind, rates in (('land_mons', LAND_RATES), ('water_mons', WATER_RATES)):
            block = TABLES[name].get(kind)
            if not block:
                continue
            total, span = {}, {}
            for slot, mon in enumerate(block['mons']):
                s = mon['species'][8:]
                total[s] = total.get(s, 0) + rates[slot]
                lo, hi = span.get(s, (99, 0))
                span[s] = (min(lo, mon['min_level']), max(hi, mon['max_level']))
            print('  %-11s rate %2d  %s' % (kind.replace('_mons', ''),
                                            block['encounter_rate'],
                                            ', '.join('%s %d%% L%d-%d' % (s, total[s], *span[s])
                                                      for s in sorted(total, key=lambda k: -total[k]))))
        block = TABLES[name].get('fishing_mons')
        if block:
            for label, lo, hi in RODS:
                parts = []
                for slot in range(lo, hi):
                    mon = block['mons'][slot]
                    parts.append('%s %d%% L%d-%d' % (mon['species'][8:], FISH_RATES[slot],
                                                     mon['min_level'], mon['max_level']))
                print('  %-11s %s' % (label, ', '.join(parts)))


data = json.load(open(PATH))
group = next(g for g in data['wild_encounter_groups'] if g['label'] == 'gWildMonHeaders')
by_map = {e['map']: e for e in group['encounters']}

added = 0
for name, t in TABLES.items():
    key = 'MAP_ORANGE_ISLANDS_' + name.upper()
    entry = {'map': key, 'base_label': 'sOrangeIslands%s_FireRed' % camel(name)}
    entry.update(t)
    if key in by_map:
        group['encounters'][group['encounters'].index(by_map[key])] = entry
    else:
        group['encounters'].append(entry)
        added += 1

json.dump(data, open(PATH, 'w'), indent=2)
open(PATH, 'a').write('\n')

report()
print('\norange encounters ok: %d tables (%d new)' % (len(TABLES), added))


def write_doc(path='docs/orange-islands/encounter-plan.md'):
    """Record what was implemented and the rates each table actually rolls."""
    import io, contextlib
    buf = io.StringIO()
    with contextlib.redirect_stdout(buf):
        report()
    with open(path, 'w') as f:
        f.write("""# Orange Islands encounters

## What is implemented

The first playable stretch only: **Valencia, its coast, the sea route to Mikan, and the
Sunburst branch off Route 4**. Nine tables, listed below with the rate each species
actually rolls. Everything further south is deliberately empty until these have been
played; adding more means extending this file, not replacing it.

Levels are built around the first Orange cap of 20 and Cissy's Lv 18-20 team, so this
stretch takes a starter from about 4 to about 18 and nothing in it exceeds the cap.

Edit the tables in `_follower_port_work/patches/orange_encounters.py` and re-run it from
the repo root; it rewrites the entries in `src/data/wild_encounters.json` in place and
reprints this listing.

## Slots and rates

Slot rates are fixed by the engine, so a species' real rate is the sum of the slots it
holds:

| table | slots | rates |
| --- | --- | --- |
| land | 12 | 20, 20, 10, 10, 10, 10, 5, 5, 4, 4, 1, 1 |
| water (Surf) | 5 | 60, 30, 5, 4, 1 |
| fishing | 10 | old 70/30, good 60/20/20, super 40/40/15/4/1 |

`encounter_rate` is how often a step rolls at all: 20 for open land, 10 in the cave,
4 on water.

## Habitat pools

| habitat | common | uncommon | rare |
| --- | --- | --- | --- |
| Island woods | Oddish, Caterpie, Wurmple, Spinarak | Bellsprout, Exeggcute, Aipom, Shroomish | Heracross, Tropius, Kecleon, Pinsir |
| Beach and shoreline | Wingull, Krabby, Zigzagoon | Slowpoke, Corphish, Taillow | Shuckle, Farfetch'd, Exeggutor |
| Open sea | Tentacool, Wingull, Magikarp | Horsea, Remoraid, Wailmer | Mantine, Lapras, Sharpedo |
| Coastal caves | Zubat, Geodude, Whismur | Machop, Aron, Golbat | Onix, Nosepass, Dunsparce |

Coastal water sub-pools, used where the geography suits them: sandy shallows
(Tentacool, Staryu, Shellder), coral reefs (Corsola, Horsea, Luvdisc, Clamperl), rocky
coves (Krabby, Shellder, Chinchou), harbors (Magikarp, Tentacool, Remoraid), deep
offshore (Wailmer, Chinchou, Clamperl, Relicanth), dangerous channels (Carvanha,
Sharpedo, Tentacruel), quiet bays (Slowpoke, Seel, Lapras).

## Island themes

| island | theme | signature |
| --- | --- | --- |
| Valencia | welcoming tropical habitat | Oddish, Wingull, Aipom, Exeggcute |
| Mikan | warm beaches and reefs | Krabby, Staryu, Corsola, Horsea |
| Navel | mountain island | Geodude, Machop, Nosepass, Spheal/Snorunt in genuinely cold summit areas |
| Pinkan | protected wildlife sanctuary | Nidoran, Rhyhorn, Scyther (pink colouring is a separate art decision) |
| Moro | ruins and ghost ship | Gastly, Duskull, Shuppet, Sableye |
| Kinnow | lush woodland and fruit groves | Bellsprout, Aipom, Shroomish, Tropius |
| Kumquat | busy resort island | Meowth, Snubbull, Wingull, Luvdisc in the water |
| Trovita | wooded streams and waterfalls | Surskit, Marill, Lotad, Goldeen |

Only Valencia, Mikan and the Sunburst cave are built so far; the rest are the plan for
later islands.

## Not implemented

Day and night preferences (Ledyba, Hoothoot, Spinarak, Dustox, Beautifly, Chinchou) are
a future idea. There is no clock, and nothing in these tables depends on one.

## The tables as built
```
%s```
""" % buf.getvalue().lstrip('\n'))
    print('wrote', path)


write_doc()
