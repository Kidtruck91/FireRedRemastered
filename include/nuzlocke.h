#ifndef GUARD_NUZLOCKE_H
#define GUARD_NUZLOCKE_H

// Nuzlocke mode rules (src/nuzlocke.c). Nuzlocke mode is on when FLAG_NUZLOCKE is set.

bool8 Nuzlocke_IsActive(void);

// "Fainted = dead": a Pokemon that faints is marked dead and can't be healed or revived.
bool8 Nuzlocke_IsMonDead(struct Pokemon *mon);
bool8 Nuzlocke_IsBoxMonDead(struct BoxPokemon *boxMon);
void Nuzlocke_SetMonDead(struct Pokemon *mon);
void Nuzlocke_MarkFaintedPartyMonsDead(void);
bool8 Nuzlocke_ItemBlocked(struct Pokemon *mon, u16 item);
void Nuzlocke_OnItemUsed(struct Pokemon *mon, u16 item);

// One encounter per area: the first wild battle in each area uses it up.
bool8 Nuzlocke_IsCurrentAreaUsed(void);
bool8 Nuzlocke_ShouldWarnBeforeCatch(void);
void Nuzlocke_OnBattleEnd(void);
void Nuzlocke_ResetAreas(void);

// Called when the run is lost (whiteout): clears the dead marks so the game can go on normally
void Nuzlocke_EndRun(void);

#endif // GUARD_NUZLOCKE_H
