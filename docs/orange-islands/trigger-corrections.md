# Trigger and collision corrections

These changes apply to the current Orange maps following the town, Route 15–18 and interior additions. The editable map binaries and event scripts contain the fixes; no new engine code or tileset artwork is required.

## Corrected exits

- **Moro → Route 7:** land at Y=1, the clear water row across the complete clamped X=20–59 arrival range. The old Y=2 range contained reef tiles. Reef artwork stays intact. The four missing Moro edge triggers at X=28–31 are restored because their arrival positions are now safe.
- **Route 7 → Sunburst:** clamp arrivals to Sunburst Y=1–62. This avoids landing on the other sea exit at the top corner or directly on the southern edge.
- **Seven sea corners:** each corner now has one coordinate event. Its script reads the player's facing direction and chooses the corresponding exit. Both destinations remain available; JSON event order no longer chooses the route.
- **Navel ↔ Navel Beach:** remove the four obsolete trail-warp records. Their replacement is the existing seamless north/south map connection.
- **Route 3 ↔ Mikan Beach:** retain the user's Mikan offset of +5 and make Route 3's reciprocal offset -5. Route 3's existing sand landing receives an east-facing shoreline edge and two corner tiles, closing its previously uncapped end without changing its footprint.

| Corner | Travel direction → destination |
| --- | --- |
| Kumquat (71,59) | South → Route 13; east → Route 14 |
| Mandarin South (0,75) | South → Route 9; west → Route 10 |
| Mandarin South (95,75) | South → Route 9; east → Route 12 |
| Kinnow (0,0) | West → Route 8; north → Route 11 |
| Route 7 (71,0) | East → Sunburst; north → Moro |
| Trovita (0,0) | West → Route 12; north → Route 14 |
| Sunburst (0,0) | North → Route 6; west → Route 7 |

## Corrected movement data

Map-cell collision and elevation changes preserve their existing metatile artwork and palettes.

- **Navel:** 36 pier deck cells use walking elevation 3. Their water trim retains elevation 1. The deck rectangles are X=74–79/Y=42–43, X=67–72/Y=48–49 and X=67–72/Y=52–53.
- **Four active shoreline cells:** Sunburst (10,48), Pinkan (16,83), and Route 15 (8,20)/(8,21) change from `0x3124` to `0x1124`: same ocean metatile, correct Surf elevation.
- **Older coast copies:** 14,622 passable water cells now use elevation 1 in Pummelo Beach, Sunburst Beach, Kinnow Beach, Pinkan Beach, Kumquat Beach, Mandarin South Beach and Moro Beach. Geometry, artwork, event data and connection choices are retained.
- **Valencia Beach:** sand (17,4) and the shallow-water strip X=64–72/Y=36 use walking elevation 3. Shallow water has walking behavior, not Surf behavior.
- **Solid boundaries:** Navel's rock caps at (24,1)/(58,1) and Kinnow's five roof cells at X=38–42/Y=9 have explicit collision. Their former elevation mismatch already prevented normal on-foot entry, but they were still passable to compatible heights.

Mandarin now uses 246 of its 255 coordinate-event slots. Ordinary arrival anchors used by the ghost ship, stadium and caves remain intact. Door/interior placeholders are not converted into new gameplay events.

## Validation

From the repository root:

```sh
python3 -B docs/orange-islands/validate_triggers.py
make -j5 -W data/maps.s
python3 -B docs/orange-islands/validate_triggers.py --rom
```

In the prepared cloud environment, activate the toolchain first with `source /workspace/.setup-cache/activate.sh`.

The focused validator reads the real map binaries, assigned tilesets and event scripts. It evaluates the clamp and branch instructions for all four headings at every coordinate trigger and separately checks both travel directions at every sea corner. It verifies safe arrival surfaces, no landings on another coordinate trigger, unique event conditions, labels, heights, event-count limits, map-buffer sizes, metatile references, loaded palettes, pier approaches and trim, reciprocal Mikan offsets and the correct shore cap. It follows all 729 current cells to ensure no loops or blocked steps. `--rom` checks that all 45 current Orange map binaries are embedded in the ROM.

These are static checks. Emulator traversal is still needed for event timing, followers, Surf mounting/dismounting and screen transitions.

`_follower_port_work/patches/edge_links.py` retains the authored sea-link table, including these arrival corrections and direction-aware corner generation. Running it recreates the sea-edge events and scripts for the selected maps, so review later Porymap event edits before regenerating. It does not repaint the maps.

Earlier `water-routes/build_routes.py` and `navel/build_navel.py` scripts recreate historical design drafts and overwrite later Porymap terrain changes. Their matching validators and image galleries describe those drafts. Use `validate_triggers.py` for the current region's event and movement checks.
