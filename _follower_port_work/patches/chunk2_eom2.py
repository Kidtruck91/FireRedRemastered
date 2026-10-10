# Chunk 2 pass 2: follower step animations and follower movement actions
import re
P='src/event_object_movement.c'
s=open(P).read(); crlf='\r\n' in s; s=s.replace('\r\n','\n')
def rep(old,new):
    global s
    n=s.count(old)
    if n!=1: raise SystemExit(f'expected 1 match, got {n} for:\n{old}')
    s=s.replace(old,new)

rep('#include "constants/songs.h"\n', '#include "constants/songs.h"\n#include "constants/species.h"\n')
rep('static void MovementType_FollowingPokemon(struct Sprite *);\n','''static void MovementType_FollowingPokemon(struct Sprite *);

static bool8 IsFollowerObjectEvent(struct ObjectEvent *objectEvent)
{
    return gSaveBlock2Ptr->follower.inProgress && objectEvent == &gObjectEvents[gSaveBlock2Ptr->follower.objId];
}

// Follower Pokemon sprites have their own walk animations (see follower_anims.h). The animation is only
// restarted when the kind of step changes, so the idle bounce keeps going while walking.
// 'stepKind' is a tag stored in sprite->data[7]; x2 recenters the 32x32 side-facing frames.
static void FollowerSetStepAnim(struct ObjectEvent *objectEvent, struct Sprite *sprite, u8 stepKind, u8 animNum, s16 x2)
{
    if (!IsFollowerObjectEvent(objectEvent))
        return;
    if (sprite->data[7] != stepKind)
    {
        sprite->data[7] = stepKind;
        sprite->animNum = animNum;
        sprite->x2 = x2;
        SeekSpriteAnim(sprite, ++sprite->animCmdIndex);
    }
}

static void FollowerPrepareJump(struct ObjectEvent *objectEvent, struct Sprite *sprite, s16 x2)
{
    if (!IsFollowerObjectEvent(objectEvent))
        return;
    sprite->x2 = x2;
    sprite->data[7] = 10;
}
''')

# step animations: (function, stepKind, animNum, x2)
table=[]
dirs=[('Down',0),('Up',0),('Left',8),('Right',-8)]
for base,(tag,anim) in [('WalkSlow',(19,4)),('WalkNormal',(2,4)),('WalkFast',(6,8)),('WalkFaster',(11,12)),('Slide',(15,16))]:
    for i,(d,x2) in enumerate(dirs):
        table.append((f'MovementAction_{base}{d}_Step0',tag+i,anim+i,x2))
for name,tag,anim,x2 in table:
    m=re.search(r'(static )?bool8 '+name+r'\(struct ObjectEvent \*objectEvent, struct Sprite \*sprite\)\n\{\n', s)
    if not m: raise SystemExit('missing '+name)
    s=s[:m.end()]+f'    FollowerSetStepAnim(objectEvent, sprite, {tag}, {anim}, {x2});\n'+s[m.end():]
for d,x2 in dirs:
    name=f'MovementAction_Jump2{d}_Step0'
    m=re.search(r'(static )?bool8 '+name+r'\(struct ObjectEvent \*objectEvent, struct Sprite \*sprite\)\n\{\n', s)
    if not m: raise SystemExit('missing '+name)
    s=s[:m.end()]+f'    FollowerPrepareJump(objectEvent, sprite, {x2});\n'+s[m.end():]

# movement type
rep('movement_type_def(MovementType_WanderAroundSlower, gMovementTypeFuncs_WanderAroundSlower)\n','''movement_type_def(MovementType_WanderAroundSlower, gMovementTypeFuncs_WanderAroundSlower)
movement_type_def(MovementType_FollowingPokemon, gMovementTypeFuncs_FollowingPokemon)

static bool8 MovementType_FollowingPokemon_Step0(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    ClearObjectEventMovement(objectEvent, sprite);
    sprite->data[1] = 1;
    return TRUE;
}

static bool8 MovementType_FollowingPokemon_Step1(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    ObjectEventSetSingleMovement(objectEvent, sprite, MOVEMENT_ACTION_FOLLOWING_POKEMON);
    sprite->data[1] = 2;
    return TRUE;
}

static bool8 MovementType_FollowingPokemon_Step2(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    if (ObjectEventExecSingleMovementAction(objectEvent, sprite))
    {
        objectEvent->singleMovementActive = FALSE;
        sprite->data[1] = 3;
    }
    return FALSE;
}

static bool8 MovementType_FollowingPokemon_Step3(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    sprite->data[1] = 1;
    return TRUE;
}
''')

ACTIONS=open('_follower_port_work/patches/follower_actions.c').read()
rep('''u8 MovementAction_Finish(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    return TRUE;
}
''','''u8 MovementAction_Finish(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    return TRUE;
}
'''+ACTIONS)
if crlf: s=s.replace('\n','\r\n')
open(P,'w').write(s)
print('pass2 ok')
