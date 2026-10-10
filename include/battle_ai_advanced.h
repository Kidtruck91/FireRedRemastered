#ifndef GUARD_BATTLE_AI_ADVANCED_H
#define GUARD_BATTLE_AI_ADVANCED_H

// "Advanced" trainer AI (AI_LEVEL_ADVANCED / AI_SCRIPT_ADVANCED).
// It runs on top of the normal script AI and adds:
//  - a memory of every move each of the player's party members has used this battle
//  - damage-calc based kill priority (take a KO it is sure to land, prefer priority finishers)
//  - setup valuation (boost when the player can't threaten it, stop when boosted enough)
//  - switch prediction (if the player is in a bad matchup it may aim for the likely switch-in)
//  - smarter switching for its own team based on the moves it has seen

u32 AI_GetTrainerAIFlags(void);
bool8 AI_IsAdvanced(void);

void AI_ResetPlayerMoveMemory(void);
void AI_RecordPlayerMove(u8 battler, u16 move);
void AI_ObservePlayerSide(void);
void AI_LoadTargetMoveHistory(u8 target);

void AI_AdvancedScoreMoves(u8 attacker, u8 target, s8 *scores);
bool8 AI_AdvancedShouldSwitch(void);
u8 AI_AdvancedGetBestSwitchIn(void);

#endif // GUARD_BATTLE_AI_ADVANCED_H
