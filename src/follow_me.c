#include "global.h"
#include "follow_me.h"
#include "event_data.h"
#include "event_object_lock.h"
#include "characters.h"
#include "event_object_movement.h"
#include "field_door.h"
#include "field_effect.h"
#include "field_player_avatar.h"
#include "field_weather.h"
#include "fieldmap.h"
#include "metatile_behavior.h"
#include "overworld.h"
#include "palette.h"
#include "pokemon.h"
#include "quest_log.h"
#include "random.h"
#include "script.h"
#include "sound.h"
#include "sprite.h"
#include "string_util.h"
#include "task.h"
#include "trig.h"
#include "constants/event_object_movement.h"
#include "constants/event_objects.h"
#include "constants/field_effects.h"
#include "constants/flags.h"
#include "constants/items.h"
#include "constants/map_types.h"
#include "constants/metatile_behaviors.h"
#include "constants/quest_log.h"
#include "constants/songs.h"
#include "constants/species.h"

/*
    Follower Pokemon for FireRed.

    Ported from the FollowerEm project (pokeemerald), which combines Ghoulslash's "follow me"
    NPC follower with Pokemon follower graphics. Differences from FollowerEm:
    - The follower is not a map object. It is spawned from a built-in object event template
      (local id OBJ_EVENT_ID_FOLLOWER), so it works on every map and needs no map edits.
    - It starts automatically once the player has a Pokemon, unless FLAG_FOLLOWERS_DISABLED is set
      (see the enablefollower / disablefollower script macros).
    - Its sprite is always the lead (first non-fainted, non-egg) party Pokemon.
    - No dynamic overworld palette system: follower palettes are loaded on demand into free
      sprite palette slots (see LoadDynamicObjectEventPalette in event_object_movement.c).
    - No dive / underwater / surf-blob follower; while surfing the follower stays in its ball.
*/

#define PLAYER_AVATAR_FLAG_BIKE (PLAYER_AVATAR_FLAG_MACH_BIKE | PLAYER_AVATAR_FLAG_ACRO_BIKE)

// Flags the automatic Pokemon follower uses (see include/constants/follow_me.h)
#define FOLLOWER_POKEMON_FLAGS (FOLLOWER_FLAG_CAN_BIKE | FOLLOWER_FLAG_CAN_LEAVE_ROUTE | FOLLOWER_FLAG_CAN_SURF \
                              | FOLLOWER_FLAG_CAN_WATERFALL | FOLLOWER_FLAG_CAN_DIVE | FOLLOWER_FLAG_CAN_ROCK_CLIMB)

extern const u8 EventScript_FollowerPokemon[];
extern const u8 gText_FollowerDefault[];
extern const struct SpritePalette gSpritePalette_SmallSparkle;

static u8 GetFollowerMapObjId(void);
static void Task_ReallowPlayerMovement(u8 taskId);
static u8 DetermineFollowerDirection(struct ObjectEvent *player, struct ObjectEvent *follower);
static void PlayerLogCoordinates(struct ObjectEvent *player);
static u8 DetermineFollowerState(struct ObjectEvent *follower, u8 state, u8 direction);
static bool8 IsStateMovement(u8 state);
static u8 ReturnFollowerDelayedState(u8 direction);
static void Task_FollowerOutOfDoor(u8 taskId);
static void Task_FollowerHandleIndoorStairs(u8 taskId);
static void Task_FollowerHandleEscalator(u8 taskId);
static void Task_FollowerHandleEscalatorFinish(u8 taskId);
static void CalculateFollowerEscalatorTrajectoryUp(struct Task *task);
static void CalculateFollowerEscalatorTrajectoryDown(struct Task *task);
static void RecreateFollowerObject(u16 newGraphicsId);
static u8 GetLeadMonNotFaintedIndex(void);
static void CreateFollowerSparkles(s16 x, s16 y, bool8 big);
static bool8 FollowerObjectExists(void);

// ---------------------------------------------------------------------------
// Data
// ---------------------------------------------------------------------------

// Built-in template the follower object is spawned from.
static const struct ObjectEventTemplate sFollowerObjectEventTemplate =
{
    .localId = OBJ_EVENT_ID_FOLLOWER,
    .kind = OBJ_KIND_NORMAL,
    .graphicsId = OBJ_EVENT_GFX_FOLLOWER(SPECIES_PIKACHU),
    .x = 0,
    .y = 0,
    .objUnion = {
        .normal = {
            .elevation = 3,
            .movementType = MOVEMENT_TYPE_NONE,
            .movementRangeX = 0,
            .movementRangeY = 0,
            .trainerType = 0,
            .trainerRange_berryTreeId = 0,
        },
    },
    .script = EventScript_FollowerPokemon,
    .flagId = 0,
};

// Where the sparkle appears around each follower sprite when it comes out of its ball:
// {south x, south y, north x, north y, side x, side y}. Indexed with GetSparkleCoordsIndex().
static const u8 sFollowerSparkleCoords[][6] =
{
    {15, 9, 15, 9, 11, 9}, // Bulbasaur
    {15, 11, 15, 9, 10, 9}, // Ivysaur
    {16, 13, 16, 12, 13, 13}, // Venusaur
    {16, 9, 16, 10, 11, 9}, // Charmander
    {15, 12, 15, 10, 10, 11}, // Charmeleon
    {16, 13, 16, 13, 16, 13}, // Charizard
    {15, 9, 15, 9, 7, 9}, // Squirtle
    {16, 10, 15, 9, 9, 11}, // Wartortle
    {16, 13, 16, 13, 14, 12}, // Blastoise
    {15, 10, 15, 9, 9, 10}, // Caterpie
    {16, 10, 16, 11, 8, 10}, // Metapod
    {16, 13, 16, 13, 12, 12}, // Butterfree
    {16, 10, 16, 10, 8, 9}, // Weedle
    {15, 10, 15, 10, 8, 10}, // Kakuna
    {15, 13, 15, 15, 9, 13}, // Beedrill
    {15, 8, 15, 8, 7, 8}, // Pidgey
    {15, 16, 15, 15, 11, 14}, // Pidgeotto
    {16, 17, 16, 17, 15, 16}, // Pidgeot
    {16, 11, 16, 9, 9, 8}, // Rattata
    {16, 12, 16, 9, 11, 8}, // Raticate
    {16, 8, 16, 8, 10, 8}, // Spearow
    {16, 15, 16, 16, 15, 19}, // Fearow
    {16, 10, 13, 11, 9, 10}, // Ekans
    {15, 12, 14, 13, 13, 12}, // Arbok
    {16, 10, 16, 9, 8, 10}, // Pikachu
    {13, 12, 20, 12, 14, 11}, // Raichu
    {16, 9, 16, 9, 9, 9}, // Sandshrew
    {15, 11, 15, 11, 10, 11}, // Sandslash
    {16, 9, 16, 9, 7, 9}, // Nidoran_F
    {15, 10, 15, 10, 11, 10}, // Nidorina
    {16, 12, 16, 12, 13, 12}, // Nidoqueen
    {16, 9, 16, 9, 8, 8}, // Nidoran_M
    {16, 10, 16, 11, 11, 9}, // Nidorino
    {16, 12, 16, 12, 14, 12}, // Nidoking
    {15, 9, 16, 9, 7, 9}, // Clefairy
    {16, 11, 16, 11, 9, 11}, // Clefable
    {15, 11, 15, 11, 12, 9}, // Vulpix
    {15, 13, 15, 14, 13, 11}, // Ninetales
    {15, 9, 15, 9, 7, 9}, // Jigglypuff
    {15, 11, 15, 11, 8, 11}, // Wigglytuff
    {15, 12, 15, 13, 10, 11}, // Zubat
    {16, 12, 16, 12, 12, 12}, // Golbat
    {15, 8, 15, 8, 7, 8}, // Oddish
    {16, 10, 16, 10, 9, 11}, // Gloom
    {16, 10, 16, 10, 11, 10}, // Vileplume
    {16, 9, 16, 8, 9, 9}, // Paras
    {15, 13, 16, 12, 12, 12}, // Parasect
    {16, 10, 16, 10, 11, 10}, // Venonat
    {15, 14, 15, 14, 13, 14}, // Venomoth
    {15, 7, 16, 7, 7, 7}, // Diglett
    {16, 11, 16, 11, 11, 11}, // Dugtrio
    {16, 13, 15, 11, 11, 9}, // Meowth
    {15, 14, 15, 12, 13, 9}, // Persian
    {15, 10, 15, 10, 7, 10}, // Psyduck
    {15, 12, 15, 12, 8, 11}, // Golduck
    {15, 9, 15, 9, 9, 9}, // Mankey
    {15, 11, 15, 11, 9, 12}, // Primeape
    {16, 11, 16, 11, 9, 10}, // Growlithe
    {15, 15, 15, 15, 16, 12}, // Arcanine
    {16, 9, 16, 9, 9, 9}, // Poliwag
    {16, 9, 16, 9, 7, 10}, // Poliwhirl
    {15, 11, 15, 11, 10, 11}, // Poliwrath
    {15, 11, 15, 11, 11, 12}, // Abra
    {15, 11, 16, 11, 12, 11}, // Kadabra
    {16, 12, 16, 13, 11, 13}, // Alakazam
    {15, 9, 15, 9, 8, 10}, // Machop
    {15, 12, 15, 12, 8, 12}, // Machoke
    {16, 13, 16, 13, 12, 12}, // Machamp
    {15, 11, 15, 11, 7, 10}, // Bellsprout
    {15, 11, 15, 10, 7, 11}, // Weepinbell
    {15, 14, 15, 14, 14, 13}, // Victreebel
    {16, 10, 16, 10, 9, 10}, // Tentacool
    {15, 13, 15, 13, 13, 12}, // Tentacruel
    {15, 10, 15, 10, 7, 11}, // Geodude
    {15, 9, 15, 9, 10, 11}, // Graveler
    {15, 11, 15, 11, 12, 11}, // Golem
    {15, 12, 15, 12, 11, 11}, // Ponyta
    {15, 15, 15, 14, 15, 13}, // Rapidash
    {15, 11, 15, 9, 12, 8}, // Slowpoke
    {16, 11, 16, 11, 14, 11}, // Slowbro
    {15, 10, 15, 10, 8, 11}, // Magnemite
    {15, 15, 15, 15, 10, 15}, // Magneton
    {16, 11, 16, 11, 8, 11}, // Farfetch'd
    {15, 11, 15, 11, 9, 12}, // Doduo
    {16, 16, 15, 16, 15, 15}, // Dodrio
    {15, 11, 15, 11, 13, 10}, // Seel
    {15, 13, 15, 12, 15, 12}, // Dewgong
    {16, 9, 16, 9, 9, 10}, // Grimer
    {15, 11, 15, 11, 11, 11}, // Muk
    {15, 9, 15, 8, 7, 9}, // Shellder
    {15, 13, 15, 13, 13, 14}, // Cloyster
    {15, 15, 16, 15, 10, 14}, // Gastly
    {16, 13, 16, 13, 10, 14}, // Haunter
    {15, 11, 15, 11, 9, 11}, // Gengar
    {16, 15, 15, 16, 13, 16}, // Onix
    {16, 10, 16, 10, 10, 11}, // Drowzee
    {16, 12, 16, 12, 10, 12}, // Hypno
    {16, 9, 16, 9, 13, 7}, // Krabby
    {16, 13, 16, 12, 16, 13}, // Kingler
    {15, 9, 15, 9, 7, 9}, // Voltorb
    {16, 12, 16, 12, 13, 12}, // Electrode
    {16, 13, 16, 11, 14, 12}, // Exeggcute
    {16, 15, 16, 14, 12, 14}, // Exeggutor
    {14, 10, 16, 10, 9, 11}, // Cubone
    {14, 11, 16, 11, 10, 12}, // Marowak
    {15, 11, 15, 11, 9, 11}, // Hitmonlee
    {16, 12, 16, 11, 9, 11}, // Hitmonchan
    {15, 11, 15, 11, 12, 10}, // Lickitung
    {17, 13, 17, 13, 12, 12}, // Koffing
    {15, 14, 16, 14, 13, 13}, // Weezing
    {15, 12, 15, 11, 14, 9}, // Rhyhorn
    {16, 12, 16, 12, 16, 12}, // Rhydon
    {15, 10, 15, 10, 10, 10}, // Chansey
    {16, 11, 16, 11, 10, 10}, // Tangela
    {16, 12, 16, 12, 12, 13}, // Kangaskhan
    {15, 9, 15, 9, 9, 9}, // Horsea
    {15, 13, 15, 13, 11, 12}, // Seadra
    {15, 11, 15, 10, 12, 9}, // Goldeen
    {15, 13, 15, 11, 14, 13}, // Seaking
    {16, 10, 16, 10, 8, 10}, // Staryu
    {16, 12, 16, 12, 9, 13}, // Starmie
    {16, 11, 16, 10, 10, 12}, // Mr. Mime
    {15, 13, 15, 13, 12, 13}, // Scyther
    {16, 13, 16, 12, 11, 13}, // Jynx
    {16, 12, 16, 12, 12, 12}, // Electabuzz
    {15, 13, 15, 13, 12, 12}, // Magmar
    {15, 12, 15, 12, 11, 12}, // Pinsir
    {15, 15, 16, 12, 15, 11}, // Tauros
    {16, 10, 16, 11, 12, 10}, // Magikarp
    {16, 16, 16, 16, 15, 16}, // Gyarados
    {16, 12, 16, 13, 14, 13}, // Lapras
    {16, 8, 16, 8, 9, 7}, // Ditto
    {15, 9, 15, 9, 11, 9}, // Eevee
    {15, 15, 15, 12, 15, 11}, // Vaporeon
    {15, 12, 15, 11, 14, 10}, // Jolteon
    {15, 13, 15, 12, 14, 11}, // Flareon
    {15, 10, 15, 10, 12, 9}, // Porygon
    {15, 10, 15, 10, 10, 9}, // Omanyte
    {15, 12, 15, 12, 12, 11}, // Omastar
    {15, 8, 15, 8, 9, 8}, // Kabuto
    {16, 11, 15, 11, 11, 12}, // Kabutops
    {15, 16, 15, 16, 16, 15}, // Aerodactyl
    {16, 14, 16, 14, 12, 15}, // Snorlax
    {15, 17, 15, 16, 14, 17}, // Articuno
    {15, 18, 15, 17, 14, 15}, // Zapdos
    {15, 16, 15, 17, 15, 17}, // Moltres
    {17, 12, 14, 11, 12, 11}, // Dratini
    {18, 15, 14, 15, 14, 14}, // Dragonair
    {15, 16, 15, 16, 13, 15}, // Dragonite
    {15, 15, 15, 14, 13, 14}, // Mewtwo
    {16, 13, 16, 11, 13, 13}, // Mew
    {16, 10, 15, 10, 9, 10}, // Chikorita
    {16, 13, 15, 14, 13, 13}, // Bayleef
    {15, 14, 15, 15, 14, 15}, // Meganium
    {16, 11, 16, 10, 12, 9}, // Cyndaquil
    {15, 13, 15, 14, 15, 10}, // Quilava
    {15, 14, 15, 14, 14, 13}, // Typhlosion
    {16, 10, 16, 10, 10, 10}, // Totodile
    {15, 13, 15, 14, 13, 13}, // Croconaw
    {15, 15, 15, 15, 15, 15}, // Feraligatr
    {15, 10, 15, 10, 12, 9}, // Sentret
    {15, 14, 15, 13, 15, 11}, // Furret
    {16, 10, 16, 10, 10, 10}, // Hoothoot
    {15, 13, 15, 14, 14, 16}, // Noctowl
    {15, 14, 15, 12, 13, 12}, // Ledyba
    {16, 12, 16, 13, 13, 16}, // Ledian
    {15, 9, 15, 9, 11, 7}, // Spinarak
    {15, 13, 15, 12, 15, 13}, // Ariados
    {15, 13, 15, 13, 12, 15}, // Crobat
    {16, 8, 16, 8, 10, 8}, // Chinchou
    {16, 14, 16, 13, 15, 11}, // Lanturn
    {15, 9, 15, 9, 9, 9}, // Pichu
    {16, 8, 16, 8, 9, 8}, // Cleffa
    {15, 9, 15, 9, 8, 9}, // Igglybuff
    {16, 9, 16, 9, 9, 9}, // Togepi
    {15, 14, 15, 15, 10, 13}, // Togetic
    {16, 8, 16, 8, 9, 9}, // Natu
    {16, 12, 16, 12, 12, 12}, // Xatu
    {16, 11, 15, 10, 12, 10}, // Mareep
    {18, 12, 15, 11, 12, 11}, // Flaaffy
    {16, 13, 16, 13, 13, 13}, // Ampharos
    {15, 10, 15, 10, 8, 10}, // Bellossom
    {19, 8, 12, 8, 12, 9}, // Marill
    {18, 10, 13, 10, 12, 11}, // Azumarill
    {16, 12, 16, 12, 10, 12}, // Sudowoodo
    {15, 12, 15, 12, 9, 12}, // Politoed
    {16, 12, 16, 12, 9, 12}, // Hoppip
    {15, 10, 15, 10, 9, 13}, // Skiploom
    {16, 14, 16, 14, 9, 13}, // Jumpluff
    {16, 12, 16, 11, 13, 11}, // Aipom
    {16, 8, 16, 8, 7, 9}, // Sunkern
    {15, 11, 15, 11, 7, 11}, // Sunflora
    {16, 15, 16, 15, 15, 13}, // Yanma
    {16, 8, 16, 8, 8, 8}, // Wooper
    {17, 11, 15, 11, 12, 11}, // Quagsire
    {15, 14, 15, 12, 13, 12}, // Espeon
    {15, 13, 15, 12, 14, 11}, // Umbreon
    {15, 10, 15, 10, 9, 10}, // Murkrow
    {16, 14, 16, 14, 10, 13}, // Slowking
    {16, 14, 16, 13, 11, 13}, // Misdreavus
    {15, 10, 15, 10, 7, 11}, // Unown (A)
    {16, 11, 16, 11, 10, 11}, // Wobbuffet
    {16, 13, 16, 13, 15, 13}, // Girafarig
    {16, 9, 16, 9, 8, 9}, // Pineco
    {16, 10, 16, 10, 11, 10}, // Forretress
    {15, 13, 15, 9, 13, 8}, // Dunsparce
    {15, 12, 15, 12, 11, 13}, // Gligar
    {33, 17, 32, 19, 36, 17}, // Steelix
    {16, 10, 16, 10, 10, 10}, // Snubbull
    {16, 12, 16, 12, 10, 13}, // Granbull
    {16, 12, 16, 10, 14, 9}, // Qwilfish
    {16, 13, 16, 12, 12, 13}, // Scizor
    {16, 10, 16, 11, 11, 10}, // Shuckle
    {15, 13, 15, 13, 12, 14}, // Heracross
    {17, 10, 15, 10, 9, 10}, // Sneasel
    {15, 10, 15, 10, 9, 10}, // Teddiursa
    {16, 13, 16, 13, 11, 14}, // Ursaring
    {15, 11, 16, 11, 11, 10}, // Slugma
    {16, 13, 14, 12, 15, 11}, // Magcargo
    {15, 8, 15, 8, 11, 8}, // Swinub
    {15, 11, 15, 12, 12, 11}, // Piloswine
    {16, 9, 16, 9, 10, 10}, // Corsola
    {16, 10, 16, 10, 13, 7}, // Remoraid
    {15, 11, 15, 12, 11, 11}, // Ocitllery
    {16, 10, 15, 10, 9, 11}, // Delibird
    {16, 15, 16, 15, 16, 16}, // Mantine
    {15, 13, 15, 16, 15, 16}, // Skarmory
    {16, 9, 16, 10, 11, 10}, // Houndour
    {15, 12, 15, 13, 14, 12}, // Houndoom
    {16, 14, 16, 14, 14, 13}, // Kingdra
    {16, 9, 16, 9, 11, 9}, // Phanpy
    {16, 11, 16, 11, 15, 12}, // Donphan
    {16, 11, 16, 12, 13, 12}, // Porygon2
    {16, 14, 15, 14, 12, 15}, // Stantler
    {14, 11, 18, 11, 9, 11}, // Smeargle
    {15, 10, 15, 10, 8, 10}, // Tyrogue
    {15, 12, 15, 12, 9, 11}, // Hitmontop
    {15, 10, 15, 10, 9, 10}, // Smoochum
    {15, 10, 15, 10, 8, 10}, // Elekid
    {16, 10, 16, 10, 8, 10}, // Magby
    {16, 11, 16, 11, 12, 11}, // Miltank
    {16, 11, 16, 11, 11, 12}, // Blissey
    {15, 15, 15, 13, 15, 13}, // Raikou
    {15, 15, 15, 13, 15, 12}, // Entei
    {15, 15, 15, 13, 16, 12}, // Suicune
    {15, 10, 15, 10, 10, 10}, // Larvitar
    {15, 11, 15, 11, 8, 11}, // Pupitar
    {15, 14, 15, 14, 14, 13}, // Tyranitar
    {32, 21, 32, 23, 40, 21}, // Lugia
    {32, 22, 31, 22, 38, 24}, // Ho-oh
    {16, 14, 16, 14, 9, 14}, // Celebi
    {15, 9, 15, 9, 9, 9}, // Treecko
    {16, 11, 16, 11, 11, 11}, // Grovyle
    {15, 13, 15, 13, 15, 14}, // Sceptile
    {15, 10, 15, 10, 8, 10}, // Torchic
    {15, 11, 15, 11, 8, 11}, // Combusken
    {16, 13, 16, 13, 12, 13}, // Blaziken
    {15, 9, 15, 9, 10, 9}, // Mudkip
    {15, 10, 15, 10, 9, 10}, // Marshtomp
    {15, 13, 15, 13, 15, 13}, // Swampert
    {15, 10, 15, 10, 12, 10}, // Poochyena
    {16, 14, 16, 12, 15, 12}, // Mightyena
    {16, 12, 16, 10, 14, 9}, // Zigzagoon
    {16, 13, 14, 10, 15, 8}, // Linoone
    {15, 10, 15, 11, 11, 9}, // Wurmple
    {16, 8, 16, 8, 10, 8}, // Silcoon
    {16, 13, 16, 13, 13, 12}, // Beautifly
    {16, 8, 16, 8, 10, 8}, // Cascoon
    {15, 11, 15, 11, 12, 15}, // Dustox
    {15, 9, 15, 9, 10, 9}, // Lotad
    {15, 12, 15, 12, 10, 11}, // Lombre
    {16, 14, 16, 14, 13, 14}, // Ludicolo
    {15, 9, 15, 9, 8, 9}, // Seedot
    {15, 12, 15, 12, 9, 12}, // Nuzleaf
    {16, 12, 16, 12, 13, 12}, // Shiftry
    {15, 9, 15, 8, 12, 8}, // Nincada
    {15, 12, 15, 12, 12, 12}, // Ninjask
    {16, 13, 16, 14, 12, 13}, // Shedinja
    {15, 9, 15, 9, 10, 8}, // Taillow
    {15, 13, 15, 12, 16, 15}, // Swellow
    {15, 9, 15, 9, 10, 9}, // Shroomish
    {16, 12, 16, 12, 12, 12}, // Breloom
    {16, 10, 16, 10, 9, 11}, // Spinda
    {15, 11, 15, 10, 12, 13}, // Wingull
    {16, 14, 16, 14, 14, 13}, // Pelipper
    {15, 9, 15, 9, 10, 9}, // Surskit
    {16, 14, 16, 15, 10, 14}, // Masquerain
    {16, 11, 16, 11, 13, 11}, // Wailmer
    {32, 19, 32, 18, 38, 12}, // Wailord
    {15, 10, 15, 9, 12, 9}, // Skitty
    {16, 13, 16, 12, 13, 12}, // Delcatty
    {16, 11, 16, 11, 10, 11}, // Kecleon
    {16, 11, 16, 11, 7, 11}, // Baltoy
    {16, 14, 16, 14, 11, 13}, // Claydol
    {15, 11, 15, 11, 11, 11}, // Nosepass
    {16, 10, 16, 11, 15, 10}, // Torkoal
    {16, 9, 16, 9, 9, 10}, // Sableye
    {15, 9, 16, 9, 9, 11}, // Barboach
    {15, 10, 15, 11, 14, 9}, // Whiscash
    {16, 9, 16, 9, 8, 9}, // Luvdisc
    {15, 10, 15, 11, 11, 10}, // Corphish
    {16, 13, 16, 13, 13, 12}, // Crawdaunt
    {16, 9, 16, 9, 10, 9}, // Feebas
    {19, 13, 11, 12, 14, 13}, // Milotic
    {15, 12, 15, 12, 11, 11}, // Carvanha
    {16, 14, 16, 12, 13, 14}, // Sharpedo
    {16, 8, 16, 8, 10, 8}, // Trapinch
    {16, 13, 16, 11, 15, 14}, // Vibrava
    {16, 17, 16, 17, 15, 14}, // Flygon
    {16, 10, 16, 10, 9, 11}, // Makuhita
    {14, 14, 14, 13, 14, 13}, // Hariyama
    {15, 11, 15, 9, 13, 8}, // Electrike
    {15, 13, 15, 13, 14, 13}, // Manectric
    {15, 12, 15, 12, 15, 12}, // Numel
    {15, 14, 15, 14, 17, 12}, // Camerupt
    {15, 9, 15, 9, 11, 9}, // Spheal
    {15, 12, 15, 13, 15, 12}, // Sealeo
    {15, 13, 15, 14, 16, 14}, // Walrein
    {15, 9, 15, 9, 8, 9}, // Cacnea
    {15, 12, 15, 12, 11, 12}, // Cacturne
    {15, 9, 15, 9, 9, 9}, // Snorunt
    {16, 12, 16, 12, 11, 14}, // Glalie
    {16, 11, 16, 11, 10, 11}, // Lunatone
    {16, 13, 16, 13, 8, 13}, // Solrock
    {17, 9, 14, 8, 13, 7}, // Azurill
    {15, 11, 15, 11, 8, 10}, // Spoink
    {15, 11, 15, 11, 11, 11}, // Grumpig
    {15, 9, 15, 9, 9, 9}, // Plusle
    {15, 9, 15, 9, 9, 9}, // Minun
    {17, 11, 14, 11, 13, 11}, // Mawile
    {15, 12, 15, 11, 8, 11}, // Meditite
    {15, 13, 15, 13, 8, 13}, // Medicham
    {16, 12, 16, 11, 11, 10}, // Swablu
    {16, 16, 16, 16, 14, 15}, // Altaria
    {16, 10, 16, 10, 10, 9}, // Wynaut
    {16, 12, 16, 11, 9, 12}, // Duskull
    {15, 13, 15, 13, 13, 13}, // Dusclops
    {15, 10, 15, 10, 8, 10}, // Roselia
    {15, 9, 15, 9, 12, 8}, // Slakoth
    {16, 11, 16, 11, 14, 11}, // Vigoroth
    {16, 15, 16, 14, 14, 14}, // Slaking
    {16, 9, 16, 9, 10, 9}, // Gulpin
    {16, 14, 16, 14, 14, 14}, // Swalot
    {16, 13, 16, 13, 16, 13}, // Tropius
    {16, 8, 16, 8, 8, 8}, // Whismur
    {16, 11, 16, 11, 9, 12}, // Loudred
    {16, 14, 16, 14, 17, 15}, // Exploud
    {15, 8, 15, 8, 8, 9}, // Clamperl
    {18, 15, 11, 14, 14, 14}, // Huntail
    {17, 16, 12, 14, 15, 11}, // Gorebyss
    {14, 14, 16, 14, 15, 13}, // Absol
    {15, 11, 15, 11, 8, 11}, // Shuppet
    {17, 10, 16, 10, 9, 10}, // Banette
    {18, 14, 14, 14, 16, 10}, // Seviper
    {15, 12, 15, 12, 13, 12}, // Zangoose
    {15, 13, 15, 12, 14, 10}, // Relicanth
    {16, 8, 16, 8, 9, 8}, // Aron
    {15, 13, 15, 12, 16, 10}, // Lairon
    {15, 15, 15, 14, 16, 14}, // Aggron
    {15, 11, 15, 10, 7, 10}, // Castform
    {15, 14, 15, 14, 12, 12}, // Volbeat
    {16, 13, 16, 13, 11, 12}, // Illumise
    {16, 11, 16, 12, 10, 11}, // Lileep
    {16, 15, 16, 15, 12, 15}, // Cradily
    {16, 11, 16, 10, 14, 9}, // Anorith
    {15, 13, 15, 12, 13, 13}, // Armaldo
    {15, 9, 15, 9, 9, 9}, // Ralts
    {15, 11, 15, 11, 9, 11}, // Kirlia
    {16, 12, 16, 13, 10, 12}, // Gardevoir
    {16, 11, 16, 11, 10, 10}, // Bagon
    {16, 10, 16, 10, 11, 10}, // Shelgon
    {16, 14, 16, 13, 16, 13}, // Salamence
    {15, 13, 15, 12, 12, 12}, // Beldum
    {16, 14, 16, 13, 15, 13}, // Metang
    {16, 14, 16, 14, 13, 13}, // Metagross
    {15, 14, 15, 14, 11, 14}, // Regirock
    {15, 13, 15, 13, 12, 13}, // Regice
    {16, 13, 16, 13, 10, 13}, // Registeel
    {16, 17, 16, 15, 16, 14}, // Kyogre
    {15, 16, 16, 16, 16, 16}, // Groudon
    {16, 17, 16, 16, 16, 16}, // Rayquaza
    {16, 15, 16, 14, 16, 15}, // Latias
    {16, 15, 16, 14, 15, 15}, // Latios
    {15, 13, 15, 13, 10, 12}, // Jirachi
    {16, 14, 16, 14, 9, 13}, // Deoxys (Normal)
    {16, 11, 16, 11, 7, 11}, // Chimecho
};

// Poke Ball sprites (the follower goes into / comes out of the ball it was caught in)
static const u32 sPokeBallPic[]    = INCBIN_U32("graphics/object_events/pics/misc/item_ball.4bpp");
static const u32 sGreatBallPic[]   = INCBIN_U32("graphics/object_events/pics/misc/great_ball.4bpp");
static const u32 sUltraBallPic[]   = INCBIN_U32("graphics/object_events/pics/misc/ultra_ball.4bpp");
static const u32 sMasterBallPic[]  = INCBIN_U32("graphics/object_events/pics/misc/master_ball.4bpp");
static const u32 sSafariBallPic[]  = INCBIN_U32("graphics/object_events/pics/misc/safari_ball.4bpp");
static const u32 sNetBallPic[]     = INCBIN_U32("graphics/object_events/pics/misc/net_ball.4bpp");
static const u32 sDiveBallPic[]    = INCBIN_U32("graphics/object_events/pics/misc/dive_ball.4bpp");
static const u32 sNestBallPic[]    = INCBIN_U32("graphics/object_events/pics/misc/nest_ball.4bpp");
static const u32 sRepeatBallPic[]  = INCBIN_U32("graphics/object_events/pics/misc/repeat_ball.4bpp");
static const u32 sTimerBallPic[]   = INCBIN_U32("graphics/object_events/pics/misc/timer_ball.4bpp");
static const u32 sLuxuryBallPic[]  = INCBIN_U32("graphics/object_events/pics/misc/luxury_ball.4bpp");
static const u32 sPremierBallPic[] = INCBIN_U32("graphics/object_events/pics/misc/premier_ball.4bpp");

static const u16 sPokeBallPal[]    = INCBIN_U16("graphics/object_events/palettes/npc_white.gbapal");
static const u16 sGreatBallPal[]   = INCBIN_U16("graphics/object_events/palettes/great_ball.gbapal");
static const u16 sUltraBallPal[]   = INCBIN_U16("graphics/object_events/palettes/ultra_ball.gbapal");
static const u16 sMasterBallPal[]  = INCBIN_U16("graphics/object_events/palettes/master_ball.gbapal");
static const u16 sSafariBallPal[]  = INCBIN_U16("graphics/object_events/palettes/safari_ball.gbapal");
static const u16 sNetBallPal[]     = INCBIN_U16("graphics/object_events/palettes/net_ball.gbapal");
static const u16 sDiveBallPal[]    = INCBIN_U16("graphics/object_events/palettes/dive_ball.gbapal");
static const u16 sNestBallPal[]    = INCBIN_U16("graphics/object_events/palettes/nest_ball.gbapal");
static const u16 sRepeatBallPal[]  = INCBIN_U16("graphics/object_events/palettes/repeat_ball.gbapal");
static const u16 sTimerBallPal[]   = INCBIN_U16("graphics/object_events/palettes/timer_ball.gbapal");
static const u16 sLuxuryBallPal[]  = INCBIN_U16("graphics/object_events/palettes/luxury_ball.gbapal");
static const u16 sPremierBallPal[] = INCBIN_U16("graphics/object_events/palettes/premier_ball.gbapal");

static const struct SpriteFrameImage sPicTable_PokeBall[] = { overworld_frame(sPokeBallPic, 2, 2, 0) };
static const struct SpriteFrameImage sPicTable_GreatBall[] = { overworld_frame(sGreatBallPic, 2, 2, 0) };
static const struct SpriteFrameImage sPicTable_UltraBall[] = { overworld_frame(sUltraBallPic, 2, 2, 0) };
static const struct SpriteFrameImage sPicTable_MasterBall[] = { overworld_frame(sMasterBallPic, 2, 2, 0) };
static const struct SpriteFrameImage sPicTable_SafariBall[] = { overworld_frame(sSafariBallPic, 2, 2, 0) };
static const struct SpriteFrameImage sPicTable_NetBall[] = { overworld_frame(sNetBallPic, 2, 2, 0) };
static const struct SpriteFrameImage sPicTable_DiveBall[] = { overworld_frame(sDiveBallPic, 2, 2, 0) };
static const struct SpriteFrameImage sPicTable_NestBall[] = { overworld_frame(sNestBallPic, 2, 2, 0) };
static const struct SpriteFrameImage sPicTable_RepeatBall[] = { overworld_frame(sRepeatBallPic, 2, 2, 0) };
static const struct SpriteFrameImage sPicTable_TimerBall[] = { overworld_frame(sTimerBallPic, 2, 2, 0) };
static const struct SpriteFrameImage sPicTable_LuxuryBall[] = { overworld_frame(sLuxuryBallPic, 2, 2, 0) };
static const struct SpriteFrameImage sPicTable_PremierBall[] = { overworld_frame(sPremierBallPic, 2, 2, 0) };

#define FOLLOWER_BALL_PAL_TAG_BASE 0x2200

struct FollowerBall
{
    u16 item;
    const struct SpriteFrameImage *images;
    const u16 *pal;
};

static const struct FollowerBall sFollowerBalls[] =
{
    {ITEM_POKE_BALL, sPicTable_PokeBall, sPokeBallPal}, // default, keep first
    {ITEM_GREAT_BALL, sPicTable_GreatBall, sGreatBallPal},
    {ITEM_ULTRA_BALL, sPicTable_UltraBall, sUltraBallPal},
    {ITEM_MASTER_BALL, sPicTable_MasterBall, sMasterBallPal},
    {ITEM_SAFARI_BALL, sPicTable_SafariBall, sSafariBallPal},
    {ITEM_NET_BALL, sPicTable_NetBall, sNetBallPal},
    {ITEM_DIVE_BALL, sPicTable_DiveBall, sDiveBallPal},
    {ITEM_NEST_BALL, sPicTable_NestBall, sNestBallPal},
    {ITEM_REPEAT_BALL, sPicTable_RepeatBall, sRepeatBallPal},
    {ITEM_TIMER_BALL, sPicTable_TimerBall, sTimerBallPal},
    {ITEM_LUXURY_BALL, sPicTable_LuxuryBall, sLuxuryBallPal},
    {ITEM_PREMIER_BALL, sPicTable_PremierBall, sPremierBallPal},
};

static const struct OamData sOam_FollowerBall =
{
    .shape = SPRITE_SHAPE(16x16),
    .size = SPRITE_SIZE(16x16),
    .priority = 2,
};

static const union AnimCmd sAnim_FollowerBall[] =
{
    ANIMCMD_FRAME(0, 1),
    ANIMCMD_END,
};

static const union AnimCmd *const sAnimTable_FollowerBall[] =
{
    sAnim_FollowerBall,
};

// Expanding sparkle shown when the follower comes out of its ball
static const u32 sExpandingSparklePic[] = INCBIN_U32("graphics/field_effects/pics/expanding_sparkle.4bpp");

static const struct SpriteFrameImage sPicTable_ExpandingSparkle[] =
{
    overworld_frame(sExpandingSparklePic, 4, 4, 0),
    overworld_frame(sExpandingSparklePic, 4, 4, 1),
    overworld_frame(sExpandingSparklePic, 4, 4, 2),
    overworld_frame(sExpandingSparklePic, 4, 4, 3),
    overworld_frame(sExpandingSparklePic, 4, 4, 4),
    overworld_frame(sExpandingSparklePic, 4, 4, 5),
};

static const union AnimCmd sAnim_ExpandingSparkle[] =
{
    ANIMCMD_FRAME(0, 3),
    ANIMCMD_FRAME(1, 3),
    ANIMCMD_FRAME(2, 3),
    ANIMCMD_FRAME(3, 3),
    ANIMCMD_FRAME(4, 3),
    ANIMCMD_FRAME(5, 3),
    ANIMCMD_END,
};

static const union AnimCmd *const sAnimTable_ExpandingSparkle[] =
{
    sAnim_ExpandingSparkle,
};

static const union AffineAnimCmd sAffineAnim_ExpandingSparkle_North[] =
{
    AFFINEANIMCMD_FRAME(0, 0, 0, 1),
    AFFINEANIMCMD_FRAME(0, 0, -4, 16),
    AFFINEANIMCMD_END,
};

static const union AffineAnimCmd sAffineAnim_ExpandingSparkle_NorthEast[] =
{
    AFFINEANIMCMD_FRAME(0, 0, -32, 1),
    AFFINEANIMCMD_FRAME(0, 0, -4, 16),
    AFFINEANIMCMD_END,
};

static const union AffineAnimCmd sAffineAnim_ExpandingSparkle_East[] =
{
    AFFINEANIMCMD_FRAME(0, 0, -64, 1),
    AFFINEANIMCMD_FRAME(0, 0, -4, 16),
    AFFINEANIMCMD_END,
};

static const union AffineAnimCmd sAffineAnim_ExpandingSparkle_SouthEast[] =
{
    AFFINEANIMCMD_FRAME(0, 0, -96, 1),
    AFFINEANIMCMD_FRAME(0, 0, -4, 16),
    AFFINEANIMCMD_END,
};

static const union AffineAnimCmd sAffineAnim_ExpandingSparkle_South[] =
{
    AFFINEANIMCMD_FRAME(0, 0, -128, 1),
    AFFINEANIMCMD_FRAME(0, 0, -4, 16),
    AFFINEANIMCMD_END,
};

static const union AffineAnimCmd sAffineAnim_ExpandingSparkle_SouthWest[] =
{
    AFFINEANIMCMD_FRAME(0, 0, 96, 1),
    AFFINEANIMCMD_FRAME(0, 0, -4, 16),
    AFFINEANIMCMD_END,
};

static const union AffineAnimCmd sAffineAnim_ExpandingSparkle_West[] =
{
    AFFINEANIMCMD_FRAME(0, 0, 64, 1),
    AFFINEANIMCMD_FRAME(0, 0, -4, 16),
    AFFINEANIMCMD_END,
};

static const union AffineAnimCmd sAffineAnim_ExpandingSparkle_NorthWest[] =
{
    AFFINEANIMCMD_FRAME(0, 0, 32, 1),
    AFFINEANIMCMD_FRAME(0, 0, -4, 16),
    AFFINEANIMCMD_END,
};

static const union AffineAnimCmd *const sAffineAnimTable_ExpandingSparkle[] = {
    sAffineAnim_ExpandingSparkle_North,
    sAffineAnim_ExpandingSparkle_NorthEast,
    sAffineAnim_ExpandingSparkle_East,
    sAffineAnim_ExpandingSparkle_SouthEast,
    sAffineAnim_ExpandingSparkle_South,
    sAffineAnim_ExpandingSparkle_SouthWest,
    sAffineAnim_ExpandingSparkle_West,
    sAffineAnim_ExpandingSparkle_NorthWest,
};

static const struct OamData sOam_ExpandingSparkle =
{
    .affineMode = ST_OAM_AFFINE_NORMAL,
    .shape = SPRITE_SHAPE(32x32),
    .size = SPRITE_SIZE(32x32),
    .priority = 1,
};

static void SpriteCB_FollowerSparkle(struct Sprite *sprite);
static void SpriteCB_FollowerBigSparkle(struct Sprite *sprite);

static const struct SpriteTemplate sSpriteTemplate_ExpandingSparkle =
{
    .tileTag = TAG_NONE,
    .paletteTag = FLDEFF_PAL_TAG_SMALL_SPARKLE,
    .oam = &sOam_ExpandingSparkle,
    .anims = sAnimTable_ExpandingSparkle,
    .images = sPicTable_ExpandingSparkle,
    .affineAnims = sAffineAnimTable_ExpandingSparkle,
    .callback = SpriteCB_FollowerSparkle,
};

// ---------------------------------------------------------------------------
// Basic accessors
// ---------------------------------------------------------------------------

u8 GetFollowerObjectId(void)
{
    if (!gSaveBlock2Ptr->follower.inProgress)
        return OBJECT_EVENTS_COUNT;

    return gSaveBlock2Ptr->follower.objId;
}

u8 GetFollowerLocalId(void)
{
    if (!gSaveBlock2Ptr->follower.inProgress)
        return 0;

    return gObjectEvents[gSaveBlock2Ptr->follower.objId].localId;
}

const u8 *GetFollowerScriptPointer(void)
{
    if (!gSaveBlock2Ptr->follower.inProgress)
        return NULL;

    return gSaveBlock2Ptr->follower.script;
}

const struct ObjectEventTemplate *GetFollowerObjectEventTemplate(void)
{
    return &sFollowerObjectEventTemplate;
}

static u8 GetFollowerMapObjId(void)
{
    return gSaveBlock2Ptr->follower.objId;
}

bool8 PlayerHasFollower(void)
{
    return gSaveBlock2Ptr->follower.inProgress;
}

bool8 IsPlayerOnFoot(void)
{
    if (gPlayerAvatar.flags & PLAYER_AVATAR_FLAG_ON_FOOT)
        return TRUE;
    else
        return FALSE;
}

bool8 FollowerComingThroughDoor(void)
{
    if (!PlayerHasFollower())
        return FALSE;

    if (gSaveBlock2Ptr->follower.comeOutDoorStairs)
        return TRUE;

    return FALSE;
}

void HideFollower(void)
{
    if (!gSaveBlock2Ptr->follower.inProgress)
        return;

    gObjectEvents[GetFollowerMapObjId()].invisible = TRUE;
}

void ShowFollower(void)
{
    if (!gSaveBlock2Ptr->follower.inProgress)
        return;

    gObjectEvents[GetFollowerMapObjId()].invisible = FALSE;
}

// Puts the follower away instantly (used when the player starts surfing)
void FollowMe_HideFollowerForSurf(void)
{
    if (!gSaveBlock2Ptr->follower.inProgress)
        return;

    gSaveBlock2Ptr->follower.comeOutDoorStairs = 0;
    gSaveBlock2Ptr->follower.warpEnd = 0;
    HideFollower();
}

void FollowMe_SetIndicatorToComeOutDoor(void)
{
    if (gSaveBlock2Ptr->follower.inProgress)
        gSaveBlock2Ptr->follower.comeOutDoorStairs = 1;
}

void FollowMe_SetIndicatorToRecreateSurfBlob(void)
{
    if (gSaveBlock2Ptr->follower.inProgress)
        gSaveBlock2Ptr->follower.createSurfBlob = 2;
}

void FollowMe_TryRemoveFollowerOnWhiteOut(void)
{
    if (gSaveBlock2Ptr->follower.inProgress)
    {
        if (gSaveBlock2Ptr->follower.flags & FOLLOWER_FLAG_CLEAR_ON_WHITE_OUT)
            gSaveBlock2Ptr->follower.inProgress = FALSE;
        else
            FollowMe_WarpSetEnd();
    }
}

// ---------------------------------------------------------------------------
// Follower Pokemon
// ---------------------------------------------------------------------------

static u8 GetLeadMonNotFaintedIndex(void)
{
    u8 i;
    u8 partyCount = CalculatePlayerPartyCount();

    for (i = 0; i < partyCount; i++)
    {
        u16 species = GetMonData(&gPlayerParty[i], MON_DATA_SPECIES_OR_EGG, NULL);
        if (species != SPECIES_NONE && species != SPECIES_EGG && GetMonData(&gPlayerParty[i], MON_DATA_HP, NULL) != 0)
            return i;
    }
    for (i = 0; i < partyCount; i++)
    {
        u16 species = GetMonData(&gPlayerParty[i], MON_DATA_SPECIES_OR_EGG, NULL);
        if (species != SPECIES_NONE && species != SPECIES_EGG)
            return i;
    }
    return 0;
}

u16 GetFollowerPokemonSpecies(void)
{
    u16 species;

    if (CalculatePlayerPartyCount() == 0)
        return SPECIES_NONE;

    species = GetMonData(&gPlayerParty[GetLeadMonNotFaintedIndex()], MON_DATA_SPECIES_OR_EGG, NULL);
    if (species == SPECIES_EGG)
        return SPECIES_NONE;
    if (species >= FOLLOWER_GFX_COUNT) // shouldn't happen in the party
        species = SPECIES_UNOWN;
    return species;
}

u16 GetFollowerPokemonGraphicsId(void)
{
    return OBJ_EVENT_GFX_FOLLOWER(GetFollowerPokemonSpecies());
}

bool8 FollowerPokemon_IsEnabled(void)
{
    return !FlagGet(FLAG_FOLLOWERS_DISABLED);
}

// Starts the follower if it is enabled and the player has a Pokemon to show.
// Only sets up the follower state; the object itself is created by CreateFollowerAvatar.
void FollowerPokemon_TryStart(void)
{
    if (gSaveBlock2Ptr->follower.inProgress)
        return;
    if (!FollowerPokemon_IsEnabled() || QL_IS_PLAYBACK_STATE)
        return;
    if (GetFollowerPokemonSpecies() == SPECIES_NONE)
        return;

    memset(&gSaveBlock2Ptr->follower, 0, sizeof(gSaveBlock2Ptr->follower));
    gSaveBlock2Ptr->follower.inProgress = TRUE;
    gSaveBlock2Ptr->follower.objId = OBJECT_EVENTS_COUNT;
    gSaveBlock2Ptr->follower.graphicsId = GetFollowerPokemonGraphicsId();
    gSaveBlock2Ptr->follower.map.id = OBJ_EVENT_ID_FOLLOWER;
    gSaveBlock2Ptr->follower.map.number = gSaveBlock1Ptr->location.mapNum;
    gSaveBlock2Ptr->follower.map.group = gSaveBlock1Ptr->location.mapGroup;
    gSaveBlock2Ptr->follower.script = EventScript_FollowerPokemon;
    gSaveBlock2Ptr->follower.flag = 0;
    gSaveBlock2Ptr->follower.flags = FOLLOWER_POKEMON_FLAGS;
    gSaveBlock2Ptr->follower.currentSprite = FOLLOWER_SPRITE_INDEX_NORMAL;
}

// Script: enablefollower. The follower pops out of its ball on the player's next step.
void FollowerPokemon_Enable(void)
{
    FlagClear(FLAG_FOLLOWERS_DISABLED);
    if (gSaveBlock2Ptr->follower.inProgress)
        return;

    FollowerPokemon_TryStart();
    if (gSaveBlock2Ptr->follower.inProgress)
    {
        CreateFollowerAvatar();
        FollowMe_WarpSetEnd();
    }
}

// Script: disablefollower
void FollowerPokemon_Disable(void)
{
    FlagSet(FLAG_FOLLOWERS_DISABLED);
    DestroyFollower();
}

// ---------------------------------------------------------------------------
// Follower Pokemon dialogue
// ---------------------------------------------------------------------------
// The lines live in data/text/follower_dialogue.inc. Each entry has up to two conditions;
// every matching entry gets a score (the sum of its conditions' weights below), and one of the
// matching entries with the highest score is picked at random. So a line for "Pikachu in the
// Power Plant" beats a line for "Pikachu" or "the Power Plant", which beat type/generic lines.

extern const u8 gFollowerMessages[];

#define FOLLOWER_MSG_END 0xFF

static const u8 sFollowerConditionWeights[] =
{
    [FOLLOWER_COND_NONE]       = 0,
    [FOLLOWER_COND_SPECIES]    = 10,
    [FOLLOWER_COND_TYPE]       = 3,
    [FOLLOWER_COND_EGG_GROUP]  = 3,
    [FOLLOWER_COND_MAP]        = 8,
    [FOLLOWER_COND_MAPSEC]     = 7,
    [FOLLOWER_COND_MAP_TYPE]   = 4,
    [FOLLOWER_COND_TERRAIN]    = 5,
    [FOLLOWER_COND_WEATHER]    = 5,
    [FOLLOWER_COND_FRIENDSHIP] = 2,
    [FOLLOWER_COND_LOW_HP]     = 9,
    [FOLLOWER_COND_STATUS]     = 9,
    [FOLLOWER_COND_FLAG]       = 1,
    [FOLLOWER_COND_NOT_FLAG]   = 1,
};

// Entry layout (see the follower_message macro): cond1, cond2, value1 (2 bytes), value2 (2 bytes), text...EOS
#define MSG_COND1(p)  ((p)[0])
#define MSG_COND2(p)  ((p)[1])
#define MSG_VALUE1(p) ((p)[2] | ((p)[3] << 8))
#define MSG_VALUE2(p) ((p)[4] | ((p)[5] << 8))
#define MSG_TEXT(p)   ((p) + 6)

static const u8 *NextFollowerMessage(const u8 *entry)
{
    const u8 *text = MSG_TEXT(entry);
    while (*text != EOS)
        text++;
    return text + 1;
}

static bool8 IsFollowerNextToWater(s16 x, s16 y)
{
    return MetatileBehavior_IsSurfable(MapGridGetMetatileBehaviorAt(x + 1, y))
        || MetatileBehavior_IsSurfable(MapGridGetMetatileBehaviorAt(x - 1, y))
        || MetatileBehavior_IsSurfable(MapGridGetMetatileBehaviorAt(x, y + 1))
        || MetatileBehavior_IsSurfable(MapGridGetMetatileBehaviorAt(x, y - 1));
}

static bool8 FollowerConditionMatches(u8 condition, u16 value, struct Pokemon *mon, u16 species)
{
    struct ObjectEvent *obj;
    u8 behavior;

    if (FollowerObjectExists())
        obj = &gObjectEvents[GetFollowerMapObjId()];
    else
        obj = &gObjectEvents[gPlayerAvatar.objectEventId];
    behavior = MapGridGetMetatileBehaviorAt(obj->currentCoords.x, obj->currentCoords.y);

    switch (condition)
    {
    case FOLLOWER_COND_NONE:
        return TRUE;
    case FOLLOWER_COND_SPECIES:
        return species == value;
    case FOLLOWER_COND_TYPE:
        return gSpeciesInfo[species].types[0] == value || gSpeciesInfo[species].types[1] == value;
    case FOLLOWER_COND_EGG_GROUP:
        return gSpeciesInfo[species].eggGroups[0] == value || gSpeciesInfo[species].eggGroups[1] == value;
    case FOLLOWER_COND_MAP:
        return ((gSaveBlock1Ptr->location.mapGroup << 8) | gSaveBlock1Ptr->location.mapNum) == value;
    case FOLLOWER_COND_MAPSEC:
        return gMapHeader.regionMapSectionId == value;
    case FOLLOWER_COND_MAP_TYPE:
        return gMapHeader.mapType == value;
    case FOLLOWER_COND_TERRAIN:
        switch (value)
        {
        case FOLLOWER_TERRAIN_SAND:
            return MetatileBehavior_IsSand(behavior);
        case FOLLOWER_TERRAIN_TALL_GRASS:
            return MetatileBehavior_IsTallGrass(behavior) || MetatileBehavior_IsLongGrass(behavior);
        case FOLLOWER_TERRAIN_WATER:
            return IsFollowerNextToWater(obj->currentCoords.x, obj->currentCoords.y);
        case FOLLOWER_TERRAIN_ICE:
            return MetatileBehavior_IsIce(behavior);
        }
        return FALSE;
    case FOLLOWER_COND_WEATHER:
        return GetCurrentWeather() == value;
    case FOLLOWER_COND_FRIENDSHIP:
        return GetMonData(mon, MON_DATA_FRIENDSHIP, NULL) >= value;
    case FOLLOWER_COND_LOW_HP:
        return GetMonData(mon, MON_DATA_HP, NULL) * 4 <= GetMonData(mon, MON_DATA_MAX_HP, NULL);
    case FOLLOWER_COND_STATUS:
        return GetMonData(mon, MON_DATA_STATUS, NULL) != 0;
    case FOLLOWER_COND_FLAG:
        return FlagGet(value);
    case FOLLOWER_COND_NOT_FLAG:
        return !FlagGet(value);
    }
    return FALSE;
}

// Returns the entry's score, or -1 if it doesn't apply
static s16 GetFollowerMessageScore(const u8 *entry, struct Pokemon *mon, u16 species)
{
    u8 cond1 = MSG_COND1(entry), cond2 = MSG_COND2(entry);

    if (cond1 >= ARRAY_COUNT(sFollowerConditionWeights) || cond2 >= ARRAY_COUNT(sFollowerConditionWeights))
        return -1;
    if (!FollowerConditionMatches(cond1, MSG_VALUE1(entry), mon, species))
        return -1;
    if (!FollowerConditionMatches(cond2, MSG_VALUE2(entry), mon, species))
        return -1;
    return sFollowerConditionWeights[cond1] + sFollowerConditionWeights[cond2];
}

static const u8 *ChooseFollowerMessage(struct Pokemon *mon, u16 species)
{
    const u8 *entry;
    s16 score, bestScore = -1;
    u16 count = 0, pick;

    for (entry = gFollowerMessages; MSG_COND1(entry) != FOLLOWER_MSG_END; entry = NextFollowerMessage(entry))
    {
        score = GetFollowerMessageScore(entry, mon, species);
        if (score > bestScore)
        {
            bestScore = score;
            count = 1;
        }
        else if (score == bestScore && score >= 0)
        {
            count++;
        }
    }
    if (count == 0)
        return NULL;

    pick = Random() % count;
    for (entry = gFollowerMessages; MSG_COND1(entry) != FOLLOWER_MSG_END; entry = NextFollowerMessage(entry))
    {
        if (GetFollowerMessageScore(entry, mon, species) == bestScore)
        {
            if (pick == 0)
                return MSG_TEXT(entry);
            pick--;
        }
    }
    return NULL;
}

// Run when the player talks to the follower: buffers the lead Pokemon's nickname in gStringVar1,
// its species in VAR_0x8004 (for the cry) and the chosen line, with placeholders expanded, in gStringVar4.
void FollowerPokemon_BufferLeadMonInfo(void)
{
    struct Pokemon *mon = &gPlayerParty[GetLeadMonNotFaintedIndex()];
    u16 species = GetFollowerPokemonSpecies();
    const u8 *text;

    GetMonData(mon, MON_DATA_NICKNAME, gStringVar1);
    StringGet_Nickname(gStringVar1);
    gSpecialVar_0x8004 = species;

    text = ChooseFollowerMessage(mon, species);
    if (text == NULL)
        text = gText_FollowerDefault;
    StringExpandPlaceholders(gStringVar4, text);
}

// Called when the lead Pokemon may have changed (party menu, PC, battle, Pokemon Center...)
void UpdateFollowerPokemonGraphic(void)
{
    u16 newGraphicsId;
    struct ObjectEvent *follower;

    if (!gSaveBlock2Ptr->follower.inProgress)
        FollowerPokemon_TryStart();
    if (!gSaveBlock2Ptr->follower.inProgress || QL_IS_PLAYBACK_STATE)
        return;

    if (GetFollowerPokemonSpecies() == SPECIES_NONE)
        return;

    // Just got its first Pokemon (or the follower couldn't spawn before): create it now.
    // It pops out of its ball on the player's next step.
    if (GetFollowerMapObjId() >= OBJECT_EVENTS_COUNT)
    {
        CreateFollowerAvatar();
        return;
    }

    newGraphicsId = GetFollowerPokemonGraphicsId();
    if (newGraphicsId == gSaveBlock2Ptr->follower.graphicsId)
        return;

    gSaveBlock2Ptr->follower.graphicsId = newGraphicsId;

    // Only update the sprite if the follower object exists on this map right now
    follower = &gObjectEvents[GetFollowerMapObjId()];
    if (GetFollowerMapObjId() < OBJECT_EVENTS_COUNT && follower->active && follower->localId == OBJ_EVENT_ID_FOLLOWER)
    {
        bool8 invisible = follower->invisible;
        RecreateFollowerObject(newGraphicsId);
        gObjectEvents[GetFollowerMapObjId()].invisible = invisible;
    }
    gSpecialVar_0x8014 = 0;
}

// ---------------------------------------------------------------------------
// Following logic
// ---------------------------------------------------------------------------

void FollowMe(struct ObjectEvent *npc, u8 state, bool8 ignoreScriptActive)
{
    struct ObjectEvent *player = &gObjectEvents[gPlayerAvatar.objectEventId];
    struct ObjectEvent *follower = &gObjectEvents[GetFollowerMapObjId()];
    u8 dir;
    u8 newState;
    u8 taskId;

    if (player != npc) // Only when the player moves
        return;
    else if (!gSaveBlock2Ptr->follower.inProgress || GetFollowerMapObjId() >= OBJECT_EVENTS_COUNT)
        return;
    else if (QL_IS_PLAYBACK_STATE)
        return;
    else if (ArePlayerFieldControlsLocked() && !ignoreScriptActive)
        return; // Don't follow during a script

    // Check if state would cause hidden follower to reappear
    if (IsStateMovement(state) && gSaveBlock2Ptr->follower.warpEnd)
    {
        gSaveBlock2Ptr->follower.warpEnd = 0;

        if (gSaveBlock2Ptr->follower.comeOutDoorStairs == 1)
        {
            if (TestPlayerAvatarFlags(PLAYER_AVATAR_FLAG_BIKE))
            {
                gSaveBlock2Ptr->follower.comeOutDoorStairs = 0;
            }
            else
            {
                gPlayerAvatar.preventStep = TRUE;
                taskId = CreateTask(Task_FollowerOutOfDoor, 1);
                gTasks[taskId].data[0] = 0;
                gTasks[taskId].data[2] = follower->currentCoords.x;
                gTasks[taskId].data[3] = follower->currentCoords.y;
                goto RESET;
            }
        }
        else if (gSaveBlock2Ptr->follower.comeOutDoorStairs == 2)
        {
            gSaveBlock2Ptr->follower.comeOutDoorStairs = 0;
        }

        // Stay hidden while surfing or biking; the follower comes back out of its ball later
        if (!(TestPlayerAvatarFlags(PLAYER_AVATAR_FLAG_UNDERWATER) || TestPlayerAvatarFlags(PLAYER_AVATAR_FLAG_SURFING) || TestPlayerAvatarFlags(PLAYER_AVATAR_FLAG_BIKE)))
            follower->invisible = FALSE;
        MoveObjectEventToMapCoords(follower, player->currentCoords.x, player->currentCoords.y);
        ObjectEventTurn(follower, player->facingDirection); // Face the same way as the player when coming out of hiding
    }

    dir = DetermineFollowerDirection(player, follower);

    if (dir == DIR_NONE)
        goto RESET;

    newState = DetermineFollowerState(follower, state, dir);
    if (newState == MOVEMENT_INVALID)
        goto RESET;

    ObjectEventClearHeldMovementIfActive(follower);
    ObjectEventSetHeldMovement(follower, newState);
    PlayerLogCoordinates(player);

    switch (newState)
    {
    case MOVEMENT_ACTION_JUMP_2_DOWN ... MOVEMENT_ACTION_JUMP_2_RIGHT:
        if (follower->invisible == FALSE)
        {
            CreateTask(Task_ReallowPlayerMovement, 1); // Synchronize movements on stairs and ledges
            gPlayerAvatar.preventStep = TRUE;   // allow follower to catch up
        }
    }

RESET:
    ObjectEventClearHeldMovementIfFinished(follower);
}

static void Task_ReallowPlayerMovement(u8 taskId)
{
    bool8 animStatus = ObjectEventClearHeldMovementIfFinished(&gObjectEvents[GetFollowerMapObjId()]);
    if (animStatus == 0 && gObjectEvents[GetFollowerMapObjId()].movementActionId != MOVEMENT_ACTION_FOLLOWING_POKEMON)
    {
        if (TestPlayerAvatarFlags(PLAYER_AVATAR_FLAG_DASH)
         && ObjectEventClearHeldMovementIfFinished(&gObjectEvents[gPlayerAvatar.objectEventId]))
            SetPlayerAvatarTransitionFlags(PLAYER_AVATAR_FLAG_ON_FOOT); // Temporarily stop running
        return;
    }

    gPlayerAvatar.preventStep = FALSE;
    DestroyTask(taskId);
}

static u8 DetermineFollowerDirection(struct ObjectEvent *player, struct ObjectEvent *follower)
{
    // Move the follower towards the player
    s8 delta_x = follower->currentCoords.x - player->currentCoords.x;
    s8 delta_y = follower->currentCoords.y - player->currentCoords.y;

    if (delta_x < 0)
        return DIR_EAST;
    else if (delta_x > 0)
        return DIR_WEST;

    if (delta_y < 0)
        return DIR_SOUTH;
    else if (delta_y > 0)
        return DIR_NORTH;

    return DIR_NONE;
}

static void PlayerLogCoordinates(struct ObjectEvent *player)
{
    gSaveBlock2Ptr->follower.log.x = player->currentCoords.x;
    gSaveBlock2Ptr->follower.log.y = player->currentCoords.y;
}

#define RETURN_STATE(state, dir) return newState == MOVEMENT_INVALID ? state + (dir - 1) : ReturnFollowerDelayedState(dir - 1);
static u8 DetermineFollowerState(struct ObjectEvent *follower, u8 state, u8 direction)
{
    u8 newState = MOVEMENT_INVALID;

    if (IsStateMovement(state) && gSaveBlock2Ptr->follower.delayedState)
        newState = gSaveBlock2Ptr->follower.delayedState + direction;

    // Clear ice tile stuff
    follower->disableAnim = FALSE;

    switch (state)
    {
    case MOVEMENT_ACTION_WALK_SLOW_DOWN ... MOVEMENT_ACTION_WALK_SLOW_RIGHT:
        RETURN_STATE(MOVEMENT_ACTION_WALK_SLOW_DOWN, direction);

    case MOVEMENT_ACTION_WALK_NORMAL_DOWN ... MOVEMENT_ACTION_WALK_NORMAL_RIGHT:
        RETURN_STATE(MOVEMENT_ACTION_WALK_NORMAL_DOWN, direction);

    case MOVEMENT_ACTION_JUMP_2_DOWN ... MOVEMENT_ACTION_JUMP_2_RIGHT:
        // If the follower is hidden because the player is on the bike, just walk
        if (!TestPlayerAvatarFlags(PLAYER_AVATAR_FLAG_BIKE))
        {
            // Ledge jump
            if ((newState - direction) >= MOVEMENT_ACTION_JUMP_2_DOWN && (newState - direction) <= MOVEMENT_ACTION_JUMP_2_RIGHT) // Previously jumped
            {
                newState = MOVEMENT_INVALID;
                RETURN_STATE(MOVEMENT_ACTION_JUMP_2_DOWN, direction); // Jump right away
            }

            gSaveBlock2Ptr->follower.delayedState = MOVEMENT_ACTION_JUMP_2_DOWN;
            RETURN_STATE(MOVEMENT_ACTION_WALK_NORMAL_DOWN, direction);
        }
        else
        {
            RETURN_STATE(MOVEMENT_ACTION_WALK_NORMAL_DOWN, direction);
        }

    case MOVEMENT_ACTION_WALK_FAST_DOWN ... MOVEMENT_ACTION_WALK_FAST_RIGHT:
        // Handle player on waterfall
        if (PlayerIsUnderWaterfall(&gObjectEvents[gPlayerAvatar.objectEventId]) && (state == MOVEMENT_ACTION_WALK_FAST_UP))
            return MOVEMENT_INVALID;

        // Ice tile: freeze the follower's animation
        if (MetatileBehavior_IsIce(follower->currentMetatileBehavior) || MetatileBehavior_IsTrickHouseSlipperyFloor(follower->currentMetatileBehavior))
            follower->disableAnim = TRUE;

        RETURN_STATE(MOVEMENT_ACTION_WALK_FAST_DOWN, direction);

    case MOVEMENT_ACTION_WALK_FASTER_DOWN ... MOVEMENT_ACTION_WALK_FASTER_RIGHT:
        // Bike
        if (MetatileBehavior_IsIce(follower->currentMetatileBehavior) || MetatileBehavior_IsTrickHouseSlipperyFloor(follower->currentMetatileBehavior))
            follower->disableAnim = TRUE;

        RETURN_STATE(MOVEMENT_ACTION_WALK_FASTER_DOWN, direction);

    case MOVEMENT_ACTION_RIDE_WATER_CURRENT_DOWN ... MOVEMENT_ACTION_RIDE_WATER_CURRENT_RIGHT:
        // Handle player on waterfall
        if (PlayerIsUnderWaterfall(&gObjectEvents[gPlayerAvatar.objectEventId]) && IsPlayerSurfingNorth())
            return MOVEMENT_INVALID;

        RETURN_STATE(MOVEMENT_ACTION_RIDE_WATER_CURRENT_DOWN, direction);

    case MOVEMENT_ACTION_SLIDE_DOWN ... MOVEMENT_ACTION_SLIDE_RIGHT:
        RETURN_STATE(MOVEMENT_ACTION_SLIDE_DOWN, direction);

    case MOVEMENT_ACTION_PLAYER_RUN_DOWN ... MOVEMENT_ACTION_PLAYER_RUN_RIGHT:
        // Running frames
        if (gSaveBlock2Ptr->follower.flags & FOLLOWER_FLAG_HAS_RUNNING_FRAMES)
            RETURN_STATE(MOVEMENT_ACTION_PLAYER_RUN_DOWN, direction);

        RETURN_STATE(MOVEMENT_ACTION_WALK_FAST_DOWN, direction);

    case MOVEMENT_ACTION_PLAYER_RUN_DOWN_SLOW ... MOVEMENT_ACTION_PLAYER_RUN_RIGHT_SLOW:
        RETURN_STATE(MOVEMENT_ACTION_WALK_NORMAL_DOWN, direction);

    case MOVEMENT_ACTION_JUMP_SPECIAL_DOWN ... MOVEMENT_ACTION_JUMP_SPECIAL_RIGHT:
        gSaveBlock2Ptr->follower.delayedState = MOVEMENT_ACTION_JUMP_SPECIAL_DOWN;
        RETURN_STATE(MOVEMENT_ACTION_WALK_NORMAL_DOWN, direction);

    case MOVEMENT_ACTION_JUMP_DOWN ... MOVEMENT_ACTION_JUMP_RIGHT:
        if (!TestPlayerAvatarFlags(PLAYER_AVATAR_FLAG_BIKE))
            gSaveBlock2Ptr->follower.delayedState = MOVEMENT_ACTION_JUMP_DOWN;
        RETURN_STATE(MOVEMENT_ACTION_WALK_NORMAL_DOWN, direction);

    default:
        return MOVEMENT_INVALID;
    }

    return newState;
}

static bool8 IsStateMovement(u8 state)
{
    switch (state)
    {
    case MOVEMENT_ACTION_FACE_DOWN:
    case MOVEMENT_ACTION_FACE_UP:
    case MOVEMENT_ACTION_FACE_LEFT:
    case MOVEMENT_ACTION_FACE_RIGHT:
    case MOVEMENT_ACTION_DELAY_1:
    case MOVEMENT_ACTION_DELAY_2:
    case MOVEMENT_ACTION_DELAY_4:
    case MOVEMENT_ACTION_DELAY_8:
    case MOVEMENT_ACTION_DELAY_16:
    case MOVEMENT_ACTION_FACE_PLAYER:
    case MOVEMENT_ACTION_FACE_AWAY_PLAYER:
    case MOVEMENT_ACTION_LOCK_FACING_DIRECTION:
    case MOVEMENT_ACTION_UNLOCK_FACING_DIRECTION:
    case MOVEMENT_ACTION_SET_INVISIBLE:
    case MOVEMENT_ACTION_SET_VISIBLE:
    case MOVEMENT_ACTION_EMOTE_EXCLAMATION_MARK:
    case MOVEMENT_ACTION_EMOTE_QUESTION_MARK:
    case MOVEMENT_ACTION_WALK_IN_PLACE_SLOW_DOWN:
    case MOVEMENT_ACTION_WALK_IN_PLACE_SLOW_UP:
    case MOVEMENT_ACTION_WALK_IN_PLACE_SLOW_LEFT:
    case MOVEMENT_ACTION_WALK_IN_PLACE_SLOW_RIGHT:
    case MOVEMENT_ACTION_WALK_IN_PLACE_NORMAL_DOWN:
    case MOVEMENT_ACTION_WALK_IN_PLACE_NORMAL_UP:
    case MOVEMENT_ACTION_WALK_IN_PLACE_NORMAL_LEFT:
    case MOVEMENT_ACTION_WALK_IN_PLACE_NORMAL_RIGHT:
    case MOVEMENT_ACTION_WALK_IN_PLACE_FAST_DOWN:
    case MOVEMENT_ACTION_WALK_IN_PLACE_FAST_UP:
    case MOVEMENT_ACTION_WALK_IN_PLACE_FAST_LEFT:
    case MOVEMENT_ACTION_WALK_IN_PLACE_FAST_RIGHT:
    case MOVEMENT_ACTION_WALK_IN_PLACE_FASTER_DOWN:
    case MOVEMENT_ACTION_WALK_IN_PLACE_FASTER_UP:
    case MOVEMENT_ACTION_WALK_IN_PLACE_FASTER_LEFT:
    case MOVEMENT_ACTION_WALK_IN_PLACE_FASTER_RIGHT:
    case MOVEMENT_ACTION_JUMP_IN_PLACE_DOWN:
    case MOVEMENT_ACTION_JUMP_IN_PLACE_UP:
    case MOVEMENT_ACTION_JUMP_IN_PLACE_LEFT:
    case MOVEMENT_ACTION_JUMP_IN_PLACE_RIGHT:
    case MOVEMENT_ACTION_JUMP_IN_PLACE_DOWN_UP:
    case MOVEMENT_ACTION_JUMP_IN_PLACE_UP_DOWN:
    case MOVEMENT_ACTION_JUMP_IN_PLACE_LEFT_RIGHT:
    case MOVEMENT_ACTION_JUMP_IN_PLACE_RIGHT_LEFT:
        return FALSE;
    }

    return TRUE;
}

static u8 ReturnFollowerDelayedState(u8 direction)
{
    u8 newState = gSaveBlock2Ptr->follower.delayedState;
    gSaveBlock2Ptr->follower.delayedState = 0;

    return newState + direction;
}

bool8 FollowMe_IsCollisionExempt(struct ObjectEvent *obstacle, struct ObjectEvent *collider)
{
    struct ObjectEvent *follower = &gObjectEvents[GetFollowerMapObjId()];
    struct ObjectEvent *player = &gObjectEvents[gPlayerAvatar.objectEventId];

    if (!gSaveBlock2Ptr->follower.inProgress || GetFollowerMapObjId() >= OBJECT_EVENTS_COUNT)
        return FALSE;

    if (obstacle == follower && collider == player)
        return TRUE;

    return FALSE;
}

// ---------------------------------------------------------------------------
// Doors, stairs, escalators
// ---------------------------------------------------------------------------

static u8 GetPlayerFaceToDoorDirection(struct ObjectEvent *player, struct ObjectEvent *follower)
{
    s16 delta_x = player->currentCoords.x - follower->currentCoords.x;

    if (delta_x < 0)
        return DIR_EAST;
    else if (delta_x > 0)
        return DIR_WEST;

    return DIR_NORTH;
}

static void Task_FollowerOutOfDoor(u8 taskId)
{
    struct ObjectEvent *follower = &gObjectEvents[GetFollowerMapObjId()];
    struct ObjectEvent *player = &gObjectEvents[gPlayerAvatar.objectEventId];
    struct Task *task = &gTasks[taskId];
    s16 *x = &task->data[2];
    s16 *y = &task->data[3];

    if (ObjectEventClearHeldMovementIfFinished(player))
        ObjectEventTurn(player, GetPlayerFaceToDoorDirection(player, follower)); // Face the follower as it comes out the door

    switch (task->data[0])
    {
    case 0:
        FreezeObjectEvents();
        task->data[1] = FieldAnimateDoorOpen(follower->currentCoords.x, follower->currentCoords.y);
        if (task->data[1] != -1)
            PlaySE(GetDoorSoundEffect(*x, *y)); // only play SE for animating doors

        task->data[0] = 1;
        break;
    case 1:
        if (task->data[1] < 0 || gTasks[task->data[1]].isActive != TRUE) // if Door isn't still opening
        {
            follower->invisible = FALSE;
            ObjectEventTurn(follower, DIR_SOUTH); // The follower should be facing down when it comes out the door
            follower->singleMovementActive = FALSE;
            follower->heldMovementActive = FALSE;
            ObjectEventSetHeldMovement(follower, MOVEMENT_ACTION_WALK_NORMAL_DOWN); // follower step down
            task->data[0] = 2;
        }
        break;
    case 2:
        if (follower->movementActionId == MOVEMENT_ACTION_FOLLOWING_POKEMON || ObjectEventClearHeldMovementIfFinished(follower))
        {
            task->data[1] = FieldAnimateDoorClose(*x, *y);
            task->data[0] = 3;
        }
        break;
    case 3:
        if (task->data[1] < 0 || gTasks[task->data[1]].isActive != TRUE) // Door is closed
        {
            UnfreezeObjectEvents();
            task->data[0] = 4;
        }
        break;
    case 4:
        FollowMe_HandleSprite();
        gSaveBlock2Ptr->follower.comeOutDoorStairs = 0;
        gPlayerAvatar.preventStep = FALSE; // Player can move again
        DestroyTask(taskId);
        break;
    }
}

void StairsMoveFollower(void)
{
    if (!gSaveBlock2Ptr->follower.inProgress)
        return;

    if (!FuncIsActiveTask(Task_FollowerHandleIndoorStairs) && gSaveBlock2Ptr->follower.comeOutDoorStairs != 2)
        CreateTask(Task_FollowerHandleIndoorStairs, 1);
}

static void Task_FollowerHandleIndoorStairs(u8 taskId)
{
    struct ObjectEvent *follower = &gObjectEvents[GetFollowerMapObjId()];
    struct ObjectEvent *player = &gObjectEvents[gPlayerAvatar.objectEventId];
    struct Task *task = &gTasks[taskId];

    switch (task->data[0])
    {
    case 0:
        gSaveBlock2Ptr->follower.comeOutDoorStairs = 2; // So the task doesn't get created more than once
        ObjectEventClearHeldMovementIfActive(follower);
        ObjectEventSetHeldMovement(follower, DetermineFollowerState(follower, MOVEMENT_ACTION_WALK_NORMAL_DOWN, DetermineFollowerDirection(player, follower)));
        task->data[0]++;
        break;
    case 1:
        if (ObjectEventClearHeldMovementIfFinished(follower))
        {
            ObjectEventSetHeldMovement(follower, DetermineFollowerState(follower, MOVEMENT_ACTION_WALK_NORMAL_DOWN, player->movementDirection));
            DestroyTask(taskId);
        }
        break;
    }
}

void EscalatorMoveFollower(u8 movementType)
{
    u8 taskId;

    if (!gSaveBlock2Ptr->follower.inProgress)
        return;

    taskId = CreateTask(Task_FollowerHandleEscalator, 1);
    gTasks[taskId].data[1] = movementType;
}

static void Task_FollowerHandleEscalator(u8 taskId)
{
    struct ObjectEvent *follower = &gObjectEvents[GetFollowerMapObjId()];
    struct ObjectEvent *player = &gObjectEvents[gPlayerAvatar.objectEventId];

    ObjectEventClearHeldMovementIfActive(follower);
    ObjectEventSetHeldMovement(follower, DetermineFollowerState(follower, MOVEMENT_ACTION_WALK_NORMAL_DOWN, DetermineFollowerDirection(player, follower)));
    DestroyTask(taskId);
}

void EscalatorMoveFollowerFinish(void)
{
    if (!gSaveBlock2Ptr->follower.inProgress)
        return;

    CreateTask(Task_FollowerHandleEscalatorFinish, 1);
}

static void Task_FollowerHandleEscalatorFinish(u8 taskId)
{
    s16 x, y;
    struct ObjectEvent *follower = &gObjectEvents[GetFollowerMapObjId()];
    struct ObjectEvent *player = &gObjectEvents[gPlayerAvatar.objectEventId];
    struct Sprite *sprite = &gSprites[follower->spriteId];
    struct Task *task = &gTasks[taskId];

    switch (task->data[0])
    {
    case 0:
        MoveObjectEventToMapCoords(follower, player->currentCoords.x, player->currentCoords.y);
        PlayerGetDestCoords(&x, &y);
        task->data[2] = MapGridGetMetatileBehaviorAt(x, y);
        task->data[7] = 0;
        task->data[0]++;
        break;
    case 1:
        if (task->data[7]++ < 0x20) // Wait half a second before revealing the follower
            break;

        task->data[0]++;
        task->data[1] = 16;
        CalculateFollowerEscalatorTrajectoryUp(task);
        gSaveBlock2Ptr->follower.warpEnd = 0;
        gPlayerAvatar.preventStep = TRUE;
        ObjectEventSetHeldMovement(follower, GetFaceDirectionMovementAction(DIR_EAST));
        if (task->data[2] == MB_DOWN_ESCALATOR)
            task->data[0] = 4;
        break;
    case 2:
        follower->invisible = FALSE;
        CalculateFollowerEscalatorTrajectoryDown(task);
        task->data[0]++;
        break;
    case 3:
        CalculateFollowerEscalatorTrajectoryDown(task);
        task->data[2]++;
        if (task->data[2] & 1)
            task->data[1]--;

        if (task->data[1] == 0)
        {
            sprite->x2 = 0;
            sprite->y2 = 0;
            task->data[0] = 6;
        }
        break;
    case 4:
        follower->invisible = FALSE;
        CalculateFollowerEscalatorTrajectoryUp(task);
        task->data[0]++;
        break;
    case 5:
        CalculateFollowerEscalatorTrajectoryUp(task);
        task->data[2]++;
        if (task->data[2] & 1)
            task->data[1]--;

        if (task->data[1] == 0)
        {
            sprite->x2 = 0;
            sprite->y2 = 0;
            task->data[0]++;
        }
        break;
    case 6:
        if (ObjectEventClearHeldMovementIfFinished(follower))
        {
            gPlayerAvatar.preventStep = FALSE;
            DestroyTask(taskId);
        }
    }
}

static void CalculateFollowerEscalatorTrajectoryDown(struct Task *task)
{
    struct Sprite *sprite = &gSprites[gObjectEvents[GetFollowerMapObjId()].spriteId];

    sprite->x2 = Cos(0x84, task->data[1]);
    sprite->y2 = Sin(0x94, task->data[1]);
}

static void CalculateFollowerEscalatorTrajectoryUp(struct Task *task)
{
    struct Sprite *sprite = &gSprites[gObjectEvents[GetFollowerMapObjId()].spriteId];

    sprite->x2 = Cos(0x7c, task->data[1]);
    sprite->y2 = Sin(0x76, task->data[1]);
}

// ---------------------------------------------------------------------------
// Sprite / object management
// ---------------------------------------------------------------------------

bool8 FollowerCanBike(void)
{
    if (!gSaveBlock2Ptr->follower.inProgress)
        return TRUE;
    else if (gSaveBlock2Ptr->follower.flags & FOLLOWER_FLAG_CAN_BIKE)
        return TRUE;
    else
        return FALSE;
}

// The follower Pokemon looks the same on foot and on a bike (it's hidden in its ball while biking)
void FollowMe_HandleBike(void)
{
    SetFollowerSprite(FOLLOWER_SPRITE_INDEX_NORMAL);
}

void FollowMe_HandleSprite(void)
{
    SetFollowerSprite(FOLLOWER_SPRITE_INDEX_NORMAL);
}

// Removes the follower object and spawns it again with a new graphics id, keeping its state.
static void RecreateFollowerObject(u16 newGraphicsId)
{
    u8 newSpriteId;
    struct ObjectEventTemplate clone;
    struct ObjectEvent backupFollower;
    struct ObjectEvent *follower = &gObjectEvents[GetFollowerMapObjId()];

    backupFollower = *follower;
    backupFollower.graphicsId = newGraphicsId;
    RemoveObjectEvent(follower);

    clone = *GetFollowerObjectEventTemplate();
    clone.graphicsId = newGraphicsId;
    gSaveBlock2Ptr->follower.objId = TrySpawnObjectEventTemplate(&clone, gSaveBlock1Ptr->location.mapNum, gSaveBlock1Ptr->location.mapGroup, clone.x, clone.y);
    if (gSaveBlock2Ptr->follower.objId >= OBJECT_EVENTS_COUNT)
        return;

    follower = &gObjectEvents[GetFollowerMapObjId()];
    newSpriteId = follower->spriteId;
    *follower = backupFollower;
    follower->spriteId = newSpriteId;
    MoveObjectEventToMapCoords(follower, follower->currentCoords.x, follower->currentCoords.y);
    ObjectEventTurn(follower, follower->facingDirection);
}

void SetFollowerSprite(u8 spriteIndex)
{
    u16 newGraphicsId;

    if (!gSaveBlock2Ptr->follower.inProgress || GetFollowerMapObjId() >= OBJECT_EVENTS_COUNT)
        return;

    newGraphicsId = GetFollowerPokemonGraphicsId();
    if (gSaveBlock2Ptr->follower.currentSprite == spriteIndex
     && gObjectEvents[GetFollowerMapObjId()].graphicsId == newGraphicsId)
        return;

    gSaveBlock2Ptr->follower.currentSprite = spriteIndex;
    gSaveBlock2Ptr->follower.graphicsId = newGraphicsId;
    RecreateFollowerObject(newGraphicsId);
}

void FollowMe_WarpSetEnd(void)
{
    struct ObjectEvent *player;
    struct ObjectEvent *follower;
    u8 toY;

    if (!gSaveBlock2Ptr->follower.inProgress || GetFollowerMapObjId() >= OBJECT_EVENTS_COUNT)
        return;

    player = &gObjectEvents[gPlayerAvatar.objectEventId];
    follower = &gObjectEvents[GetFollowerMapObjId()];

    gSaveBlock2Ptr->follower.warpEnd = 1;
    PlayerLogCoordinates(player);

    toY = gSaveBlock2Ptr->follower.comeOutDoorStairs == 1 ? (player->currentCoords.y - 1) : player->currentCoords.y;
    MoveObjectEventToMapCoords(follower, player->currentCoords.x, toY);

    follower->facingDirection = player->facingDirection;
    follower->movementDirection = player->movementDirection;
}

// Called when the player avatar is created on a map. Spawns the (hidden) follower next to the player.
void CreateFollowerAvatar(void)
{
    struct ObjectEvent *player;
    struct ObjectEventTemplate clone;

    if (QL_IS_PLAYBACK_STATE)
        return;

    FollowerPokemon_TryStart();
    if (!gSaveBlock2Ptr->follower.inProgress)
        return;

    // The lead Pokemon may have changed (or the party was emptied) while the follower wasn't on screen
    if (GetFollowerPokemonSpecies() == SPECIES_NONE)
    {
        gSaveBlock2Ptr->follower.inProgress = FALSE;
        return;
    }
    gSaveBlock2Ptr->follower.graphicsId = GetFollowerPokemonGraphicsId();

    // A new map: forget any door exit that was still pending from the last map. (Leaving through
    // a door and walking straight back in before taking a step used to leave comeOutDoorStairs
    // set, so the first step in the next map started a "come out of the door" sequence with no
    // door and the player could never move again.) A door exit on this map sets it again
    // afterwards in FollowMe_DoorExitEnd.
    gSaveBlock2Ptr->follower.comeOutDoorStairs = 0;
    gSaveBlock2Ptr->follower.warpEnd = 0;

    player = &gObjectEvents[gPlayerAvatar.objectEventId];
    clone = *GetFollowerObjectEventTemplate();
    clone.graphicsId = gSaveBlock2Ptr->follower.graphicsId;
    clone.x = player->currentCoords.x - MAP_OFFSET;
    clone.y = player->currentCoords.y - MAP_OFFSET;
    clone.objUnion.normal.elevation = player->currentElevation;
    clone.objUnion.normal.movementType = MOVEMENT_TYPE_NONE; // Doesn't get to move on its own

    switch (GetPlayerFacingDirection())
    {
    case DIR_NORTH:
        clone.objUnion.normal.movementType = MOVEMENT_TYPE_FACE_UP;
        break;
    case DIR_WEST:
        clone.objUnion.normal.movementType = MOVEMENT_TYPE_FACE_LEFT;
        break;
    case DIR_EAST:
        clone.objUnion.normal.movementType = MOVEMENT_TYPE_FACE_RIGHT;
        break;
    }

    // Create the object and store its id
    gSaveBlock2Ptr->follower.objId = TrySpawnObjectEventTemplate(&clone, gSaveBlock1Ptr->location.mapNum, gSaveBlock1Ptr->location.mapGroup, clone.x, clone.y);
    if (gSaveBlock2Ptr->follower.objId >= OBJECT_EVENTS_COUNT)
    {
        // Couldn't load the sprite (too many objects). Try again on the next map.
        gSaveBlock2Ptr->follower.objId = OBJECT_EVENTS_COUNT;
        return;
    }

    gSaveBlock2Ptr->follower.currentSprite = FOLLOWER_SPRITE_INDEX_NORMAL;
    gSaveBlock2Ptr->follower.createSurfBlob = 0;
    gObjectEvents[gSaveBlock2Ptr->follower.objId].invisible = TRUE;
}

// FollowerEm's "turn an NPC into a follower" (setfollower script command). Kept for the script
// command, but the Pokemon follower doesn't use it. Only one follower at a time.
void SetUpFollowerSprite(u8 localId, u16 followerFlags)
{
    struct ObjectEvent *follower;
    u8 eventObjId;
    const u8 *script;

    if (gSaveBlock2Ptr->follower.inProgress)
        return; // Only 1 follower at a time

    for (eventObjId = 0; eventObjId < OBJECT_EVENTS_COUNT; eventObjId++)
    {
        if (!gObjectEvents[eventObjId].active || gObjectEvents[eventObjId].isPlayer)
            continue;

        if (gObjectEvents[eventObjId].localId == localId)
        {
            follower = &gObjectEvents[eventObjId];
            ObjectEventSetMovementTypeNone(follower); // Doesn't get to move on its own anymore
            SetObjEventTemplateMovementType(localId, 0);
            script = GetObjectEventScriptPointerByObjectEventId(eventObjId);

            gSaveBlock2Ptr->follower.inProgress = TRUE;
            gSaveBlock2Ptr->follower.objId = eventObjId;
            gSaveBlock2Ptr->follower.graphicsId = follower->graphicsId;
            gSaveBlock2Ptr->follower.map.id = gObjectEvents[eventObjId].localId;
            gSaveBlock2Ptr->follower.map.number = gSaveBlock1Ptr->location.mapNum;
            gSaveBlock2Ptr->follower.map.group = gSaveBlock1Ptr->location.mapGroup;
            gSaveBlock2Ptr->follower.script = script;
            gSaveBlock2Ptr->follower.flag = 0;
            gSaveBlock2Ptr->follower.flags = followerFlags;
            gSaveBlock2Ptr->follower.createSurfBlob = 0;
            gSaveBlock2Ptr->follower.comeOutDoorStairs = 0;

            if (!(gSaveBlock2Ptr->follower.flags & FOLLOWER_FLAG_CAN_BIKE) // Follower can't bike
             && TestPlayerAvatarFlags(PLAYER_AVATAR_FLAG_BIKE))          // Player on bike
                SetPlayerAvatarTransitionFlags(PLAYER_AVATAR_FLAG_ON_FOOT);   // Dismount bike
        }
    }
}

bool8 CheckFollowerFlag(u16 flag)
{
    if (!gSaveBlock2Ptr->follower.inProgress)
        return TRUE;

    if (gSaveBlock2Ptr->follower.flags & flag)
        return TRUE;

    return FALSE;
}

enum
{
    GoDown,
    GoUp,
    GoLeft,
    GoRight
};

void FollowerPositionFix(u8 offset)
{
    u8 playerObjId = gPlayerAvatar.objectEventId;
    u8 followerObjid = gSaveBlock2Ptr->follower.objId;
    u16 playerX = gObjectEvents[playerObjId].currentCoords.x;
    u16 playerY = gObjectEvents[playerObjId].currentCoords.y;
    u16 npcX = gObjectEvents[followerObjid].currentCoords.x;
    u16 npcY = gObjectEvents[followerObjid].currentCoords.y;

    gSpecialVar_Result = 0xFFFF;

    if (!gSaveBlock2Ptr->follower.inProgress || followerObjid >= OBJECT_EVENTS_COUNT)
        return;

    if (playerX == npcX)
    {
        if (playerY > npcY)
        {
            if (playerY != npcY + offset) // Player and follower are not 1 tile apart
            {
                if (gSpecialVar_0x8000 == 0)
                    gSpecialVar_Result = GoDown;
                else
                    gObjectEvents[followerObjid].currentCoords.y = playerY - offset;
            }
        }
        else // Player Y <= npcY
        {
            if (playerY != npcY - offset) // Player and follower are not 1 tile apart
            {
                if (gSpecialVar_0x8000 == 0)
                    gSpecialVar_Result = GoUp;
                else
                    gObjectEvents[followerObjid].currentCoords.y = playerY + offset;
            }
        }
    }
    else // playerY == npcY
    {
        if (playerX > npcX)
        {
            if (playerX != npcX + offset) // Player and follower are not 1 tile apart
            {
                if (gSpecialVar_0x8000 == 0)
                    gSpecialVar_Result = GoRight;
                else
                    gObjectEvents[followerObjid].currentCoords.x = playerX - offset;
            }
        }
        else // Player X <= npcX
        {
            if (playerX != npcX - offset) // Player and follower are not 1 tile apart
            {
                if (gSpecialVar_0x8000 == 0)
                    gSpecialVar_Result = GoLeft;
                else
                    gObjectEvents[followerObjid].currentCoords.x = playerX + offset;
            }
        }
    }
}

void FollowerTrainerSightingPositionFix(void)
{
    FollowerPositionFix(1);
}

void FollowerIntoPlayer(void)
{
    FollowerPositionFix(0);
}

// ---------------------------------------------------------------------------
// Script commands
// ---------------------------------------------------------------------------

// Ends the follower (until the next map load, unless FLAG_FOLLOWERS_DISABLED is set)
void DestroyFollower(void)
{
    if (gSaveBlock2Ptr->follower.inProgress)
    {
        if (GetFollowerMapObjId() < OBJECT_EVENTS_COUNT)
            RemoveObjectEvent(&gObjectEvents[gSaveBlock2Ptr->follower.objId]);
        if (gSaveBlock2Ptr->follower.flag != 0)
            FlagSet(gSaveBlock2Ptr->follower.flag);
        gSaveBlock2Ptr->follower.inProgress = FALSE;
    }
}

// Faces the player and the follower towards each other
void PlayerFaceFollowerSprite(void)
{
    if (gSaveBlock2Ptr->follower.inProgress && GetFollowerMapObjId() < OBJECT_EVENTS_COUNT)
    {
        u8 playerDirection, followerDirection;
        struct ObjectEvent *player, *follower;

        player = &gObjectEvents[gPlayerAvatar.objectEventId];
        follower = &gObjectEvents[gSaveBlock2Ptr->follower.objId];
        playerDirection = DetermineFollowerDirection(player, follower);
        followerDirection = playerDirection;

        // Flip direction
        switch (playerDirection)
        {
        case DIR_NORTH:
            playerDirection = DIR_SOUTH;
            followerDirection = DIR_NORTH;
            break;
        case DIR_SOUTH:
            playerDirection = DIR_NORTH;
            followerDirection = DIR_SOUTH;
            break;
        case DIR_WEST:
            playerDirection = DIR_EAST;
            followerDirection = DIR_WEST;
            break;
        case DIR_EAST:
            playerDirection = DIR_WEST;
            followerDirection = DIR_EAST;
            break;
        }

        ObjectEventTurn(player, playerDirection);
        ObjectEventTurn(follower, followerDirection);
    }
}

// Sets VAR_RESULT to TRUE if the player has a follower
void CheckPlayerHasFollower(void)
{
    gSpecialVar_Result = gSaveBlock2Ptr->follower.inProgress;
}

// setfollowerwarppos: place the follower relative to the player after a scripted warp
void SetFollowerWarpPosition(void)
{
    gSaveBlock2Ptr->follower.createSurfBlob = 1;
}

// pokemonfaceplayer (callnative): the follower (default) or the object with local id VAR_0x8014 faces the player
void FollowerPokemonFacePlayer(void)
{
    struct ObjectEvent *obj;
    struct ObjectEvent *player = &gObjectEvents[gPlayerAvatar.objectEventId];
    s16 dx, dy;

    if (gSpecialVar_0x8014 == OBJ_EVENT_ID_FOLLOWER)
    {
        if (!gSaveBlock2Ptr->follower.inProgress || GetFollowerMapObjId() >= OBJECT_EVENTS_COUNT)
            return;
        ObjectEventClearHeldMovement(&gObjectEvents[GetFollowerMapObjId()]);
        ObjectEventPokemonFacePlayer(&gObjectEvents[GetFollowerMapObjId()], player);
        return;
    }

    gSpecialVar_0x8014 = GetObjectEventIdByLocalIdAndMap(gSpecialVar_0x8014, gSaveBlock1Ptr->location.mapNum, gSaveBlock1Ptr->location.mapGroup);
    if (gSpecialVar_0x8014 >= OBJECT_EVENTS_COUNT)
        return;
    obj = &gObjectEvents[gSpecialVar_0x8014];
    ObjectEventClearHeldMovement(obj);

    dx = obj->currentCoords.x - player->currentCoords.x;
    dy = obj->currentCoords.y - player->currentCoords.y;
    if (dx * dx < dy * dy)
    {
        if (obj->currentCoords.y < player->currentCoords.y) // Player is south of the object
            ObjectEventSetHeldMovement(obj, MOVEMENT_ACTION_FOLLOWING_POKEMON_FACESOUTH);
        else
            ObjectEventSetHeldMovement(obj, MOVEMENT_ACTION_FOLLOWING_POKEMON_FACENORTH);
    }
    else
    {
        if (obj->currentCoords.x < player->currentCoords.x) // Player is east of the object
            ObjectEventSetHeldMovement(obj, MOVEMENT_ACTION_FOLLOWING_POKEMON_FACEEAST);
        else
            ObjectEventSetHeldMovement(obj, MOVEMENT_ACTION_FOLLOWING_POKEMON_FACEWEST);
    }
}

bool8 ScrCmd_setfollower(struct ScriptContext *ctx)
{
    u8 localId = ScriptReadByte(ctx);
    u16 flags = ScriptReadHalfword(ctx);

    SetUpFollowerSprite(localId, flags);
    return FALSE;
}

bool8 ScrCmd_destroyfollower(struct ScriptContext *ctx)
{
    DestroyFollower();
    return FALSE;
}

bool8 ScrCmd_facefollower(struct ScriptContext *ctx)
{
    PlayerFaceFollowerSprite();
    return FALSE;
}

bool8 ScrCmd_checkfollower(struct ScriptContext *ctx)
{
    CheckPlayerHasFollower();
    return FALSE;
}

// ---------------------------------------------------------------------------
// Poke Ball and sparkles
// ---------------------------------------------------------------------------

bool8 IsBigSprite(u16 graphicsId)
{
    switch (graphicsId)
    {
    case OBJ_EVENT_GFX_FOLLOWER(SPECIES_STEELIX):
    case OBJ_EVENT_GFX_FOLLOWER(SPECIES_LUGIA):
    case OBJ_EVENT_GFX_FOLLOWER(SPECIES_HO_OH):
    case OBJ_EVENT_GFX_FOLLOWER(SPECIES_WAILORD):
        return TRUE;
    default:
        return FALSE;
    }
}

static const struct FollowerBall *GetFollowerBall(void)
{
    u16 ball = GetMonData(&gPlayerParty[GetLeadMonNotFaintedIndex()], MON_DATA_POKEBALL, NULL);
    u8 i;

    for (i = 0; i < ARRAY_COUNT(sFollowerBalls); i++)
    {
        if (sFollowerBalls[i].item == ball)
            return &sFollowerBalls[i];
    }
    return &sFollowerBalls[0];
}

static void SpriteCB_FollowerBallDefault(struct Sprite *sprite)
{
    if (++sprite->data[0] == 21)
        DestroyFollowerPokeballSprite(sprite);
}

// Creates a sprite of the Poke Ball the lead Pokemon was caught in. Its palette is loaded into a
// free sprite palette slot; if none is free, no sprite is made (returns MAX_SPRITES).
// If callback is NULL, the ball disappears after 21 frames.
u8 CreateFollowerPokeballSprite(s16 x, s16 y, u8 subpriority, SpriteCallback callback)
{
    const struct FollowerBall *ball = GetFollowerBall();
    u16 paletteTag = FOLLOWER_BALL_PAL_TAG_BASE + (ball - sFollowerBalls);
    struct SpritePalette spritePalette;
    struct SpriteTemplate template;
    u8 spriteId;

    spritePalette.data = ball->pal;
    spritePalette.tag = paletteTag;
    if (LoadSpritePalette(&spritePalette) == 0xFF)
        return MAX_SPRITES;
    UpdateSpritePaletteWithWeather(IndexOfSpritePaletteTag(paletteTag));

    template.tileTag = TAG_NONE;
    template.paletteTag = paletteTag;
    template.oam = &sOam_FollowerBall;
    template.anims = sAnimTable_FollowerBall;
    template.images = ball->images;
    template.affineAnims = gDummySpriteAffineAnimTable;
    template.callback = callback != NULL ? callback : SpriteCB_FollowerBallDefault;

    spriteId = CreateSprite(&template, x, y, subpriority);
    if (spriteId == MAX_SPRITES)
    {
        FreeSpritePaletteByTag(paletteTag);
        return MAX_SPRITES;
    }
    gSprites[spriteId].data[1] = paletteTag;
    gSprites[spriteId].data[0] = 0;
    return spriteId;
}

void DestroyFollowerPokeballSprite(struct Sprite *sprite)
{
    u16 paletteTag = sprite->data[1];

    DestroySprite(sprite);
    FreeSpritePaletteByTag(paletteTag);
}

static void SpriteCB_FollowerSparkle(struct Sprite *sprite)
{
    if (++sprite->data[0] >= 19)
    {
        u8 i, paletteNum = sprite->oam.paletteNum;

        FreeSpriteOamMatrix(sprite);
        DestroySprite(sprite);
        // Free the sparkle palette once no sprite uses it anymore
        for (i = 0; i < MAX_SPRITES; i++)
        {
            if (gSprites[i].inUse && gSprites[i].oam.paletteNum == paletteNum)
                return;
        }
        FreeSpritePaletteByTag(FLDEFF_PAL_TAG_SMALL_SPARKLE);
    }
}

static void SpriteCB_FollowerBigSparkle(struct Sprite *sprite)
{
    if (sprite->data[0] % 2 == 0)
    {
        switch (sprite->data[7])
        {
        case 0: // North
            sprite->x++;
            break;
        case 1: // NorthEast
            if (sprite->data[0] % 4 == 0)
                sprite->y++;
            else
                sprite->x++;
            break;
        case 2: // East
            sprite->y++;
            break;
        case 3: // SouthEast
            if (sprite->data[0] % 4 == 0)
                sprite->y++;
            else
                sprite->x--;
            break;
        case 4: // South
            sprite->x--;
            break;
        case 5: // SouthWest
            if (sprite->data[0] % 4 == 0)
                sprite->y--;
            else
                sprite->x--;
            break;
        case 6: // West
            sprite->y--;
            break;
        case 7: // NorthWest
            if (sprite->data[0] % 4 == 0)
                sprite->y--;
            else
                sprite->x++;
            break;
        }
    }

    SpriteCB_FollowerSparkle(sprite);
}

// Eight sparkles, one per direction, around (x, y)
static void CreateFollowerSparkles(s16 x, s16 y, bool8 big)
{
    u8 i, spriteId;

    if (LoadSpritePalette(&gSpritePalette_SmallSparkle) == 0xFF)
        return;
    UpdateSpritePaletteWithWeather(IndexOfSpritePaletteTag(FLDEFF_PAL_TAG_SMALL_SPARKLE));

    for (i = 0; i < 8; i++)
    {
        spriteId = CreateSprite(&sSpriteTemplate_ExpandingSparkle, x, y, 0);
        if (spriteId == MAX_SPRITES)
            break;
        gSprites[spriteId].coordOffsetEnabled = TRUE;
        gSprites[spriteId].oam.priority = 1;
        gSprites[spriteId].data[0] = 0;
        gSprites[spriteId].data[7] = i;
        if (big)
            gSprites[spriteId].callback = SpriteCB_FollowerBigSparkle;
        StartSpriteAffineAnim(&gSprites[spriteId], i);
    }
}

static u16 GetSparkleCoordsIndex(void)
{
    u16 species = gSaveBlock2Ptr->follower.graphicsId - OBJ_EVENT_GFX_FOLLOWERS_START;

    if (species == SPECIES_NONE || species >= FOLLOWER_GFX_COUNT)
        return SPECIES_PIKACHU - 1;
    if (species >= SPECIES_OLD_UNOWN_B && species < SPECIES_TREECKO)
        return SPECIES_UNOWN - 1;
    if (species >= SPECIES_TREECKO)
        return species - 1 - (SPECIES_TREECKO - SPECIES_OLD_UNOWN_B); // skip the old Unown slots
    return species - 1;
}

// The ball the follower comes out of: when it's gone, put the sparkles where the Pokemon appears
static void SpriteCB_FollowerComeOutBall(struct Sprite *sprite)
{
    sprite->data[0]++;

    if (sprite->data[0] >= 10)
    {
        struct ObjectEvent *player = &gObjectEvents[gPlayerAvatar.objectEventId];
        struct ObjectEvent *follower = &gObjectEvents[gSaveBlock2Ptr->follower.objId];
        s16 x = gSprites[follower->spriteId].x;
        s16 y = gSprites[follower->spriteId].y;
        u16 idx = GetSparkleCoordsIndex();
        bool8 big = IsBigSprite(follower->graphicsId);

        DestroyFollowerPokeballSprite(sprite);

        gSprites[follower->spriteId].oam.priority = ElevationToPriority(follower->previousElevation);
        gSprites[gPlayerAvatar.spriteId].oam.priority = ElevationToPriority(player->previousElevation);
        player->fixedPriority = FALSE;

        // Shift the location of the sparkle, depending on which way the follower will be facing
        switch (player->facingDirection)
        {
        case DIR_SOUTH:
            x -= 16 - sFollowerSparkleCoords[idx][0];
            if (big)
                x -= 16;
            y += 16 - sFollowerSparkleCoords[idx][1];
            break;
        case DIR_NORTH:
            x -= 16 - sFollowerSparkleCoords[idx][2];
            if (big)
                x -= 16;
            y += 16 - sFollowerSparkleCoords[idx][3];
            break;
        case DIR_EAST:
            x += 7 - sFollowerSparkleCoords[idx][4]; // 7 looks better than 8
            if (big)
                x += 16;
            y += 16 - sFollowerSparkleCoords[idx][5];
            break;
        case DIR_WEST:
            x -= 8 - sFollowerSparkleCoords[idx][4];
            if (big)
                x -= 16;
            y += 16 - sFollowerSparkleCoords[idx][5];
            break;
        }

        CreateFollowerSparkles(x, y, big);
    }
}

// Called when the player takes a step: if the follower is in its ball, it comes back out
void FollowerPokeballSparkle(void)
{
    struct ObjectEvent *player;
    struct ObjectEvent *follower;
    s16 x, y;
    u8 spriteId;

    if (!gSaveBlock2Ptr->follower.inProgress || GetFollowerMapObjId() >= OBJECT_EVENTS_COUNT || QL_IS_PLAYBACK_STATE)
        return;
    if (gObjectEvents[gSaveBlock2Ptr->follower.objId].invisible != TRUE || gSaveBlock2Ptr->follower.comeOutDoorStairs != 0)
        return;
    if (gPlayerAvatar.flags & (PLAYER_AVATAR_FLAG_UNDERWATER | PLAYER_AVATAR_FLAG_SURFING | PLAYER_AVATAR_FLAG_BIKE))
        return;

    player = &gObjectEvents[gPlayerAvatar.objectEventId];
    follower = &gObjectEvents[gSaveBlock2Ptr->follower.objId];
    x = player->currentCoords.x;
    y = player->currentCoords.y;

    switch (player->facingDirection)
    {
    case DIR_SOUTH:
        SetSpritePosToOffsetMapCoords(&x, &y, 8, 4);
        gSprites[follower->spriteId].y = y - 4;
        break;
    case DIR_NORTH:
        SetSpritePosToOffsetMapCoords(&x, &y, 8, 0);
        gSprites[follower->spriteId].y = y;
        break;
    case DIR_EAST:
    case DIR_WEST:
    default:
        SetSpritePosToOffsetMapCoords(&x, &y, 8, 8);
        gSprites[follower->spriteId].y = y - 8;
        break;
    }
    gSprites[follower->spriteId].x = x;

    spriteId = CreateFollowerPokeballSprite(x, y, 2, SpriteCB_FollowerComeOutBall);
    if (spriteId != MAX_SPRITES)
    {
        gSprites[spriteId].coordOffsetEnabled = TRUE;
        gSprites[spriteId].oam.priority = 2;
    }

    follower->currentCoords.x = player->currentCoords.x;
    follower->currentCoords.y = player->currentCoords.y;
    follower->facingDirection = player->facingDirection;

    gSprites[follower->spriteId].animNum = player->facingDirection - 1;

    if (player->facingDirection == DIR_NORTH)
    {
        gSprites[follower->spriteId].oam.priority = 0;
        gSprites[follower->spriteId].subpriority = 0;
        gSprites[gPlayerAvatar.spriteId].oam.priority = 1;
        gSprites[gPlayerAvatar.spriteId].subpriority = 1;
    }
    else
    {
        gSprites[follower->spriteId].oam.priority = 1;
        gSprites[follower->spriteId].subpriority = 1;
        gSprites[gPlayerAvatar.spriteId].oam.priority = 0;
        gSprites[gPlayerAvatar.spriteId].subpriority = 0;
    }
    player->fixedPriority = TRUE;
    gPlayerAvatar.preventStep = TRUE;

    SeekSpriteAnim(&gSprites[follower->spriteId], 0);
    ObjectEventForceSetHeldMovement(follower, MOVEMENT_ACTION_FOLLOWING_POKEMON_GROW);
}

// The follower shrinks back into its ball (Pokemon Center, bike, Fly...)
void FollowerIntoPokeball(void)
{
    gSpecialVar_0x8014 = 0;
    if (!gSaveBlock2Ptr->follower.inProgress || GetFollowerMapObjId() >= OBJECT_EVENTS_COUNT)
        return;

    if (gObjectEvents[gSaveBlock2Ptr->follower.objId].invisible == FALSE)
    {
        gSaveBlock2Ptr->follower.comeOutDoorStairs = 0;
        gPlayerAvatar.preventStep = TRUE;
        ObjectEventForceSetHeldMovement(&gObjectEvents[gSaveBlock2Ptr->follower.objId], MOVEMENT_ACTION_FOLLOWING_POKEMON_SHRINK);
        gSpecialVar_0x8014 = 1;
    }
}

// ---------------------------------------------------------------------------
// Hooks used by the rest of the overworld code
// ---------------------------------------------------------------------------

static bool8 FollowerObjectExists(void)
{
    return gSaveBlock2Ptr->follower.inProgress && GetFollowerMapObjId() < OBJECT_EVENTS_COUNT
        && gObjectEvents[GetFollowerMapObjId()].active && !QL_IS_PLAYBACK_STATE;
}

static bool8 IsFollowerVisible(void)
{
    return FollowerObjectExists() && !gObjectEvents[GetFollowerMapObjId()].invisible;
}

// Door warps (field_fadetransition.c, Task_DoorWarp)
void FollowMe_DoorWarp_Start(void)
{
    if (!gSaveBlock2Ptr->follower.inProgress)
        return;
    if (TestPlayerAvatarFlags(PLAYER_AVATAR_FLAG_DASH))
        SetPlayerAvatarTransitionFlags(PLAYER_AVATAR_FLAG_ON_FOOT); // Stop running
    gSaveBlock2Ptr->follower.comeOutDoorStairs = 0; // Just in case it came out and went right back in
}

// The player started walking into the door: the follower steps to where the player was
void FollowMe_DoorWarp_PlayerWalkedIn(void)
{
    struct ObjectEvent *player = &gObjectEvents[gPlayerAvatar.objectEventId];
    struct ObjectEvent *follower;
    u8 newState;

    if (!IsFollowerVisible())
        return;
    follower = &gObjectEvents[GetFollowerMapObjId()];
    newState = DetermineFollowerState(follower, MOVEMENT_ACTION_WALK_NORMAL_UP, DetermineFollowerDirection(player, follower));
    if (newState == MOVEMENT_INVALID)
        return;
    ObjectEventClearHeldMovementIfActive(follower);
    ObjectEventSetHeldMovement(follower, newState);
}

// Don't close the door in the follower's face
bool8 FollowMe_DoorWarp_ShouldCloseDoor(void)
{
    return !IsFollowerVisible();
}

// The follower walks into the door too
void FollowMe_DoorWarp_FollowerEnter(void)
{
    if (!IsFollowerVisible())
        return;
    ObjectEventClearHeldMovementIfActive(&gObjectEvents[GetFollowerMapObjId()]);
    ObjectEventSetHeldMovement(&gObjectEvents[GetFollowerMapObjId()], MOVEMENT_ACTION_WALK_NORMAL_UP);
}

// Door exits (Task_ExitDoor / Task_ExitNonAnimDoor): the follower comes out of the door after the player
// Exiting through a door: hide the follower while the player steps out...
void FollowMe_DoorExit(void)
{
    if (!FollowerObjectExists())
        return;
    HideFollower();
}

// ...then, once the player has finished stepping out, put the follower in the doorway
// (one tile behind the player) so it walks out on the player's next step.
void FollowMe_DoorExitEnd(void)
{
    if (!FollowerObjectExists())
        return;
    FollowMe_SetIndicatorToComeOutDoor();
    FollowMe_WarpSetEnd();
}

// The player is standing still: keep the follower's idle animation going
void FollowMe_IdleUpdate(void)
{
    struct ObjectEvent *follower;

    if (!FollowerObjectExists() || gSaveBlock2Ptr->follower.createSurfBlob == 2)
        return;
    follower = &gObjectEvents[GetFollowerMapObjId()];
    ObjectEventClearHeldMovementIfFinished(follower);
    ObjectEventSetHeldMovement(follower, MOVEMENT_ACTION_FOLLOWING_POKEMON);
}

// The player stopped running but is still holding B: fix the follower's frame
void FollowMe_StopRunningUpdate(void)
{
    struct ObjectEvent *follower;

    if (!FollowerObjectExists() || !IsPlayerOnFoot() || !walkrun_is_standing_still())
        return;
    follower = &gObjectEvents[GetFollowerMapObjId()];
    ObjectEventSetHeldMovement(follower, GetFaceDirectionMovementAction(follower->facingDirection));
}

// Side-facing follower frames are drawn off-center; keep them lined up (every frame, from CB2_Overworld)
void FollowMe_FixSpriteOffset(void)
{
    struct Sprite *sprite;

    if (!FollowerObjectExists())
        return;
    sprite = &gSprites[gObjectEvents[GetFollowerMapObjId()].spriteId];
    if (sprite->oam.affineMode & ST_OAM_AFFINE_ON_MASK) // going into / out of its ball
        return;
    switch (gObjectEvents[GetFollowerMapObjId()].facingDirection)
    {
    case DIR_EAST:
        sprite->x2 = -8;
        break;
    case DIR_WEST:
        sprite->x2 = 8;
        break;
    }
}

static bool8 IsFlyingFollower(u16 graphicsId)
{
    switch (graphicsId - OBJ_EVENT_GFX_FOLLOWERS_START)
    {
    case SPECIES_BUTTERFREE:
    case SPECIES_BEEDRILL:
    case SPECIES_PIDGEOTTO:
    case SPECIES_PIDGEOT:
    case SPECIES_FEAROW:
    case SPECIES_ZUBAT:
    case SPECIES_GOLBAT:
    case SPECIES_VENOMOTH:
    case SPECIES_AERODACTYL:
    case SPECIES_ARTICUNO:
    case SPECIES_ZAPDOS:
    case SPECIES_MOLTRES:
    case SPECIES_DRAGONITE:
    case SPECIES_NOCTOWL:
    case SPECIES_LEDYBA:
    case SPECIES_LEDIAN:
    case SPECIES_CROBAT:
    case SPECIES_TOGETIC:
    case SPECIES_HOPPIP:
    case SPECIES_SKIPLOOM:
    case SPECIES_JUMPLUFF:
    case SPECIES_YANMA:
    case SPECIES_MANTINE:
    case SPECIES_SKARMORY:
    case SPECIES_LUGIA:
    case SPECIES_HO_OH:
    case SPECIES_CELEBI:
    case SPECIES_BEAUTIFLY:
    case SPECIES_DUSTOX:
    case SPECIES_SWELLOW:
    case SPECIES_WINGULL:
    case SPECIES_PELIPPER:
    case SPECIES_MASQUERAIN:
    case SPECIES_FLYGON:
    case SPECIES_SWABLU:
    case SPECIES_ALTARIA:
    case SPECIES_VOLBEAT:
    case SPECIES_ILLUMISE:
    case SPECIES_KYOGRE:
    case SPECIES_LATIAS:
    case SPECIES_LATIOS:
        return TRUE;
    default:
        return FALSE;
    }
}

// The little hop the follower does with each animation frame (not for flying Pokemon)
void FollowMe_UpdateBounce(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    if (!FollowerObjectExists() || objectEvent != &gObjectEvents[GetFollowerMapObjId()])
        return;
    if (sprite->y2 < -1 || (sprite->oam.affineMode & ST_OAM_AFFINE_ON_MASK) || IsFlyingFollower(objectEvent->graphicsId))
        return;
    if (sprite->animCmdIndex % 2 == 1)
        sprite->y2 = 0;
    else
        sprite->y2 = -1;
}

// setfollowerwarppos: place the follower after the scripted warp finished loading the map
void FollowMe_TrySetCoordsFromWarp(void)
{
    struct ObjectEvent *player;
    struct ObjectEvent *follower;

    if (!FollowerObjectExists() || gSaveBlock2Ptr->follower.createSurfBlob != 1)
        return;

    player = &gObjectEvents[gPlayerAvatar.objectEventId];
    follower = &gObjectEvents[GetFollowerMapObjId()];
    gSaveBlock2Ptr->follower.warpEnd = 0;

    switch (gSpecialVar_Result)
    {
    case DIR_NONE: // Pops out of its ball
        MoveObjectEventToMapCoords(follower, player->currentCoords.x, player->currentCoords.y);
        gSaveBlock2Ptr->follower.createSurfBlob = 2;
        break;
    case DIR_SOUTH:
        MoveObjectEventToMapCoords(follower, player->currentCoords.x, player->currentCoords.y + 1);
        follower->invisible = FALSE;
        gSaveBlock2Ptr->follower.createSurfBlob = 0;
        break;
    case DIR_NORTH:
        MoveObjectEventToMapCoords(follower, player->currentCoords.x, player->currentCoords.y - 1);
        follower->invisible = FALSE;
        gSaveBlock2Ptr->follower.createSurfBlob = 0;
        break;
    case DIR_WEST:
        MoveObjectEventToMapCoords(follower, player->currentCoords.x - 1, player->currentCoords.y);
        follower->invisible = FALSE;
        gSaveBlock2Ptr->follower.createSurfBlob = 0;
        break;
    case DIR_EAST:
        MoveObjectEventToMapCoords(follower, player->currentCoords.x + 1, player->currentCoords.y);
        follower->invisible = FALSE;
        gSaveBlock2Ptr->follower.createSurfBlob = 0;
        break;
    default:
        gSaveBlock2Ptr->follower.createSurfBlob = 0;
        break;
    }
}

// Quest log: the follower isn't recorded (its graphics id doesn't fit the quest log's object data)
bool8 FollowMe_IsFollowerObjectEventId(u8 objectEventId)
{
    return gSaveBlock2Ptr->follower.inProgress && objectEventId == GetFollowerMapObjId();
}
