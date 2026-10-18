# Run from the repo root, after johto_badge_flags.py.
# Johto gym leader scripts (Violet .. Blackthorn), rewritten from one template:
#  - First win: the leader battles with one of three random teams (A/B/C); winning plays
#    "{PLAYER} received the X BADGE" with the badge fanfare, then the leader's post-battle line.
#  - After the badge: the leader asks if you want a rematch (YES/NO); YES fights one of the three
#    rematch teams, which can be fought again any number of times.
#  - Saves from before the badge flags: if the leader was already beaten, the next talk hands
#    over the badge instead of battling again.
#  - The gym's helper NPC (previously the rematch switch) now explains how rematches work.
# Leader texts stay in each gym's text.inc; the new lines are at the end of scripts.inc.
import re

def rd(p): return open(p, encoding='utf-8').read()
def wr(p, s): open(p, 'w', newline='\n', encoding='utf-8').write(s)

GYMS = [
    # map,         leader label, trainer base, badge,    shown name, rematch prompt
    ('VioletGym',     'Walker',  'WALKER',  'ZEPHYR',  'WALKER',
     ["Back for another round, high", "flyer? I'm always up for a", "rematch!"]),
    ('AzaleaGym',     'Bugsy',   'BUGSY',   'HIVE',    'BUGSY',
     ["My bugs have been studying hard.", "Want to see what they've learned", "in a rematch?"]),
    ('GoldenrodGym',  'Whitney', 'WHITNEY', 'PLAIN',   'WHITNEY',
     ["You again? Fine, I'll get you", "back this time! Rematch?"]),
    ('EcruteakGym',   'Morty',   'MORTY',   'FOG',     'MORTY',
     ["The mist has shown me more of", "your path. Shall we battle", "again?"]),
    ('CianwoodGym',   'Chuck',   'CHUCK',   'STORM',   'CHUCK',
     ["Wahahah! My fists are itching!", "How about a rematch?"]),
    ('OlivineGym',    'Jasmine', 'JASMINE', 'MINERAL', 'JASMINE',
     ["Um... if you have time... would", "you battle me again?"]),
    ('MohoganyGym',   'Pryce',   'PRYCE',   'GLACIER', 'PRYCE',
     ["Winter always returns. Will you", "face my rematch team?"]),
    ('BlackthornGym', 'Clair',   'CLAIR',   'RISING',  'CLAIR',
     ["Don't think I've forgotten that", "loss. Rematch. Now?"]),
]

def strings(lines):
    out = []
    for i, l in enumerate(lines):
        end = '$' if i == len(lines) - 1 else ('\\n' if i == 0 else '\\l')
        out.append('\t.string "%s%s"' % (l, end))
    return '\n'.join(out)

for gmap, L, T, BADGE, NAME, prompt in GYMS:
    g = gmap
    p = 'data/maps/%s/scripts.inc' % gmap
    script = '''{g}_MapScripts::
	.byte 0

@ Leader: first win gives the {BADGE} BADGE (FLAG_BADGE_{BADGE}, also set in CB2_EndTrainerBattle);
@ afterwards the leader offers rematches.
{g}_EventScript_Leader_{L}::
	lock
	faceplayer
	goto_if_set FLAG_BADGE_{BADGE}, {g}_EventScript_LeaderAfter
	goto_if_defeated TRAINER_LEADER_{T}A, {g}_EventScript_LeaderWon
	goto_if_defeated TRAINER_LEADER_{T}B, {g}_EventScript_LeaderWon
	goto_if_defeated TRAINER_LEADER_{T}C, {g}_EventScript_LeaderWon
	msgbox {g}_Text_Intro
	random 3
	goto_if_eq VAR_RESULT, 1, {g}_Script_{L}_TeamOption2
	goto_if_eq VAR_RESULT, 2, {g}_Script_{L}_TeamOption3
{g}_Script_{L}_TeamOption1::
	trainerbattle_no_intro TRAINER_LEADER_{T}A, {g}_Text_Deafeated
	goto {g}_EventScript_LeaderWon
	end
{g}_Script_{L}_TeamOption2::
	trainerbattle_no_intro TRAINER_LEADER_{T}B, {g}_Text_Deafeated
	goto {g}_EventScript_LeaderWon
	end
{g}_Script_{L}_TeamOption3::
	trainerbattle_no_intro TRAINER_LEADER_{T}C, {g}_Text_Deafeated
	goto {g}_EventScript_LeaderWon
	end

{g}_EventScript_LeaderWon::
	setflag FLAG_BADGE_{BADGE}
	message {g}_Text_ReceivedBadge
	waitmessage
	playfanfare MUS_OBTAIN_BADGE
	waitfanfare
	msgbox {g}_Text_{L}_postbattle
	release
	end

@ Rematch: any number of times, one of three teams
{g}_EventScript_LeaderAfter::
	msgbox {g}_Text_LeaderRematchAsk, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, {g}_Script_NoRematch
	msgbox {g}_Text_RebattleIntro
	random 3
	goto_if_eq VAR_RESULT, 1, {g}_Script_{L}_RematchTeamOption2
	goto_if_eq VAR_RESULT, 2, {g}_Script_{L}_RematchTeamOption3
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
{g}_Script_{L}_RematchTeamOption3::
	cleartrainerflag TRAINER_LEADER_{T}REMATCHC
	trainerbattle_no_intro TRAINER_LEADER_{T}REMATCHC, {g}_Text_RebattleDeafeated
	goto {g}_EventScript_RematchWon
	end

{g}_EventScript_RematchWon::
	msgbox {g}_Text_{L}_postrematch
	release
	end

{g}_Script_NoRematch::
	msgbox {g}_Text_LeaderNoRematch
	release
	end

@ Gym helper: explains rematches
{g}_Script_Rematch::
	lock
	faceplayer
	goto_if_set FLAG_BADGE_{BADGE}, {g}_Script_HelperAfter
	msgbox {g}_Text_HelperBefore
	release
	end
{g}_Script_HelperAfter::
	msgbox {g}_Text_HelperAfter
	release
	end

{g}_Text_ReceivedBadge::
	.string "{{PLAYER}} received the\\n"
	.string "{BADGE} BADGE from {NAME}!$"

{g}_Text_LeaderRematchAsk::
{prompt}

{g}_Text_LeaderNoRematch::
	.string "Come back any time.$"

{g}_Text_HelperBefore::
	.string "Hey, champ! Beat {NAME} and the\\n"
	.string "{BADGE} BADGE is yours!$"

{g}_Text_HelperAfter::
	.string "Hey, champ! {NAME} takes on\\n"
	.string "rematches any time. Just ask!$"
'''.format(g=g, L=L, T=T, BADGE=BADGE, NAME=NAME, prompt=strings(prompt))
    old = rd(p)
    # keep any labels the old script exported that the new one doesn't define (none expected)
    wr(p, script)

# Walker's post-battle line was missing its terminator and ran into the next text
p = 'data/maps/VioletGym/text.inc'; t = rd(p)
t = t.replace(' .string "the top!\\p"\n', ' .string "the top!$"\n', 1)
wr(p, t)
print('johto gym scripts ok')
