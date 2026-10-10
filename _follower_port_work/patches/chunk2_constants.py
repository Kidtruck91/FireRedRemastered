# Chunk 2: movement constants and func tables
import re
def patch(path, pairs, crlf_keep=True):
    s=open(path).read(); crlf='\r\n' in s; s=s.replace('\r\n','\n')
    for old,new in pairs:
        if s.count(old)!=1: raise SystemExit(f'{path}: expected 1 match, got {s.count(old)} for:\n{old}')
        s=s.replace(old,new)
    if crlf: s=s.replace('\n','\r\n')
    open(path,'w').write(s)

patch('include/constants/event_object_movement.h', [
("""#define MOVEMENT_TYPE_WANDER_AROUND_SLOWER             0x50
#define MOVEMENT_TYPES_COUNT                           0x51""",
"""#define MOVEMENT_TYPE_WANDER_AROUND_SLOWER             0x50
#define MOVEMENT_TYPE_FOLLOWING_POKEMON                0x51
#define MOVEMENT_TYPES_COUNT                           0x52"""),
("""#define MOVEMENT_ACTION_JUMP_SPECIAL_WITH_EFFECT_RIGHT  0xA9
""",
"""#define MOVEMENT_ACTION_JUMP_SPECIAL_WITH_EFFECT_RIGHT  0xA9
// Follower Pokemon
#define MOVEMENT_ACTION_FOLLOWING_POKEMON               0xAA
#define MOVEMENT_ACTION_FOLLOWING_POKEMON_FACESOUTH     0xAB
#define MOVEMENT_ACTION_FOLLOWING_POKEMON_FACENORTH     0xAC
#define MOVEMENT_ACTION_FOLLOWING_POKEMON_FACEWEST      0xAD
#define MOVEMENT_ACTION_FOLLOWING_POKEMON_FACEEAST      0xAE
#define MOVEMENT_ACTION_FOLLOWING_POKEMON_SHRINK        0xAF
#define MOVEMENT_ACTION_FOLLOWING_POKEMON_GROW          0xB0
"""),
])

patch('src/data/object_events/movement_type_func_tables.h', [
("""u8 (*const gMovementTypeFuncs_WanderAroundSlower[])(struct ObjectEvent *, struct Sprite *) = {""",
"""static bool8 MovementType_FollowingPokemon_Step0(struct ObjectEvent *objectEvent, struct Sprite *sprite);
static bool8 MovementType_FollowingPokemon_Step1(struct ObjectEvent *objectEvent, struct Sprite *sprite);
static bool8 MovementType_FollowingPokemon_Step2(struct ObjectEvent *objectEvent, struct Sprite *sprite);
static bool8 MovementType_FollowingPokemon_Step3(struct ObjectEvent *objectEvent, struct Sprite *sprite);

u8 (*const gMovementTypeFuncs_FollowingPokemon[])(struct ObjectEvent *, struct Sprite *) = {
    MovementType_FollowingPokemon_Step0,
    MovementType_FollowingPokemon_Step1,
    MovementType_FollowingPokemon_Step2,
    MovementType_FollowingPokemon_Step3,
};

u8 (*const gMovementTypeFuncs_WanderAroundSlower[])(struct ObjectEvent *, struct Sprite *) = {"""),
])

patch('src/data/object_events/movement_action_func_tables.h', [
("""static bool8 (*const sMovementActionFuncs_JumpSpecialWithEffectRight[])(struct ObjectEvent *, struct Sprite *);
""",
"""static bool8 (*const sMovementActionFuncs_JumpSpecialWithEffectRight[])(struct ObjectEvent *, struct Sprite *);
static bool8 MovementAction_FollowingPokemon_Step0(struct ObjectEvent *, struct Sprite *);
static bool8 MovementAction_FollowingPokemon_FaceSouth_Step0(struct ObjectEvent *, struct Sprite *);
static bool8 MovementAction_FollowingPokemon_FaceNorth_Step0(struct ObjectEvent *, struct Sprite *);
static bool8 MovementAction_FollowingPokemon_FaceWest_Step0(struct ObjectEvent *, struct Sprite *);
static bool8 MovementAction_FollowingPokemon_FaceEast_Step0(struct ObjectEvent *, struct Sprite *);
static bool8 MovementAction_FollowingPokemon_Shrink_Step0(struct ObjectEvent *, struct Sprite *);
static bool8 MovementAction_FollowingPokemon_Shrink_Step1(struct ObjectEvent *, struct Sprite *);
static bool8 MovementAction_FollowingPokemon_Grow_Step0(struct ObjectEvent *, struct Sprite *);
static bool8 MovementAction_FollowingPokemon_Grow_Step1(struct ObjectEvent *, struct Sprite *);
static bool8 (*const sMovementActionFuncs_FollowingPokemon[])(struct ObjectEvent *, struct Sprite *);
static bool8 (*const sMovementActionFuncs_FollowingPokemon_FaceSouth[])(struct ObjectEvent *, struct Sprite *);
static bool8 (*const sMovementActionFuncs_FollowingPokemon_FaceNorth[])(struct ObjectEvent *, struct Sprite *);
static bool8 (*const sMovementActionFuncs_FollowingPokemon_FaceWest[])(struct ObjectEvent *, struct Sprite *);
static bool8 (*const sMovementActionFuncs_FollowingPokemon_FaceEast[])(struct ObjectEvent *, struct Sprite *);
static bool8 (*const sMovementActionFuncs_FollowingPokemon_Shrink[])(struct ObjectEvent *, struct Sprite *);
static bool8 (*const sMovementActionFuncs_FollowingPokemon_Grow[])(struct ObjectEvent *, struct Sprite *);
"""),
("""    [MOVEMENT_ACTION_JUMP_SPECIAL_WITH_EFFECT_RIGHT]     = sMovementActionFuncs_JumpSpecialWithEffectRight,
};""",
"""    [MOVEMENT_ACTION_JUMP_SPECIAL_WITH_EFFECT_RIGHT]     = sMovementActionFuncs_JumpSpecialWithEffectRight,
    [MOVEMENT_ACTION_FOLLOWING_POKEMON]                  = sMovementActionFuncs_FollowingPokemon,
    [MOVEMENT_ACTION_FOLLOWING_POKEMON_FACESOUTH]        = sMovementActionFuncs_FollowingPokemon_FaceSouth,
    [MOVEMENT_ACTION_FOLLOWING_POKEMON_FACENORTH]        = sMovementActionFuncs_FollowingPokemon_FaceNorth,
    [MOVEMENT_ACTION_FOLLOWING_POKEMON_FACEWEST]         = sMovementActionFuncs_FollowingPokemon_FaceWest,
    [MOVEMENT_ACTION_FOLLOWING_POKEMON_FACEEAST]         = sMovementActionFuncs_FollowingPokemon_FaceEast,
    [MOVEMENT_ACTION_FOLLOWING_POKEMON_SHRINK]           = sMovementActionFuncs_FollowingPokemon_Shrink,
    [MOVEMENT_ACTION_FOLLOWING_POKEMON_GROW]             = sMovementActionFuncs_FollowingPokemon_Grow,
};"""),
])
s=open('src/data/object_events/movement_action_func_tables.h').read()
nl='\r\n' if '\r\n' in s else '\n'
add="""
static bool8 (*const sMovementActionFuncs_FollowingPokemon[])(struct ObjectEvent *, struct Sprite *) = {
    MovementAction_FollowingPokemon_Step0,
};

static bool8 (*const sMovementActionFuncs_FollowingPokemon_FaceSouth[])(struct ObjectEvent *, struct Sprite *) = {
    MovementAction_FollowingPokemon_FaceSouth_Step0,
    MovementAction_Finish,
};

static bool8 (*const sMovementActionFuncs_FollowingPokemon_FaceNorth[])(struct ObjectEvent *, struct Sprite *) = {
    MovementAction_FollowingPokemon_FaceNorth_Step0,
    MovementAction_Finish,
};

static bool8 (*const sMovementActionFuncs_FollowingPokemon_FaceWest[])(struct ObjectEvent *, struct Sprite *) = {
    MovementAction_FollowingPokemon_FaceWest_Step0,
    MovementAction_Finish,
};

static bool8 (*const sMovementActionFuncs_FollowingPokemon_FaceEast[])(struct ObjectEvent *, struct Sprite *) = {
    MovementAction_FollowingPokemon_FaceEast_Step0,
    MovementAction_Finish,
};

static bool8 (*const sMovementActionFuncs_FollowingPokemon_Shrink[])(struct ObjectEvent *, struct Sprite *) = {
    MovementAction_FollowingPokemon_Shrink_Step0,
    MovementAction_FollowingPokemon_Shrink_Step1,
    MovementAction_Finish,
};

static bool8 (*const sMovementActionFuncs_FollowingPokemon_Grow[])(struct ObjectEvent *, struct Sprite *) = {
    MovementAction_FollowingPokemon_Grow_Step0,
    MovementAction_FollowingPokemon_Grow_Step1,
    MovementAction_Finish,
};
"""
open('src/data/object_events/movement_action_func_tables.h','w').write(s.rstrip()+nl+add.replace('\n',nl))
print('constants ok')
