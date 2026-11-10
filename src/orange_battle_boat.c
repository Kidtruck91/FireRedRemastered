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
#include "data.h"
#include "constants/battle_factory.h"
#include "constants/items.h"
#include "constants/moves.h"
#include "constants/species.h"
#include "constants/trainers.h"
#include "constants/event_objects.h"
#include "constants/maps.h"

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
// Every fourth round is a GYM LEADER.  Three rented rounds, then a leader,
// eight times over: thirty-two battles for a full run, and all eight of
// HOENN's leaders in it. A leader brings a full six of their own type at
// level fifty and does not use rental POKEMON -- that is the point of those
// rounds. The order is drawn at the desk when the run starts, never the same
// leader twice, and kept in the save so it survives a run saved part-way
// through.
//
// The house pays by depth.  A purse per round cleared and a bonus for each
// leader taken down, and you can walk away at the desk between rounds for
// three quarters of what you have earned. Losing pays half. So stopping
// always beats being stopped, and neither beats going on.
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
    const u8 *line;      // what they say walking onto the floor
    u16 gfxId;           // ported from pokeemerald's gym_leaders sheets
    u8 trainerPic;
    u8 mugshotColor;     // the banner behind them in the MUGSHOT transition
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

// HOENN's leaders have no overworld sprites in FireRed, so each gets a stand-in
// for the walk onto the floor. The battle sprite is the real one.
static const u8 sLine_Roxanne[] = _("I still teach. Consider this a\nlesson in what rock will take.");
static const u8 sLine_Brawly[] = _("Big waves out past the harbour\ntoday. You will do instead.");
static const u8 sLine_Wattson[] = _("Wahahah! Mind the floor, it is\nmetal and I am not careful.");
static const u8 sLine_Flannery[] = _("I was told to go easy on the\nguests. I have decided not to.");
static const u8 sLine_Norman[] = _("No speeches. Show me what the\nhouse lent you.");
static const u8 sLine_Winona[] = _("Six of mine never touch the\nground. Let us see yours.");
static const u8 sLine_TateAndLiza[] = _("We agreed on how this goes.\nYou were not part of the talk.");
static const u8 sLine_Wallace[] = _("A boat, of all the places to be\nasked for a match. Delightful.");

// HOENN's ELITE FOUR and its CHAMPION. Nothing draws on these yet: a run is
// eight leaders over thirty-two rounds and stops there. They are written down
// now so that the day the run grows a top floor, the teams are already here.
// Their overworld sprites are the leaders' for the moment - pokeemerald has
// theirs too, and they can come across the same way when they are wanted.

static const struct BoatLeaderMon sParty_Sidney[] = {    // DARK
    { SPECIES_MIGHTYENA, {MOVE_CRUNCH, MOVE_DOUBLE_EDGE, MOVE_ROAR, MOVE_SWAGGER}, ITEM_LUM_BERRY, 0 },
    { SPECIES_SHIFTRY,   {MOVE_FAINT_ATTACK, MOVE_GIGA_DRAIN, MOVE_SWAGGER, MOVE_EXPLOSION}, ITEM_LEFTOVERS, 0 },
    { SPECIES_CACTURNE,  {MOVE_NEEDLE_ARM, MOVE_FAINT_ATTACK, MOVE_LEECH_SEED, MOVE_COTTON_SPORE}, ITEM_SITRUS_BERRY, 0 },
    { SPECIES_CRAWDAUNT, {MOVE_CRABHAMMER, MOVE_CRUNCH, MOVE_SURF, MOVE_SWORDS_DANCE}, ITEM_CHOICE_BAND, 0 },
    { SPECIES_SHARPEDO,  {MOVE_CRUNCH, MOVE_SURF, MOVE_ICE_BEAM, MOVE_DOUBLE_EDGE}, ITEM_QUICK_CLAW, 0 },
    { SPECIES_ABSOL,     {MOVE_AERIAL_ACE, MOVE_SLASH, MOVE_SWORDS_DANCE, MOVE_ROCK_SLIDE}, ITEM_LEFTOVERS, 0 },
};

static const struct BoatLeaderMon sParty_Phoebe[] = {    // GHOST
    { SPECIES_DUSCLOPS,  {MOVE_SHADOW_BALL, MOVE_WILL_O_WISP, MOVE_CONFUSE_RAY, MOVE_REST}, ITEM_LEFTOVERS, 0 },
    { SPECIES_BANETTE,   {MOVE_SHADOW_BALL, MOVE_THUNDERBOLT, MOVE_WILL_O_WISP, MOVE_PSYCHIC}, ITEM_SITRUS_BERRY, 0 },
    { SPECIES_SABLEYE,   {MOVE_SHADOW_BALL, MOVE_FAINT_ATTACK, MOVE_CONFUSE_RAY, MOVE_TOXIC}, ITEM_LEFTOVERS, 0 },
    { SPECIES_SHEDINJA,  {MOVE_SHADOW_BALL, MOVE_CONFUSE_RAY, MOVE_TOXIC, MOVE_PROTECT}, ITEM_FOCUS_BAND, 0 },
    { SPECIES_MISDREAVUS,{MOVE_SHADOW_BALL, MOVE_THUNDERBOLT, MOVE_MEAN_LOOK, MOVE_PERISH_SONG}, ITEM_LEFTOVERS, 0 },
    { SPECIES_GENGAR,    {MOVE_SHADOW_BALL, MOVE_THUNDERBOLT, MOVE_GIGA_DRAIN, MOVE_HYPNOSIS}, ITEM_LUM_BERRY, 0 },
};

static const struct BoatLeaderMon sParty_Glacia[] = {    // ICE
    { SPECIES_GLALIE,    {MOVE_ICE_BEAM, MOVE_CRUNCH, MOVE_EXPLOSION, MOVE_PROTECT}, ITEM_LEFTOVERS, 0 },
    { SPECIES_WALREIN,   {MOVE_ICE_BEAM, MOVE_SURF, MOVE_BODY_SLAM, MOVE_REST}, ITEM_LEFTOVERS, 0 },
    { SPECIES_SEALEO,    {MOVE_ICE_BEAM, MOVE_SURF, MOVE_BODY_SLAM, MOVE_ENCORE}, ITEM_SITRUS_BERRY, 0 },
    { SPECIES_LAPRAS,    {MOVE_ICE_BEAM, MOVE_SURF, MOVE_THUNDERBOLT, MOVE_CONFUSE_RAY}, ITEM_LEFTOVERS, 0 },
    { SPECIES_DEWGONG,   {MOVE_ICE_BEAM, MOVE_SURF, MOVE_ENCORE, MOVE_REST}, ITEM_CHESTO_BERRY, 0 },
    { SPECIES_CLOYSTER,  {MOVE_ICE_BEAM, MOVE_SURF, MOVE_SPIKES, MOVE_EXPLOSION}, ITEM_QUICK_CLAW, 0 },
};

static const struct BoatLeaderMon sParty_Drake[] = {     // DRAGON
    { SPECIES_SALAMENCE, {MOVE_DRAGON_CLAW, MOVE_AERIAL_ACE, MOVE_EARTHQUAKE, MOVE_ROCK_SLIDE}, ITEM_LUM_BERRY, 0 },
    { SPECIES_FLYGON,    {MOVE_DRAGON_CLAW, MOVE_EARTHQUAKE, MOVE_ROCK_SLIDE, MOVE_CRUNCH}, ITEM_CHOICE_BAND, 0 },
    { SPECIES_ALTARIA,   {MOVE_DRAGON_DANCE, MOVE_DRAGON_CLAW, MOVE_AERIAL_ACE, MOVE_REST}, ITEM_CHESTO_BERRY, 0 },
    { SPECIES_KINGDRA,   {MOVE_SURF, MOVE_ICE_BEAM, MOVE_DRAGON_DANCE, MOVE_DRAGON_CLAW}, ITEM_LEFTOVERS, 0 },
    { SPECIES_SHELGON,   {MOVE_DRAGON_CLAW, MOVE_BRICK_BREAK, MOVE_ROCK_SLIDE, MOVE_PROTECT}, ITEM_SITRUS_BERRY, 0 },
    { SPECIES_DRAGONITE, {MOVE_DRAGON_CLAW, MOVE_THUNDERBOLT, MOVE_ICE_BEAM, MOVE_EARTHQUAKE}, ITEM_LEFTOVERS, 0 },
};

static const struct BoatLeaderMon sParty_Steven[] = {    // STEEL
    { SPECIES_METAGROSS, {MOVE_METEOR_MASH, MOVE_PSYCHIC, MOVE_EARTHQUAKE, MOVE_PROTECT}, ITEM_LEFTOVERS, 0 },
    { SPECIES_AGGRON,    {MOVE_IRON_TAIL, MOVE_EARTHQUAKE, MOVE_ROCK_SLIDE, MOVE_DOUBLE_EDGE}, ITEM_CHOICE_BAND, 0 },
    { SPECIES_SKARMORY,  {MOVE_DRILL_PECK, MOVE_STEEL_WING, MOVE_SPIKES, MOVE_ROAR}, ITEM_LEFTOVERS, 0 },
    { SPECIES_STEELIX,   {MOVE_IRON_TAIL, MOVE_EARTHQUAKE, MOVE_ROCK_SLIDE, MOVE_EXPLOSION}, ITEM_QUICK_CLAW, 0 },
    { SPECIES_MAGNETON,  {MOVE_THUNDERBOLT, MOVE_THUNDER_WAVE, MOVE_HIDDEN_POWER, MOVE_PROTECT}, ITEM_SITRUS_BERRY, 0 },
    { SPECIES_FORRETRESS,{MOVE_SPIKES, MOVE_EXPLOSION, MOVE_EARTHQUAKE, MOVE_TOXIC}, ITEM_LEFTOVERS, 0 },
};

static const u8 sName_Sidney[] = _("SIDNEY");
static const u8 sName_Phoebe[] = _("PHOEBE");
static const u8 sName_Glacia[] = _("GLACIA");
static const u8 sName_Drake[] = _("DRAKE");
static const u8 sName_Steven[] = _("STEVEN");

static const u8 sLine_Sidney[] = _("No hard feelings, all right?\nI just like a straight fight.");
static const u8 sLine_Phoebe[] = _("I grew up on MT. PYRE. The sea\ndoes not frighten me either.");
static const u8 sLine_Glacia[] = _("I came north for the heat, and\nfound it. Let us cool it down.");
static const u8 sLine_Drake[] = _("I sailed for years before this.\nA deck is the right place.");
static const u8 sLine_Steven[] = _("I came out to look at the rocks\nand found a challenge instead.");

// Unused for now, but exported so that it is kept and so the day a top floor
// is written the order is already settled: the FOUR, then the CHAMPION.
const struct BoatLeader gBoatChampions[] = {
    { sName_Sidney, sLine_Sidney, OBJ_EVENT_GFX_LEADER_BRAWLY, TRAINER_PIC_ELITE_FOUR_SIDNEY, MUGSHOT_COLOR_DARK_PURPLES, ARRAY_COUNT(sParty_Sidney), sParty_Sidney },
    { sName_Phoebe, sLine_Phoebe, OBJ_EVENT_GFX_LEADER_FLANNERY, TRAINER_PIC_ELITE_FOUR_PHOEBE, MUGSHOT_COLOR_LAVENDER_PURPLES, ARRAY_COUNT(sParty_Phoebe), sParty_Phoebe },
    { sName_Glacia, sLine_Glacia, OBJ_EVENT_GFX_LEADER_WINONA, TRAINER_PIC_ELITE_FOUR_GLACIA, MUGSHOT_COLOR_LIGHT_BLUE, ARRAY_COUNT(sParty_Glacia), sParty_Glacia },
    { sName_Drake, sLine_Drake, OBJ_EVENT_GFX_LEADER_NORMAN, TRAINER_PIC_ELITE_FOUR_DRAKE, MUGSHOT_COLOR_DARK_RED, ARRAY_COUNT(sParty_Drake), sParty_Drake },
    { sName_Steven, sLine_Steven, OBJ_EVENT_GFX_LEADER_WALLACE, TRAINER_PIC_CHAMPION_STEVEN, MUGSHOT_COLOR_SKY_BLUE_GREY, ARRAY_COUNT(sParty_Steven), sParty_Steven },
};

const u8 gBoatChampionCount = ARRAY_COUNT(gBoatChampions);

static const struct BoatLeader sBoatLeaders[] = {
    { sName_Roxanne, sLine_Roxanne, OBJ_EVENT_GFX_LEADER_ROXANNE, TRAINER_PIC_LEADER_ROXANNE, MUGSHOT_COLOR_BROWN_GREY,        ARRAY_COUNT(sParty_Roxanne),     sParty_Roxanne },
    { sName_Brawly, sLine_Brawly, OBJ_EVENT_GFX_LEADER_BRAWLY, TRAINER_PIC_LEADER_BRAWLY, MUGSHOT_COLOR_DARK_RED,         ARRAY_COUNT(sParty_Brawly),      sParty_Brawly },
    { sName_Wattson, sLine_Wattson, OBJ_EVENT_GFX_LEADER_WATTSON, TRAINER_PIC_LEADER_WATTSON, MUGSHOT_COLOR_YELLOW,        ARRAY_COUNT(sParty_Wattson),     sParty_Wattson },
    { sName_Flannery, sLine_Flannery, OBJ_EVENT_GFX_LEADER_FLANNERY, TRAINER_PIC_LEADER_FLANNERY, MUGSHOT_COLOR_RED_ORANGE,       ARRAY_COUNT(sParty_Flannery),    sParty_Flannery },
    { sName_Norman, sLine_Norman, OBJ_EVENT_GFX_LEADER_NORMAN, TRAINER_PIC_LEADER_NORMAN, MUGSHOT_COLOR_STARS,         ARRAY_COUNT(sParty_Norman),      sParty_Norman },
    { sName_Winona, sLine_Winona, OBJ_EVENT_GFX_LEADER_WINONA, TRAINER_PIC_LEADER_WINONA, MUGSHOT_COLOR_SKY_BLUE_GREY,         ARRAY_COUNT(sParty_Winona),      sParty_Winona },
    { sName_TateAndLiza, sLine_TateAndLiza, OBJ_EVENT_GFX_LEADER_TATE_LIZA, TRAINER_PIC_LEADER_TATE_AND_LIZA, MUGSHOT_COLOR_LAVENDER_PURPLES,  ARRAY_COUNT(sParty_TateAndLiza), sParty_TateAndLiza },
    { sName_Wallace, sLine_Wallace, OBJ_EVENT_GFX_LEADER_WALLACE, TRAINER_PIC_LEADER_WALLACE, MUGSHOT_COLOR_LIGHT_BLUE,        ARRAY_COUNT(sParty_Wallace),     sParty_Wallace },
};

#define LEADER_COUNT ARRAY_COUNT(sBoatLeaders)

static const u8 sText_LeaderWin[] = _("That is what the last round is for.");
static const u8 sText_LeaderLose[] = _("Through all seven. Take the purse.");
static const u8 sText_HouseWin[] = _("The house keeps the round.");
static const u8 sText_HouseLose[] = _("Yours. Pick one of mine on the way out.");

// What the house pays per round survived, and what it adds for the leader.
#define PURSE_PER_ROUND   400
#define PURSE_LEADER     2500
#define PURSE_RETIRED_NUM   3   // three quarters for walking away,
#define PURSE_LOST_NUM      1   // a half for being carried out
#define PURSE_DEN           4

// The last round of each block of four.
bool8 OrangeBoat_IsLeaderRound(void)
{
    return gSaveBlock2Ptr->factory.curChallengeBattleNum % FACTORY_ROUNDS_PER_BLOCK
           == FACTORY_ROUNDS_PER_BLOCK - 1;
}

// How many leaders are behind you once `cleared` rounds are won.
static u8 LeadersBeaten(u32 cleared)
{
    return cleared / FACTORY_ROUNDS_PER_BLOCK;
}

static const struct BoatLeader *LeaderForSlot(u8 slot)
{
    if (slot >= FACTORY_LEADERS_PER_RUN)
        slot = FACTORY_LEADERS_PER_RUN - 1;
    return &sBoatLeaders[gSaveBlock2Ptr->factory.leaders[slot] % LEADER_COUNT];
}

// Drawn once, at the desk, before the rentals are brought up: one per leader
// round, each re-rolled until it is one the run has not drawn yet. With as
// many leader rounds as there are leaders, that comes out a shuffle of all
// eight.
void OrangeBoat_DrawLeaders(void)
{
    u8 i, j;

    // Drawn once per run, so this is also where a new run starts empty-handed.
    gSaveBlock2Ptr->factory.spoilCount = 0;

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
                         / FACTORY_ROUNDS_PER_BLOCK);
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

// The MUGSHOT transition reads these two off gTrainers[], which holds nothing
// useful here: the opponent index is a frontier trainer id. battle_transition.c
// asks us instead whenever InBattleFactory().

u8 OrangeBoat_MugshotColor(void)
{
    if (OrangeBoat_IsLeaderRound())
        return CurrentLeader()->mugshotColor;
    return MUGSHOT_COLOR_STARS;
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
    u8 i, j, count, ivs;

    // A leader's six against three rentals is not a fight, it is a wall. They
    // bring one more than the player is holding, so the shape stays theirs -
    // you are always outnumbered - without the first leader being the end of
    // every run. By the time the player has grown to six it is a full team.
    // A leader is built a shelf above the house, not at a flat thirty-one:
    // against a team rolled at three IVs that was the whole difficulty.
    ivs = GetFactoryMonFixedIV(
              gSaveBlock2Ptr->factory.curChallengeBattleNum / FACTORY_ROUNDS_PER_BLOCK,
              TRUE);

    count = CalculatePlayerPartyCount() + 1;
    if (count > leader->count)
        count = leader->count;

    ZeroEnemyPartyMons();
    for (i = 0; i < count; i++)
    {
        const struct BoatLeaderMon *mon = &leader->party[i];

        CreateMon(&gEnemyParty[i], mon->species, LEADER_LEVEL, ivs,
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
    // A leader gets the mugshot; the rest of the boat gets the tower's usual.
    BattleTransition_StartOnField(OrangeBoat_IsLeaderRound()
                                  ? B_TRANSITION_MUGSHOT
                                  : BattleSetup_GetBattleTowerBattleTransition());
}

// Called at the top of each round, before the battle. Fills in the round
// number and the leader's name for the desk's lines, and answers whether this
// is the seventh.

void OrangeBoat_PrepareRound(void)
{
    ConvertIntToDecimalStringN(gStringVar3,
                               gSaveBlock2Ptr->factory.curChallengeBattleNum + 1,
                               STR_CONV_MODE_LEFT_ALIGN, 2);
    StringCopy(gStringVar2, CurrentLeader()->name);
    gSpecialVar_Result = OrangeBoat_IsLeaderRound();
}

// Who walks onto the floor, and what they say when they get there.
//
// A rented trainer's overworld sprite comes from the FACTORY's own
// facility-class table, which SET_OPPONENT_GFX puts in VAR_OBJ_GFX_ID_0; a
// leader has no sprite in FireRed, so the table above gives each a stand-in.
// Either way the object is removed and re-added after the var is set, which
// is what makes it re-read its graphics.
//
// The line is the one piece of theatre the slim trainer table cannot pay for:
// Emerald's frontier trainers carry speeches and these do not, so a rented
// one draws from the list below, chosen by who they are rather than at
// random, so the same opponent says the same thing twice in a row.

static const u8 sChallengerLine0[] = _("Rented, same as yours. Let us see\nwho got the better draw.");
static const u8 sChallengerLine1[] = _("I have been down here all week.\nYou are the first new face.");
static const u8 sChallengerLine2[] = _("No idea what I am holding. That\nis half the fun of it.");
static const u8 sChallengerLine3[] = _("The house takes a cut either way.\nMight as well enjoy ourselves.");
static const u8 sChallengerLine4[] = _("Three each. Short and honest.");
static const u8 sChallengerLine5[] = _("Lose and you keep walking. Win\nand you take one of mine.");
static const u8 sChallengerLine6[] = _("Do not look at the floor. Look at\nwhat I send out.");
static const u8 sChallengerLine7[] = _("Been beaten four times tonight.\nI am owed one.");

static const u8 *const sChallengerLines[] = {
    sChallengerLine0, sChallengerLine1, sChallengerLine2, sChallengerLine3,
    sChallengerLine4, sChallengerLine5, sChallengerLine6, sChallengerLine7,
};

void OrangeBoat_GetChallengerName(void)
{
    OrangeBoat_OpponentName(gStringVar1);
}

void OrangeBoat_GetChallengerLine(void)
{
    if (OrangeBoat_IsLeaderRound())
        StringCopy(gStringVar4, CurrentLeader()->line);
    else
        StringCopy(gStringVar4, sChallengerLines[gTrainerBattleOpponent_A
                                                 % ARRAY_COUNT(sChallengerLines)]);
}

// The sprite that walks in. VAR_OBJ_GFX_ID_0 is what the object event's
// graphics id is set to, so this is read when the object is re-added.
void OrangeBoat_SetChallengerGfx(void)
{
    if (OrangeBoat_IsLeaderRound())
        VarSet(VAR_OBJ_GFX_ID_0, CurrentLeader()->gfxId);
    else
        SetBattleFacilityTrainerGfxId(gTrainerBattleOpponent_A, 0);
}

// No follower aboard.
//
// The boat lends you POKEMON, and a rented one trotting along behind the
// player reads as theirs -- worse, the rental stayed out after a lost round,
// because the follower only re-reads the party when a map loads and the swap
// happens without one. Rather than chase the refresh, the boat is simply a
// place where the follower is not out: nine maps, and the duration of a run
// wherever it is checked from.
//
// This is the boat's own suppression and nothing to do with the player's
// setting, so it neither reads nor writes FLAG_FOLLOWERS_DISABLED. Walking
// back down the gangway loads KINNOW and the follower comes back on its own.

bool8 OrangeBoat_SuppressesFollower(void)
{
    u8 group = gSaveBlock1Ptr->location.mapGroup;
    u8 num = gSaveBlock1Ptr->location.mapNum;

    if (InBattleFactory())
        return TRUE;
    if (group != MAP_GROUP(ORANGE_ISLANDS_KINNOW_BATTLE_BOAT_DECK))
        return FALSE;
    return num == MAP_NUM(ORANGE_ISLANDS_KINNOW_BATTLE_BOAT_DECK)
        || num == MAP_NUM(ORANGE_ISLANDS_KINNOW_BATTLE_BOAT_SALOON)
        || num == MAP_NUM(ORANGE_ISLANDS_KINNOW_BATTLE_BOAT_HOLD)
        || num == MAP_NUM(ORANGE_ISLANDS_KINNOW_BATTLE_BOAT_CABIN1)
        || num == MAP_NUM(ORANGE_ISLANDS_KINNOW_BATTLE_BOAT_CABIN2)
        || num == MAP_NUM(ORANGE_ISLANDS_KINNOW_BATTLE_BOAT_CABIN3)
        || num == MAP_NUM(ORANGE_ISLANDS_KINNOW_BATTLE_BOAT_GALLEY)
        || num == MAP_NUM(ORANGE_ISLANDS_KINNOW_BATTLE_BOAT_ARENA);
}

// Whether the rented three are the party right now, as opposed to the run
// being open but put down. The desk needs to know: stashing the player's own
// team again when it is not the one in hand would write the rentals over it.

void OrangeBoat_RentalsAreLive(void)
{
    gSpecialVar_Result =
        (gSaveBlock2Ptr->factory.challengeStatus == FACTORY_CHALLENGE_RUNNING);
}

void OrangeBoat_MarkRentalsLive(void)
{
    gSaveBlock2Ptr->factory.challengeStatus = FACTORY_CHALLENGE_RUNNING;
    gSaveBlock2Ptr->factory.challengePaused = FALSE;
}

// Bookkeeping between rounds. VAR_RESULT comes back TRUE when that was the
// seventh and the run is over.

// The house draws from a higher shelf every four rounds, but the three the
// player drafted in round one were rolled at the bottom of it and never move.
// Nothing here can change what they are - a rental is an index into
// gBattleFrontierMons, species, moves and all - so what rises is how well
// built they are: at each block the stored IVs step up to the tier's own
// figure, and the party is rebuilt around it. The team the player started
// with keeps pace instead of being carried by the spoils alone.

static void SharpenRentals(void)
{
    u8 i, ivs;

    if (gSaveBlock2Ptr->factory.curChallengeBattleNum % FACTORY_ROUNDS_PER_BLOCK)
        return;

    ivs = GetFactoryMonFixedIV(
              gSaveBlock2Ptr->factory.curChallengeBattleNum / FACTORY_ROUNDS_PER_BLOCK,
              FALSE);
    if (ivs > MAX_PER_STAT_IVS)
        ivs = MAX_PER_STAT_IVS;

    for (i = 0; i < FACTORY_RENTAL_COUNT; i++)
    {
        if (gSaveBlock2Ptr->factory.rentalMons[i].ivs < ivs)
            gSaveBlock2Ptr->factory.rentalMons[i].ivs = ivs;
    }

    for (i = 0; i < CalculatePlayerPartyCount(); i++)
    {
        u8 j;
        u32 iv = ivs;

        if (!GetMonData(&gPlayerParty[i], MON_DATA_SPECIES, NULL))
            continue;
        for (j = 0; j < NUM_STATS; j++)
        {
            if (GetMonData(&gPlayerParty[i], MON_DATA_HP_IV + j, NULL) < ivs)
                SetMonData(&gPlayerParty[i], MON_DATA_HP_IV + j, &iv);
        }
        CalculateMonStats(&gPlayerParty[i]);
    }
}

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
    SharpenRentals();
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
    bool8 finished;

    if (cleared > FACTORY_STAGES_PER_CHALLENGE)
        cleared = FACTORY_STAGES_PER_CHALLENGE;
    finished = (cleared == FACTORY_STAGES_PER_CHALLENGE);

    purse = cleared * PURSE_PER_ROUND + LeadersBeaten(cleared) * PURSE_LEADER;

    // How the run ended decides what fraction of that is paid. The script
    // puts TRUE in VAR_0x8005 when the player walked away at the desk.
    if (!finished)
    {
        u32 num = gSpecialVar_0x8005 ? PURSE_RETIRED_NUM : PURSE_LOST_NUM;

        purse = purse * num / PURSE_DEN;
    }

    if (purse != 0)
        AddMoney(&gSaveBlock1Ptr->money, purse);
    ConvertIntToDecimalStringN(gStringVar1, purse, STR_CONV_MODE_LEFT_ALIGN, 6);
    ConvertIntToDecimalStringN(gStringVar3, cleared, STR_CONV_MODE_LEFT_ALIGN, 2);
    gSpecialVar_Result = (purse != 0);

    if (!finished)
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
    gSaveBlock2Ptr->factory.spoilCount = 0;
}

// Growing the team.
//
// Emerald's factory hands you three and keeps you at three: after each win
// the swap screen trades one of yours for one you have just beaten. Here the
// first three wins add instead -- you take one POKEMON off each rented team
// you beat, up to six -- and the swap only starts once the team is full.
//
// Drafting six at the desk would have been the other way to do it, but both
// factory screens lay their rows out for three: the chosen-mon row sits at
// x = i * 72 + 16 and the swap screen's balls at i * 48 + 72, so the fourth
// of either is off the right of a 240-pixel screen. Growing the team needs no
// new screen at all -- the three names go into a multichoice.

EWRAM_DATA u8 gBoatSpoilNames[FACTORY_PARTY_SIZE][POKEMON_NAME_LENGTH + 1] = {};

// The team you just beat, as rental entries. SET_OPPONENT_MONS puts them in
// the back half of rentalMons, which is where the swap screen reads them too.
static struct FactoryRentalMon *BeatenMon(u8 i)
{
    return &gSaveBlock2Ptr->factory.rentalMons[i + FACTORY_PARTY_SIZE];
}

static void BuildSpoilMon(struct Pokemon *dest, const struct FactoryRentalMon *rental)
{
    const struct FacilityMon *src = &gFacilityTrainerMons[rental->monId];
    u8 friendship = 0;
    u8 j;

    CreateMonWithEVSpreadNatureOTID(dest, src->species, FACTORY_MON_LEVEL,
                                    src->nature, rental->ivs, src->evSpread,
                                    T1_READ_32(gSaveBlock2Ptr->playerTrainerId));
    for (j = 0; j < MAX_MON_MOVES; j++)
        SetMonMoveAvoidReturn(dest, src->moves[j], j);
    SetMonData(dest, MON_DATA_FRIENDSHIP, &friendship);
    SetMonData(dest, MON_DATA_HELD_ITEM, &gBattleFrontierHeldItems[src->itemTableId]);
    SetMonData(dest, MON_DATA_ABILITY_NUM, &rental->abilityNum);
}

// Is there room, and if so what are the three called. VAR_RESULT comes back
// TRUE when the menu is worth opening.
void OrangeBoat_OfferSpoils(void)
{
    u8 i;

    if (gSaveBlock2Ptr->factory.spoilCount >= FACTORY_MAX_SPOILS)
    {
        gSpecialVar_Result = FALSE;
        return;
    }
    for (i = 0; i < FACTORY_PARTY_SIZE; i++)
        StringCopy(gBoatSpoilNames[i],
                   gSpeciesNames[gFacilityTrainerMons[BeatenMon(i)->monId].species]);
    gSpecialVar_Result = TRUE;
}

// VAR_RESULT holds which of the three the player picked.
void OrangeBoat_TakeSpoil(void)
{
    u8 slot = gSaveBlock2Ptr->factory.spoilCount;
    u8 picked = gSpecialVar_Result;

    if (picked >= FACTORY_PARTY_SIZE || slot >= FACTORY_MAX_SPOILS)
        return;

    gSaveBlock2Ptr->factory.spoils[slot] = *BeatenMon(picked);
    gSaveBlock2Ptr->factory.spoilCount++;
    BuildSpoilMon(&gPlayerParty[FACTORY_PARTY_SIZE + slot],
                  &gSaveBlock2Ptr->factory.spoils[slot]);
    CalculatePlayerPartyCount();
    StringCopy(gStringVar2, gBoatSpoilNames[picked]);
}

// Put the taken POKEMON back on the end of the party. The factory's own
// SET_PARTIES only knows about the three you drafted, so a resumed challenge
// calls this straight after it.
void OrangeBoat_RestoreSpoils(void)
{
    u8 i;

    for (i = 0; i < gSaveBlock2Ptr->factory.spoilCount; i++)
        BuildSpoilMon(&gPlayerParty[FACTORY_PARTY_SIZE + i],
                      &gSaveBlock2Ptr->factory.spoils[i]);
    CalculatePlayerPartyCount();
}

// What the desk quotes when you ask to stop: what walking away now is worth,
// without taking it. VAR_RESULT comes back TRUE if there is anything to take.

void OrangeBoat_QuoteRetirement(void)
{
    u32 cleared = gSaveBlock2Ptr->factory.curChallengeBattleNum;
    u32 purse = (cleared * PURSE_PER_ROUND + LeadersBeaten(cleared) * PURSE_LEADER)
                * PURSE_RETIRED_NUM / PURSE_DEN;

    ConvertIntToDecimalStringN(gStringVar1, purse, STR_CONV_MODE_LEFT_ALIGN, 6);
    ConvertIntToDecimalStringN(gStringVar3, cleared, STR_CONV_MODE_LEFT_ALIGN, 2);
    gSpecialVar_Result = (purse != 0);
}

// The leader waiting at the end of the current block, for the desk to name.

void OrangeBoat_GetLeaderName(void)
{
    StringCopy(gStringVar2, LeaderForSlot(
        gSaveBlock2Ptr->factory.curChallengeBattleNum / FACTORY_ROUNDS_PER_BLOCK)->name);
}
