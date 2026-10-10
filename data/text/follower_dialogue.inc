@ ===========================================================================
@  FOLLOWER POKEMON DIALOGUE
@ ===========================================================================
@
@  What your follower Pokemon says when you talk to it. Add as many lines as you
@  like; each one is two lines of asm:
@
@      follower_message CONDITION, VALUE
@      .string "Text goes here.$"
@
@  or, with two conditions that must BOTH be true:
@
@      follower_message CONDITION, VALUE, CONDITION2, VALUE2
@      .string "Text goes here.$"
@
@  Conditions (full list in include/constants/follow_me.h):
@      FOLLOWER_COND_NONE        always (generic lines; use value 0)
@      FOLLOWER_COND_SPECIES     SPECIES_PIKACHU, ...
@      FOLLOWER_COND_TYPE        TYPE_FIRE, ...            (either of its types)
@      FOLLOWER_COND_EGG_GROUP   EGG_GROUP_FIELD, ...      ("groups" of Pokemon)
@      FOLLOWER_COND_MAP         MAP_POKEMON_TOWER_7F, ... (one exact map)
@      FOLLOWER_COND_MAPSEC      MAPSEC_POWER_PLANT, ...   (a whole area / every floor)
@      FOLLOWER_COND_MAP_TYPE    MAP_TYPE_UNDERGROUND (caves), MAP_TYPE_INDOOR, MAP_TYPE_TOWN, MAP_TYPE_ROUTE
@      FOLLOWER_COND_TERRAIN     FOLLOWER_TERRAIN_SAND (beaches), _TALL_GRASS, _WATER (next to water), _ICE
@      FOLLOWER_COND_WEATHER     WEATHER_SUNNY, WEATHER_FOG_HORIZONTAL, ...
@      FOLLOWER_COND_FRIENDSHIP  minimum friendship, 0-255
@      FOLLOWER_COND_LOW_HP      0 (HP at or below 1/4)
@      FOLLOWER_COND_STATUS      0 (poisoned, burned, ...)
@      FOLLOWER_COND_FLAG        FLAG_... that must be set (e.g. FLAG_BADGE01_GET)
@      FOLLOWER_COND_NOT_FLAG    FLAG_... that must NOT be set
@
@  Which line gets picked: every line whose conditions are true gets points
@  (species 10, low HP / status 9, exact map 8, area 7, terrain / weather 5,
@  map type 4, type / egg group 3, friendship 2, flag 1, none 0; two conditions
@  add up). One of the lines with the MOST points is picked at random. So
@  "Pikachu in the Power Plant" wins over "Pikachu", which wins over "the Power
@  Plant", which wins over "Electric types", which win over generic lines.
@  Add several lines with the same conditions for variety.
@
@  Text: {STR_VAR_1} is the Pokemon's nickname, {PLAYER} is the player.
@  \n = new line, \p = new box, $ = end. Keep lines short (about 30 characters).
@ ===========================================================================

	.macro follower_message cond1:req, value1:req, cond2=FOLLOWER_COND_NONE, value2=0
	.byte \cond1, \cond2
	.2byte \value1, \value2
	.endm

	.macro follower_message_end
	.byte 0xFF, 0xFF
	.endm

	.align 2
gText_FollowerDefault::
	.string "{STR_VAR_1} seems happy\n"
	.string "to be with you!$"

gFollowerMessages::

@ --- Generic -----------------------------------------------------------------
	follower_message FOLLOWER_COND_NONE, 0
	.string "{STR_VAR_1} seems happy\n"
	.string "to be with you!$"

	follower_message FOLLOWER_COND_NONE, 0
	.string "{STR_VAR_1} is looking around\n"
	.string "curiously.$"

	follower_message FOLLOWER_COND_NONE, 0
	.string "{STR_VAR_1} is sniffing at\n"
	.string "the ground.$"

	follower_message FOLLOWER_COND_NONE, 0
	.string "{STR_VAR_1} gave a little\n"
	.string "hop of joy!$"

	follower_message FOLLOWER_COND_NONE, 0
	.string "{STR_VAR_1} is staying\n"
	.string "close to you.$"

@ --- Friendship / condition -------------------------------------------------
	follower_message FOLLOWER_COND_FRIENDSHIP, 220
	.string "{STR_VAR_1} is gazing at you\n"
	.string "with total trust.$"

	follower_message FOLLOWER_COND_FRIENDSHIP, 220
	.string "{STR_VAR_1} snuggled up\n"
	.string "against you!$"

	follower_message FOLLOWER_COND_LOW_HP, 0
	.string "{STR_VAR_1} is exhausted…\n"
	.string "Maybe it needs a rest.$"

	follower_message FOLLOWER_COND_STATUS, 0
	.string "{STR_VAR_1} doesn't look\n"
	.string "like it feels well…$"

@ --- Types --------------------------------------------------------------------
	follower_message FOLLOWER_COND_TYPE, TYPE_FIRE
	.string "{STR_VAR_1}'s body is\n"
	.string "warm to the touch.$"

	follower_message FOLLOWER_COND_TYPE, TYPE_WATER
	.string "{STR_VAR_1} wishes it could\n"
	.string "go for a swim.$"

	follower_message FOLLOWER_COND_TYPE, TYPE_GRASS
	.string "{STR_VAR_1} is soaking up\n"
	.string "the sunlight.$"

	follower_message FOLLOWER_COND_TYPE, TYPE_ELECTRIC
	.string "{STR_VAR_1}'s fur is\n"
	.string "crackling with static!$"

	follower_message FOLLOWER_COND_TYPE, TYPE_GHOST
	.string "{STR_VAR_1} is floating\n"
	.string "eerily behind you…$"

	follower_message FOLLOWER_COND_TYPE, TYPE_PSYCHIC
	.string "{STR_VAR_1} seems to know\n"
	.string "what you're thinking.$"

	follower_message FOLLOWER_COND_TYPE, TYPE_BUG
	.string "{STR_VAR_1} is buzzing\n"
	.string "around happily.$"

	follower_message FOLLOWER_COND_TYPE, TYPE_ICE
	.string "{STR_VAR_1}'s breath is\n"
	.string "cold and frosty.$"

	follower_message FOLLOWER_COND_TYPE, TYPE_DRAGON
	.string "{STR_VAR_1} is standing\n"
	.string "proud and tall.$"

	follower_message FOLLOWER_COND_TYPE, TYPE_FIGHTING
	.string "{STR_VAR_1} is throwing\n"
	.string "practice punches!$"

	follower_message FOLLOWER_COND_TYPE, TYPE_POISON
	.string "{STR_VAR_1} is giving off\n"
	.string "a funny smell…$"

	follower_message FOLLOWER_COND_TYPE, TYPE_ROCK
	.string "{STR_VAR_1} is as steady\n"
	.string "as a boulder.$"

	follower_message FOLLOWER_COND_TYPE, TYPE_GROUND
	.string "{STR_VAR_1} is digging at\n"
	.string "the dirt.$"

	follower_message FOLLOWER_COND_TYPE, TYPE_FLYING
	.string "{STR_VAR_1} is watching\n"
	.string "the sky.$"

@ --- Egg groups ("groups" of Pokemon) ----------------------------------------
	follower_message FOLLOWER_COND_EGG_GROUP, EGG_GROUP_FIELD
	.string "{STR_VAR_1} is wagging its\n"
	.string "tail.$"

	follower_message FOLLOWER_COND_EGG_GROUP, EGG_GROUP_FAIRY
	.string "{STR_VAR_1} is humming a\n"
	.string "cheerful little tune.$"

@ --- Places -----------------------------------------------------------------
	@ Caves (every underground map)
	follower_message FOLLOWER_COND_MAP_TYPE, MAP_TYPE_UNDERGROUND
	.string "{STR_VAR_1} is listening to\n"
	.string "drips echoing in the cave.$"

	follower_message FOLLOWER_COND_MAP_TYPE, MAP_TYPE_UNDERGROUND
	.string "{STR_VAR_1} is staying close\n"
	.string "in the dark.$"

	follower_message FOLLOWER_COND_MAP_TYPE, MAP_TYPE_UNDERGROUND, FOLLOWER_COND_TYPE, TYPE_ROCK
	.string "{STR_VAR_1} feels right at\n"
	.string "home in the cave!$"

	@ Indoors
	follower_message FOLLOWER_COND_MAP_TYPE, MAP_TYPE_INDOOR
	.string "{STR_VAR_1} is looking around\n"
	.string "the room.$"

	@ Beaches (standing on sand) and water
	follower_message FOLLOWER_COND_TERRAIN, FOLLOWER_TERRAIN_SAND
	.string "{STR_VAR_1} is making\n"
	.string "footprints in the sand.$"

	follower_message FOLLOWER_COND_TERRAIN, FOLLOWER_TERRAIN_SAND, FOLLOWER_COND_TYPE, TYPE_WATER
	.string "{STR_VAR_1} can smell the\n"
	.string "sea and looks excited!$"

	follower_message FOLLOWER_COND_TERRAIN, FOLLOWER_TERRAIN_WATER, FOLLOWER_COND_TYPE, TYPE_FIRE
	.string "{STR_VAR_1} is keeping well\n"
	.string "away from the water…$"

	follower_message FOLLOWER_COND_TERRAIN, FOLLOWER_TERRAIN_TALL_GRASS
	.string "{STR_VAR_1} is rustling\n"
	.string "through the grass.$"

	@ Power Plant
	follower_message FOLLOWER_COND_MAPSEC, MAPSEC_POWER_PLANT
	.string "The air is buzzing…\n"
	.string "{STR_VAR_1} feels jumpy.$"

	follower_message FOLLOWER_COND_MAPSEC, MAPSEC_POWER_PLANT, FOLLOWER_COND_TYPE, TYPE_ELECTRIC
	.string "{STR_VAR_1} is charged up\n"
	.string "and sparking with energy!$"

	follower_message FOLLOWER_COND_MAPSEC, MAPSEC_POWER_PLANT, FOLLOWER_COND_TYPE, TYPE_WATER
	.string "{STR_VAR_1} is nervous about\n"
	.string "all the electricity…$"

	@ Pokemon Tower (Lavender Town)
	follower_message FOLLOWER_COND_MAPSEC, MAPSEC_POKEMON_TOWER
	.string "{STR_VAR_1} is shivering…\n"
	.string "It wants to leave.$"

	follower_message FOLLOWER_COND_MAPSEC, MAPSEC_POKEMON_TOWER, FOLLOWER_COND_TYPE, TYPE_GHOST
	.string "{STR_VAR_1} is chatting with\n"
	.string "something you can't see…$"

	follower_message FOLLOWER_COND_MAPSEC, MAPSEC_LAVENDER_TOWN
	.string "{STR_VAR_1} looks a little\n"
	.string "sad here.$"

	@ Other places
	follower_message FOLLOWER_COND_MAPSEC, MAPSEC_VIRIDIAN_FOREST
	.string "{STR_VAR_1} is peeking at\n"
	.string "the Bug POKéMON in the trees.$"

	follower_message FOLLOWER_COND_MAPSEC, MAPSEC_MT_MOON
	.string "{STR_VAR_1} is gazing up as\n"
	.string "if it could see the moon.$"

	follower_message FOLLOWER_COND_MAPSEC, MAPSEC_SEAFOAM_ISLANDS
	.string "{STR_VAR_1} is shivering\n"
	.string "from the cold!$"

	follower_message FOLLOWER_COND_MAPSEC, MAPSEC_SEAFOAM_ISLANDS, FOLLOWER_COND_TYPE, TYPE_ICE
	.string "{STR_VAR_1} loves the\n"
	.string "chilly air!$"

	follower_message FOLLOWER_COND_MAPSEC, MAPSEC_POKEMON_MANSION
	.string "{STR_VAR_1} is spooked by\n"
	.string "the creaky floors…$"

	follower_message FOLLOWER_COND_MAPSEC, MAPSEC_PALLET_TOWN
	.string "{STR_VAR_1} likes your\n"
	.string "hometown!$"

@ --- Specific Pokemon -------------------------------------------------------
	follower_message FOLLOWER_COND_SPECIES, SPECIES_PIKACHU
	.string "Pika pika!\n"
	.string "{STR_VAR_1} looks thrilled!$"

	follower_message FOLLOWER_COND_SPECIES, SPECIES_PIKACHU, FOLLOWER_COND_MAPSEC, MAPSEC_POWER_PLANT
	.string "{STR_VAR_1}'s cheeks are\n"
	.string "sparking like crazy!$"

	follower_message FOLLOWER_COND_SPECIES, SPECIES_CHARMANDER
	.string "{STR_VAR_1}'s tail flame is\n"
	.string "burning brightly.$"

	follower_message FOLLOWER_COND_SPECIES, SPECIES_CHARMANDER, FOLLOWER_COND_TERRAIN, FOLLOWER_TERRAIN_WATER
	.string "{STR_VAR_1} is keeping its\n"
	.string "tail away from the water!$"

	follower_message FOLLOWER_COND_SPECIES, SPECIES_BULBASAUR
	.string "{STR_VAR_1}'s bulb gives off\n"
	.string "a sweet smell.$"

	follower_message FOLLOWER_COND_SPECIES, SPECIES_SQUIRTLE
	.string "{STR_VAR_1} blew a\n"
	.string "few bubbles!$"

	follower_message FOLLOWER_COND_SPECIES, SPECIES_EEVEE
	.string "{STR_VAR_1} is chasing\n"
	.string "its own tail!$"

	follower_message FOLLOWER_COND_SPECIES, SPECIES_SNORLAX
	.string "{STR_VAR_1} yawned…\n"
	.string "It's ready for a nap.$"

	follower_message FOLLOWER_COND_SPECIES, SPECIES_PSYDUCK
	.string "{STR_VAR_1} is holding\n"
	.string "its head… Headache?$"

	follower_message FOLLOWER_COND_SPECIES, SPECIES_SLOWPOKE
	.string "{STR_VAR_1} is staring off\n"
	.string "into space…$"

	follower_message FOLLOWER_COND_SPECIES, SPECIES_MAGIKARP
	.string "{STR_VAR_1} is flopping\n"
	.string "around!$"

	follower_message FOLLOWER_COND_SPECIES, SPECIES_JIGGLYPUFF
	.string "{STR_VAR_1} wants to sing\n"
	.string "you a song!$"

	follower_message FOLLOWER_COND_SPECIES, SPECIES_CLEFAIRY, FOLLOWER_COND_MAPSEC, MAPSEC_MT_MOON
	.string "{STR_VAR_1} is dancing\n"
	.string "happily! It's home!$"

	follower_message FOLLOWER_COND_SPECIES, SPECIES_GENGAR
	.string "{STR_VAR_1} is grinning\n"
	.string "mischievously…$"

	follower_message FOLLOWER_COND_SPECIES, SPECIES_DITTO
	.string "{STR_VAR_1} is trying to\n"
	.string "look like you!$"

	follower_message FOLLOWER_COND_SPECIES, SPECIES_MEW
	.string "{STR_VAR_1} is giggling and\n"
	.string "floating in circles.$"

	follower_message_end
