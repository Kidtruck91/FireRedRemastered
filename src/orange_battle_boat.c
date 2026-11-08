#include "global.h"
#include "gflib.h"
#include "battle.h"
#include "battle_factory.h"
#include "battle_main.h"
#include "battle_setup.h"
#include "battle_transition.h"
#include "event_data.h"
#include "money.h"
#include "overworld.h"
#include "random.h"
#include "task.h"
#include "constants/battle_factory.h"
#include "constants/items.h"
#include "constants/moves.h"
#include "constants/species.h"
#include "constants/trainers.h"

// The KINNOW battle boat's arena.
//
// The challenge itself is Emerald's BATTLE FACTORY, ported in battle_factory.c
// and battle_factory_screen.c: you are shown six rented POKEMON, keep three,
// and after each win may trade one of yours for one you have just beaten.
// What this file adds is the three things the boat does differently.
//
// It runs the battle.  Emerald starts a factory battle through the frontier's
// DoSpecialTrainerBattle, which FireRed has no equivalent of. FireRed's own
// TRAINER TOWER does the same job with the same battle type bit -- 1 << 19 is
// BATTLE_TYPE_TRAINER_TOWER here and BATTLE_TYPE_FACTORY in Emerald -- so
// OrangeBoat_StartBattle is DoTrainerTowerBattle with the tower's party build
// swapped for the factory's.
//
// Rounds four and eight are GYM LEADERS.  Three rented rounds, a leader,
// three more, a leader. Those two are HOENN leaders with full six-strong
// teams of their own type at level fifty, and they do not use rental POKEMON
// -- that is the point of those rounds. The pair is drawn at random when the
// run starts, never the same leader twice in one run, and kept in the save so
// it survives a challenge saved part-way through.
//
// The house pays by depth.  A purse per round cleared, and a bonus on top for
// each leader taken down.
//
// Because the battle runs on the TRAINER TOWER's battle type, the five places
// battle_message.c and battle_controller_opponent.c ask the tower who it is
// fighting have to ask this file first when a factory challenge is running.
// InBattleFactory() is the switch, and trainer_tower.c does the asking.

struct BoatLeaderMon
{
    u16 species;
    u16 moves[MAX_MON_MOVES];
    u16 item;
    u8 abilityNum;
};

struct BoatLeader
{
    const u8 *name;
    u8 trainerPic;
    u8 count;
    const struct BoatLeaderMon *party;
};

#define LEADER_LEVEL  FACTORY_MON_LEVEL   // fifty, the same as the rentals

// A leader brings a full team of their own type. The player has three rented
// POKEMON against six, which is the shape of these two rounds and the reason
// they pay what they pay.

static const struct BoatLeaderMon sParty_Roxanne[] = {   // ROCK
    { SPECIES_NOSEPASS,  {MOVE_ROCK_SLIDE, MOVE_THUNDER_WAVE, MOVE_EARTHQUAKE, MOVE_REST}, ITEM_LEFTOVERS, 0 },
    { SPECIES_SUDOWOODO, {MOVE_ROCK_SLIDE, MOVE_EARTHQUAKE, MOVE_SLAM, MOVE_TOXIC}, ITEM_SITRUS_BERRY, 0 },
    { SPECIES_GOLEM,     {MOVE_ROCK_SLIDE, MOVE_EARTHQUAKE, MOVE_EXPLOSION, MOVE_PROTECT}, ITEM_QUICK_CLAW, 1 },
    { SPECIES_ARMALDO,   {MOVE_ROCK_SLIDE, MOVE_BRICK_BREAK, MOVE_SWORDS_DANCE, MOVE_EARTHQUAKE}, ITEM_CHOICE_BAND, 0 },
    { SPECIES_CRADILY,   {MOVE_GIGA_DRAIN, MOVE_ANCIENT_POWER, MOVE_CONFUSE_RAY, MOVE_TOXIC}, ITEM_LEFTOVERS, 0 },
    { SPECIES_AGGRON,    {MOVE_ROCK_SLIDE, MOVE_EARTHQUAKE, MOVE_IRON_TAIL, MOVE_ROAR}, ITEM_LUM_BERRY, 0 },
};

static const struct BoatLeaderMon sParty_Brawly[] = {    // FIGHTING
    { SPECIES_MACHAMP,   {MOVE_CROSS_CHOP, MOVE_ROCK_SLIDE, MOVE_EARTHQUAKE, MOVE_BULK_UP}, ITEM_LEFTOVERS, 1 },
    { SPECIES_HARIYAMA,  {MOVE_BRICK_BREAK, MOVE_ROCK_SLIDE, MOVE_FAKE_OUT, MOVE_BELLY_DRUM}, ITEM_SITRUS_BERRY, 0 },
    { SPECIES_BRELOOM,   {MOVE_SPORE, MOVE_SKY_UPPERCUT, MOVE_MACH_PUNCH, MOVE_LEECH_SEED}, ITEM_QUICK_CLAW, 0 },
    { SPECIES_MEDICHAM,  {MOVE_BRICK_BREAK, MOVE_SHADOW_BALL, MOVE_CALM_MIND, MOVE_RECOVER}, ITEM_LUM_BERRY, 0 },
    { SPECIES_HITMONLEE, {MOVE_HI_JUMP_KICK, MOVE_EARTHQUAKE, MOVE_REVERSAL, MOVE_MIMIC}, ITEM_CHOICE_BAND, 0 },
    { SPECIES_HERACROSS, {MOVE_MEGAHORN, MOVE_BRICK_BREAK, MOVE_ROCK_SLIDE, MOVE_SWORDS_DANCE}, ITEM_LEFTOVERS, 0 },
};

static const struct BoatLeaderMon sParty_Wattson[] = {   // ELECTRIC
    { SPECIES_MAGNETON,  {MOVE_THUNDERBOLT, MOVE_THUNDER_WAVE, MOVE_SUBSTITUTE, MOVE_HIDDEN_POWER}, ITEM_LEFTOVERS, 0 },
    { SPECIES_ELECTRODE, {MOVE_THUNDERBOLT, MOVE_EXPLOSION, MOVE_THUNDER_WAVE, MOVE_LIGHT_SCREEN}, ITEM_SITRUS_BERRY, 0 },
    { SPECIES_LANTURN,   {MOVE_SURF, MOVE_THUNDERBOLT, MOVE_ICE_BEAM, MOVE_CONFUSE_RAY}, ITEM_LEFTOVERS, 0 },
    { SPECIES_MANECTRIC, {MOVE_THUNDERBOLT, MOVE_CRUNCH, MOVE_THUNDER_WAVE, MOVE_PROTECT}, ITEM_LUM_BERRY, 0 },
    { SPECIES_AMPHAROS,  {MOVE_THUNDERBOLT, MOVE_FIRE_PUNCH, MOVE_LIGHT_SCREEN, MOVE_REST}, ITEM_QUICK_CLAW, 0 },
    { SPECIES_JOLTEON,   {MOVE_THUNDERBOLT, MOVE_HIDDEN_POWER, MOVE_DOUBLE_TEAM, MOVE_SUBSTITUTE}, ITEM_CHOICE_BAND, 0 },
};

static const struct BoatLeaderMon sParty_Flannery[] = {  // FIRE
    { SPECIES_TORKOAL,   {MOVE_OVERHEAT, MOVE_BODY_SLAM, MOVE_CURSE, MOVE_PROTECT}, ITEM_LEFTOVERS, 0 },
    { SPECIES_MAGCARGO,  {MOVE_FLAMETHROWER, MOVE_ROCK_SLIDE, MOVE_LIGHT_SCREEN, MOVE_REST}, ITEM_SITRUS_BERRY, 0 },
    { SPECIES_NINETALES, {MOVE_FLAMETHROWER, MOVE_CONFUSE_RAY, MOVE_TOXIC, MOVE_PROTECT}, ITEM_LUM_BERRY, 0 },
    { SPECIES_CAMERUPT,  {MOVE_FLAMETHROWER, MOVE_EARTHQUAKE, MOVE_ROCK_SLIDE, MOVE_AMNESIA}, ITEM_QUICK_CLAW, 0 },
    { SPECIES_ARCANINE,  {MOVE_FLAMETHROWER, MOVE_EXTREME_SPEED, MOVE_CRUNCH, MOVE_ROAR}, ITEM_CHOICE_BAND, 0 },
    { SPECIES_HOUNDOOM,  {MOVE_FLAMETHROWER, MOVE_CRUNCH, MOVE_WILL_O_WISP, MOVE_PROTECT}, ITEM_LEFTOVERS, 0 },
};

static const struct BoatLeaderMon sParty_Norman[] = {    // NORMAL
    { SPECIES_LINOONE,    {MOVE_BELLY_DRUM, MOVE_EXTREME_SPEED, MOVE_SHADOW_BALL, MOVE_SURF}, ITEM_SITRUS_BERRY, 0 },
    { SPECIES_EXPLOUD,    {MOVE_HYPER_VOICE, MOVE_OVERHEAT, MOVE_ICE_BEAM, MOVE_REST}, ITEM_LEFTOVERS, 0 },
    { SPECIES_ZANGOOSE,   {MOVE_SWORDS_DANCE, MOVE_BRICK_BREAK, MOVE_SHADOW_BALL, MOVE_QUICK_ATTACK}, ITEM_LUM_BERRY, 0 },
    { SPECIES_KANGASKHAN, {MOVE_DOUBLE_EDGE, MOVE_EARTHQUAKE, MOVE_SHADOW_BALL, MOVE_FAKE_OUT}, ITEM_QUICK_CLAW, 0 },
    { SPECIES_SNORLAX,    {MOVE_BODY_SLAM, MOVE_CURSE, MOVE_SHADOW_BALL, MOVE_REST}, ITEM_LEFTOVERS, 1 },
    { SPECIES_SLAKING,    {MOVE_DOUBLE_EDGE, MOVE_EARTHQUAKE, MOVE_SHADOW_BALL, MOVE_BRICK_BREAK}, ITEM_CHOICE_BAND, 0 },
};

static const struct BoatLeaderMon sParty_Winona[] = {    // FLYING
    { SPECIES_SWELLOW,  {MOVE_AERIAL_ACE, MOVE_QUICK_ATTACK, MOVE_DOUBLE_TEAM, MOVE_FACADE}, ITEM_SITRUS_BERRY, 0 },
    { SPECIES_PELIPPER, {MOVE_SURF, MOVE_ICE_BEAM, MOVE_PROTECT, MOVE_TOXIC}, ITEM_LEFTOVERS, 0 },
    { SPECIES_TROPIUS,  {MOVE_SOLAR_BEAM, MOVE_AERIAL_ACE, MOVE_SYNTHESIS, MOVE_LIGHT_SCREEN}, ITEM_LUM_BERRY, 0 },
    { SPECIES_CROBAT,   {MOVE_AERIAL_ACE, MOVE_SLUDGE_BOMB, MOVE_CONFUSE_RAY, MOVE_TOXIC}, ITEM_QUICK_CLAW, 0 },
    { SPECIES_SKARMORY, {MOVE_DRILL_PECK, MOVE_STEEL_WING, MOVE_SPIKES, MOVE_REST}, ITEM_LEFTOVERS, 0 },
    { SPECIES_ALTARIA,  {MOVE_DRAGON_DANCE, MOVE_AERIAL_ACE, MOVE_EARTHQUAKE, MOVE_REST}, ITEM_CHOICE_BAND, 0 },
};

static const struct BoatLeaderMon sParty_TateAndLiza[] = {  // PSYCHIC
    { SPECIES_XATU,      {MOVE_PSYCHIC, MOVE_CONFUSE_RAY, MOVE_LIGHT_SCREEN, MOVE_REFLECT}, ITEM_SITRUS_BERRY, 0 },
    { SPECIES_LUNATONE,  {MOVE_PSYCHIC, MOVE_ICE_BEAM, MOVE_HYPNOSIS, MOVE_CALM_MIND}, ITEM_LUM_BERRY, 0 },
    { SPECIES_SOLROCK,   {MOVE_PSYCHIC, MOVE_ROCK_SLIDE, MOVE_FLAMETHROWER, MOVE_EXPLOSION}, ITEM_QUICK_CLAW, 0 },
    { SPECIES_CLAYDOL,   {MOVE_PSYCHIC, MOVE_EARTHQUAKE, MOVE_LIGHT_SCREEN, MOVE_REFLECT}, ITEM_LEFTOVERS, 0 },
    { SPECIES_GARDEVOIR, {MOVE_PSYCHIC, MOVE_CALM_MIND, MOVE_THUNDERBOLT, MOVE_RECOVER}, ITEM_LEFTOVERS, 0 },
    { SPECIES_ALAKAZAM,  {MOVE_PSYCHIC, MOVE_CALM_MIND, MOVE_ICE_BEAM, MOVE_RECOVER}, ITEM_CHOICE_BAND, 0 },
};

static const struct BoatLeaderMon sParty_Wallace[] = {   // WATER
    { SPECIES_WHISCASH,  {MOVE_SURF, MOVE_EARTHQUAKE, MOVE_AMNESIA, MOVE_REST}, ITEM_LEFTOVERS, 0 },
    { SPECIES_LUDICOLO,  {MOVE_SURF, MOVE_GIGA_DRAIN, MOVE_ICE_BEAM, MOVE_LEECH_SEED}, ITEM_SITRUS_BERRY, 0 },
    { SPECIES_CRAWDAUNT, {MOVE_SURF, MOVE_CRUNCH, MOVE_BRICK_BREAK, MOVE_SWORDS_DANCE}, ITEM_QUICK_CLAW, 0 },
    { SPECIES_WALREIN,   {MOVE_SURF, MOVE_ICE_BEAM, MOVE_BODY_SLAM, MOVE_REST}, ITEM_LUM_BERRY, 1 },
    { SPECIES_GYARADOS,  {MOVE_DRAGON_DANCE, MOVE_EARTHQUAKE, MOVE_DOUBLE_EDGE, MOVE_HYPER_BEAM}, ITEM_CHOICE_BAND, 0 },
    { SPECIES_MILOTIC,   {MOVE_SURF, MOVE_ICE_BEAM, MOVE_RECOVER, MOVE_TOXIC}, ITEM_LEFTOVERS, 0 },
};

static const u8 sName_Roxanne[] = _("ROXANNE");
static const u8 sName_Brawly[] = _("BRAWLY");
static const u8 sName_Wattson[] = _("WATTSON");
static const u8 sName_Flannery[] = _("FLANNERY");
static const u8 sName_Norman[] = _("NORMAN");
static const u8 sName_Winona[] = _("WINONA");
static const u8 sName_TateAndLiza[] = _("TATE&LIZA");
static const u8 sName_Wallace[] = _("WALLACE");

static const struct BoatLeader sBoatLeaders[] = {
    { sName_Roxanne,     TRAINER_PIC_LEADER_ROXANNE,        ARRAY_COUNT(sParty_Roxanne),     sParty_Roxanne },
    { sName_Brawly,      TRAINER_PIC_LEADER_BRAWLY,         ARRAY_COUNT(sParty_Brawly),      sParty_Brawly },
    { sName_Wattson,     TRAINER_PIC_LEADER_WATTSON,        ARRAY_COUNT(sParty_Wattson),     sParty_Wattson },
    { sName_Flannery,    TRAINER_PIC_LEADER_FLANNERY,       ARRAY_COUNT(sParty_Flannery),    sParty_Flannery },
    { sName_Norman,      TRAINER_PIC_LEADER_NORMAN,         ARRAY_COUNT(sParty_Norman),      sParty_Norman },
    { sName_Winona,      TRAINER_PIC_LEADER_WINONA,         ARRAY_COUNT(sParty_Winona),      sParty_Winona },
    { sName_TateAndLiza, TRAINER_PIC_LEADER_TATE_AND_LIZA,  ARRAY_COUNT(sParty_TateAndLiza), sParty_TateAndLiza },
    { sName_Wallace,     TRAINER_PIC_LEADER_WALLACE,        ARRAY_COUNT(sParty_Wallace),     sParty_Wallace },
};

#define LEADER_COUNT ARRAY_COUNT(sBoatLeaders)

static const u8 sText_LeaderWin[] = _("That is what the last round is for.");
static const u8 sText_LeaderLose[] = _("Through all seven. Take the purse.");
static const u8 sText_HouseWin[] = _("The house keeps the round.");
static const u8 sText_HouseLose[] = _("Yours. Pick one of mine on the way out.");

// What the house pays per round survived, and what it adds for the leader.
#define PURSE_PER_ROUND  600
#define PURSE_LEADER     2500

bool8 OrangeBoat_IsLeaderRound(void)
{
    u8 round = gSaveBlock2Ptr->factory.curChallengeBattleNum;

    return round == FACTORY_LEADER_ROUND_A || round == FACTORY_LEADER_ROUND_B;
}

static const struct BoatLeader *LeaderForSlot(u8 slot)
{
    return &sBoatLeaders[gSaveBlock2Ptr->factory.leaders[slot] % LEADER_COUNT];
}

// Drawn once, at the desk, before the rentals are brought up. The second is
// re-rolled until it is not the first, so a run never fights the same leader
// twice; with eight to choose from that loop ends quickly.
void OrangeBoat_DrawLeaders(void)
{
    u8 i, j;

    for (i = 0; i < FACTORY_LEADERS_PER_RUN; i++)
    {
        do
        {
            gSaveBlock2Ptr->factory.leaders[i] = Random() % LEADER_COUNT;
            for (j = 0; j < i; j++)
            {
                if (gSaveBlock2Ptr->factory.leaders[j]
                    == gSaveBlock2Ptr->factory.leaders[i])
                    break;
            }
        } while (j != i);
    }
}

// Away from a leader round this names the next one up, which is what the desk
// wants when it is telling you who is waiting.
static const struct BoatLeader *CurrentLeader(void)
{
    return LeaderForSlot(gSaveBlock2Ptr->factory.curChallengeBattleNum
                         > FACTORY_LEADER_ROUND_A ? 1 : 0);
}

// The five questions the battle asks the TRAINER TOWER, answered for the boat.
// Away from the leader round the opponent is an ordinary frontier trainer, so
// these read the factory's own trainer list.

u8 OrangeBoat_OpponentClass(void)
{
    if (OrangeBoat_IsLeaderRound())
        return TRAINER_CLASS_LEADER;
    return gFacilityClassToTrainerClass[
        gFacilityTrainers[gTrainerBattleOpponent_A].facilityClass];
}

void OrangeBoat_OpponentName(u8 *dest)
{
    if (OrangeBoat_IsLeaderRound())
        StringCopy(dest, CurrentLeader()->name);
    else
        StringCopyN(dest, gFacilityTrainers[gTrainerBattleOpponent_A].trainerName,
                    PLAYER_NAME_LENGTH + 1);
}

u8 OrangeBoat_OpponentPicId(void)
{
    if (OrangeBoat_IsLeaderRound())
        return CurrentLeader()->trainerPic;
    return gFacilityClassToPicIndex[
        gFacilityTrainers[gTrainerBattleOpponent_A].facilityClass];
}

void OrangeBoat_OpponentWinText(u8 *dest)
{
    StringCopy(dest, OrangeBoat_IsLeaderRound() ? sText_LeaderWin : sText_HouseWin);
}

void OrangeBoat_OpponentLoseText(u8 *dest)
{
    StringCopy(dest, OrangeBoat_IsLeaderRound() ? sText_LeaderLose : sText_HouseLose);
}

// The leader's team, written rather than rented.

static void BuildLeaderParty(void)
{
    const struct BoatLeader *leader = CurrentLeader();
    u8 i, j;

    ZeroEnemyPartyMons();
    for (i = 0; i < leader->count; i++)
    {
        const struct BoatLeaderMon *mon = &leader->party[i];

        CreateMon(&gEnemyParty[i], mon->species, LEADER_LEVEL, MAX_PER_STAT_IVS,
                  FALSE, 0, OT_ID_PLAYER_ID, 0);
        for (j = 0; j < MAX_MON_MOVES; j++)
            SetMonMoveAvoidReturn(&gEnemyParty[i], mon->moves[j], j);
        SetMonData(&gEnemyParty[i], MON_DATA_HELD_ITEM, &mon->item);
        SetMonData(&gEnemyParty[i], MON_DATA_ABILITY_NUM, &mon->abilityNum);
        CalculateMonStats(&gEnemyParty[i]);
    }
}

static void Task_StartBoatBattle(u8 taskId)
{
    if (IsBattleTransitionDone() == TRUE)
    {
        gMain.savedCallback = CB2_ReturnToFieldContinueScriptPlayMapMusic;
        CleanupOverworldWindowsAndTilemaps();
        SetMainCallback2(CB2_InitBattle);
        DestroyTask(taskId);
    }
}

void OrangeBoat_StartBattle(void)
{
    if (OrangeBoat_IsLeaderRound())
        BuildLeaderParty();
    else
        FillFactoryOpponentParty();

    gBattleTypeFlags = BATTLE_TYPE_TRAINER | BATTLE_TYPE_TRAINER_TOWER;
    CreateTask(Task_StartBoatBattle, 1);
    PlayMapChosenOrBattleBGM(0);
    BattleTransition_StartOnField(BattleSetup_GetBattleTowerBattleTransition());
}

// Called at the top of each round, before the battle. Fills in the round
// number and the leader's name for the desk's lines, and answers whether this
// is the seventh.

void OrangeBoat_PrepareRound(void)
{
    ConvertIntToDecimalStringN(gStringVar3,
                               gSaveBlock2Ptr->factory.curChallengeBattleNum + 1,
                               STR_CONV_MODE_LEFT_ALIGN, 1);
    StringCopy(gStringVar2, CurrentLeader()->name);
    gSpecialVar_Result = OrangeBoat_IsLeaderRound();
}

// Bookkeeping between rounds. VAR_RESULT comes back TRUE when that was the
// seventh and the run is over.

void OrangeBoat_RoundWon(void)
{
    // VAR_0x8006 says the round just won was a leader's. The swap screen
    // trades one of yours for one of the team you just beat, and a leader's
    // six were never rentals, so there is nothing there to trade for.
    gSpecialVar_0x8006 = OrangeBoat_IsLeaderRound();

    if (gSaveBlock2Ptr->factory.winStreak < 9999)
        gSaveBlock2Ptr->factory.winStreak++;
    gSaveBlock2Ptr->factory.winStreakActive = TRUE;
    gSaveBlock2Ptr->factory.curChallengeBattleNum++;
    gSpecialVar_Result = gSaveBlock2Ptr->factory.curChallengeBattleNum
                         >= FACTORY_STAGES_PER_CHALLENGE;
}

// What the house pays, and the end of the run. Called once however the run
// ended, so it has to read the round count before clearing it.
//
// A run that fell short takes the streak down with it, which is what moves
// the leader ladder: clear all seven and the next run's seventh round is the
// next leader along, lose and you are back in front of ROXANNE.

void OrangeBoat_SettleUp(void)
{
    u32 cleared = gSaveBlock2Ptr->factory.curChallengeBattleNum;
    u32 purse;

    if (cleared > FACTORY_STAGES_PER_CHALLENGE)
        cleared = FACTORY_STAGES_PER_CHALLENGE;
    purse = cleared * PURSE_PER_ROUND;
    if (cleared > FACTORY_LEADER_ROUND_A)
        purse += PURSE_LEADER;
    if (cleared > FACTORY_LEADER_ROUND_B)
        purse += PURSE_LEADER;

    if (purse != 0)
        AddMoney(&gSaveBlock1Ptr->money, purse);
    ConvertIntToDecimalStringN(gStringVar1, purse, STR_CONV_MODE_LEFT_ALIGN, 6);
    ConvertIntToDecimalStringN(gStringVar3, cleared, STR_CONV_MODE_LEFT_ALIGN, 1);
    gSpecialVar_Result = (purse != 0);

    if (cleared < FACTORY_STAGES_PER_CHALLENGE)
    {
        gSaveBlock2Ptr->factory.winStreak = 0;
        gSaveBlock2Ptr->factory.winStreakActive = FALSE;
    }
    else if (gSaveBlock2Ptr->factory.winStreak > gSaveBlock2Ptr->factory.bestWinStreak)
    {
        gSaveBlock2Ptr->factory.bestWinStreak = gSaveBlock2Ptr->factory.winStreak;
    }
    gSaveBlock2Ptr->factory.curChallengeBattleNum = 0;
    gSaveBlock2Ptr->factory.challengeStatus = FACTORY_CHALLENGE_NONE;
    gSaveBlock2Ptr->factory.rentsCount = 0;
}

// The two leaders this run would put in rounds four and seven, for the desk
// to name before you pay your entry.

void OrangeBoat_GetLeaderName(void)
{
    StringCopy(gStringVar2, LeaderForSlot(0)->name);
    StringCopy(gStringVar3, LeaderForSlot(1)->name);
}
