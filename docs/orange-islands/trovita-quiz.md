# Trovita Gym — Rudy's panels

Six hedges, six panels. Each panel holds **two** questions and picks one at
random, so a second run is not the same six answers.

No answer below is written down anywhere. `_follower_port_work/patches/`
derives each one from the data the battle engine itself uses, and refuses to
emit a question whose four options do not yield exactly one answer — which is
the failure that actually matters, since a plausible-looking option set with
two right answers is not something you can spot by eye.

| Panel | Kind | Asks | Options | Answer | Derived from |
| --- | --- | --- | --- | --- | --- |
| 1A | type | Water/Flying: which type hits hardest | Rock ×2 · Ice ×1 · **Electric ×4** · Grass ×1 | Electric | `gTypeEffectiveness` (src/battle_main.c) |
| 1B | type | Bug/Steel: which type hits hardest | Fighting ×1 · **Fire ×4** · Ground ×1 · Flying ×1 | Fire | `gTypeEffectiveness` (src/battle_main.c) |
| 2A | type | Grass/Dragon: which type hits hardest | Fire ×1 · Bug ×2 · Dragon ×2 · **Ice ×4** | Ice | `gTypeEffectiveness` (src/battle_main.c) |
| 2B | type | Water/Ground: which type hits hardest | **Grass ×4** · Electric ×0 · Ice ×1 · Water ×1 | Grass | `gTypeEffectiveness` (src/battle_main.c) |
| 3A | type | Normal/Flying: which type does nothing | Rock ×2 · Fighting ×1 · Bug ×0.5 · **Ghost ×0** | Ghost | `gTypeEffectiveness` (src/battle_main.c) |
| 3B | type | Rock/Ground: which type it resists most | **Poison ×0.2** · Fighting ×2 · Ice ×2 · Grass ×4 | Poison | `gTypeEffectiveness` (src/battle_main.c) |
| 4A | move status | paralysis: which move inflicts it | Razor Leaf · **Stun Spore** · Absorb · Growth | Stun Spore | `.effect` in src/data/battle_moves.h |
| 4B | move status | burn: which move inflicts it | **Will O Wisp** · Confuse Ray · Night Shade · Spite | Will O Wisp | `.effect` in src/data/battle_moves.h |
| 5A | ability vs move | Limber: which move the ability wastes | Toxic · **Thunder Wave** · Sleep Powder · Will O Wisp | Thunder Wave | move effects + the ABILITYEFFECT_IMMUNITY switch in src/battle_util.c |
| 5B | ability vs move | Water Veil: which move the ability wastes | Stun Spore · Spore · **Will O Wisp** · Poison Powder | Will O Wisp | move effects + the ABILITYEFFECT_IMMUNITY switch in src/battle_util.c |
| 6A | move vs ability | Thunder Wave: which ability blocks the move | Insomnia · Immunity · **Limber** · Magma Armor | Limber | move effects + the ABILITYEFFECT_IMMUNITY switch in src/battle_util.c |
| 6B | berry | paralysis: which berry cures it | Pecha · Rawst · **Cheri** · Aspear | Cheri | `holdEffect` in src/data/items.json |

## What the generator reads

| Fact | Source | How |
| --- | --- | --- |
| type matchups | `src/battle_main.c` | the `gTypeEffectiveness` triples |
| what a move inflicts | `src/data/battle_moves.h` | each move's `.effect` |
| what an ability blocks | `src/battle_util.c` | the `case ABILITY_x:` arms of the `ABILITYEFFECT_IMMUNITY` switch, matched to their `STATUS1_`/`STATUS2_` test |
| what a berry cures | `src/data/items.json` | `holdEffect` |

Option text is not duplicated either: the menus point straight at `gTypeNames`,
`gMoveNames`, `gAbilityNames` and `gItems[].name`, so a renamed move renames
itself in the quiz.

What the ROM actually carries is an ordinary `multichoice` list and a
`goto_if_eq VAR_RESULT, n`. None of the above runs on the GBA.

## Abilities and berries, as parsed

| Ability | Blocks |
| --- | --- |
| Immunity | poison |
| Insomnia | sleep |
| Limber | paralysis |
| Magma Armor | freeze |
| Own Tempo | confusion |
| Vital Spirit | sleep |
| Water Veil | burn |

| Berry | Cures |
| --- | --- |
| Rawst | burn |
| Persim | confusion |
| Aspear | freeze |
| Cheri | paralysis |
| Pecha | poison |
| Chesto | sleep |
