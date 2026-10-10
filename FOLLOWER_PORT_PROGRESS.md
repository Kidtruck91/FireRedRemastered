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

### Chunk 3: follow_me.c ✅ (committed). **`make` now builds a ROM.**
- [x] `src/follow_me.c` rewritten for FireRed:
  - The follower uses a built-in template (`sFollowerObjectEventTemplate`, local id 0xFE, script `EventScript_FollowerPokemon`).
  - Auto-start: `FollowerPokemon_TryStart()`, called from `CreateFollowerAvatar()` and `UpdateFollowerPokemonGraphic()`.
  - `UpdateFollowerPokemonGraphic()` recreates the object when the lead Pokémon changes.
  - The ball and sparkle code from FollowerEm's `overworld.c` moved here: `FollowerPokeballSparkle`, `FollowerIntoPokeball`, `CreateFollowerPokeballSprite`, `IsBigSprite`, and the sparkle coords table re-indexed for FireRed species. All 12 FireRed ball types are supported; palettes load dynamically.
  - Removed: rival/NPC alternate bike and surf sprites, the surf blob, underwater handling, `FAST_FOLLOWERS`, sideways stairs, and `Task_DoDoorWarp`. FireRed's door warp is in `field_fadetransition.c` and gets hooked in chunk 4.
  - All functions are guarded against `objId == OBJECT_EVENTS_COUNT`, meaning the follower couldn't spawn on a full map, and against quest-log playback.
- [x] `FLAG_FOLLOWER_ACTIVE` (0x0C0) renamed to `FLAG_FOLLOWERS_DISABLED`. `FLAG_FOLLOWER_IN_BUILDING` (0x0C1) is now unused.
- [x] `asm/macros/event.inc`: the follower macros use `VAR_0x8014`; new `enablefollower` / `disablefollower`. `pokemonfaceplayer` calls `FollowerPokemonFacePlayer` (a callnative function, not a ScrCmd).
- [x] `data/scripts/follower.inc`: the talk-to-follower script (cry and "X seems happy to be with you!") plus FollowerEm's position-fix scripts.
- [x] `ObjectEventSetMovementTypeNone()` added to `event_object_movement.c` for `setfollower`.
- [x] `ld_script.ld` needs no change; it has a `src/*.o` catch-all.
- Sizes checked: `gObjectEvents` is 0x240 (16 × 0x24, unchanged), SaveBlock2 is 0xF28, SaveBlock1 is 0x3E44.
- **Nothing calls `CreateFollowerAvatar` yet, so no follower shows up in game until chunk 4.**

### Chunk 4: hooks + dialogue ✅ (committed). ROM builds; the follower is live in game.
All hooks are applied by `_follower_port_work/patches/chunk4_hooks.py`, which can be re-run because it resets the files first:
- **`field_player_avatar.c`:** `CreateFollowerAvatar()` in `InitPlayerAvatar`; `FollowerPokeballSparkle()` before walk/run and ledge jumps (with a delayed jump if the follower is in its ball); `FollowerIntoPokeball()` in `StartPlayerAvatarSummonMonForFieldMoveAnim`, so every field move (Surf, Fly, Cut, Strength…) puts it away.
- **`field_control_avatar.c`:** no wild encounter check while `preventStep`; `FollowMe_IdleUpdate()` when no step was taken; an invisible follower can't be talked to.
- **`event_object_movement.c`:**
  - talking to the follower runs `follower.script`
  - `FollowMe_UpdateBounce()` gives the idle hop that FollowerEm did in `sprite.c`; flying Pokémon don't hop
  - `IsFollowerObjectEvent` ignores quest-log playback
- **`overworld.c`:** whiteout; `FollowMe_TrySetCoordsFromWarp()` in `CB2_LoadMap2`; `FollowMe_FixSpriteOffset()` in `CB2_Overworld`; `FollowMe_HandleSprite()` after map objects spawn; `UpdateFollowerPokemonGraphic()` on every return to the field (party menu, PC, battles, bag…).
- **`field_fadetransition.c`:** door warp in (the follower steps up and walks in, and the door isn't closed on it); door exits use `FollowMe_DoorExit()`, so the follower comes out of the door after the player.
- **Other files:** `bike.c` (into its ball when getting on the bike), `new_game.c` (clears the follower), `script_movement.c`, `quest_log_objects.c` (the follower isn't recorded).
- **Pokémon Center nurse script:** the follower goes into its ball while the party is healed, and `updatefollowerpokemongraphic` runs after healing.
- `UpdateFollowerPokemonGraphic()` also creates the follower immediately if it has just started (for example after getting the starter).

**Dialogue system (requested):**
- `data/text/follower_dialogue.inc` is a data file with instructions at the top. Each line is `follower_message COND, VALUE[, COND2, VALUE2]` followed by `.string "…$"`.
- Conditions: species, type, egg group, exact map, map section (area), map type (caves = `MAP_TYPE_UNDERGROUND`), terrain (sand/beach, tall grass, next to water, ice), weather, friendship, low HP, status, flag set / not set.
- Scoring: each condition has a weight (in `sFollowerConditionWeights`, `follow_me.c`). The highest-scoring matching lines win, with a random pick among ties.
- About 60 starter lines: generic, types, places (caves, Power Plant, Pokémon Tower, beaches, Mt. Moon, Seafoam…) and specific Pokémon. Selection code: `ChooseFollowerMessage()` in `follow_me.c`.
- Constants are in `include/constants/follow_me.h`.

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

## Separate change: Nuzlocke rules (not part of the follower port)
Its own commit. Hooks are applied by `_follower_port_work/patches/nuzlocke_hooks.py`. It resets the files to the commit before the Nuzlocke change, using the tag `nuzlocke-base` in the scratch copy. The code is in `src/nuzlocke.c` and `include/nuzlocke.h`.

**Switch:**
- `FLAG_NUZLOCKE`, set from Oak's intro or the Options menu.
- The Options menu now reads and writes that flag; before, it only changed an unused `enableNuzlocke` bit.

**Fainted = dead:**
- After every battle the player didn't lose (`Nuzlocke_OnBattleEnd` in `battle_main.c`), party Pokémon at 0 HP are marked dead, using bit 1 of `MON_DATA_UNUSED_RIBBONS`. Fainting from field poison also marks a Pokémon dead.
- Dead Pokémon stay in the party but can't be healed:
  - `HealPlayerParty` skips them
  - `CalculateMonStats` keeps them at 0 HP (taking them out of the PC or Day Care)
  - every item except Revive and Revival Herb has no effect on them
- **Revive and Revival Herb are the only way back.** Using one is a deliberate rule bend, and `Nuzlocke_OnItemUsed` clears the dead mark.
- Max Revive and Sacred Ash never work on fainted Pokémon in Nuzlocke mode. To allow them, add them to `IsNuzlockeReviveItem` in `nuzlocke.c`.

**One encounter per area:**
- The first wild battle in a map section uses up that area. The bits are in `SaveBlock2.nuzlockeAreas[32]`, inside the old filler, so the size is unchanged.
- Throwing a ball in a later wild battle there asks: "Your encounter here is used. Throw it anyway?" Yes throws, No goes back to the bag.
- Trainer, Safari, ghost, tutorial and Pokédude battles don't count.

**Losing:**
- The whiteout script's fail message now also calls `Nuzlocke_EndRun()`, which clears all dead marks (party and PC) and heals the party.

## Separate change: trainer AI levels (not part of the follower port)
Its own commit. There's no in-game option: each trainer's level is set in `src/data/trainers.h` with `.aiFlags = AI_LEVEL_SIMPLE / AI_LEVEL_NORMAL / AI_LEVEL_ADVANCED`. These are defined in `include/constants/battle_ai.h`.

**Levels:**
- **SIMPLE** = `CHECK_BAD_MOVE`, vanilla route-trainer behaviour.
- **NORMAL** = `CHECK_BAD_MOVE | CHECK_VIABILITY | TRY_TO_FAINT | HP_AWARE`. This is the vanilla gym-leader set plus HP_AWARE.
- **ADVANCED** = NORMAL | `AI_SCRIPT_ADVANCED` (bit 10). Bit 10 is AI_Ret in the script table, so all of its logic is C in `src/battle_ai_advanced.c`. The tuning numbers are at the top of that file.

**Defaults set by the conversion:**
- The 776 vanilla `CHECK_BAD_MOVE` trainers became SIMPLE.
- The other old gym-leader-style trainers became NORMAL.
- Every `TRAINER_CLASS_LEADER`, `ELITE_FOUR` and `CHAMPION` entry (84, rematches included) became ADVANCED.

**What Advanced does:**
- **Memory:**
  - `AI_RecordPlayerMove` is hooked in `battle_script_commands.c` where `gLastMoves` is set. It records each player party slot's used moves, `sSeenMoves[6][4]`, and which slots have been out.
  - It is reset once per battle in `BattleAI_HandleItemUseBeforeAISetup`.
  - The script history (`if_has_move AI_TARGET`) is replaced with the current mon's own remembered moves.
  - Unseen moves are guessed as a typical STAB attack of the mon's types, at 80%.
- **Move scoring:** `AI_AdvancedScoreMoves` runs after the scripts in `BattleAI_ChooseMoveOrAction` and uses real damage calcs.
  - A sure KO when it moves first, or with a priority move: +8, minus an accuracy penalty.
  - A KO when it's slower but survives the player's hit: +6.
  - Its best-damage move: +3.
  - When it will be KO'd before moving: status moves are discouraged; priority moves and Explosion are favoured.
  - Setup moves are worth +5/+3 when it survives two of the player's best known hits. That drops once it's at +2, when the boost is useless, or when it's unsafe.
  - **Switch prediction** (50% of turns, singles only): if the player's mon is in trouble and a *seen* teammate walls the AI's best attack, moves that also hit that teammate get a bonus.
- **Switching:** in singles, `AI_AdvancedShouldSwitch` replaces vanilla's random type-matchup switches. Vanilla's Perish Song, Wonder Guard, absorb-ability and Natural Cure checks still run first.
  - It switches only when it would be KO'd first, or is walled (under 1/8 damage), and only into something that takes the known moves much better.
  - It never switches when it has a sure KO, never on consecutive turns, and stays in 25% of the time anyway.
  - Replacements after a faint use the same scoring (`AI_AdvancedGetBestSwitchIn`).
- **Doubles:** only the move scoring applies; switching is vanilla.

**Status:** builds; **not play-tested.**

## Log
- 2026-10-09: survey done, backup branch made, Chunk 1 done and committed.
- 2026-10-10: Chunk 2 done and committed. Level cap behaviour finished (separate commit). Chunk 3 done; the ROM builds. Chunk 4 done (hooks and dialogue); the follower is live, so testing is next. Nuzlocke rules added (separate commit).
- 2026-10-10: Trainer AI levels (Simple / Normal / Advanced as trainer data) added as a separate commit. Builds; not play-tested.
- 2026-10-10: Play-testing fixes: follower door exit placement, Mom's house (map header + coord triggers), option menu CANCEL row. NOTE: VisualBoyAdvance was writing the 128K save as 64K (sectors alias), which mixes old and new save data and caused the black quest-log recap and a crash on continue. Set the emulator save type to Flash 128K (or use mGBA) and start a new save.
