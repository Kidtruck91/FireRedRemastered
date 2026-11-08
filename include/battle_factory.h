#ifndef GUARD_BATTLE_FACTORY_H
#define GUARD_BATTLE_FACTORY_H

struct FacilityMon
{
    u16 species;
    u16 moves[MAX_MON_MOVES];
    u8 itemTableId;
    u8 evSpread;
    u8 nature;
};

// Emerald's entry also carries three easy-chat speeches and a monSet. Neither
// is reachable here: the speeches belong to frontier dialogue code that is not
// part of this port, and a factory opponent's team comes from the rental pool
// rather than the trainer's own set. So the entry is the two fields that are
// actually read.
struct BattleFrontierTrainer
{
    u8 facilityClass;
    u8 trainerName[PLAYER_NAME_LENGTH + 1];
};

extern const struct FacilityMon gBattleFrontierMons[];
extern const struct BattleFrontierTrainer gBattleFrontierTrainers[];
extern const struct BattleFrontierTrainer *gFacilityTrainers;
extern const struct FacilityMon *gFacilityTrainerMons;
extern const u16 gBattleFrontierHeldItems[];

u16 GetRandomScaledFrontierTrainerId(u8 challengeNum, u8 battleNum);
void SetBattleFacilityTrainerGfxId(u16 trainerId, u8 tempVarId);
u8 SetFacilityPtrsGetLevel(void);
void CreateMonWithEVSpreadNatureOTID(struct Pokemon *mon, u16 species, u8 level,
                                     u8 nature, u8 fixedIV, u8 evSpread, u32 otId);

void CallBattleFactoryFunction(void);
bool8 InBattleFactory(void);
u8 GetFactoryMonFixedIV(u8 challengeNum, bool8 isLastBattle);
u8 GetNumPastRentalsRank(void);
u32 GetAiScriptsInBattleFactory(void);
void SetMonMoveAvoidReturn(struct Pokemon *mon, u16 moveArg, u8 moveSlot);
void FillFactoryOpponentParty(void);

#endif // GUARD_BATTLE_FACTORY_H
