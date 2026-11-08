#ifndef GUARD_ORANGE_BATTLE_BOAT_H
#define GUARD_ORANGE_BATTLE_BOAT_H

// The KINNOW battle boat's arena. See src/orange_battle_boat.c.

bool8 OrangeBoat_IsLeaderRound(void);
u8 OrangeBoat_OpponentClass(void);
void OrangeBoat_OpponentName(u8 *dest);
u8 OrangeBoat_OpponentPicId(void);
void OrangeBoat_OpponentWinText(u8 *dest);
void OrangeBoat_OpponentLoseText(u8 *dest);
void OrangeBoat_StartBattle(void);
void OrangeBoat_PrepareRound(void);
void OrangeBoat_RoundWon(void);
void OrangeBoat_SettleUp(void);
void OrangeBoat_GetLeaderName(void);

#endif // GUARD_ORANGE_BATTLE_BOAT_H
