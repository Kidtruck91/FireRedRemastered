#ifndef GUARD_SAVE_H
#define GUARD_SAVE_H

#include "global.h"

// Each 4 KiB flash sector contains 3968 bytes of actual data followed by a 128 byte footer.
// Only 12 bytes of the footer are used.
#define SECTOR_DATA_SIZE 3968
#define SECTOR_FOOTER_SIZE 128
#define SECTOR_SIZE (SECTOR_DATA_SIZE + SECTOR_FOOTER_SIZE)

#define NUM_SAVE_SLOTS 1 // one slot: the space of the backup slot holds the stored PCs

// If the sector's signature field is not this value, then the sector is either invalid or empty.
#define SECTOR_SIGNATURE 0x08012025
#define TOTAL_FLASH_SIZE 128 * 1024 
#define SPECIAL_SECTOR_SENTINEL 0xB39D

// Updated sector IDs based on expanded SaveBlock layout
#define SECTOR_ID_SAVEBLOCK2          0
  
#define SECTOR_ID_SAVEBLOCK1_START    1
#define SECTOR_ID_SAVEBLOCK1_END      5 // 5 sectors (1-5)
#define SECTOR_ID_PKMN_STORAGE_START  6
#define SECTOR_ID_PKMN_STORAGE_END    14 // 9 sectors (6-14)

// Total sectors per save slot with the new layout
#define NUM_SECTORS_PER_SLOT          15

// Additional sectors outside the save slots
// Save: sectors 0-14. Stored Kanto PC: 15-23. Stored Orange PC: 24-27 and 30-31.
#define SECTOR_ID_HOF_1               28
#define SECTOR_ID_HOF_2               29
#define SECTOR_ID_TRAINER_TOWER_1     30 // e-Reader Trainer Tower: disabled, reused by the Orange PC
#define SECTOR_ID_TRAINER_TOWER_2     31
#define SECTOR_ID_KANTO_PC_START      15
#define NUM_KANTO_PC_SECTORS          9
#define SECTORS_COUNT                 32

#define NUM_HOF_SECTORS 2

// Stored PCs for the Orange Islands trip (see src/orange_islands.c)
enum { STORED_PC_KANTO, STORED_PC_ORANGE };
struct StoredPCPart { void *data; u32 size; };
bool8 SaveStoredPC(u8 which, const struct StoredPCPart *parts, u8 numParts);
bool8 LoadStoredPC(u8 which, const struct StoredPCPart *parts, u8 numParts);

#define SAVE_STATUS_EMPTY    0
#define SAVE_STATUS_OK       1
#define SAVE_STATUS_INVALID  2
#define SAVE_STATUS_NO_FLASH 4
#define SAVE_STATUS_ERROR    0xFF

// Special sector ID value for certain save functions
// to indicate that all sectors should be used
// instead of a specific sector.
#define FULL_SAVE_SLOT 0xFFFF

enum
{
    SAVE_NORMAL,
    SAVE_LINK,
    SAVE_EREADER,
    SAVE_HALL_OF_FAME,
    SAVE_OVERWRITE_DIFFERENT_FILE,
    SAVE_HALL_OF_FAME_ERASE_BEFORE, // unused
};

// A save sector location holds a pointer to the data for a particular sector
// and the size of that data. Size cannot be greater than SECTOR_DATA_SIZE.
struct SaveSectorLocation
{
    u8 *data;
    u16 size;
};

struct SaveSector
{
    u8 data[SECTOR_DATA_SIZE];
    u8 unused[SECTOR_FOOTER_SIZE - 12]; // Unused portion of the footer
    u16 id;
    u16 checksum;
    u32 signature;
    u32 counter;
}; // size is SECTOR_SIZE (0x1000)

#define SECTOR_SIGNATURE_OFFSET offsetof(struct SaveSector, signature)
#define SECTOR_COUNTER_OFFSET   offsetof(struct SaveSector, counter)

// operations for SetDamagedSectorBits
enum
{
    ENABLE,
    DISABLE,
    CHECK // unused
};

extern u32 gDamagedSaveSectors;
extern struct SaveSector *gSaveDataBufferPtr; // the pointer is in fast IWRAM but points to the slower EWRAM.
extern u16 gSaveFileStatus;
extern void (*gGameContinueCallback)(void);
extern u16 gSaveAttemptStatus;

extern struct SaveSector gSaveDataBuffer;

void ClearSaveData(void);
void Save_ResetSaveCounters(void);
u8 HandleSavingData(u8 saveType);
u8 TrySavingData(u8 saveType);
bool8 LinkFullSave_Init(void);
bool8 LinkFullSave_WriteSector(void);
bool8 LinkFullSave_ReplaceLastSector(void);
bool8 LinkFullSave_SetLastSectorSignature(void);
bool8 WriteSaveBlock2(void);
bool8 WriteSaveBlock1Sector(void);
u8 LoadGameSave(u8 saveType);
u32 TryReadSpecialSaveSector(u8 sector, u8 *dst);
u32 TryWriteSpecialSaveSector(u8 sector, u8 *src);
void Task_LinkFullSave(u8 taskId);

#endif // GUARD_SAVE_H
