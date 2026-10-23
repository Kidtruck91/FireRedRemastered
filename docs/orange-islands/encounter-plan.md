# Orange Islands encounters

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
Valencia
  land        rate 20  ODDISH 30% L3-6, CATERPIE 24% L3-7, BELLSPROUT 15% L4-7, AIPOM 15% L4-7, EXEGGCUTE 14% L4-7, HERACROSS 1% L6-8, TROPIUS 1% L6-8
  water       rate  4  TENTACOOL 60% L5-10, WINGULL 30% L5-10, HORSEA 5% L8-12, REMORAID 4% L8-12, LAPRAS 1% L10-14
  old rod     MAGIKARP 70% L5-10, TENTACOOL 30% L5-10
  good rod    TENTACOOL 60% L10-14, MAGIKARP 20% L10-14, HORSEA 20% L10-14
  super rod   HORSEA 40% L12-16, REMORAID 40% L12-16, CHINCHOU 15% L12-16, WAILMER 4% L14-18, LAPRAS 1% L14-18

Valencia_Beach
  land        rate 20  WINGULL 30% L4-7, ZIGZAGOON 30% L4-7, TAILLOW 15% L5-8, KRABBY 14% L5-8, SLOWPOKE 9% L6-8, FARFETCHD 1% L7-9, SHUCKLE 1% L7-9
  water       rate  4  TENTACOOL 60% L5-10, WINGULL 30% L5-10, KRABBY 5% L8-12, STARYU 4% L8-12, LAPRAS 1% L10-14
  old rod     MAGIKARP 70% L5-10, TENTACOOL 30% L5-10
  good rod    TENTACOOL 60% L10-14, KRABBY 20% L10-14, STARYU 20% L10-14
  super rod   SHELLDER 40% L12-16, HORSEA 40% L12-16, CORSOLA 15% L12-16, WAILMER 4% L14-18, LAPRAS 1% L14-18

Route_1
  land        rate 20  WINGULL 30% L5-8, ODDISH 25% L5-9, TAILLOW 15% L6-9, ZIGZAGOON 14% L6-9, EXEGGCUTE 14% L6-9, PINSIR 1% L8-10, HERACROSS 1% L8-10
  water       rate  4  TENTACOOL 60% L8-13, WINGULL 30% L8-13, HORSEA 5% L10-15, REMORAID 4% L10-15, MANTINE 1% L12-17
  old rod     MAGIKARP 70% L5-10, TENTACOOL 30% L5-10
  good rod    TENTACOOL 60% L12-16, MAGIKARP 20% L12-16, HORSEA 20% L12-16
  super rod   REMORAID 40% L14-18, HORSEA 40% L14-18, CHINCHOU 15% L14-18, WAILMER 4% L16-20, MANTINE 1% L16-20

Route_3
  water       rate  4  TENTACOOL 60% L10-15, WINGULL 30% L10-15, HORSEA 5% L12-17, REMORAID 4% L12-17, LAPRAS 1% L14-19
  old rod     MAGIKARP 70% L5-10, TENTACOOL 30% L5-10
  good rod    TENTACOOL 60% L12-16, MAGIKARP 20% L12-16, HORSEA 20% L12-16
  super rod   HORSEA 40% L15-19, REMORAID 40% L15-19, CHINCHOU 15% L15-19, WAILMER 4% L16-20, LAPRAS 1% L16-20

Route_4
  water       rate  4  TENTACOOL 60% L10-15, WINGULL 30% L10-15, WAILMER 5% L12-17, CARVANHA 4% L12-17, SHARPEDO 1% L15-19
  old rod     MAGIKARP 70% L5-10, TENTACOOL 30% L5-10
  good rod    TENTACOOL 60% L12-16, CARVANHA 20% L12-16, REMORAID 20% L12-16
  super rod   WAILMER 40% L15-19, HORSEA 40% L15-19, CLAMPERL 15% L15-19, SHARPEDO 4% L16-20, MANTINE 1% L16-20

Sunburst
  land        rate 20  WINGULL 30% L8-12, ZIGZAGOON 24% L8-13, TAILLOW 15% L9-13, KRABBY 15% L9-13, SLOWPOKE 14% L9-13, SHUCKLE 1% L11-14, FARFETCHD 1% L11-14
  water       rate  4  TENTACOOL 60% L10-15, WINGULL 30% L10-15, HORSEA 5% L12-17, REMORAID 4% L12-17, MANTINE 1% L14-19
  old rod     MAGIKARP 70% L5-10, TENTACOOL 30% L5-10
  good rod    TENTACOOL 60% L12-16, MAGIKARP 20% L12-16, HORSEA 20% L12-16
  super rod   HORSEA 40% L15-19, SHELLDER 40% L15-19, CHINCHOU 15% L15-19, WAILMER 4% L16-20, LAPRAS 1% L16-20

Sunburst_Cave
  land        rate 10  ZUBAT 30% L10-14, GEODUDE 30% L10-14, WHISMUR 15% L11-15, MACHOP 14% L11-15, ARON 9% L12-15, ONIX 1% L13-16, NOSEPASS 1% L13-16
  water       rate  4  TENTACOOL 60% L10-15, ZUBAT 30% L10-15, GOLBAT 5% L14-18, HORSEA 4% L12-17, CHINCHOU 1% L14-18

Mikan_Beach
  land        rate 20  WINGULL 30% L10-14, KRABBY 25% L10-15, TAILLOW 15% L11-15, ZIGZAGOON 14% L11-15, SLOWPOKE 14% L11-15, EXEGGUTOR 1% L13-16, SHUCKLE 1% L13-16
  water       rate  4  TENTACOOL 60% L10-15, KRABBY 30% L10-15, STARYU 5% L12-17, CORSOLA 4% L12-17, LAPRAS 1% L15-19
  old rod     MAGIKARP 70% L5-10, TENTACOOL 30% L5-10
  good rod    TENTACOOL 60% L12-16, KRABBY 20% L12-16, STARYU 20% L12-16
  super rod   CORSOLA 40% L15-19, HORSEA 40% L15-19, SHELLDER 15% L15-19, LUVDISC 4% L16-20, MANTINE 1% L16-20

Mikan
  land        rate 20  WINGULL 30% L11-15, ZIGZAGOON 20% L11-14, KRABBY 15% L12-16, TAILLOW 15% L12-16, SLOWPOKE 14% L12-16, MEOWTH 4% L13-16, EXEGGUTOR 1% L14-17, FARFETCHD 1% L14-17
  water       rate  4  TENTACOOL 60% L10-15, KRABBY 30% L10-15, STARYU 5% L12-17, CORSOLA 4% L12-17, LAPRAS 1% L15-19
  old rod     MAGIKARP 70% L5-10, TENTACOOL 30% L5-10
  good rod    TENTACOOL 60% L12-16, KRABBY 20% L12-16, STARYU 20% L12-16
  super rod   CORSOLA 40% L15-19, HORSEA 40% L15-19, LUVDISC 15% L15-19, SHELLDER 4% L16-20, LAPRAS 1% L16-20
```
