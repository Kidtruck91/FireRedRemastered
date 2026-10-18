# Run from the repo root, after orange_pc.py.
# After beating Champion Drake, every PC can switch between the Kanto boxes and the Orange Islands
# boxes ("Which boxes?" when opening POKEMON STORAGE). The party stays with the player, so
# Pokemon can be carried from one set of boxes to the other. Only one set fits in RAM, so the
# other is kept in its flash sectors, exactly like during the trip.
#   FLAG_PC_SHOWS_ORANGE: the RAM PC is the Orange PC (box count, which copy to store on switch).
import re

def rd(p): return open(p, encoding='utf-8').read()
def wr(p, s): open(p, 'w', newline='\n', encoding='utf-8').write(s)
def sub1(t, old, new, label):
    assert old in t, label
    return t.replace(old, new, 1)

p = 'include/constants/flags.h'; t = rd(p)
if 'FLAG_PC_SHOWS_ORANGE' not in t:
    t = sub1(t, '#define FLAG_0x20B               0x20B', '#define FLAG_PC_SHOWS_ORANGE     0x20B // the PC in RAM is the Orange Islands PC', 'flag')
    wr(p, t)

# LoadStoredPC: a part with data == NULL is skipped (e.g. load only the boxes, not the team)
p = 'src/save.c'; t = rd(p)
if 'parts[i].data == NULL' not in t:
    t = sub1(t, '''            if (toParts)
                memcpy(p, buf, n);''', '''            if (toParts)
            {
                if (parts[i].data != NULL) // parts[i].data == NULL: skip this part
                    memcpy(p, buf, n);
            }''', 'skip')
    t = sub1(t, '            u8 *p = (u8 *)parts[i].data + (offset - start);\n',
             '            u8 *p = (u8 *)parts[i].data + (offset - start); // unused when data is NULL\n', 'p')
    wr(p, t)

# ---------------------------------------------------------------- orange_islands.c
p = 'src/orange_islands.c'; t = rd(p)
if 'OrangePC_ShowBoxSet' not in t:
    start = t.index('u8 GetActiveBoxCount(void)')
    end = t.index('// ---------------------------------------------------------------------------\n// Gym rules')
    NEW = r'''u8 GetActiveBoxCount(void)
{
    return FlagGet(FLAG_PC_SHOWS_ORANGE) ? ORANGE_BOXES_COUNT : TOTAL_BOXES_COUNT;
}

// `team` is the team stored with that PC (NULL: leave it out when loading).
static void GetKantoParts(struct StoredPCPart *parts, struct Pokemon *team)
{
    parts[0].data = gPokemonStoragePtr;
    parts[0].size = sizeof(struct PokemonStorage);
    parts[1].data = team;
    parts[1].size = sizeof(gPlayerParty);
}

static void GetOrangeParts(struct StoredPCPart *parts, struct Pokemon *team)
{
    parts[0].data = gPokemonStoragePtr;
    parts[0].size = ORANGE_PC_HEAD_BYTES;
    parts[1].data = gPokemonStoragePtr->boxNames;
    parts[1].size = sizeof(gPokemonStoragePtr->boxNames[0]) * ORANGE_BOXES_COUNT;
    parts[2].data = gPokemonStoragePtr->boxWallpapers;
    parts[2].size = ORANGE_BOXES_COUNT;
    parts[3].data = team;
    parts[3].size = sizeof(gPlayerParty);
}

static void ResetOrangePC(void)
{
    u8 box, pos;

    gPokemonStoragePtr->currentBox = 0;
    for (box = 0; box < ORANGE_BOXES_COUNT; box++)
    {
        u8 *name = StringCopy(gPokemonStoragePtr->boxNames[box], sText_Orange);
        ConvertIntToDecimalStringN(name, box + 1, STR_CONV_MODE_LEFT_ALIGN, 1);
        gPokemonStoragePtr->boxWallpapers[box] = WALLPAPER_BEACH;
        for (pos = 0; pos < IN_BOX_COUNT; pos++)
            ZeroBoxMonData(&gPokemonStoragePtr->boxes[box][pos]);
    }
}

// After loading the Orange PC into RAM: the boxes past ORANGE_BOXES_COUNT hold leftovers.
static void TidyOrangePC(void)
{
    u8 box, pos;

    for (box = ORANGE_BOXES_COUNT; box < TOTAL_BOXES_COUNT; box++)
        for (pos = 0; pos < IN_BOX_COUNT; pos++)
            ZeroBoxMonData(&gPokemonStoragePtr->boxes[box][pos]);
    if (gPokemonStoragePtr->currentBox >= ORANGE_BOXES_COUNT)
        gPokemonStoragePtr->currentBox = 0;
}

static void AfterPartyChange(void)
{
    CompactPartySlots();
    CalculatePlayerPartyCount();
    if (gPlayerPartyCount == 0)
        DestroyFollower();
    else
        UpdateFollowerPokemonGraphic();
}

// Stores the PC that is in RAM. The team kept with that stored copy (the home team with the
// Kanto PC during the trip, the Orange team with the Orange PC at home) is carried over as is;
// otherwise the slot is unused and gets the current party as filler.
static bool8 StoreShownPC(void)
{
    struct StoredPCPart parts[4];
    bool8 orange = FlagGet(FLAG_PC_SHOWS_ORANGE);
    bool8 keepTeam = orange ? !FlagGet(FLAG_ORANGE_CHALLENGE) : FlagGet(FLAG_ORANGE_CHALLENGE);
    struct Pokemon *team = gPlayerParty;

    if (keepTeam && (!orange || FlagGet(FLAG_ORANGE_PC_SAVED)))
    {
        // gEnemyParty is free outside battle
        if (orange)
        {
            GetOrangeParts(parts, gEnemyParty);
            parts[0].data = parts[1].data = parts[2].data = NULL;
            if (!LoadStoredPC(STORED_PC_ORANGE, parts, 4))
                return FALSE;
        }
        else
        {
            GetKantoParts(parts, gEnemyParty);
            parts[0].data = NULL;
            if (!LoadStoredPC(STORED_PC_KANTO, parts, 2))
                return FALSE;
        }
        team = gEnemyParty;
    }
    if (orange)
    {
        GetOrangeParts(parts, team);
        if (!SaveStoredPC(STORED_PC_ORANGE, parts, 4))
            return FALSE;
        FlagSet(FLAG_ORANGE_PC_SAVED);
    }
    else
    {
        GetKantoParts(parts, team);
        if (!SaveStoredPC(STORED_PC_KANTO, parts, 2))
            return FALSE;
    }
    return TRUE;
}

// Puts the Kanto (FALSE) or Orange (TRUE) boxes in RAM. The party is not touched.
static bool8 ShowBoxSet(bool8 orange)
{
    struct StoredPCPart parts[4];

    if (FlagGet(FLAG_PC_SHOWS_ORANGE) == orange)
        return TRUE;
    if (!StoreShownPC())
        return FALSE;
    if (orange)
    {
        GetOrangeParts(parts, NULL);
        if (!FlagGet(FLAG_ORANGE_PC_SAVED) || !LoadStoredPC(STORED_PC_ORANGE, parts, 4))
            ResetOrangePC();
        TidyOrangePC();
        FlagSet(FLAG_PC_SHOWS_ORANGE);
    }
    else
    {
        GetKantoParts(parts, NULL);
        if (!LoadStoredPC(STORED_PC_KANTO, parts, 2))
            return FALSE; // RAM still holds the Orange PC, nothing lost
        FlagClear(FLAG_PC_SHOWS_ORANGE);
    }
    return TRUE;
}

// After beating Drake. VAR_0x8004: 0 = Kanto boxes, 1 = Orange boxes. VAR_RESULT: TRUE ok.
void OrangePC_ShowBoxSet(void)
{
    gSpecialVar_Result = ShowBoxSet(gSpecialVar_0x8004 != 0);
}

void OrangeChallenge_Begin(void)
{
    struct StoredPCPart parts[4];
    u8 i;

    gSpecialVar_Result = FALSE;
    if (FlagGet(FLAG_ORANGE_CHALLENGE))
        return;
    if (!ShowBoxSet(FALSE)) // the Kanto boxes may be switched out (after Drake)
        return;
    GetKantoParts(parts, gPlayerParty);
    if (!SaveStoredPC(STORED_PC_KANTO, parts, 2))
        return;

    // Kanto PC + home team are safe in flash: bring in the Orange PC + Orange team.
    GetOrangeParts(parts, gPlayerParty);
    if (!FlagGet(FLAG_ORANGE_PC_SAVED) || !LoadStoredPC(STORED_PC_ORANGE, parts, 4))
    {
        // First trip: a fresh Orange PC and no team (Prof. Ivy gives a starter).
        ResetOrangePC();
        for (i = 0; i < PARTY_SIZE; i++)
            ZeroMonData(&gPlayerParty[i]);
    }
    TidyOrangePC();
    FlagSet(FLAG_PC_SHOWS_ORANGE);
    FlagSet(FLAG_ORANGE_CHALLENGE);
    AfterPartyChange();
    gSpecialVar_Result = TRUE;
}

void OrangeChallenge_End(void)
{
    struct StoredPCPart parts[4];

    gSpecialVar_Result = FALSE;
    if (!FlagGet(FLAG_ORANGE_CHALLENGE))
        return;
    if (!ShowBoxSet(TRUE)) // the Orange boxes may be switched out (after Drake)
        return;
    GetOrangeParts(parts, gPlayerParty);
    if (!SaveStoredPC(STORED_PC_ORANGE, parts, 4))
        return;
    FlagSet(FLAG_ORANGE_PC_SAVED);

    // Only now replace RAM; a damaged Kanto copy leaves everything as it was.
    GetKantoParts(parts, gPlayerParty);
    if (!LoadStoredPC(STORED_PC_KANTO, parts, 2))
        return;

    FlagClear(FLAG_PC_SHOWS_ORANGE);
    FlagClear(FLAG_ORANGE_CHALLENGE);
    AfterPartyChange();
    gSpecialVar_Result = TRUE;
}

'''
    t = t[:start] + NEW + t[end:]
    t = sub1(t, '//   callnative OrangeChallenge_End     @ VAR_RESULT: TRUE ok, FALSE flash error (nothing changed)\n',
             '//   callnative OrangeChallenge_End     @ VAR_RESULT: TRUE ok, FALSE flash error (nothing changed)\n'
             '//   setvar VAR_0x8004, 0/1  callnative OrangePC_ShowBoxSet  @ after Drake: Kanto/Orange boxes in any PC\n', 'doc')
    wr(p, t)

# ---------------------------------------------------------------- "Which boxes?" menu
p = 'include/constants/menu.h'; t = rd(p)
if 'MULTICHOICE_PC_BOX_SET' not in t:
    t = sub1(t, '#define MULTICHOICE_TRAINER_TOWER_MODE                         64\n',
             '#define MULTICHOICE_TRAINER_TOWER_MODE                         64\n'
             '#define MULTICHOICE_PC_BOX_SET                                 65 // KANTO / ORANGE ISLES\n', 'mc')
    wr(p, t)
p = 'src/script_menu.c'; t = rd(p)
if 'sMultichoiceList_PCBoxSet' not in t:
    t = sub1(t, 'static const struct MultichoiceListStruct sMultichoiceLists[] = {\n',
             'static const u8 sText_KantoBoxes[] = _("KANTO");\n'
             'static const u8 sText_OrangeBoxes[] = _("ORANGE ISLES");\n'
             'static const struct MenuAction sMultichoiceList_PCBoxSet[] = {\n'
             '    { sText_KantoBoxes },\n'
             '    { sText_OrangeBoxes }\n'
             '};\n\n'
             'static const struct MultichoiceListStruct sMultichoiceLists[] = {\n', 'list')
    t = sub1(t, '    [MULTICHOICE_TRAINER_TOWER_MODE]                         = MULTICHOICE(sMultichoiceList_TrainerTowerMode),\n',
             '    [MULTICHOICE_TRAINER_TOWER_MODE]                         = MULTICHOICE(sMultichoiceList_TrainerTowerMode),\n'
             '    [MULTICHOICE_PC_BOX_SET]                                 = MULTICHOICE(sMultichoiceList_PCBoxSet),\n', 'entry')
    wr(p, t)

p = 'data/scripts/pc.inc'; t = rd(p)
if 'EventScript_PCChooseBoxSet' not in t:
    t = sub1(t, '''EventScript_AccessPokemonStorage::
	playse SE_PC_LOGIN
''', '''EventScript_AccessPokemonStorage::
	playse SE_PC_LOGIN
	goto_if_defeated TRAINER_CHAMPION_DRAKE, EventScript_PCChooseBoxSet
EventScript_OpenPokemonStorage::
''', 'access')
    t = t.rstrip('\n') + '''

@ After beating Drake: open the KANTO or the ORANGE ISLANDS boxes. The party stays with the
@ player, so POKeMON can be carried from one set of boxes to the other.
EventScript_PCChooseBoxSet::
	message Text_PCWhichBoxes
	waitmessage
	multichoice 16, 7, MULTICHOICE_PC_BOX_SET, FALSE
	goto_if_eq VAR_RESULT, SCR_MENU_CANCEL, EventScript_PCMainMenu
	copyvar VAR_0x8004, VAR_RESULT
	callnative OrangePC_ShowBoxSet
	goto_if_eq VAR_RESULT, FALSE, EventScript_PCBoxSetFailed
	goto EventScript_OpenPokemonStorage
	end

EventScript_PCBoxSetFailed::
	msgbox Text_PCBoxSetFailed
	goto EventScript_PCMainMenu
	end

Text_PCWhichBoxes:
	.string "Which boxes?$"

Text_PCBoxSetFailed:
	.string "The link to those boxes failed.$"
'''
    wr(p, t)
print('orange pc merge ok')
