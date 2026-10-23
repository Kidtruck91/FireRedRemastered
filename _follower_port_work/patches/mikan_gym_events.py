#!/usr/bin/env python3
# Re-aim Cissy's gym events at the reworked 25x32 layout. The map itself is the
# user's Porymap work and is not touched -- only map.json events and scripts.inc.
#
# How the rebuilt room plays: the bottom maze loops you round and drops you on the
# platform at (3,24), which is the way up into the main pool. From the pool you pick a
# column and head north through the funnel on row 6; only x=12 carries you up to
# Cissy's landing. Every other column sweeps you into the chute at x=1 or x=22, which
# runs unbroken to the bottom and puts you back at the entrance to try again.
#
# Three things no longer matched the new map:
#   - the swimmers stood ON the chute tiles (1,10) and (22,10). An object event blocks
#     forced movement, so each one dammed its own chute.
#   - the pocket triggers could never fire. input->tookStep gates coordinate events and
#     is only set when the player is NOT on a forced-movement tile, so a trigger inside
#     a current is dead. The old room worked because the player stopped in a still-water
#     pocket; these chutes never stop. The swimmers are sight trainers on the platforms
#     instead -- CheckForTrainersWantingBattle runs ahead of that gate, and in the open
#     pool the player is not being forced anyway.
#   - the post-battle sweep warped to (12,17), the old dock, which in the new map is
#     open water halfway up the pool and skipped the whole maze. The currents already
#     return the player to the entrance, so the script is gone.
import json
import re

M = 'data/maps/OrangeIslands_Mikan_Gym/map.json'
P = 'OrangeIslands_Mikan_Gym_EventScript_'
j = json.load(open(M))


def obj(gfx, x, y, face, script, sight=0):
    return {'type': 'object', 'graphics_id': gfx, 'x': x, 'y': y, 'elevation': 0,
            'movement_type': 'MOVEMENT_TYPE_FACE_' + face,
            'movement_range_x': 1, 'movement_range_y': 1,
            'trainer_type': 'TRAINER_TYPE_NORMAL' if sight else 'TRAINER_TYPE_NONE',
            'trainer_sight_or_berry_tree_id': str(sight), 'script': script, 'flag': '0'}


j['object_events'] = [
    obj('OBJ_EVENT_GFX_CISSY', 12, 4, 'DOWN', P + 'Cissy'),
    # On the platforms in the pool, watching the lanes the player has to cross.
    obj('OBJ_EVENT_GFX_SWIMMER_M_WATER', 3, 9, 'RIGHT', P + 'Marco', sight=5),
    obj('OBJ_EVENT_GFX_SWIMMER_F_WATER', 20, 18, 'LEFT', P + 'Coral', sight=5),
    obj('OBJ_EVENT_GFX_GYM_GUY', 7, 31, 'DOWN', P + 'GymGuy'),
]
j['coord_events'] = []
j['bg_events'] = [{'type': 'sign', 'x': x, 'y': 31, 'elevation': 0,
                   'player_facing_dir': 'BG_EVENT_PLAYER_FACING_ANY',
                   'script': P + 'GymStatue'} for x in (5, 17)]

json.dump(j, open(M, 'w'), indent=2)
open(M, 'a').write('\n')

S = 'data/maps/OrangeIslands_Mikan_Gym/scripts.inc'
s = open(S).read()

# The pocket/sweep-back scripts are unreachable now; the currents do that job.
s = re.sub(r'@ -+ the pockets.*?(?=OrangeIslands_Mikan_Gym_EventScript_Marco::)', '', s, flags=re.S)
s = s.replace("""OrangeIslands_Mikan_Gym_EventScript_Marco::
	msgbox OrangeIslands_Mikan_Gym_Text_MarcoTalk, MSGBOX_NPC
	end

OrangeIslands_Mikan_Gym_EventScript_Coral::
	msgbox OrangeIslands_Mikan_Gym_Text_CoralTalk, MSGBOX_NPC
	end""",
"""OrangeIslands_Mikan_Gym_EventScript_Marco::
	trainerbattle_single TRAINER_SWIMMER_M_MARCO, OrangeIslands_Mikan_Gym_Text_MarcoIntro, OrangeIslands_Mikan_Gym_Text_MarcoDefeat
	msgbox OrangeIslands_Mikan_Gym_Text_MarcoTalk, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Mikan_Gym_EventScript_Coral::
	trainerbattle_single TRAINER_SWIMMER_F_CORAL, OrangeIslands_Mikan_Gym_Text_CoralIntro, OrangeIslands_Mikan_Gym_Text_CoralDefeat
	msgbox OrangeIslands_Mikan_Gym_Text_CoralTalk, MSGBOX_AUTOCLOSE
	end""")
# Their intros referred to being washed into a corner, which no longer happens.
s = s.replace('''	.string "MARCO: Wrong channel! The cross\\n"
	.string "current always ends up here.\\p"
	.string "And so do I.$"''',
'''	.string "MARCO: You got this far up the\\n"
	.string "pool? Then you can spare a\\l"
	.string "battle!$"''')
s = s.replace('''	.string "CORAL: The current carried you to\\n"
	.string "my corner. Rain or shine, I'm\\l"
	.string "ready!$"''',
'''	.string "CORAL: Nobody crosses my lane\\n"
	.string "without a battle. Rain or shine,\\l"
	.string "I'm ready!$"''')
s = s.replace('''	.string "Four channels feed this pool, and\\n"
	.string "three of them wash out here.$"''',
'''	.string "Pick your lane at the top and keep\\n"
	.string "count. Only one of them goes up.$"''')
s = s.replace('''	.string "Four channels run north out of\\n"
	.string "this bay. They all feed one cross\\l"
	.string "current up top.\\p"
	.string "Only one of them lines up with the\\n"
	.string "gap that leads to her. The rest\\l"
	.string "wash you into a corner.\\p"
	.string "Count the channels from the left\\n"
	.string "and remember which one you tried!\\p"''',
'''	.string "The bottom of the pool is a maze.\\n"
	.string "Ride it round and it sets you on\\l"
	.string "the rock that gets you up top.\\p"
	.string "Then you pick a lane and swim\\n"
	.string "north. Only the middle one carries\\l"
	.string "you up to CISSY.\\p"
	.string "Pick wrong and the outflow takes\\n"
	.string "you all the way back down here!\\p"''')
open(S, 'w').write(s)
print('mikan gym events ok: swimmers are sight trainers on the platforms; dead pocket scripts removed')
