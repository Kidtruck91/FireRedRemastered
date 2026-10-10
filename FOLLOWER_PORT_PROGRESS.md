# Follower Pokémon port: progress and handoff notes

This file tracks porting the follower system from `Decomps/Followers/FollowerEm` (pokeemerald) into this FireRed project.
The work is split into chunks, and each chunk is committed on the git branch **`follower-port`**.
If a session runs out, a new one should read this file, check `git log follower-port`, and continue with the first unchecked chunk.

**Start of a new session:** "Continue the follower port. Read FOLLOWER_PORT_PROGRESS.md in my FireRedRemastered folder."

## Safety / branches
- `backup/pre-follower-fix`: snapshot of the whole project (including all uncommitted maps, trainers and level-cap work) taken before any of this started.
- `follower-port`: the work branch. It starts from that snapshot.
- `master`: untouched.

## Important facts found while surveying
- **FollowerEm's copy is partly damaged.** About 25 headers in `FollowerEm/include/*.h` have been overwritten with their namesakes from `include/constants/` (`global.h`, `follow_me.h`, `pokemon.h`, `tv.h`, `pokedex.h` and others). So `struct Follower` and the `follow_me.h` prototypes had to be rebuilt from the code that uses them. Use FollowerEm's `src/` files and `src/data/` files as the reference, not its `include/*.h`.
- FollowerEm is based on pret/pokeemerald around commit `8e5347b` (Apr 30 2023). Diffing against that commit isolates the follower changes.
- FollowerEm also bundles Ghoulslash's **dynamic overworld palettes (DOWP)**. That part is **deliberately not ported**: see the design below.
- The follower code is Ghoulslash's "follow me" NPC follower plus Pokémon follower additions: `UpdateFollowerPokemonGraphic`, pokeball shrink/grow, sparkles, and the bounce in `gflib/sprite.c`.

## Design decisions (FireRed version)
1. **Object event graphics ids are now `u16`.**
   - `ObjectEventTemplate`: `localId, kind, u16 graphicsId, x, y…`. The size is still 0x18.
   - `ObjectEvent`: `graphicsId` moved to offset 0x06 and `trainerType` to 0x23. The size is still 0x24, so the saveblock layout size is unchanged.
   - `asm/macros/map.inc` `object_event` and `clone_event` now emit `.byte index, kind` / `.2byte gfx`.
2. **Follower graphics ids are `OBJ_EVENT_GFX_FOLLOWER(species)` = 256 + species.**
   - This keeps all existing FireRed ids (0–167) and `OBJ_EVENT_GFX_VAR_0..F` (240–255) unchanged. It avoids the name clashes from before (FireRed already has `OBJ_EVENT_GFX_PIDGEY`, `_SNORLAX` and so on).
   - Data lives in `src/data/object_events/follower_graphics.h`, which is generated, indexed by species, and covers species 1–411. The old-Unown placeholder slots point at Unown.
   - Animations and affine anims are in `follower_anims.h`.
   - `GetObjectEventGraphicsInfo()` must look up `gFollowerGraphicsInfoPointers[id - 256]` for ids in that range.
3. **Palettes: no DOWP.**
   - Follower infos use `paletteSlot = PALSLOT_DYNAMIC` (15) and `paletteTag = FOLLOWER_PAL_TAG(species)` (0x2000 + species).
   - Sprite-spawn code must load these through the normal sprite-palette allocator (`LoadSpritePalette`, slots 12–15) using `sFollowerPalettes[species]`, set `oam.paletteNum` from `IndexOfSpritePaletteTag`, apply weather tint, and free the palette when the follower object is removed.
   - Reflection map entries 12–15 should map to themselves.
4. **The follower does not need a map object.**
   - FollowerEm clones a map NPC template (`setfollower localId`). Here the follower uses a built-in template with `localId = OBJ_EVENT_ID_FOLLOWER` (0xFE), so it works on every map and on existing saves.
   - It starts automatically once the party has a non-egg Pokémon. Planned: `FLAG_FOLLOWER_ACTIVE` (0x0C0) becomes "followers disabled" (rename it to `FLAG_FOLLOWERS_DISABLED`), with `enablefollower` / `disablefollower` macros.
5. **Ball sprites** (shrink/grow, sparkle) are created as plain sprites with their ball palette loaded dynamically. If a palette slot can't be allocated, skip the sprite. Pokéball types: the 12 that FireRed has.
6. **`struct Follower`** is in `include/global.h`, inside `SaveBlock2` in place of the first 0x18 bytes of `filler_B20`, so the SaveBlock2 size is unchanged.
7. **Quest log:** don't record the follower in `SetQuestLogObjectEventsData` (mark it inactive), and don't create or move the follower while `QL_IS_PLAYBACK_STATE`.
8. **Surfing:** hide the follower instantly when surf starts. It pops out of its ball on the first step on land, which is the existing `FollowerPokeballSparkle` behaviour.
9. **Old saves:** object-event layouts changed, so starting a **new game** is recommended for testing.

## Chunks

### Chunk 1: data, structs, graphics ✅ (committed)
- [x] Reverted the earlier hand-edits of `include/constants/event_objects.h` and the five `src/data/object_events/*.h` tables to HEAD. Kept the Mom pic-table fix.
- [x] `u16` graphicsId in `include/global.fieldmap.h`; `asm/macros/map.inc` macros.
- [x] Constants in `include/constants/event_objects.h`: `OBJ_EVENT_GFX_FOLLOWERS_START`, `OBJ_EVENT_GFX_FOLLOWER()`, `FOLLOWER_GFX_COUNT`, `TRACKS_SLITHER/BUG/SPOT`, `OBJ_EVENT_ID_FOLLOWER`.
- [x] `struct Follower` and `SaveBlock2.follower` in `include/global.h`.
- [x] New `include/follow_me.h` with prototypes. Some prototypes are planned and not implemented yet; adjust them in Chunk 3.
- [x] Generated `src/data/object_events/follower_graphics.h` and `follower_anims.h`. Generator scripts are in `_follower_port_work/gen/`.
- [x] Assets:
  - follower palettes in `graphics/object_events/palettes/followers/*.pal` (386)
  - ball pics and palettes in `pics/misc/*_ball.png` and `palettes/*_ball.pal`
  - `graphics/field_effects/pics/{bug,spot,slither}_tracks.png` and `expanding_sparkle.png`
  - Follower PNGs reuse the existing `graphics/object_events/pics/followers/` folder.
- [x] `spritesheet_rules.mk`: rules for the follower sprites (32x32, or 64x64 for Steelix, Lugia, Ho-Oh and Wailord), the tracks, and the expanding sparkle.
- **The build does not pass yet.** `follow_me.c` and the `event_object_movement.c` hooks still need porting.

### Chunk 2: event_object_movement.c and graphics plumbing ✅ (committed)
- [x] `GetObjectEventGraphicsInfo()` handles the follower range (256+). Spawn and set-graphics paths use `GetObjectEventPaletteNum()`, which loads `PALSLOT_DYNAMIC` palettes on demand (`LoadDynamicObjectEventPalette`) and frees them when the object is removed. Reflection map entries 12–15 map to themselves.
- [x] Movement type `MOVEMENT_TYPE_FOLLOWING_POKEMON` (0x51). Movement actions `MOVEMENT_ACTION_FOLLOWING_POKEMON*` are **0xAA–0xB0**; the earlier 0xB0–0xB7 values were replaced. Func tables are in `src/data/object_events/movement_*_func_tables.h`.
- [x] The follower step animations go through one helper, `FollowerSetStepAnim()`, called at the top of the WalkSlow/WalkNormal/WalkFast/WalkFaster/Slide `_Step0` functions; `FollowerPrepareJump()` covers Jump2.
- [x] The follower movement actions (face, shrink and grow with a pokeball, `RecreateObjectEvent`) are in `event_object_movement.c`. The source is `_follower_port_work/patches/follower_actions.c`. They call `CreateFollowerPokeballSprite()` and `IsBigSprite()`, which **chunk 3 must implement in follow_me.c**.
- [x] Collision exemption, the `FollowMe()` hook in `ObjectEventSetHeldMovement`, local id 0xFE mapping to the follower, no despawn outside the view, no ground effects while hidden, elevation fix, and ministep helpers.
- [x] Track field effects (`FLDEFF_TRACKS_SLITHER/SPOT/BUG` = 71–73) with scripts, templates, pics and `FldEff_Tracks*`. `FLDEFF_USE_HEADBUTT` (70) had no script, so it points at the PhotoFlash script as a placeholder to keep the table aligned.
- [x] Not widened, on purpose, because follower ids never reach them: the field_player_avatar gender/state getters, `VarGetObjectEventGraphicsId`, vs_seeker, union room, and `CheckObjectGraphicsInFrontOfPlayer`. The quest log is handled in chunk 4.
- **Build status:** everything compiles except `src/follow_me.c` (chunk 3).
- Patch scripts used are kept in `_follower_port_work/patches/` for reference.

### Chunk 3: follow_me.c
- [ ] Adapt `src/follow_me.c` (currently a straight copy of FollowerEm's):
  - FireRed names: `gSpecialVar_0x8014` instead of `gSpecialVar_Unused_0x8014`
  - the FireRed metatile behaviours and movement action names
  - no Acro/Mach split in FireRed (only one bike)
  - no underwater or dive
- [ ] Built-in follower template; auto-start; `UpdateFollowerPokemonGraphic` and the sparkle/pokeball code from FollowerEm's `overworld.c`.
- [ ] Add `src/follow_me.o` to `ld_script.txt` if FireRed's linker script lists objects explicitly.

### Chunk 4: hooks in the rest of the game
- [ ] `overworld.c`: whiteout, warp placement, `CB1/CB2` fixes, `FollowMe_HandleSprite` after spawning objects, `UpdateFollowerPokemonGraphic` on return to field.
- [ ] `field_player_avatar.c` (ledge, run, walk sparkle; `CreateFollowerAvatar` in `InitPlayerAvatar`), `field_control_avatar.c`, `field_screen_effect.c` (door warps), `field_effect.c` (fly, teleport, escalator, waterfall, warp spin), `bike.c`, `battle_setup.c`, `script_movement.c`, `scrcmd.c` (follower script commands 0xd5–0xd8 and `pokemonfaceplayer`), `gflib/sprite.c` (bounce), `new_game.c`, `pokemon_storage_system.c`, `field_specials.c` (`GetLeadMonNotFaintedIndex`), quest log.
- [ ] Scripts: `pkmn_center_nurse` (ball in and out), `field_move_scripts` (`FollowerPositionFix` helpers), the macros in `asm/macros/event.inc` (already added; switch them to `VAR_0x8014`), and talking to the follower.

### Chunk 5: build and test
- [ ] `make` succeeds.
- [ ] Test on a new game: the follower appears after getting the starter; walk, run, ledges, doors, stairs, bike, surf, fly, Pokémon Center, battles, lead swap in the party and PC, and a big sprite (Steelix).
- [ ] Remove `_follower_port_work/` when finished.

## Separate change: level cap (not part of the follower port)
Committed on the same branch but in its own commit, touching only level-cap files:
- `src/level_cap.c` and `include/level_cap.h`: the cap is now calculated from badge flags whenever it's needed. Nuzlocke off means no cap. The user's cap table is unchanged.
- `src/battle_script_commands.c`: EXP is clamped so a Pokémon can't level past the cap.
- `src/party_menu.c`: Rare Candy has no effect at the cap.
- `data/maps/PalletTown/text.inc`: the sign text for "no cap".

## Log
- 2026-10-09: survey done, backup branch made, Chunk 1 done and committed.
- 2026-10-10: Chunk 2 done and committed. Level cap behaviour finished (separate commit).
