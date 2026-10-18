# Run from the repo root, after badge_pages.py.
# Johto badge flags FLAG_BADGE_ZEPHYR .. FLAG_BADGE_RISING (0x212-0x219).
#  - Set automatically when the player wins against that gym's leader (any of the A/B/C teams;
#    rematch teams don't count), in CB2_EndTrainerBattle (src/battle_setup.c). The gym scripts
#    don't need to change, but can also `setflag FLAG_BADGE_ZEPHYR` etc. themselves.
#  - The trainer card's Johto page reads these flags (a leader beaten on an older save still
#    counts, so old saves keep their badges).
def rd(p): return open(p, encoding='utf-8').read()
def wr(p, s): open(p, 'w', newline='\n', encoding='utf-8').write(s)
def sub1(t, old, new, label):
    assert old in t, label
    return t.replace(old, new, 1)

NAMES = ['ZEPHYR', 'HIVE', 'PLAIN', 'FOG', 'STORM', 'MINERAL', 'GLACIER', 'RISING']
GYMS = ['Falkner/Walker, Violet', 'Bugsy, Azalea', 'Whitney, Goldenrod', 'Morty, Ecruteak',
        'Chuck, Cianwood', 'Jasmine, Olivine', 'Pryce, Mahogany', 'Clair, Blackthorn']
LEADERS = ['WALKER', 'BUGSY', 'WHITNEY', 'MORTY', 'CHUCK', 'JASMINE', 'PRYCE', 'CLAIR']

# ---------------------------------------------------------------- flags
p = 'include/constants/flags.h'; t = rd(p)
for i, n in enumerate(NAMES):
    num = 0x212 + i
    old = '#define FLAG_0x%03X               0x%03X' % (num, num)
    new = '#define FLAG_BADGE_%-18s 0x%03X // Johto badge %d (%s)' % (n, num, i + 1, GYMS[i])
    if 'FLAG_BADGE_%s ' % n not in t:
        t = sub1(t, old, new, old)
wr(p, t)

# ---------------------------------------------------------------- set on a win
p = 'src/battle_setup.c'; t = rd(p)
if 'GiveJohtoBadgeForLeader' not in t:
    rows = '\n'.join('    {FLAG_BADGE_%s, {TRAINER_LEADER_%sA, TRAINER_LEADER_%sB, TRAINER_LEADER_%sC}},'
                     % (n, l, l, l) for n, l in zip(NAMES, LEADERS))
    t = sub1(t, 'static void CB2_EndTrainerBattle(void)\n{\n', '''// Johto gyms: beating the leader (any of the gym's three teams) gives the badge.
static const struct {
    u16 flag;
    u16 leaders[3];
} sJohtoBadges[] = {
%s
};

static void GiveJohtoBadgeForLeader(u16 trainerId)
{
    u32 i, j;

    for (i = 0; i < NELEMS(sJohtoBadges); i++)
        for (j = 0; j < NELEMS(sJohtoBadges[i].leaders); j++)
            if (sJohtoBadges[i].leaders[j] == trainerId)
                FlagSet(sJohtoBadges[i].flag);
}

static void CB2_EndTrainerBattle(void)
{
    if (IsPlayerDefeated(gBattleOutcome) != TRUE)
        GiveJohtoBadgeForLeader(gTrainerBattleOpponent_A);
''' % rows, 'battle_setup')
    if '#include "constants/opponents.h"' not in t:
        t = sub1(t, '#include "global.h"\n', '#include "global.h"\n#include "constants/opponents.h"\n', 'inc')
    wr(p, t)

# ---------------------------------------------------------------- trainer card: read the flags
p = 'src/trainer_card.c'; t = rd(p)
if 'sJohtoBadgeFlags' not in t:
    t = sub1(t, '''static const u16 sOrangeBadgeFlags[4] = {''', '''static const u16 sJohtoBadgeFlags[8] = {
    FLAG_BADGE_ZEPHYR, FLAG_BADGE_HIVE, FLAG_BADGE_PLAIN, FLAG_BADGE_FOG,
    FLAG_BADGE_STORM, FLAG_BADGE_MINERAL, FLAG_BADGE_GLACIER, FLAG_BADGE_RISING,
};

static const u16 sOrangeBadgeFlags[4] = {''', 'card table')
    t = sub1(t, '''    case BADGE_PAGE_JOHTO:
        for (j = 0; j < 3; j++)''', '''    case BADGE_PAGE_JOHTO:
        if (FlagGet(sJohtoBadgeFlags[i]))
            return TRUE;
        for (j = 0; j < 3; j++) // saves from before the badge flags: the leader beaten counts''', 'card read')
    t = t.replace('// Johto: a badge = that gym\'s leader beaten (the gyms pick one of three teams)',
                  '// Johto: FLAG_BADGE_ZEPHYR.., set when that gym\'s leader is beaten (any of three teams)')
    wr(p, t)
print('johto badge flags ok')
