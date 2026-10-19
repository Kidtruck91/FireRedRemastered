# Run from the repo root, after orange_drake_battle.py.
# Hidden abilities for trainer Pokemon: .ability = TRAINER_MON_HIDDEN_ABILITY (4) gives the
# species' hidden ability (when it has one). Pools keep 0-3 as before.
# Johto and Orange leaders (and Drake) get it where it fits the set:
#   Alakazam Magic Guard, Charizard Solar Power (Sunny Day set), Ditto Imposter,
#   Dragonite Multiscale, Heracross Moxie, Kangaskhan Inner Focus (Fake Out), Magneton Analytic,
#   Piloswine Thick Fat, Primeape Defiant, Quagsire Unaware, Steelix Sheer Force,
#   Voltorb Aftermath, Wartortle Rain Dish (Cissy's rain), Xatu Magic Bounce
# Rudy's counter pool: Crawdaunt Adaptability, Kingler Sheer Force, Ninetales Drought,
#   Xatu Magic Bounce.
# Nurse Joy's Chansey all know Memento (instead of Fake Tears).
import re
def rd(p): return open(p, encoding='utf-8').read()
def wr(p, s): open(p, 'w', newline='\n', encoding='utf-8').write(s)
def sub1(s, old, new, p):
    assert old in s, (p, old[:70]); return s.replace(old, new, 1)

p = 'include/pokemon.h'; s = rd(p)
if 'TRAINER_MON_HIDDEN_ABILITY' not in s:
    s = sub1(s, '#define ABILITY_NUM_HIDDEN 2\n', '#define ABILITY_NUM_HIDDEN 2\n'
             '// .ability on a trainer Pokemon (parties and pools): the species\' hidden ability\n'
             '#define TRAINER_MON_HIDDEN_ABILITY 4\n', p); wr(p, s)

p = 'src/battle_main.c'; s = rd(p)
if 'TRAINER_MON_HIDDEN_ABILITY' not in s:
    s = sub1(s, '''// .ability on a trainer Pokemon: 1 = first ability, 2 = second (if the species has one).
// 0 or 3 = either: keep whatever CreateMon picked. (Hidden abilities are wild-only.)''',
'''// .ability on a trainer Pokemon: 1 = first ability, 2 = second (if the species has one),
// TRAINER_MON_HIDDEN_ABILITY = hidden ability (if the species has one).
// 0 or 3 = either: keep whatever CreateMon picked.''', p)
    s = sub1(s, '''    else if (abilitySlot == 2 && gSpeciesInfo[species].abilities[1] != ABILITY_NONE)
        abilityNum = 1;
    else
        return;''', '''    else if (abilitySlot == 2 && gSpeciesInfo[species].abilities[1] != ABILITY_NONE)
        abilityNum = 1;
    else if (abilitySlot == TRAINER_MON_HIDDEN_ABILITY && gSpeciesInfo[species].hiddenAbility != ABILITY_NONE)
        abilityNum = ABILITY_NUM_HIDDEN;
    else
        return;''', p)
    wr(p, s)

p = 'include/trainer_pools.h'; s = rd(p)
if 'TRAINER_MON_HIDDEN_ABILITY' not in s:
    s = sub1(s, '// .ability: 0 or 3 = either, 1 = the species\' first ability, 2 = its second ability',
             '// .ability: 0 or 3 = either, 1 = the species\' first ability, 2 = its second ability,\n'
             '//           TRAINER_MON_HIDDEN_ABILITY (4) = its hidden ability', p); wr(p, s)

p = 'src/orange_islands.c'; s = rd(p)
if 'TRAINER_MON_HIDDEN_ABILITY' not in s:
    s = sub1(s, '''static u8 RudyPoolAbility(const struct PoolMon *poolMon)
{
''', '''static u8 RudyPoolAbility(const struct PoolMon *poolMon)
{
    if (poolMon->ability == TRAINER_MON_HIDDEN_ABILITY && gSpeciesInfo[poolMon->species].hiddenAbility != ABILITY_NONE)
        return gSpeciesInfo[poolMon->species].hiddenAbility;
''', p)
    s = sub1(s, '''        u8 iv = poolMon->iv * MAX_PER_STAT_IVS / 255;
''', '''        u8 iv = poolMon->iv * MAX_PER_STAT_IVS / 255;

        if (poolMon->ability == TRAINER_MON_HIDDEN_ABILITY && gSpeciesInfo[poolMon->species].hiddenAbility != ABILITY_NONE)
            abilityNum = ABILITY_NUM_HIDDEN;
''', p)
    for sp in ('CRAWDAUNT', 'KINGLER', 'NINETALES', 'XATU'):
        s, n = re.subn(r'(\{ \.species = SPECIES_%s, [^}]*?\.ability = )\d' % sp, r'\1TRAINER_MON_HIDDEN_ABILITY', s); assert n == 1, sp
    wr(p, s)

# leader parties
HA_ALWAYS = {'ALAKAZAM', 'DITTO', 'DRAGONITE', 'HERACROSS', 'KANGASKHAN', 'MAGNETON', 'PILOSWINE',
             'PRIMEAPE', 'QUAGSIRE', 'STEELIX', 'VOLTORB', 'WARTORTLE', 'XATU'}
HA_IF_MOVE = {'CHARIZARD': 'MOVE_SUNNY_DAY'}
LEADERS = re.compile(r'TRAINER_(LEADER_(WALKER|BUGSY|WHITNEY|MORTY|CHUCK|JASMINE|PRYCE|CLAIR)\w*|LEADER_(CISSY|DANNY|RUDY|LUANA)|CHAMPION_DRAKE\w*)$')
tr = rd('src/data/trainers.h')
parties = set()
for m in re.finditer(r'\[(TRAINER_\w+)\] = \{(.*?)\n    \},', tr, re.S):
    if LEADERS.match(m.group(1)):
        parties.add(re.search(r'\.party = \w+\((\w+)\)', m.group(2)).group(1))
p = 'src/data/trainer_parties.h'; s = rd(p)
count = 0
for party in sorted(parties):
    i = s.index('%s[] = {' % party); j = s.index('\n};', i)
    body = s[i:j]
    def fix(mm):
        global count
        e = mm.group(0)
        sp = re.search(r'SPECIES_(\w+)', e).group(1)
        if not (sp in HA_ALWAYS or (sp in HA_IF_MOVE and HA_IF_MOVE[sp] in e)):
            return e
        count += 1
        if re.search(r'\.ability\s*=', e):
            return re.sub(r'\.ability\s*=\s*\w+', '.ability = TRAINER_MON_HIDDEN_ABILITY', e)
        return re.sub(r'(\n(\s*)\.species\s*=\s*SPECIES_\w+,)', r'\1\n\2.ability = TRAINER_MON_HIDDEN_ABILITY,', e, count=1)
    body = re.sub(r'\{[^{}]*?\.species\s*=\s*SPECIES_\w+[^{}]*?(?:\{[^{}]*\}[^{}]*?)?\}', fix, body)
    s = s[:i] + body + s[j:]
# Nurse Joy's Chansey: Memento
i = s.index('sParty_NurseJoy[] = {'); j = s.index('\n};', i)
s = s[:i] + s[i:j].replace('MOVE_FAKE_TEARS', 'MOVE_MEMENTO') + s[j:]
wr(p, s)
print('hidden abilities ok: %d leader Pokemon' % count)
