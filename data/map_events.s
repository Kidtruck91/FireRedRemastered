#include "constants/event_bg.h"
#include "constants/event_object_movement.h"
#include "constants/event_objects.h"
#include "constants/flags.h"
#include "constants/items.h"
#include "constants/map_scripts.h"
@ A map may place a Pokemon in the overworld with the follower sprite set, and
@ OBJ_EVENT_GFX_FOLLOWER(species) needs the species names to resolve.
#include "constants/species.h"
#include "constants/layouts.h"
#include "constants/maps.h"
#include "constants/trainer_types.h"
#include "constants/vars.h"
#include "constants/weather.h"
    .include "asm/macros.inc"
    .include "constants/constants.inc"

    .section .rodata

    .include "data/maps/events.inc"
