// The BATTLE FACTORY's two screens: the draft, where you pick three rentals
// out of six, and the swap, where you trade one of yours for one of the team
// you just beat.
//
// Stage 4 of the port. Emerald's battle_factory_screen.c is four thousand
// lines of bespoke UI and it is not here yet, so these are the two entry
// points the challenge calls, stubbed to hand control straight back.
//
// Stubbed they are not harmless, so they do not pretend to be: with no draft
// the player keeps whatever ZeroPlayerPartyMons left them, and with no swap
// the team never changes. The challenge around them -- rentals, opponents,
// streaks, difficulty scaling -- is complete and running; this is the part
// that lets a player drive it.

#include "global.h"
#include "battle_factory_screen.h"
#include "main.h"
#include "overworld.h"

void DoBattleFactorySelectScreen(void)
{
    SetMainCallback2(CB2_ReturnToFieldContinueScript);
}

void DoBattleFactorySwapScreen(void)
{
    SetMainCallback2(CB2_ReturnToFieldContinueScript);
}
