# Run from the repo root, after orange_ivy_mikan.py.
# Save layout + separate Orange Islands PC.
#
# Flash (32 sectors of 4 KiB):
#   0-14   the save (one slot): SaveBlock2 (1), SaveBlock1 (5), PC (9)
#   15-23  stored Kanto PC while on the Orange trip (whole 14-box PC)
#   24-27, 30-31  stored Orange PC (9 boxes) while back home
#   28-29  Hall of Fame
# The backup save slot is gone (it overlapped the Hall of Fame and e-Reader sectors), and the
# e-Reader Trainer Tower sectors are reused.
#
# OrangeChallenge_Begin: Kanto PC + home party -> sectors 15-23, then the Orange PC + Orange
# party are loaded (or a fresh 9-box PC and an empty party). OrangeChallenge_End does the
# reverse. Flash writes only ever store what is in RAM and the main save is only changed by
# saving, so resetting before the next save can never lose either PC or team.
# SaveBlock1 doesn't grow (main RAM is full); the waiting team lives with its stored PC.
import re

def rd(p): return open(p, encoding='utf-8').read()
def wr(p, s): open(p, 'w', newline='\n', encoding='utf-8').write(s)
def sub1(t, old, new, label):
    assert old in t, label
    return t.replace(old, new, 1)

# ---------------------------------------------------------------- save.h
p = 'include/save.h'; t = rd(p)
if 'SECTOR_ID_KANTO_PC_START' not in t:
    t = sub1(t, '#define NUM_SAVE_SLOTS 2\n', '#define NUM_SAVE_SLOTS 1 // one slot: the space of the backup slot holds the stored PCs\n', 'slots')
    t = re.sub(r'#define SECTOR_ID_SAVEBLOCK1_END\s+6.*\n', '#define SECTOR_ID_SAVEBLOCK1_END      5 // 5 sectors (1-5)\n', t)
    t = re.sub(r'#define SECTOR_ID_PKMN_STORAGE_START\s+7.*\n', '#define SECTOR_ID_PKMN_STORAGE_START  6\n', t)
    t = re.sub(r'#define SECTOR_ID_PKMN_STORAGE_END\s+15.*\n', '#define SECTOR_ID_PKMN_STORAGE_END    14 // 9 sectors (6-14)\n', t)
    t = re.sub(r'#define NUM_SECTORS_PER_SLOT\s+16\n', '#define NUM_SECTORS_PER_SLOT          15\n', t)
    t = sub1(t, '// Save Slot 1: sectors 0-18; Save Slot 2: sectors 19-37\n',
             '// Save: sectors 0-14. Stored Kanto PC: 15-23. Stored Orange PC: 24-27 and 30-31.\n', 'comment')
    t = sub1(t, '#define SECTOR_ID_TRAINER_TOWER_1     30\n#define SECTOR_ID_TRAINER_TOWER_2     31\n',
             '#define SECTOR_ID_TRAINER_TOWER_1     30 // e-Reader Trainer Tower: disabled, reused by the Orange PC\n'
             '#define SECTOR_ID_TRAINER_TOWER_2     31\n'
             '#define SECTOR_ID_KANTO_PC_START      15\n'
             '#define NUM_KANTO_PC_SECTORS          9\n', 'tt')
    t = sub1(t, '#define NUM_HOF_SECTORS 2\n', '#define NUM_HOF_SECTORS 2\n\n'
             '// Stored PCs for the Orange Islands trip (see src/orange_islands.c)\n'
             'enum { STORED_PC_KANTO, STORED_PC_ORANGE };\n'
             'struct StoredPCPart { void *data; u32 size; };\n'
             'bool8 SaveStoredPC(u8 which, const struct StoredPCPart *parts, u8 numParts);\n'
             'bool8 LoadStoredPC(u8 which, const struct StoredPCPart *parts, u8 numParts);\n', 'hof')
    wr(p, t)

# ---------------------------------------------------------------- save.c
p = 'src/save.c'; t = rd(p)
if 'SaveStoredPC' not in t:
    t = sub1(t, '    SAVEBLOCK_CHUNK(struct SaveBlock1, 3), // SECTOR_ID_SAVEBLOCK1_END\n    SAVEBLOCK_CHUNK(struct SaveBlock1, 4),\n    SAVEBLOCK_CHUNK(struct SaveBlock1, 5),\n',
             '    SAVEBLOCK_CHUNK(struct SaveBlock1, 3),\n    SAVEBLOCK_CHUNK(struct SaveBlock1, 4), // SECTOR_ID_SAVEBLOCK1_END\n', 'layout')
    # one slot: there is no second slot to check
    t = sub1(t, '''    // check save slot 2.
    validSectors = 0;
    signatureValid = FALSE;
    for (sector = 0; sector < NUM_SECTORS_PER_SLOT; sector++)''', '''    // check save slot 2.
    validSectors = 0;
    signatureValid = FALSE;
    for (sector = 0; sector < NUM_SECTORS_PER_SLOT && NUM_SAVE_SLOTS > 1; sector++)''', 'slot2')
    # A new file's first save erases the old file's Hall of Fame only. The stored PCs are left
    # alone: they are only ever read after this file wrote them (FLAG_ORANGE_PC_SAVED / the trip).
    t = t.replace('        for (i = SECTOR_ID_HOF_1; i < SECTORS_COUNT; i++)\n            EraseFlashSector(i);\n',
                  '        for (i = SECTOR_ID_HOF_1; i <= SECTOR_ID_HOF_2; i++)\n            EraseFlashSector(i);\n')
    # e-Reader Trainer Tower sectors now belong to the Orange PC
    t = sub1(t, '    if (sectorId != SECTOR_ID_TRAINER_TOWER_1 && sectorId != SECTOR_ID_TRAINER_TOWER_2)\n        return SAVE_STATUS_ERROR;\n',
             '    return SAVE_STATUS_ERROR; // e-Reader Trainer Tower disabled: its sectors hold the Orange PC\n'
             '    if (sectorId != SECTOR_ID_TRAINER_TOWER_1 && sectorId != SECTOR_ID_TRAINER_TOWER_2)\n        return SAVE_STATUS_ERROR;\n', 'ttread')
    t = sub1(t, '    if (sector != SECTOR_ID_TRAINER_TOWER_1 && sector != SECTOR_ID_TRAINER_TOWER_2)\n        return SAVE_STATUS_ERROR;\n',
             '    return SAVE_STATUS_ERROR; // e-Reader Trainer Tower disabled: its sectors hold the Orange PC\n'
             '    if (sector != SECTOR_ID_TRAINER_TOWER_1 && sector != SECTOR_ID_TRAINER_TOWER_2)\n        return SAVE_STATUS_ERROR;\n', 'ttwrite')
    t = t.rstrip('\n') + r'''

// ---------------------------------------------------------------------------
// Stored PCs (Orange Islands trip). Each sector holds SECTOR_DATA_SIZE bytes of the PC and is
// checked on load; a PC only loads if every one of its sectors is intact.

static const u8 sKantoPCSectors[NUM_KANTO_PC_SECTORS] = {15, 16, 17, 18, 19, 20, 21, 22, 23};
static const u8 sOrangePCSectors[] = {24, 25, 26, 27, SECTOR_ID_TRAINER_TOWER_1, SECTOR_ID_TRAINER_TOWER_2};

STATIC_ASSERT(SECTOR_ID_KANTO_PC_START == NUM_SECTORS_PER_SLOT * NUM_SAVE_SLOTS, StoredPCAfterSave);
STATIC_ASSERT(sizeof(struct PokemonStorage) <= SECTOR_DATA_SIZE * NUM_KANTO_PC_SECTORS, KantoPCFits);

static const u8 *GetStoredPCSectors(u8 which, u8 *count)
{
    if (which == STORED_PC_KANTO)
    {
        *count = ARRAY_COUNT(sKantoPCSectors);
        return sKantoPCSectors;
    }
    *count = ARRAY_COUNT(sOrangePCSectors);
    return sOrangePCSectors;
}

static u32 StoredPCSize(const struct StoredPCPart *parts, u8 numParts)
{
    u32 size = 0;
    u8 i;

    for (i = 0; i < numParts; i++)
        size += parts[i].size;
    return size;
}

// Copies bytes [offset, offset + len) of the parts (laid end to end) to or from buf.
static void CopyStoredPCParts(const struct StoredPCPart *parts, u8 numParts, u32 offset, u8 *buf, u32 len, bool8 toParts)
{
    u8 i;
    u32 start = 0;

    for (i = 0; i < numParts && len != 0; i++)
    {
        u32 end = start + parts[i].size;
        if (offset < end)
        {
            u32 n = end - offset;
            u8 *p = (u8 *)parts[i].data + (offset - start);
            if (n > len)
                n = len;
            if (toParts)
                memcpy(p, buf, n);
            else
                memcpy(buf, p, n);
            buf += n;
            offset += n;
            len -= n;
        }
        start = end;
    }
}

bool8 SaveStoredPC(u8 which, const struct StoredPCPart *parts, u8 numParts)
{
    u8 count, i;
    const u8 *sectors = GetStoredPCSectors(which, &count);
    u32 size = StoredPCSize(parts, numParts);
    u32 *backupPtr = gMain.vblankCounter1;
    bool8 ok = TRUE;

    if (gFlashMemoryPresent != TRUE || size > (u32)count * SECTOR_DATA_SIZE)
        return FALSE;
    gMain.vblankCounter1 = NULL;
    for (i = 0; i * SECTOR_DATA_SIZE < size; i++)
    {
        u32 chunk = size - i * SECTOR_DATA_SIZE;
        if (chunk > SECTOR_DATA_SIZE)
            chunk = SECTOR_DATA_SIZE;
        CpuFill32(0, gDecompressionBuffer, SECTOR_DATA_SIZE);
        CopyStoredPCParts(parts, numParts, i * SECTOR_DATA_SIZE, gDecompressionBuffer, chunk, FALSE);
        if (HandleWriteSectorNBytes(sectors[i], gDecompressionBuffer, chunk) != SAVE_STATUS_OK)
            ok = FALSE;
    }
    gMain.vblankCounter1 = backupPtr;
    return ok;
}

// Checks every sector first, so a damaged copy never overwrites anything halfway.
bool8 LoadStoredPC(u8 which, const struct StoredPCPart *parts, u8 numParts)
{
    u8 count, i;
    const u8 *sectors = GetStoredPCSectors(which, &count);
    u32 size = StoredPCSize(parts, numParts);

    if (gFlashMemoryPresent != TRUE || size > (u32)count * SECTOR_DATA_SIZE)
        return FALSE;
    for (i = 0; i * SECTOR_DATA_SIZE < size; i++)
    {
        u32 chunk = size - i * SECTOR_DATA_SIZE;
        if (chunk > SECTOR_DATA_SIZE)
            chunk = SECTOR_DATA_SIZE;
        ReadFlashSector(sectors[i], &gSaveDataBuffer);
        if (gSaveDataBuffer.signature != SECTOR_SIGNATURE || gSaveDataBuffer.id != CalculateChecksum(gSaveDataBuffer.data, chunk))
            return FALSE;
    }
    for (i = 0; i * SECTOR_DATA_SIZE < size; i++)
    {
        u32 chunk = size - i * SECTOR_DATA_SIZE;
        if (chunk > SECTOR_DATA_SIZE)
            chunk = SECTOR_DATA_SIZE;
        if (TryLoadSaveSector(sectors[i], gDecompressionBuffer, chunk) != SAVE_STATUS_OK)
            return FALSE;
        CopyStoredPCParts(parts, numParts, i * SECTOR_DATA_SIZE, gDecompressionBuffer, chunk, TRUE);
    }
    return TRUE;
}
'''
    wr(p, t)

# ---------------------------------------------------------------- flag: a stored Orange PC exists
p = 'include/constants/flags.h'; t = rd(p)
if 'FLAG_ORANGE_PC_SAVED' not in t:
    t = sub1(t, '#define FLAG_0x20A               0x20A', '#define FLAG_ORANGE_PC_SAVED     0x20A // the stored Orange PC sectors hold a real PC', 'flag')
    wr(p, t)
p = 'include/pokemon_storage_system.h'; t = rd(p)
if 'ORANGE_BOXES_COUNT' not in t:
    t = sub1(t, '#define BOX_NAME_LENGTH         8\n', '#define BOX_NAME_LENGTH         8\n#define ORANGE_BOXES_COUNT      9 // boxes in the Orange Islands PC\n', 'obc')
    wr(p, t)

# ---------------------------------------------------------------- box count while on the trip
p = 'include/pokemon_storage_system.h'; t = rd(p)
if 'GetActiveBoxCount' not in t:
    t = sub1(t, 'u8 StorageGetCurrentBox(void);\n',
             'u8 StorageGetCurrentBox(void);\n'
             'u8 GetActiveBoxCount(void); // ORANGE_BOXES_COUNT on the Orange Islands trip, else TOTAL_BOXES_COUNT\n', 'proto')
    wr(p, t)
EDITS = [
 ('src/pokemon_storage_system_tasks.c', [
   ('            if (gStorage->newCurrBoxId >= TOTAL_BOXES_COUNT)\n', '            if (gStorage->newCurrBoxId >= GetActiveBoxCount())\n'),
   ('                gStorage->newCurrBoxId = TOTAL_BOXES_COUNT - 1;\n', '                gStorage->newCurrBoxId = GetActiveBoxCount() - 1;\n')]),
 ('src/pokemon_storage_system_menu.c', [
   ('    if (++sChooseBoxMenu->curBox >= TOTAL_BOXES_COUNT)\n', '    if (++sChooseBoxMenu->curBox >= GetActiveBoxCount())\n'),
   ('sChooseBoxMenu->curBox == 0 ? TOTAL_BOXES_COUNT - 1 :', 'sChooseBoxMenu->curBox == 0 ? GetActiveBoxCount() - 1 :')]),
 ('src/pokemon_storage_system_graphics.c', [
   ('        if (currentBox >= TOTAL_BOXES_COUNT)\n            currentBox = 0;\n    }\n\n    return (i < TOTAL_BOXES_COUNT / 2) ? 1 : -1;',
    '        if (currentBox >= GetActiveBoxCount())\n            currentBox = 0;\n    }\n\n    return (i < GetActiveBoxCount() / 2) ? 1 : -1;')]),
 ('src/pokemon.c', [
   ('        boxNo++;\n        if (boxNo == TOTAL_BOXES_COUNT)\n            boxNo = 0;\n', '        boxNo++;\n        if (boxNo >= GetActiveBoxCount())\n            boxNo = 0;\n'),
   ('    for (i = 0; i < TOTAL_BOXES_COUNT; i++)\n        for (j = 0; j < IN_BOX_COUNT; j++)\n            if (GetBoxMonDataAt(i, j, MON_DATA_SPECIES) == SPECIES_NONE)',
    '    for (i = 0; i < GetActiveBoxCount(); i++)\n        for (j = 0; j < IN_BOX_COUNT; j++)\n            if (GetBoxMonDataAt(i, j, MON_DATA_SPECIES) == SPECIES_NONE)')]),
 ('src/field_specials.c', [
   ('    for (i = 0; i < TOTAL_BOXES_COUNT; i++)\n    {\n        for (j = 0; j < IN_BOX_COUNT; j++)\n        {\n            if (GetBoxMonDataAt(i, j, MON_DATA_SPECIES) == SPECIES_NONE)\n                return TRUE;',
    '    for (i = 0; i < GetActiveBoxCount(); i++)\n    {\n        for (j = 0; j < IN_BOX_COUNT; j++)\n        {\n            if (GetBoxMonDataAt(i, j, MON_DATA_SPECIES) == SPECIES_NONE)\n                return TRUE;'),
   ('        i++;\n        if (i == TOTAL_BOXES_COUNT)\n            i = 0;\n', '        i++;\n        if (i >= GetActiveBoxCount())\n            i = 0;\n')]),
]
for path, reps in EDITS:
    t = rd(path)
    for old, new in reps:
        if new not in t:
            t = sub1(t, old, new, path)
    if '#include "pokemon_storage_system.h"' not in t:
        t = sub1(t, '#include "global.h"\n', '#include "global.h"\n#include "pokemon_storage_system.h"\n', path + ' include')
    wr(path, t)

# ---------------------------------------------------------------- Orange challenge: swap PCs
p = 'src/orange_islands.c'; t = rd(p)
if 'SaveStoredPC' not in t:
    start = t.index('// Orange Islands challenge:')
    end = t.index('// ---------------------------------------------------------------------------\n// Gym rules')
    NEW = r'''// Orange Islands challenge (FLAG_ORANGE_CHALLENGE). The trip has its own team and its own PC.
// In RAM there is only ever one of each: the Kanto PC + home team at home, the Orange PC
// (ORANGE_BOXES_COUNT boxes) + Orange team on the trip. The pair not in use is kept in its own
// flash sectors (src/save.c, SaveStoredPC). Flash writes only ever store what is in RAM, and the
// main save only changes when the player saves, so a reset before saving can't lose anything.
//
// Script use:
//   callnative OrangeChallenge_Begin   @ VAR_RESULT: TRUE ok, FALSE flash error (nothing changed)
//   callnative OrangeChallenge_End     @ VAR_RESULT: TRUE ok, FALSE flash error (nothing changed)
#include "global.h"
#include "event_data.h"
#include "follow_me.h"
#include "pokemon.h"
#include "pokemon_storage_system.h"
#include "battle_main.h"
#include "load_save.h"
#include "main.h"
#include "overworld.h"
#include "party_menu.h"
#include "save.h"
#include "script_pokemon_util.h"
#include "string_util.h"
#include "strings.h"
#include "constants/party_menu.h"
#include "constants/flags.h"
#include "constants/vars.h"

// The Orange PC: currentBox + its boxes (the start of struct PokemonStorage), its box names and
// wallpapers, and the Orange team while it waits at home.
#define ORANGE_PC_HEAD_BYTES (offsetof(struct PokemonStorage, boxes) + sizeof(struct BoxPokemon) * IN_BOX_COUNT * ORANGE_BOXES_COUNT)

static const u8 sText_Orange[] = _("ORANGE ");

u8 GetActiveBoxCount(void)
{
    return FlagGet(FLAG_ORANGE_CHALLENGE) ? ORANGE_BOXES_COUNT : TOTAL_BOXES_COUNT;
}

static void GetKantoParts(struct StoredPCPart *parts)
{
    parts[0].data = gPokemonStoragePtr;
    parts[0].size = sizeof(struct PokemonStorage);
    parts[1].data = gPlayerParty;
    parts[1].size = sizeof(gPlayerParty);
}

static void GetOrangeParts(struct StoredPCPart *parts)
{
    parts[0].data = gPokemonStoragePtr;
    parts[0].size = ORANGE_PC_HEAD_BYTES;
    parts[1].data = gPokemonStoragePtr->boxNames;
    parts[1].size = sizeof(gPokemonStoragePtr->boxNames[0]) * ORANGE_BOXES_COUNT;
    parts[2].data = gPokemonStoragePtr->boxWallpapers;
    parts[2].size = ORANGE_BOXES_COUNT;
    parts[3].data = gPlayerParty;
    parts[3].size = sizeof(gPlayerParty);
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

void OrangeChallenge_Begin(void)
{
    struct StoredPCPart parts[4];
    u8 box, pos, i;

    gSpecialVar_Result = FALSE;
    if (FlagGet(FLAG_ORANGE_CHALLENGE))
        return;
    GetKantoParts(parts);
    if (!SaveStoredPC(STORED_PC_KANTO, parts, 2))
        return;

    // Kanto PC + home team are safe in flash: bring in the Orange PC + Orange team.
    GetOrangeParts(parts);
    if (!FlagGet(FLAG_ORANGE_PC_SAVED) || !LoadStoredPC(STORED_PC_ORANGE, parts, 4))
    {
        // First trip: a fresh Orange PC and no team (Prof. Ivy gives a starter).
        gPokemonStoragePtr->currentBox = 0;
        for (box = 0; box < ORANGE_BOXES_COUNT; box++)
        {
            u8 *name = StringCopy(gPokemonStoragePtr->boxNames[box], sText_Orange);
            ConvertIntToDecimalStringN(name, box + 1, STR_CONV_MODE_LEFT_ALIGN, 1);
            gPokemonStoragePtr->boxWallpapers[box] = WALLPAPER_BEACH;
            for (pos = 0; pos < IN_BOX_COUNT; pos++)
                ZeroBoxMonData(&gPokemonStoragePtr->boxes[box][pos]);
        }
        for (i = 0; i < PARTY_SIZE; i++)
            ZeroMonData(&gPlayerParty[i]);
    }
    for (box = ORANGE_BOXES_COUNT; box < TOTAL_BOXES_COUNT; box++)
        for (pos = 0; pos < IN_BOX_COUNT; pos++)
            ZeroBoxMonData(&gPokemonStoragePtr->boxes[box][pos]);
    if (gPokemonStoragePtr->currentBox >= ORANGE_BOXES_COUNT)
        gPokemonStoragePtr->currentBox = 0;

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
    GetOrangeParts(parts);
    if (!SaveStoredPC(STORED_PC_ORANGE, parts, 4))
        return;
    FlagSet(FLAG_ORANGE_PC_SAVED);

    // Only now replace RAM; a damaged Kanto copy leaves everything as it was.
    GetKantoParts(parts);
    if (!LoadStoredPC(STORED_PC_KANTO, parts, 2))
        return;

    FlagClear(FLAG_ORANGE_CHALLENGE);
    AfterPartyChange();
    gSpecialVar_Result = TRUE;
}

'''
    t = t[:start] + NEW + t[end:]
    wr(p, t)

# ---------------------------------------------------------------- PC opens on the trip
p = 'data/scripts/pc.inc'; t = rd(p)
t = t.replace('\tgoto_if_set FLAG_SYS_PC_STORAGE_DISABLED, EventScript_PCDisabled\n\tgoto_if_set FLAG_ORANGE_CHALLENGE, EventScript_PCOrangeIslands\n',
              '\tgoto_if_set FLAG_SYS_PC_STORAGE_DISABLED, EventScript_PCDisabled\n')
wr(p, t)

# ---------------------------------------------------------------- Ivy's lines
p = 'data/maps/OrangeIslands_Valencia_IvysLab/scripts.inc'; t = rd(p)
t = t.replace('''	.string "The team you brought rests in the\\n"
	.string "PC, and the PC stays locked until\\l"
	.string "you leave the islands.\\p"''', '''	.string "The team you brought waits for you\\n"
	.string "in your KANTO PC, and while you're\\l"
	.string "here you get an ORANGE PC instead.\\p"''')
t = t.replace('''	.string "Oh dear, your PC boxes are too full\\n"
	.string "to take your whole team.\\p"
	.string "Make some room at the POKéMON\\n"
	.string "CENTER, then come talk to me.$"''', '''	.string "Hmm, the PC link didn't go\\n"
	.string "through. Let's try that again.$"''')
t = t.replace('\t.string "I\'ll send your team to the PC now.\\n"\n\t.string "Don\'t worry, they\'ll be safe!$"',
              '\t.string "I\'ll send your team to your KANTO\\n"\n\t.string "PC now. Don\'t worry, they\'ll be safe!$"')
wr(p, t)
print('orange pc ok')
