# Trovita Gym — Rudy's type quiz

Six hedge gates, one question each. The answers below are not written from
memory: `_follower_port_work/patches/trovita_gym.py` reads `gTypeEffectiveness`
out of `src/battle_main.c` and works each one out, and refuses to emit a
question whose options do not have exactly one answer. Re-run the generator
after any change to the type chart and it will fail loudly rather than ship a
quiz that disagrees with the battle engine.

| # | Defender | Asks for | Options and multipliers | Answer |
| - | -------- | -------- | ----------------------- | ------ |
| 1 | Water/Flying | the hardest hit | Rock ×2 · Ice ×1 · **Electric ×4** · Grass ×1 | Electric |
| 2 | Bug/Steel | the hardest hit | Fighting ×1 · **Fire ×4** · Ground ×1 · Flying ×1 | Fire |
| 3 | Grass/Dragon | the hardest hit | Fire ×1 · Bug ×2 · Dragon ×2 · **Ice ×4** | Ice |
| 4 | Water/Ground | the hardest hit | **Grass ×4** · Electric ×0 · Ice ×1 · Water ×1 | Grass |
| 5 | Normal/Flying | the type that does nothing | Rock ×2 · Fighting ×1 · Bug ×0.5 · **Ghost ×0** | Ghost |
| 6 | Rock/Ground | what it resists most | **Poison ×0.2** · Fighting ×2 · Ice ×2 · Grass ×4 | Poison |

## Why these six

They escalate, and each one punishes reading only the first type:

- **1–3** are clean 4x answers: Electric on Water/Flying, Fire on Bug/Steel,
  Ice on Grass/Dragon. The third already has a trap — Fire doubles against
  Grass and halves against Dragon, so it comes out at plain neutral.
- **4** is the classic. Electric looks like the obvious pick against something
  part Water, and Ground makes it do nothing at all.
- **5** asks for an immunity rather than a weakness.
- **6** asks which type the defender *resists* hardest, and the answer is a
  quarter-damage case: Poison is halved by Rock and halved again by Ground.

## Spares

Verified but unused, if more gates are ever wanted: Fire/Flying → Rock (4x),
Dragon/Flying → Ice (4x), Ghost/Poison → Psychic (Normal and Fighting both do
nothing, Bug is quartered), Ice/Flying → Rock (4x), Ghost/Dark → Bug.
