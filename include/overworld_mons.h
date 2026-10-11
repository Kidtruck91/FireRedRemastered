#ifndef GUARD_OVERWORLD_MONS_H
#define GUARD_OVERWORLD_MONS_H

// Visible wild Pokemon that wander around on routes (src/overworld_mons.c).

void OverworldMons_OnStep(void);
bool8 OverworldMon_IsOverworldMonObjectEventId(u8 objectEventId);
bool8 OverworldMon_IsOverworldMonLocalId(u8 localId);
bool8 OverworldMon_TryWalkInto(u8 direction);
void OverworldMon_PrepareBattle(void);
void OverworldMon_RemoveTalked(void);
void OverworldMon_UpdateObject(struct ObjectEvent *objectEvent);

#endif // GUARD_OVERWORLD_MONS_H
