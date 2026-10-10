# Nuzlocke rules hooks (fainted = dead, encounter reminder)
import sys, subprocess
FILES=['include/global.h','src/script_pokemon_util.c','src/pokemon.c','src/battle_main.c','src/field_poison.c',
       'src/new_game.c','src/option_menu.c','src/item_use.c','data/scripts/white_out.inc','src/strings.c','include/strings.h']
subprocess.run(['git','checkout','--']+FILES, check=True)
def patch(path, pairs):
    s=open(path).read(); crlf='\r\n' in s; s=s.replace('\r\n','\n')
    for old,new in pairs:
        n=s.count(old)
        if n!=1: sys.exit(f'{path}: expected 1 match, got {n} for:\n{old}')
        s=s.replace(old,new)
    if crlf: s=s.replace('\n','\r\n')
    open(path,'w').write(s)
def add_include(path, after='#include "global.h"\n', inc='nuzlocke.h'):
    patch(path, [(after, after+f'#include "{inc}"\n')])

patch('include/global.h', [(
"""    /*0xB20*/ struct Follower follower;
    /*0xB38*/ u8 filler_B38[0x400 - sizeof(struct Follower)];""",
"""    /*0xB20*/ struct Follower follower;
    /*0xB38*/ u8 nuzlockeAreas[32]; // one bit per map section: its Nuzlocke encounter was used (src/nuzlocke.c)
    /*0xB58*/ u8 filler_B58[0x400 - sizeof(struct Follower) - 32];""")])

add_include('src/script_pokemon_util.c')
patch('src/script_pokemon_util.c', [(
"""    for(i = 0; i < gPlayerPartyCount; i++)
    {
        u16 maxHP = GetMonData(&gPlayerParty[i], MON_DATA_MAX_HP);""",
"""    for(i = 0; i < gPlayerPartyCount; i++)
    {
        u16 maxHP = GetMonData(&gPlayerParty[i], MON_DATA_MAX_HP);
        if (Nuzlocke_IsMonDead(&gPlayerParty[i]))
            continue; // Nuzlocke: dead Pokemon can't be healed""")])

add_include('src/pokemon.c')
patch('src/pokemon.c', [
("""        else
            return;
    }

    SetMonData(mon, MON_DATA_HP, &currentHP);
}""",
"""        else
            return;
    }

    if (Nuzlocke_IsMonDead(mon))
        currentHP = 0; // Nuzlocke: dead Pokemon stay fainted (e.g. when taken out of the PC)
    SetMonData(mon, MON_DATA_HP, &currentHP);
}"""),
("""bool8 ExecuteTableBasedItemEffect(struct Pokemon *mon, u16 item, u8 partyIndex, u8 moveIndex)
{
    return""",
"""bool8 ExecuteTableBasedItemEffect(struct Pokemon *mon, u16 item, u8 partyIndex, u8 moveIndex)
{
    if (Nuzlocke_ItemBlocked(mon, item))
        return TRUE; // no effect
    return"""),
("""    u8 curEffect;
    u32 curMoveId;

    // Get item hold effect
    heldItem = GetMonData(mon, MON_DATA_HELD_ITEM, NULL);""",
"""    u8 curEffect;
    u32 curMoveId;

    if (Nuzlocke_ItemBlocked(mon, item))
        return TRUE; // Nuzlocke: no effect on dead Pokemon / no reviving

    // Get item hold effect
    heldItem = GetMonData(mon, MON_DATA_HELD_ITEM, NULL);"""),
])

add_include('src/battle_main.c')
patch('src/battle_main.c', [(
"""        BeginFastPaletteFade(3);
        FadeOutMapMusic(5);
        gBattleMainFunc = FreeResetData_ReturnToOvOrDoEvolutions;""",
"""        Nuzlocke_OnBattleEnd();
        BeginFastPaletteFade(3);
        FadeOutMapMusic(5);
        gBattleMainFunc = FreeResetData_ReturnToOvOrDoEvolutions;""")])

add_include('src/field_poison.c')
patch('src/field_poison.c', [(
"""    AdjustFriendship(pokemon, FRIENDSHIP_EVENT_FAINT_OUTSIDE_BATTLE);
    SetMonData(pokemon, MON_DATA_STATUS, &status);""",
"""    AdjustFriendship(pokemon, FRIENDSHIP_EVENT_FAINT_OUTSIDE_BATTLE);
    SetMonData(pokemon, MON_DATA_STATUS, &status);
    if (Nuzlocke_IsActive())
        Nuzlocke_SetMonDead(pokemon);""")])

add_include('src/new_game.c')
patch('src/new_game.c', [(
"""    memset(&gSaveBlock2Ptr->follower, 0, sizeof(gSaveBlock2Ptr->follower));""",
"""    memset(&gSaveBlock2Ptr->follower, 0, sizeof(gSaveBlock2Ptr->follower));
    Nuzlocke_ResetAreas();""")])

add_include('src/option_menu.c', inc='event_data.h')
# Options menu: the Nuzlocke option now reads/writes FLAG_NUZLOCKE (option 0 = ON, 1 = OFF)
patch('src/option_menu.c', [
("""    sOptionMenuPtr->option[MENUITEM_NUZLOCKE] = gSaveBlock2Ptr->enableNuzlocke;""",
"""    sOptionMenuPtr->option[MENUITEM_NUZLOCKE] = FlagGet(FLAG_NUZLOCKE) ? 0 : 1;"""),
("""    gSaveBlock2Ptr->enableNuzlocke = sOptionMenuPtr->option[MENUITEM_NUZLOCKE];""",
"""    if (sOptionMenuPtr->option[MENUITEM_NUZLOCKE] == 0)
        FlagSet(FLAG_NUZLOCKE);
    else
        FlagClear(FLAG_NUZLOCKE);
    gSaveBlock2Ptr->enableNuzlocke = FlagGet(FLAG_NUZLOCKE);"""),
])

# Poke Ball reminder in battle
add_include('src/item_use.c')
patch('src/item_use.c', [('#include "nuzlocke.h"\n', '#include "nuzlocke.h"\n#include "bag.h"\n#include "menu_helpers.h"\n')])
patch('src/item_use.c', [(
"""void BattleUseFunc_PokeBallEtc(u8 taskId)
{
    if (!IsPlayerPartyAndPokemonStorageFull())
    {
        RemoveBagItem(gSpecialVar_ItemId, 1);
        Bag_BeginCloseWin0Animation();
        ItemMenu_StartFadeToExitCallback(taskId);
    }
    else
        DisplayItemMessageInBag(taskId, FONT_NORMAL, gText_BoxFull, Task_ReturnToBagFromContextMenu);
}""",
"""static void ThrowPokeBall(u8 taskId)
{
    RemoveBagItem(gSpecialVar_ItemId, 1);
    Bag_BeginCloseWin0Animation();
    ItemMenu_StartFadeToExitCallback(taskId);
}

// Nuzlocke: "You already had your encounter here. Throw it anyway?"
static void Task_NuzlockeThrowAnyway_Yes(u8 taskId)
{
    CloseBagWindow(5);
    ThrowPokeBall(taskId);
}

static void Task_NuzlockeThrowAnyway_No(u8 taskId)
{
    Task_ReturnToBagFromContextMenu(taskId);
}

static const struct YesNoFuncTable sYesNoMenu_NuzlockeThrowAnyway = {
    Task_NuzlockeThrowAnyway_Yes,
    Task_NuzlockeThrowAnyway_No
};

static void Task_NuzlockeAskThrowAnyway(u8 taskId)
{
    BagCreateYesNoMenuBottomRight(taskId, &sYesNoMenu_NuzlockeThrowAnyway);
}

void BattleUseFunc_PokeBallEtc(u8 taskId)
{
    if (!IsPlayerPartyAndPokemonStorageFull())
    {
        if (Nuzlocke_ShouldWarnBeforeCatch())
            DisplayItemMessageInBag(taskId, FONT_NORMAL, gText_NuzlockeAlreadyEncountered, Task_NuzlockeAskThrowAnyway);
        else
            ThrowPokeBall(taskId);
    }
    else
        DisplayItemMessageInBag(taskId, FONT_NORMAL, gText_BoxFull, Task_ReturnToBagFromContextMenu);
}""")])

patch('src/strings.c', [(
"""const u8 gText_NuzlockeMode[] = _("NUZLOCKE MODE");""",
"""const u8 gText_NuzlockeMode[] = _("NUZLOCKE MODE");
const u8 gText_NuzlockeAlreadyEncountered[] = _("Your encounter here is used.\\nThrow it anyway?");""")])
patch('include/strings.h', [(
"""extern const u8 gText_NuzlockeMode[];""",
"""extern const u8 gText_NuzlockeMode[];
extern const u8 gText_NuzlockeAlreadyEncountered[];""")])

patch('data/scripts/white_out.inc', [(
"""    clearflag FLAG_NUZLOCKE
    setflag FLAG_NUZLOCKE_FIN""",
"""    clearflag FLAG_NUZLOCKE
    setflag FLAG_NUZLOCKE_FIN
    callnative Nuzlocke_EndRun""")])
print('nuzlocke hooks ok')
