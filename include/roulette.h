#ifndef GUARD_ROULETTE_H
#define GUARD_ROULETTE_H

void PlayRoulette(void);

// Emerald declares these in strings.h; they are defined in
// data/scripts/roulette.inc, which travels with this feature.
extern const u8 Roulette_Text_ControlsInstruction[];
extern const u8 Roulette_Text_KeepPlaying[];
extern const u8 Roulette_Text_Jackpot[];
extern const u8 Roulette_Text_ItsAHit[];
extern const u8 Roulette_Text_NothingDoing[];
extern const u8 Roulette_Text_YouveWonXCoins[];
extern const u8 Roulette_Text_BoardWillBeCleared[];
extern const u8 Roulette_Text_CoinCaseIsFull[];
extern const u8 Roulette_Text_NoCoinsLeft[];
extern const u8 Roulette_Text_PlayMinimumWagerIsX[];
extern const u8 Roulette_Text_SpecialRateTable[];
extern const u8 Roulette_Text_NotEnoughCoins[];
extern const u8 Roulette_Text_YouDontHaveACoinCase[];

#endif // GUARD_ROULETTE_H
