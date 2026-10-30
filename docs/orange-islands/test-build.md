# Test build: start in Vermilion with the Sunkist Ticket

```sh
make ORANGE_TEST=1        # -> pokefirered_orangetest.gba
make                      # the real build, untouched
```

`ORANGE_TEST=1` sets `-DORANGE_TEST_START` **and** changes `BUILD_NAME`, so
the two builds never share object files and a plain `make` is unaffected.
Every line of `src/orange_test_start.c` is inside the `#ifdef`; the normal ROM
contains none of its symbols.

**Two lines of the switch are not committed**, because the `Makefile` is
protected against remote writes. Add them directly under the `CPPFLAGS :=`
line (currently line 64):

```make
CPPFLAGS := -iquote include -D$(GAME_VERSION) -DREVISION=$(GAME_REVISION) -D$(GAME_LANGUAGE) -DMODERN=$(MODERN)
ifeq ($(ORANGE_TEST),1)
CPPFLAGS += -DORANGE_TEST_START
endif
```

The matching `BUILD_NAME` half is already in `config.mk`. Until those two
lines exist, `make ORANGE_TEST=1` builds a ROM with the test name that is
really just the normal game — the define never reaches the compiler.

## What a new game gives you

Start a new game as usual (name and gender still get asked). Instead of your
bedroom in Pallet Town you appear in **Vermilion City at (23,31)**, one tile
north of the Seagallop ticket gate. Walk down and the sailor asks about the
Sunkist Ticket; say yes twice and you sail to Valencia, arriving on the dock
at (19,30) — the square Prof. Ivy's arrival scene is written around.

**Nothing about the Orange Islands story is pre-set.** `VAR_ORANGE_STORY` is
left at 0 on purpose, so the whole chain runs for real: Ivy meets you at the
dock, walks you to her lab, `OrangeChallenge_Begin` stores your Kanto team and
PC, and you pick one of her three starters.

**Party**, level 50 — this is the *home* team, the one Ivy stores:

| | moves |
|---|---|
| LAPRAS | Surf, Dive, Waterfall, Ice Beam |
| PIDGEOT | Fly, Cut, Wing Attack, Quick Attack |
| GOLEM | Strength, Rock Smash, Flash, Earthquake |
| ARCANINE | Flamethrower, Bite, Extreme Speed, Roar |
| ALAKAZAM | Psychic, Shadow Ball, Calm Mind, Recover |
| SNORLAX | Body Slam, Earthquake, Rest, Shadow Ball |

Between them they know all eight HM moves, written straight into the move
slots. They are there to make the Kanto leg playable and to give the team
swap something real to move; **on the Orange side you start from Ivy's
starter, as the real game intends.**

**Bag**: Sunkist Ticket, HM01–HM08, 50 Poké / 40 Great / 40 Ultra / 20 Dive /
20 Net Balls, 50 Rare Candies, 30 Full Restores, 20 Revives, 20 Max Repels,
10 Escape Ropes, ¥99999. The TM Case arrives with the HMs on its own —
`AddBagItem` adds it when something lands in the TM pocket and it is missing.

**The bag is not swapped by the trip**, only the party and the PC. So the
HMs, the candies and the 150 balls all come with you, and a level 10 starter
plus 50 Rare Candies gets to roughly level 60 before you have caught anything.

**Badges**: all eight Kanto badges and all four Orange ones. The gyms
themselves are untouched, because their scripts check `goto_if_defeated`
rather than the badge — so Cissy, Danny, Rudy, Luana and Drake are all still
there to fight. What the badges unlock is HM gating, including Dive's own
gate on the Coral-Eye Badge.

**Vermilion**: `VAR_MAP_SCENE_VERMILION_CITY` is 3, meaning the S.S. Anne has
sailed and the Seagallop dock is open. The ferry sailor and the pier trigger
both check that before they will look at a ticket at all.

## Notes

The party is built with `CreateMon` straight into the party slots rather than
`ScriptGiveMon`, which allocates — this runs inside `NewGameInitData`. And it
runs last in that function, after `EventScript_ResetAllMapFlags` would have
cleared the flags and after `WarpToPlayersRoom` set Pallet Town.

Because the trip is entered the normal way, through Ivy's lab, the save stays
fully consistent: `OrangeChallenge_Begin` writes the Kanto PC and home team to
flash properly, so the ferry home works and ending the trip gives the team
back.
