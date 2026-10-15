# Orange sea routes — Hoenn reference pass

Routes **3–14** are now long Surf crossings, with small rest beaches, scattered boulders, broken reef spurs and broad stretches of open sea. Their long axes span **140–180 metatiles**. About **87–90%** of each layout is usable Surf water; walkable land occupies **under 2.5%**. Reef collision occupies the remainder.

All connection records from the user's Porymap save (`f947db9ed`) are preserved, including the intentionally disconnected routes. The existing **Valencia → Valencia Beach → Route 3 → Mikan Beach** chain remains. Other island links are for the user to arrange later.

Valencia town/beach binaries, the user's tileset images, and Routes 1–2 are unchanged. Mikan Beach received an elevation correction on 1,303 water cells, from land elevation 3 to Surf elevation 1. Its metatiles, appearance, geometry, events and connections are unchanged. This prevents dismounting into the sea when entering from Route 3.

![All twelve sea routes](overview.png)

Vertical maps are rotated in this overview only. The individual previews below retain Porymap orientation.

| Route | Size | Hoenn reference | Composition | Full preview |
| --- | --- | --- | --- | --- |
| 3 | 140 × 36 | 107 | Clear east–west passage, three tiny beaches, existing beach links | [Route 3](../OrangeIslands_Route_3.png) |
| 4 | 160 × 36 | 129 | Offset sandbars and two reef spurs | [Route 4](../OrangeIslands_Route_4.png) |
| 5 | 36 × 152 | 132 | Two optional northbound current ribbons, calm bypasses, small landing beaches | [Route 5](../OrangeIslands_Route_5.png) |
| 6 | 152 × 36 | 124 | Split shoals, small rocky rest islet, open southern bypass | [Route 6](../OrangeIslands_Route_6.png) |
| 7 | 36 × 156 | 105 | Long passage with staggered reef tips and isolated rest beaches | [Route 7](../OrangeIslands_Route_7.png) |
| 8 | 180 × 36 | 130 | Scalloped boulder banks, inward reef fingers and a rocky beach | [Route 8](../OrangeIslands_Route_8.png) |
| 9 | 40 × 152 | 109 | Broad channel with distant fishing beaches | [Route 9](../OrangeIslands_Route_9.png) |
| 10 | 168 × 36 | 108 | Broken outer shoals, two widely separated small beaches | [Route 10](../OrangeIslands_Route_10.png) |
| 11 | 40 × 156 | 131 | Incomplete ring reef with multiple navigation openings | [Route 11](../OrangeIslands_Route_11.png) |
| 12 | 180 × 36 | 128 | Broad open shipping lane; tiny beaches kept near the outer banks | [Route 12](../OrangeIslands_Route_12.png) |
| 13 | 40 × 156 | 133 | Branching reef spurs around open channels and three small beaches | [Route 13](../OrangeIslands_Route_13.png) |
| 14 | 180 × 36 | 134 | Long outer crossing with reef fingers and a small rocky rest stop | [Route 14](../OrangeIslands_Route_14.png) |

## What was learned from vanilla maps

Seventeen Hoenn water routes were rendered and measured from `pret/pokeemerald` at commit `731ad5bfd6e6f265508d0efcca0ba42f9dcf5881`, the same reference checkout used for the Hoenn tileset import. The useful patterns were open water between small landmarks, reef lines that suggest a direction, offset rest beaches, alternate channels, and currents that have calm escape routes. Large landforms such as Route 126's central island were excluded from these Orange crossings.

![Selected vanilla Hoenn references](hoenn-references.png)

The Orange maps use their existing **OrangeGeneral + OrangeIslands** pairing, retaining compatible metatile IDs at the beach connections. Hoenn informs their structure. Native FireRed beach motifs from Routes 20 and 21 South supply complete shoreline corners and transitions. Straight sections can be extended; corner pieces are retained. The validator learns directional beach adjacency counts from those motifs and rejects unseen joins. This is a constrained local check, not a trained route-design model.

[Native beach motifs and source coordinates](native-beaches.json) · [Hoenn measurements](hoenn-reference-metrics.json) · [Current Orange measurements](metrics.json) · [Authored route placements](designs.json)

## Verification and editing

Passed for all twelve sea layouts:

- Engine map-buffer limits, binary sizes, metatile IDs and water/land elevations.
- Complete blocked reef objects and native beach adjacency checks.
- A continuous calm-water crossing at least five tiles wide, plus access to every passable cell and sign approach.
- Northbound current termination in calm water, with no walls or loops.
- Reciprocal Route 3 links: 12 Surf edge cells toward Valencia Beach; 25 Surf and 5 dry edge cells toward Mikan Beach.
- ROM build and exact inclusion of all twelve route binaries and the corrected Mikan Beach binary.
- Comparison against the previous commit confirming every map connection record and the user's Valencia layouts/tileset images are preserved.

Run the focused checks after editing these routes:

```sh
python3 -B docs/orange-islands/water-routes/validate_routes.py
# In the prepared cloud environment:
source /workspace/.setup-cache/activate.sh
make -j5 -W data/maps.s
python3 -B docs/orange-islands/water-routes/validate_routes.py --rom
```

`render_maps.py` now resolves actual tileset paths from the C declarations, including Valencia's condensed Sevii secondary. Use it to refresh previews from Porymap data. The optional `water-routes/build_routes.py` rebuilds the twelve authored layouts from these recipes; **running it replaces subsequent Porymap edits to those routes**. It is not needed to build the ROM or refresh previews.

The older whole-region `validate_maps.py` still fails on Valencia's newer `gTileset_SeviiIslands45Condensed` choice and contains landmarks from earlier town designs. Its prior all-region pass does not cover the user's latest town edits. The checks above cover this sea-route pass and its existing joins, not the entire region. Emulator tests of Surf entry/exit, followers, current movement and connection scrolling remain to be done. Island links, trainers, encounters, rewards, ferry events and interiors remain separate work.
