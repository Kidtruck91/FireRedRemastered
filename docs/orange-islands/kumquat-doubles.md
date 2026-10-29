# Kumquat Gym — Luana's double battles

Every fight here is a double, and each of the three is built around one idea the
single-battle game never makes you think about. Levels and items are unchanged in
spirit; what changed is that the moves and abilities now do something together.

## Twins Kumquat Ana Bel — A spread move, and a partner who doesn't care

DUGTRIO's EARTHQUAKE is `MOVE_TARGET_FOES_AND_ALLY` — it hits its own side too. DODRIO is Flying, so it takes none of it, and ANA can swing wide every single turn. ARENA TRAP stops you switching away from it.

| Pokémon | Lv | Ability | Item | Moves |
| --- | --- | --- | --- | --- |
| Dugtrio | 41 | Arena Trap | Soft Sand | Earthquake, Rock Slide, Sand Attack, Slash |
| Dodrio | 41 | Early Bird | Sharp Beak | Drill Peck, Tri Attack, Steel Wing, Agility |

## Cool Couple Kumquat Lex Nya — Weather, then a spread that does spare the partner

One SUNNY DAY does three jobs: FIRE BLAST grows, SOLARBEAM loses its charge turn, and CHLOROPHYLL doubles VILEPLUME's speed. The back pair switch idea — GOLDUCK's SURF is `MOVE_TARGET_BOTH`, so it hits both of yours and neither of theirs, and PERSIAN opens with FAKE OUT. GOLDUCK is pinned to its *first* ability on purpose: its second is CLOUD NINE, which would cancel its own partner's sun.

| Pokémon | Lv | Ability | Item | Moves |
| --- | --- | --- | --- | --- |
| Rapidash | 42 | Flash Fire | Charcoal | Sunny Day, Fire Blast, Stomp, Agility |
| Vileplume | 42 | Chlorophyll | Miracle Seed | Solar Beam, Sludge Bomb, Sleep Powder, Giga Drain |
| Golduck | 42 | Damp | Mystic Water | Surf, Ice Beam, Psychic, Disable |
| Persian | 42 | Limber | Silk Scarf | Fake Out, Slash, Screech, Bite |

## Leader Luana — Support and control, and never a move that hits her own side

MAROWAK takes LIGHTNING ROD, so it pulls Electric attacks off whoever stands beside it. Both it and STEELIX carry ROCK SLIDE rather than EARTHQUAKE — both foes, neither ally. KANGASKHAN's INNER FOCUS means your own FAKE OUT does nothing to her lead, and ALAKAZAM puts up REFLECT for the pair.

| Pokémon | Lv | Ability | Item | Moves |
| --- | --- | --- | --- | --- |
| Kangaskhan | 42 | Inner Focus | Silk Scarf | Fake Out, Body Slam, Crunch, Brick Break |
| Steelix | 42 | Sturdy | Metal Coat | Rock Slide, Iron Tail, Dragon Breath, Screech |
| Marowak | 43 | Lightning Rod | Thick Club | Bonemerang, Rock Slide, Swords Dance, Headbutt |
| Alakazam | 44 | Magic Guard | Twisted Spoon | Psychic, Calm Mind, Reflect, Recover |

## The check

`_follower_port_work/patches/kumquat_check.py` reads the move targets out of
`src/data/battle_moves.h` and the ability slots out of the species table, and
fails if either of the two things that actually break a double battle is true:

- a `MOVE_TARGET_FOES_AND_ALLY` move lands on a partner that is not immune to it
- a pinned ability slot resolves to something other than the one the fight needs

That is how DUGTRIO's EARTHQUAKE is allowed through and nothing else is, and how
GOLDUCK is kept off CLOUD NINE.

| Spread move | Lands on | Result |
| --- | --- | --- |
| DUGTRIO EARTHQUAKE | DODRIO | ×0 — deliberate |
| GOLDUCK SURF | both foes only | partner untouched |
| STEELIX / MAROWAK ROCK SLIDE | both foes only | partner untouched |
