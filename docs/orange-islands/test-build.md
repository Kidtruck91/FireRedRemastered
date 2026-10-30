# Test build: start on Valencia with everything

```sh
make ORANGE_TEST=1        # -> pokefirered_orangetest.gba
make                      # the real build, untouched
```

**`ORANGE_TEST=1` needs two lines in the `Makefile` that are not committed**,
because the Makefile is protected against remote writes. Add them directly
under the `CPPFLAGS :=` line (currently line 64):

```make
CPPFLAGS := -iquote include -D$(GAME_VERSION) -DREVISION=$(GAME_REVISION) -D$(GAME_LANGUAGE) -DMODERN=$(MODERN)
ifeq ($(ORANGE_TEST),1)
CPPFLAGS += -DORANGE_TEST_START
endif
```

The matching `BUILD_NAME` half is already in `config.mk`. Until those two
lines exist, `make ORANGE_TEST=1` builds a ROM named
`pokefirered_orangetest.gba` that is really just the normal game — the define
never reaches the compiler, so the test start compiles to nothing.

`ORANGE_TEST=1` sets `-DORANGE_TEST_START` **and** changes `BUILD_NAME`, so
the two builds never share object files and a plain `make` is unaffected.
Every line of `src/orange_test_start.c` is inside the `#ifdef`; the normal ROM
contains none of its symbols.

## What a new game gives you

Start a new game as usual (name and gender still get asked). Instead of your
bedroom in Pallet Town you appear on **Valencia, outside the Pokémon Center**
at (51,45) — the same square the Center sets as its respawn point.

**Party**, level 50. Between them they know all eight HM moves, written
straight into the move slots:

| | moves |
|---|---|
| LAPRAS | Surf, Dive, Waterfall, Ice Beam |
| PIDGEOT | Fly, Cut, Wing Attack, Quick Attack |
| GOLEM | Strength, Rock Smash, Flash, Earthquake |
| ARCANINE | Flamethrower, Bite, Extreme Speed, Roar |
| ALAKAZAM | Psychic, Shadow Ball, Calm Mind, Recover |
| SNORLAX | Body Slam, Earthquake, Rest, Shadow Ball |

**Bag**: HM01–HM08, 50 Poké / 40 Great / 40 Ultra / 20 Dive / 20 Net Balls,
50 Rare Candies, 30 Full Restores, 20 Revives, 20 Max Repels, 10 Escape
Ropes, ¥99999. The TM Case arrives with the HMs on its own — `AddBagItem`
adds it when something lands in the TM pocket and it is missing.

**Badges**: all eight Kanto badges and all four Orange ones. The gyms
themselves are untouched, because their scripts check `goto_if_defeated`
rather than the badge — so you can still fight Cissy, Danny, Rudy, Luana and
Drake. What the badges unlock is the HM gating, including Dive's own gate on
the Coral-Eye Badge.

**State**: `VAR_ORANGE_STORY` is 3, the point just after taking a starter from
Prof. Ivy, with her dock scene and her three starter balls already hidden.
Without that the dock trigger fires the moment you step on Valencia and drags
you into the lab. The Orange PC and Orange team are set up the way
`OrangeChallenge_Begin` leaves them.

## The one thing that does not work

**The ferry home to Kanto.** `OrangeChallenge_End` reads the Kanto PC back out
of flash, and in this build one was never written — the trip starts already
begun rather than being entered from Vermilion. `LoadStoredPC` checks every
sector's signature and checksum before copying anything, so it fails cleanly
and leaves the save alone; the trip just refuses to end. Nothing is lost, but
don't expect to test Kanto from this save.

Everything inside the Orange Islands is unaffected.
