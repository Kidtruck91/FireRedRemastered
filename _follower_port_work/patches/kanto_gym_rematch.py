# Run from the repo root, after johto_gym_scripts.py.
# Kanto gym leader rematches (Pewter .. Cinnabar; Viridian has no rematch teams):
#  - After the player has beaten the Champion (FLAG_DEFEATED_CHAMP), talking to the leader asks
#    for a rematch (YES/NO). YES fights one of the leader's two rematch teams, which can be fought
#    again any number of times. NO (or before the Champion) gives the usual post-battle talk.
#  - The gym's rematch NPC (G_Text_RematchCheckUnlocked) now just explains this.
# The first battle, badge and TM scripts are untouched. Leader texts stay in text.inc; the new
# lines are at the end of scripts.inc.
import re

def rd(p): return open(p, encoding='utf-8').read()
def wr(p, s): open(p, 'w', newline='\n', encoding='utf-8').write(s)

GYMS = [
    # map prefix,       flag suffix,  trainer,  script label part, shown name, rematch prompt
    ('PewterCity_Gym',     'BROCK',    'BROCK',    'Brock',    'BROCK',
     ["My rock-hard willpower hasn't", "budged. Up for a rematch?"]),
    ('CeruleanCity_Gym',   'MISTY',    'MISTY',    'Misty',    'MISTY',
     ["Hey! I've been training hard.", "Rematch? You'd better say yes!"]),
    ('VermilionCity_Gym',  'LT_SURGE', 'LT_SURGE', 'LT_Surge', 'LT. SURGE',
     ["Hey, kid! My troops are charged", "up! Ready for a rematch?"]),
    ('CeladonCity_Gym',    'ERIKA',    'ERIKA',    'Erika',    'ERIKA',
     ["Oh, hello again. My POKéMON have", "grown. Shall we have a rematch?"]),
    ('FuchsiaCity_Gym',    'KOGA',     'KOGA',     'Koga',     'KOGA',
     ["Fwahaha! A ninja never forgets", "a defeat. Will you face me again?"]),
    ('SaffronCity_Gym',    'SABRINA',  'SABRINA',  'Sabrina',  'SABRINA',
     ["I foresaw you coming back.", "Do you want a rematch?"]),
    ('CinnabarIsland_Gym', 'BLAINE',   'BLAINE',   'Blaine',   'BLAINE',
     ["Hah! Back to get burned again?", "How about a rematch?"]),
]

def strings(lines):
    out = []
    for i, l in enumerate(lines):
        end = '$' if i == len(lines) - 1 else ('\\n' if i == 0 else '\\l')
        out.append('\t.string "%s%s"' % (l, end))
    return '\n'.join(out)

for g, F, T, L, NAME, prompt in GYMS:
    p = 'data/maps/%s/scripts.inc' % g
    t = rd(p)
    if '%s_EventScript_LeaderRematchAsk' % g in t:
        continue
    # 1) leader: ask for a rematch once the Champion is beaten
    old = '\tgoto_if_set FLAG_REMATCH_%s, %s_EventScript_LeaderRematch\n' % (F, g)
    assert old in t, g + ' leader hook'
    t = t.replace(old, '\tgoto_if_set FLAG_DEFEATED_CHAMP, %s_EventScript_LeaderRematchAsk\n%s_EventScript_LeaderBattle::\n' % (g, g), 1)
    # 2) replace the old rematch block (from LeaderRematch to the end of the file)
    i = t.index('%s_EventScript_LeaderRematch::' % g)
    tail = t[i:]
    assert tail.rstrip().endswith('end') and '%s_Script_NoRematch::' % g in tail, g + ' tail'
    t = t[:i].rstrip('\n') + '\n\n' + '''@ Rematch (after the Champion): any number of times, one of two teams
{g}_EventScript_LeaderRematchAsk::
	lock
	faceplayer
	msgbox {g}_Text_LeaderRematchAsk, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, {g}_EventScript_LeaderBattle
	msgbox {g}_Text_RebattleIntro
	random 2
	goto_if_eq VAR_RESULT, 1, {g}_Script_{L}_RematchTeamOption2
{g}_Script_{L}_RematchTeamOption1::
	cleartrainerflag TRAINER_LEADER_{T}REMATCHA
	trainerbattle_no_intro TRAINER_LEADER_{T}REMATCHA, {g}_Text_RebattleDeafeated
	goto {g}_EventScript_RematchWon
	end
{g}_Script_{L}_RematchTeamOption2::
	cleartrainerflag TRAINER_LEADER_{T}REMATCHB
	trainerbattle_no_intro TRAINER_LEADER_{T}REMATCHB, {g}_Text_RebattleDeafeated
	goto {g}_EventScript_RematchWon
	end

{g}_EventScript_RematchWon::
	msgbox {g}_Text_{L}_postrematch
	release
	end

@ Rematch NPC: explains when rematches open up
{g}_Text_RematchCheckUnlocked::
	lock
	faceplayer
	goto_if_set FLAG_DEFEATED_CHAMP, {g}_Script_RematchHelperAfter
	msgbox {g}_Text_RematchNotUnlocked
	release
	end
{g}_Script_RematchHelperAfter::
	msgbox {g}_Text_RematchHelperAfter
	release
	end

{g}_Text_LeaderRematchAsk::
{prompt}

{g}_Text_RematchHelperAfter::
	.string "Hey, champ! {NAME} takes on\\n"
	.string "rematches now. Just ask!$"
'''.format(g=g, L=L, T=T, NAME=NAME, prompt=strings(prompt))
    wr(p, t)
print('kanto gym rematch ok')
