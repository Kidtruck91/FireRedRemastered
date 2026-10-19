# Run from the repo root, after leader_hidden_abilities.py.
# The trip to the Orange Islands and back:
#  1. After the Sevii Islands (Sapphire delivered, FLAG_SYS_CAN_LINK_WITH_RS) Celio passes on that
#     Prof. Oak wants to see you. Oak talks about his friend Prof. Ivy and gives the SUNKIST TICKET
#     (the old ORANGE TICKET item, renamed). FLAG_GOT_SUNKIST_TICKET.
#  2. Vermilion's Seagallop sailor: with the ticket, VALENCIA ISLAND is offered first. Before the
#     GS Ball he warns you that you may not get back to the mainland for a while.
#     The Seagallop lands at the Valencia dock (19,30) and the Ivy arrival scene runs.
#  3. Valencia dock sailor (17,29): the ship has left and won't be back for several days.
#  4. Kumquat boatman (30,50) <-> Pummelo boatman (19,67): the League boat, with all four Orange
#     badges (Seagallop scene, SEAGALLOP_KUMQUAT / SEAGALLOP_PUMMELO).
#  5. Drake gives the GS BALL (new key item, ITEM_GS_BALL, FLAG_GOT_GS_BALL) after the trophy.
#  6. With the GS Ball the Valencia sailor sails you home to Vermilion. If the Orange challenge is
#     still active, OrangeChallenge_End stores the Orange team and PC first and brings the Kanto
#     ones back. Later trips are plain visits with your own team.
import json, re
def rd(p): return open(p, encoding='utf-8').read()
def wr(p, s): open(p, 'w', newline='\n', encoding='utf-8').write(s)
def sub1(s, old, new, p):
    assert old in s, (p, old[:80]); return s.replace(old, new, 1)

# ---- flags
p = 'include/constants/flags.h'; s = rd(p)
if 'FLAG_GOT_SUNKIST_TICKET' not in s:
    s = re.sub(r'#define FLAG_0x21B\s+0x21B', '#define FLAG_GOT_SUNKIST_TICKET       0x21B // from Prof. Oak after the Sevii Islands', s)
    s = re.sub(r'#define FLAG_0x21C\s+0x21C', '#define FLAG_GOT_GS_BALL              0x21C // from Drake; the Valencia ferry home runs', s)
    assert 'FLAG_GOT_GS_BALL' in s; wr(p, s)

# ---- items: SUNKIST TICKET (renamed ORANGE TICKET) and GS BALL
p = 'include/constants/items.h'; s = rd(p)
if 'ITEM_GS_BALL' not in s:
    s = sub1(s, '#define ITEM_ORANGE_TICKET 375\n\n#define ITEMS_COUNT 376',
             '#define ITEM_SUNKIST_TICKET 375\n#define ITEM_GS_BALL 376\n\n#define ITEMS_COUNT 377', p); wr(p, s)
p = 'src/data/items.json'; s = rd(p)
if 'ITEM_GS_BALL' not in s:
    s = sub1(s, '"english": "ORANGETICKET",\n      "itemId": "ITEM_ORANGE_TICKET",',
             '"english": "SUNKISTTICKET",\n      "itemId": "ITEM_SUNKIST_TICKET",', p)
    s = sub1(s, '"description_english": "A ferry ticket to the ORANGE\\\\nISLANDS. It smells faintly of\\\\ncitrus.",',
             '"description_english": "A SEAGALLOP ticket to VALENCIA\\\\nISLAND in the ORANGE ISLANDS. It\\\\nsmells faintly of citrus.",', p)
    i = s.rindex('    }\n  ]\n}')
    s = s[:i] + '''    },
    {
      "english": "GS BALL",
      "itemId": "ITEM_GS_BALL",
      "price": 0,
      "holdEffect": "HOLD_EFFECT_NONE",
      "holdEffectParam": 0,
      "description_english": "A gold and silver BALL engraved\\\\nwith a G and an S. Nobody has\\\\nbeen able to open it.",
      "importance": 1,
      "registrability": 0,
      "pocket": "POCKET_KEY_ITEMS",
      "type": "ITEM_TYPE_BAG_MENU",
      "fieldUseFunc": "FieldUseFunc_OakStopsYou",
      "battleUsage": 0,
      "battleUseFunc": "NULL",
      "secondaryId": 0
''' + s[i:]
    wr(p, s)
p = 'src/data/item_icon_table.h'; s = rd(p)
if 'ITEM_GS_BALL' not in s:
    s = sub1(s, '    [ITEM_ORANGE_TICKET]  = {gItemIcon_MysticTicket, gItemIconPalette_OrangeTicket},\n',
             '    [ITEM_SUNKIST_TICKET] = {gItemIcon_MysticTicket, gItemIconPalette_OrangeTicket},\n'
             '    [ITEM_GS_BALL]        = {gItemIcon_PokeBall, gItemIconPalette_GSBall},\n', p); wr(p, s)
p = 'src/data/graphics/items.h'; s = rd(p)
if 'GSBall' not in s:
    s = sub1(s, 'const u32 gItemIconPalette_OrangeTicket[] = INCBIN_U32("graphics/items/icon_palettes/orange_ticket.gbapal.lz");\n',
             'const u32 gItemIconPalette_OrangeTicket[] = INCBIN_U32("graphics/items/icon_palettes/orange_ticket.gbapal.lz");\n'
             'const u32 gItemIconPalette_GSBall[] = INCBIN_U32("graphics/items/icon_palettes/gs_ball.gbapal.lz");\n', p); wr(p, s)
p = 'include/graphics.h'; s = rd(p)
if 'GSBall' not in s:
    s = sub1(s, 'extern const u32 gItemIconPalette_OrangeTicket[];\n',
             'extern const u32 gItemIconPalette_OrangeTicket[];\nextern const u32 gItemIconPalette_GSBall[];\n', p); wr(p, s)
# Poke Ball icon, gold top and silver bottom
wr('graphics/items/icon_palettes/gs_ball.pal', 'JASC-PAL\r\n0100\r\n16\r\n' + ''.join('%d %d %d\r\n' % c for c in [
    (180, 180, 180), (49, 49, 49), (74, 82, 82), (112, 116, 128), (168, 172, 188), (208, 212, 224), (255, 255, 255),
    (136, 92, 24), (168, 120, 32), (200, 152, 40), (236, 196, 64), (255, 238, 144),
    (255, 0, 255), (255, 0, 255), (255, 0, 255), (255, 0, 255)]))

# ---- Seagallop ports
p = 'include/constants/seagallop.h'; s = rd(p)
if 'SEAGALLOP_VALENCIA' not in s:
    s = sub1(s, '#define SEAGALLOP_BIRTH_ISLAND    10\n', '#define SEAGALLOP_BIRTH_ISLAND    10\n'
             '#define SEAGALLOP_VALENCIA        11 // Orange Islands: Valencia dock\n'
             '#define SEAGALLOP_KUMQUAT         12 // Orange League boat\n'
             '#define SEAGALLOP_PUMMELO         13\n', p); wr(p, s)
p = 'src/seagallop.c'; s = rd(p)
if 'SEAGALLOP_VALENCIA' not in s:
    s = sub1(s, '    [SEAGALLOP_BIRTH_ISLAND]    = {MAP(BIRTH_ISLAND_HARBOR), 0x08, 0x05}\n};',
             '    [SEAGALLOP_BIRTH_ISLAND]    = {MAP(BIRTH_ISLAND_HARBOR), 0x08, 0x05},\n'
             '    [SEAGALLOP_VALENCIA]        = {MAP(ORANGE_ISLANDS_VALENCIA), 19, 30},\n'
             '    [SEAGALLOP_KUMQUAT]         = {MAP(ORANGE_ISLANDS_KUMQUAT),  30, 49},\n'
             '    [SEAGALLOP_PUMMELO]         = {MAP(ORANGE_ISLANDS_PUMMELO),  20, 67}\n};', p)
    s = sub1(s, '    [SEAGALLOP_BIRTH_ISLAND]    = 0x000  // 00000000000\n};',
             '    [SEAGALLOP_BIRTH_ISLAND]    = 0x000, // 00000000000\n'
             '    [SEAGALLOP_VALENCIA]        = 0x001, // to Vermilion: eastbound\n'
             '    [SEAGALLOP_KUMQUAT]         = 0x2000, // to Pummelo: eastbound\n'
             '    [SEAGALLOP_PUMMELO]         = 0x000\n};', p)
    wr(p, s)

# ---- Celio: Oak wants to see you
p = 'data/maps/OneIsland_PokemonCenter_1F/scripts.inc'; s = rd(p)
if 'OneIsland_PokemonCenter_1F_Text_OakWantsToSeeYou' not in s:
    s = sub1(s, '''	setvar VAR_MAP_SCENE_ONE_ISLAND_POKEMON_CENTER_1F, 6
	special InitRoamer
	release
	end
''', '''	setvar VAR_MAP_SCENE_ONE_ISLAND_POKEMON_CENTER_1F, 6
	special InitRoamer
	msgbox OneIsland_PokemonCenter_1F_Text_OakWantsToSeeYou
	release
	end

OneIsland_PokemonCenter_1F_Text_OakWantsToSeeYou::
	.string "Oh, and before I forget...\\p"
	.string "BILL called earlier. PROF. OAK\\n"
	.string "asked if you could stop by his\\l"
	.string "LAB in PALLET TOWN.$"
''', p); wr(p, s)

# ---- Oak: Prof. Ivy and the SUNKIST TICKET
p = 'data/maps/PalletTown_ProfessorOaksLab/scripts.inc'; s = rd(p)
if 'SunkistTicket' not in s:
    s = sub1(s, '''PalletTown_ProfessorOaksLab_EventScript_ProfOak::
	lock
	faceplayer
''', '''PalletTown_ProfessorOaksLab_EventScript_ProfOak::
	lock
	faceplayer
	goto_if_unset FLAG_SYS_CAN_LINK_WITH_RS, PalletTown_ProfessorOaksLab_EventScript_ProfOakTalk
	goto_if_unset FLAG_GOT_SUNKIST_TICKET, PalletTown_ProfessorOaksLab_EventScript_OakSunkistTicket
PalletTown_ProfessorOaksLab_EventScript_ProfOakTalk::
''', p)
    s += '''
@ ---- After the Sevii Islands: Prof. Ivy and the SUNKIST TICKET (orange_trip_story.py)
PalletTown_ProfessorOaksLab_EventScript_OakSunkistTicket::
	msgbox PalletTown_ProfessorOaksLab_Text_OakIvyIntro
	giveitem ITEM_SUNKIST_TICKET
	setflag FLAG_GOT_SUNKIST_TICKET
	msgbox PalletTown_ProfessorOaksLab_Text_OakExplainSunkistTicket
	release
	end

PalletTown_ProfessorOaksLab_Text_OakIvyIntro::
	.string "OAK: Ah, {PLAYER}! I heard about\\n"
	.string "everything you did in the SEVII\\l"
	.string "ISLANDS. Well done!\\p"
	.string "I have an old friend, PROF. IVY.\\n"
	.string "She studies POKéMON in the ORANGE\\l"
	.string "ISLANDS, far to the south.\\p"
	.string "The islands have their own league,\\n"
	.string "the ORANGE CREW. Their trainers\\l"
	.string "battle by some very unusual rules.\\p"
	.string "I think you're ready for them.\\n"
	.string "Here, take this!$"

PalletTown_ProfessorOaksLab_Text_OakExplainSunkistTicket::
	.string "OAK: That's a SUNKIST TICKET.\\n"
	.string "Show it to the SEAGALLOP sailor\\l"
	.string "in VERMILION CITY.\\p"
	.string "PROF. IVY will meet you on\\n"
	.string "VALENCIA ISLAND. I'll tell her\\l"
	.string "you're coming!$"
'''
    wr(p, s)

# ---- Vermilion: VALENCIA ISLAND with the ticket
p = 'data/maps/VermilionCity/scripts.inc'; s = rd(p)
if 'VermilionCity_EventScript_TrySunkistTicket' not in s:
    for lbl in ('VermilionCity_EventScript_CheckSeagallopPresent::', 'VermilionCity_EventScript_CheckSeagallopPresentTrigger::'):
        s = sub1(s, lbl + '\n\tsetvar VAR_0x8004, SEAGALLOP_VERMILION_CITY\n',
                 lbl + '\n\tsetvar VAR_0x8004, SEAGALLOP_VERMILION_CITY\n\tcall VermilionCity_EventScript_TrySunkistTicket\n', p)
    s += '''
@ ---- SUNKIST TICKET: the Seagallop to Valencia Island (orange_trip_story.py)
@ Returns when the player says no; otherwise sails.
VermilionCity_EventScript_TrySunkistTicket::
	checkitem ITEM_SUNKIST_TICKET
	goto_if_eq VAR_RESULT, FALSE, VermilionCity_EventScript_SunkistReturn
	msgbox VermilionCity_Text_SunkistAsk, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, VermilionCity_EventScript_SunkistReturn
	goto_if_set FLAG_GOT_GS_BALL, VermilionCity_EventScript_SunkistSail
	msgbox VermilionCity_Text_SunkistWarning, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, VermilionCity_EventScript_SunkistReturn
VermilionCity_EventScript_SunkistSail::
	msgbox VermilionCity_Text_SunkistDeparting
	setvar VAR_0x8004, SEAGALLOP_VERMILION_CITY
	setvar VAR_0x8006, SEAGALLOP_VALENCIA
	goto EventScript_SetSail
	end

VermilionCity_EventScript_SunkistReturn::
	setvar VAR_0x8004, SEAGALLOP_VERMILION_CITY
	return

VermilionCity_Text_SunkistAsk::
	.string "Oh! That's a SUNKIST TICKET!\\p"
	.string "It's good for VALENCIA ISLAND\\n"
	.string "in the ORANGE ISLANDS.\\p"
	.string "Want to sail there today?$"

VermilionCity_Text_SunkistWarning::
	.string "Hold on, now. The ORANGE ISLANDS\\n"
	.string "are a long way south.\\p"
	.string "Once you're there, the ship won't\\n"
	.string "come back for a while. You might\\l"
	.string "not get back to the mainland for\\l"
	.string "some time.\\p"
	.string "Are you sure?$"

VermilionCity_Text_SunkistDeparting::
	.string "All right!\\n"
	.string "All aboard for VALENCIA ISLAND!$"
'''
    wr(p, s)

def add_object(mp, obj):
    p = 'data/maps/%s/map.json' % mp; mj = json.loads(rd(p))
    if not any(o.get('script') == obj['script'] for o in mj['object_events']):
        mj['object_events'].append(obj)
        wr(p, json.dumps(mj, indent=2, ensure_ascii=False) + '\n')
def sailor(x, y, move, script):
    return {'type': 'object', 'graphics_id': 'OBJ_EVENT_GFX_SAILOR', 'x': x, 'y': y, 'elevation': 3,
            'movement_type': move, 'movement_range_x': 0, 'movement_range_y': 0,
            'trainer_type': 'TRAINER_TYPE_NONE', 'trainer_sight_or_berry_tree_id': '0', 'script': script, 'flag': '0'}
def add_script(mp, marker, text):
    p = 'data/maps/%s/scripts.inc' % mp; s = rd(p)
    if marker not in s:
        wr(p, s.rstrip('\n') + '\n' + text)

# ---- Valencia dock: the ferry home
add_object('OrangeIslands_Valencia', sailor(17, 29, 'MOVEMENT_TYPE_FACE_RIGHT', 'OrangeIslands_Valencia_EventScript_FerrySailor'))
add_script('OrangeIslands_Valencia', 'OrangeIslands_Valencia_EventScript_FerrySailor::', '''
@ ---- Ferry sailor (orange_trip_story.py): no ship until Drake hands over the GS BALL
OrangeIslands_Valencia_EventScript_FerrySailor::
	lock
	faceplayer
	goto_if_set FLAG_GOT_GS_BALL, OrangeIslands_Valencia_EventScript_FerryHome
	msgbox OrangeIslands_Valencia_Text_ShipLeft
	release
	end

OrangeIslands_Valencia_EventScript_FerryHome::
	msgbox OrangeIslands_Valencia_Text_FerryHomeAsk, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, OrangeIslands_Valencia_EventScript_FerryNotNow
	goto_if_unset FLAG_ORANGE_CHALLENGE, OrangeIslands_Valencia_EventScript_FerrySail
	msgbox OrangeIslands_Valencia_Text_TeamsSwap
	callnative OrangeChallenge_End
	goto_if_eq VAR_RESULT, FALSE, OrangeIslands_Valencia_EventScript_FerrySaveError
	playse SE_PC_LOGIN
	waitse
OrangeIslands_Valencia_EventScript_FerrySail::
	msgbox OrangeIslands_Valencia_Text_FerryDeparting
	setvar VAR_0x8004, SEAGALLOP_VALENCIA
	setvar VAR_0x8006, SEAGALLOP_VERMILION_CITY
	goto EventScript_SetSail
	end

OrangeIslands_Valencia_EventScript_FerryNotNow::
	msgbox OrangeIslands_Valencia_Text_FerryNotNow
	release
	end

OrangeIslands_Valencia_EventScript_FerrySaveError::
	msgbox OrangeIslands_Valencia_Text_FerrySaveError
	release
	end

OrangeIslands_Valencia_Text_ShipLeft::
	.string "The SEAGALLOP that brought you\\n"
	.string "has already sailed back to KANTO.\\p"
	.string "It won't be back for several days.\\p"
	.string "The captain says he's waiting on\\n"
	.string "something from PUMMELO ISLAND.\\l"
	.string "Enjoy the islands until then!$"

OrangeIslands_Valencia_Text_FerryHomeAsk::
	.string "Ahoy! The SEAGALLOP is back.\\p"
	.string "Sailing home to VERMILION CITY?$"

OrangeIslands_Valencia_Text_TeamsSwap::
	.string "Your ORANGE ISLANDS team and PC\\n"
	.string "will wait here for you.\\p"
	.string "Your KANTO team and PC go home\\n"
	.string "with you.$"

OrangeIslands_Valencia_Text_FerryDeparting::
	.string "All right!\\n"
	.string "All aboard for VERMILION CITY!$"

OrangeIslands_Valencia_Text_FerryNotNow::
	.string "No rush. The ship will wait.$"

OrangeIslands_Valencia_Text_FerrySaveError::
	.string "Hm, something went wrong storing\\n"
	.string "your POKéMON. Nothing was changed.\\p"
	.string "Please try again.$"
''')

# ---- League boat: Kumquat <-> Pummelo
add_object('OrangeIslands_Kumquat', sailor(30, 50, 'MOVEMENT_TYPE_FACE_UP', 'OrangeIslands_Kumquat_EventScript_LeagueBoat'))
add_script('OrangeIslands_Kumquat', 'OrangeIslands_Kumquat_EventScript_LeagueBoat::', '''
@ ---- League boat to Pummelo (orange_trip_story.py): all four Orange badges
OrangeIslands_Kumquat_EventScript_LeagueBoat::
	lock
	faceplayer
	goto_if_unset FLAG_BADGE_CORAL_EYE, OrangeIslands_Kumquat_EventScript_LeagueBoatNoBadges
	goto_if_unset FLAG_BADGE_SPIKE_SHELL, OrangeIslands_Kumquat_EventScript_LeagueBoatNoBadges
	goto_if_unset FLAG_BADGE_SEA_RUBY, OrangeIslands_Kumquat_EventScript_LeagueBoatNoBadges
	goto_if_unset FLAG_BADGE_JADE_STAR, OrangeIslands_Kumquat_EventScript_LeagueBoatNoBadges
	msgbox OrangeIslands_Kumquat_Text_LeagueBoatAsk, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, OrangeIslands_Kumquat_EventScript_LeagueBoatNo
	msgbox OrangeIslands_Kumquat_Text_LeagueBoatDeparting
	setvar VAR_0x8004, SEAGALLOP_KUMQUAT
	setvar VAR_0x8006, SEAGALLOP_PUMMELO
	goto EventScript_SetSail
	end

OrangeIslands_Kumquat_EventScript_LeagueBoatNoBadges::
	msgbox OrangeIslands_Kumquat_Text_LeagueBoatNoBadges
	release
	end

OrangeIslands_Kumquat_EventScript_LeagueBoatNo::
	msgbox OrangeIslands_Kumquat_Text_LeagueBoatNo
	release
	end

OrangeIslands_Kumquat_Text_LeagueBoatNoBadges::
	.string "This boat goes to PUMMELO ISLAND,\\n"
	.string "home of the ORANGE LEAGUE.\\p"
	.string "Only trainers with all four\\n"
	.string "ORANGE badges may board.$"

OrangeIslands_Kumquat_Text_LeagueBoatAsk::
	.string "Four badges! You've earned a ride.\\p"
	.string "Sail to PUMMELO ISLAND?$"

OrangeIslands_Kumquat_Text_LeagueBoatDeparting::
	.string "Hold on tight!\\n"
	.string "Next stop, PUMMELO ISLAND!$"

OrangeIslands_Kumquat_Text_LeagueBoatNo::
	.string "Come back when you're ready.$"
''')
add_object('OrangeIslands_Pummelo', sailor(19, 67, 'MOVEMENT_TYPE_FACE_RIGHT', 'OrangeIslands_Pummelo_EventScript_LeagueBoat'))
add_script('OrangeIslands_Pummelo', 'OrangeIslands_Pummelo_EventScript_LeagueBoat::', '''
@ ---- League boat back to Kumquat (orange_trip_story.py)
OrangeIslands_Pummelo_EventScript_LeagueBoat::
	lock
	faceplayer
	msgbox OrangeIslands_Pummelo_Text_LeagueBoatAsk, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, OrangeIslands_Pummelo_EventScript_LeagueBoatNo
	msgbox OrangeIslands_Pummelo_Text_LeagueBoatDeparting
	setvar VAR_0x8004, SEAGALLOP_PUMMELO
	setvar VAR_0x8006, SEAGALLOP_KUMQUAT
	goto EventScript_SetSail
	end

OrangeIslands_Pummelo_EventScript_LeagueBoatNo::
	msgbox OrangeIslands_Pummelo_Text_LeagueBoatNo
	release
	end

OrangeIslands_Pummelo_Text_LeagueBoatAsk::
	.string "Heading back to KUMQUAT ISLAND?$"

OrangeIslands_Pummelo_Text_LeagueBoatDeparting::
	.string "Hold on tight!\\n"
	.string "Next stop, KUMQUAT ISLAND!$"

OrangeIslands_Pummelo_Text_LeagueBoatNo::
	.string "I'll be right here.$"
''')

# ---- Drake: the GS BALL
M = 'OrangeIslands_Pummelo_Stadium'
p = 'data/maps/%s/scripts.inc' % M; s = rd(p)
if M + '_EventScript_DrakeGiveGSBall' not in s:
    s = sub1(s, '''	msgbox %s_Text_DrakeAfterTrophy
	release
	end
''' % M, '''	msgbox %s_Text_DrakeAfterTrophy
	goto %s_EventScript_DrakeGiveGSBall
	end

%s_EventScript_DrakeGiveGSBall::
	msgbox %s_Text_DrakeGSBall
	giveitem ITEM_GS_BALL
	setflag FLAG_GOT_GS_BALL
	msgbox %s_Text_DrakeGSBallFerry
	release
	end
''' % (M, M, M, M, M), p)
    s = sub1(s, '''%s_EventScript_DrakeAfter::
	setflag FLAG_ORANGE_LEAGUE_CHAMPION
''' % M, '''%s_EventScript_DrakeAfter::
	setflag FLAG_ORANGE_LEAGUE_CHAMPION
	goto_if_unset FLAG_GOT_GS_BALL, %s_EventScript_DrakeGiveGSBall
''' % (M, M), p)
    s += '''
%s_Text_DrakeGSBall::
	.string "One more thing.\\p"
	.string "This BALL washed up on PUMMELO a\\n"
	.string "few years back. Nobody here can\\l"
	.string "open it. Not even me.\\p"
	.string "You take it.$"

%s_Text_DrakeGSBallFerry::
	.string "PROF. OAK might know what it is.\\n"
	.string "Show it to him.\\p"
	.string "The SEAGALLOP captain was waiting\\n"
	.string "for it. His ship is back at\\l"
	.string "VALENCIA, ready to take you home.$"
''' % (M, M)
    wr(p, s)
print('orange trip story ok')
