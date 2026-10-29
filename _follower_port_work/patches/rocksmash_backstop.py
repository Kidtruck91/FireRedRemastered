#!/usr/bin/env python3
"""A second source for HM06, because the first one is entirely optional.

Nothing in the base game ever requires Rock Smash. HM06 sits with a man in
EMBER SPA off Kindle Road, no script anywhere checks for the move or the flag
except that man and the help menu, and no Rock Smash rock in the game splits a
map's warps -- all 100 of them guard item balls and nothing else. A player can
finish the Sevii Islands, become Champion and sail to the Orange Islands having
walked straight past it.

Danny's gym needs it. So a hiker in SUNBURST CAVE keeps a spare, and someone
outside the gym on NAVEL says where to look.
"""
import json
import os
import sys

CAVE = 'OrangeIslands_Sunburst_Cave'
ISLAND = 'OrangeIslands_Navel'
CAVE_POS = (19, 2)        # the far north chamber, past both hikers
ISLAND_POS = (34, 14)     # on the approach to the gym door at (37,13)


def obj(gfx, x, y, face, script):
    return {'type': 'object', 'graphics_id': gfx, 'x': x, 'y': y, 'elevation': 3,
            'movement_type': 'MOVEMENT_TYPE_FACE_' + face,
            'movement_range_x': 1, 'movement_range_y': 1,
            'trainer_type': 'TRAINER_TYPE_NONE',
            'trainer_sight_or_berry_tree_id': '0', 'script': script, 'flag': '0'}


CAVE_SCRIPT = '''
@ ---- HM06 backstop (rocksmash_backstop.py) ----
@ The only other ROCK SMASH in the game is an optional NPC in EMBER SPA that
@ nothing sends you to. This one is deliberately on the way to DANNY's gym.
OrangeIslands_Sunburst_Cave_EventScript_RockSmashHiker::
\tlock
\tfaceplayer
\tgoto_if_set FLAG_GOT_HM06, OrangeIslands_Sunburst_Cave_EventScript_RockSmashHikerAfter
\tmsgbox OrangeIslands_Sunburst_Cave_Text_RockSmashOffer
\tgiveitem ITEM_HM06
\tsetflag FLAG_GOT_HM06
\tmsgbox OrangeIslands_Sunburst_Cave_Text_RockSmashExplain
\trelease
\tend

OrangeIslands_Sunburst_Cave_EventScript_RockSmashHikerAfter::
\tmsgbox OrangeIslands_Sunburst_Cave_Text_RockSmashAlready
\trelease
\tend

OrangeIslands_Sunburst_Cave_Text_RockSmashOffer::
\t.string "Oh, you came all the way up\\n"
\t.string "here? Then you've earned this.\\p"
\t.string "I cut this whole cave with it.\\n"
\t.string "Keep it. I know it by heart.$"

OrangeIslands_Sunburst_Cave_Text_RockSmashExplain::
\t.string "HM06 is ROCK SMASH. Teach it to\\n"
\t.string "anything with arms.\\p"
\t.string "Cracks any rock small enough to\\n"
\t.string "pick up. The big ones still need\\l"
\t.string "STRENGTH.$"

OrangeIslands_Sunburst_Cave_Text_RockSmashAlready::
\t.string "You've already got ROCK SMASH?\\n"
\t.string "Good. You'll want it on NAVEL.$"
'''

ISLAND_SCRIPT = '''
@ ---- HM06 backstop (rocksmash_backstop.py) ----
OrangeIslands_Navel_EventScript_RockSmashTip::
\tlock
\tfaceplayer
\tgoto_if_unset FLAG_GOT_HM06, OrangeIslands_Navel_EventScript_RockSmashTipNeeded
\tmsgbox OrangeIslands_Navel_Text_RockSmashTipHave
\trelease
\tend

OrangeIslands_Navel_EventScript_RockSmashTipNeeded::
\tmsgbox OrangeIslands_Navel_Text_RockSmashTipNeeded
\trelease
\tend

OrangeIslands_Navel_Text_RockSmashTipNeeded::
\t.string "Heading up to DANNY's? You'll\\n"
\t.string "get nowhere without ROCK SMASH.\\p"
\t.string "Don't have it? Nobody does.\\n"
\t.string "It's the one nobody makes you\\l"
\t.string "pick up.\\p"
\t.string "There's an old hiker deep in the\\n"
\t.string "cave on SUNBURST. He keeps a\\l"
\t.string "spare. Tell him I sent you.$"

OrangeIslands_Navel_Text_RockSmashTipHave::
\t.string "You've got ROCK SMASH and\\n"
\t.string "STRENGTH? Then DANNY's gym is\\l"
\t.string "just a puzzle.$"
'''


def add(repo, mapname, pos, gfx, face, script, text):
    p = os.path.join(repo, 'data/maps/%s/map.json' % mapname)
    j = json.load(open(p))
    j['object_events'] = [o for o in j['object_events'] if o['script'] != script]
    j['object_events'].append(obj(gfx, pos[0], pos[1], face, script))
    json.dump(j, open(p, 'w'), indent=2)
    open(p, 'a').write('\n')
    p = os.path.join(repo, 'data/maps/%s/scripts.inc' % mapname)
    s = open(p).read()
    if 'HM06 backstop' not in s:
        open(p, 'w').write(s.rstrip('\n') + '\n' + text)


def main(repo='.'):
    add(repo, CAVE, CAVE_POS, 'OBJ_EVENT_GFX_HIKER', 'DOWN',
        '%s_EventScript_RockSmashHiker' % CAVE, CAVE_SCRIPT)
    add(repo, ISLAND, ISLAND_POS, 'OBJ_EVENT_GFX_BLACKBELT', 'DOWN',
        '%s_EventScript_RockSmashTip' % ISLAND, ISLAND_SCRIPT)
    print('HM06 backstop: hiker in %s at %s, tip on %s at %s'
          % (CAVE, CAVE_POS, ISLAND, ISLAND_POS))


if __name__ == '__main__':
    main(sys.argv[1] if len(sys.argv) > 1 else '.')
