// VOLTORB FLIP.
//
// The HGSS Game Corner game. It is a DS game and there is no GBA decompilation
// of it anywhere, so unlike ROULETTE next door this is written rather than
// ported: the rules below are the published ones, the board tables are the
// real ones, and the artwork is drawn by docs/orange-islands/make_voltorb_flip_gfx.py.
//
// The rules, for anyone reading this who has not played it:
//
//   Twenty-five face-down cards. Each is a VOLTORB or a 1, 2 or 3. Flip a
//   number and it multiplies what the round is worth; flip a VOLTORB and the
//   round is over and worth nothing. Flip every 2 and every 3 and you bank the
//   lot. Beside each row and under each column are two figures: the total of
//   the numbers in it, and how many VOLTORB it holds. Those two figures are
//   the whole game -- a row totalling 5 with no VOLTORB in it is five cards
//   you can turn over without looking.
//
//   Clearing a board moves you up a level, to a maximum of eight, and the
//   boards get richer and more dangerous as you go. Hitting a VOLTORB drops
//   you back to however many 2s and 3s you had already turned over, so a bad
//   first guess costs more than a bad last one.
//
// One simplification against the DS original: marking cards. There, memo mode
// opens a little submenu per card. Here SELECT turns marking on and off, and
// while it is on, A cycles a card's mark 1, 2, 3, VOLTORB, blank. It reads the
// same way round a d-pad and needs no submenu.
//
// The level is kept in VAR_ORANGE_VOLTORB_LEVEL so it survives leaving the
// building, which is how the DS game behaves.

#include "global.h"
#include "bg.h"
#include "coins.h"
#include "decompress.h"
#include "event_data.h"
#include "field_fadetransition.h"
#include "gpu_regs.h"
#include "main.h"
#include "malloc.h"
#include "menu.h"
#include "menu_helpers.h"
#include "new_menu_helpers.h"
#include "overworld.h"
#include "palette.h"
#include "random.h"
#include "scanline_effect.h"
#include "sound.h"
#include "sprite.h"
#include "string_util.h"
#include "script.h"
#include "task.h"
#include "text.h"
#include "text_window.h"
#include "voltorb_flip.h"
#include "window.h"
#include "constants/coins.h"
#include "constants/rgb.h"
#include "constants/songs.h"

#define BOARD_SIZE   5
#define NUM_CARDS    (BOARD_SIZE * BOARD_SIZE)
#define VF_MAX_LEVEL 8
#define NUM_SETUPS   5

#define CARD_VOLTORB 0

// Marks the player can pencil onto a face-down card.
enum {
    MEMO_NONE,
    MEMO_ONE,
    MEMO_TWO,
    MEMO_THREE,
    MEMO_VOLTORB,
    MEMO_COUNT,
};

// Which 2x2 card picture to draw in a cell.
enum {
    PIC_BACK,
    PIC_ONE,
    PIC_TWO,
    PIC_THREE,
    PIC_VOLTORB,
    PIC_MEMO_ONE,
    PIC_MEMO_TWO,
    PIC_MEMO_THREE,
    PIC_MEMO_VOLTORB,
};

// The screen, by background row, on a twenty-row display:
//
//    0-1    header: level, what the round is worth, coins in the case
//    2-11   the board, five cards of two tiles square
//    12-13  the column hints
//    14     the message box frame
//    15-18  the message box
//    19     the message box frame
//
// Row hints sit to the right of the board at x=13, clear of its x=2..11.
//
// Board origin, in background tiles. Each card is two tiles square.
#define BOARD_LEFT   2
#define BOARD_TOP    2

// gMain.newAndRepeatedKeys is what held-direction movement wants; FireRed has
// no macro for it the way it does for newKeys and heldKeys.
#define JOY_NEW_OR_REPEAT(button) TEST_BUTTON(gMain.newAndRepeatedKeys, button)

// slot_machine.c keeps this as a file-local inline. One line, so here it is
// again rather than moved into a shared header for two callers.
static inline void SetBackdropColor(u16 color, u16 *pal)
{
    *pal = color;
    LoadPalette(pal, BG_PLTT_ID(0), PLTT_SIZEOF(1));
}

#define WIN_MESSAGE  0
#define WIN_HEADER   1
#define WIN_ROWS     2
#define WIN_COLS     3

#define TAG_CURSOR   0x4000
#define CARD_PALETTE 1

enum {
    STATE_PLAYING,
    STATE_MESSAGE_THEN_PLAY,
    STATE_LOST,
    STATE_WON,
    STATE_ASK_QUIT,
    STATE_LEAVING,
};

struct VoltorbFlip
{
    MainCallback savedCallback;
    u8 card[NUM_CARDS];        // 0 for a VOLTORB, else 1, 2 or 3
    u8 flipped[NUM_CARDS];
    u8 memo[NUM_CARDS];
    u8 cursor;
    u8 level;
    u8 marking;
    u8 state;
    u16 roundCoins;            // 0 until the first number is turned over
    u8 rowSum[BOARD_SIZE];
    u8 rowVoltorbs[BOARD_SIZE];
    u8 colSum[BOARD_SIZE];
    u8 colVoltorbs[BOARD_SIZE];
    u8 prizeCards;             // 2s and 3s still face down
    u8 spriteId;
    u8 tilemap[BG_SCREEN_SIZE];
    u8 textTilemap[BG_SCREEN_SIZE];
};

static EWRAM_DATA struct VoltorbFlip *sGame = NULL;

static void CB2_InitVoltorbFlip(void);
static void CB2_RunVoltorbFlip(void);
static void VBlankCB_VoltorbFlip(void);
static void Task_FadeToVoltorbFlip(u8);
static void Task_VoltorbFlipMain(u8);
static void Task_ExitVoltorbFlip(u8);
static void DealBoard(void);
static void DrawBoard(void);
static void DrawCell(u8);
static void PrintHeader(void);
static void PrintHints(void);
static void ShowMessage(const u8 *);
static void MoveCursorSprite(void);
static void SpriteCB_Cursor(struct Sprite *);

// The DS game's own board tables: how many 2s, how many 3s and how many
// VOLTORB each level can deal. Everything else on the board is a 1. The payout
// for clearing is the product of the 2s and 3s, so level one pays between 24
// and 48 and level eight between 1,944 and 2,916.
static const u8 sBoardSetups[VF_MAX_LEVEL][NUM_SETUPS][3] =
{
    {{3, 1, 6}, {0, 3, 6}, {5, 0, 6}, {2, 2, 6}, {4, 1, 6}},
    {{1, 3, 7}, {6, 0, 7}, {3, 2, 7}, {0, 4, 7}, {5, 1, 7}},
    {{2, 3, 8}, {7, 0, 8}, {4, 2, 8}, {1, 4, 8}, {6, 1, 8}},
    {{3, 3, 8}, {0, 5, 8}, {8, 0, 10}, {5, 2, 10}, {2, 4, 10}},
    {{7, 1, 10}, {4, 3, 10}, {1, 5, 10}, {9, 0, 10}, {6, 2, 10}},
    {{3, 4, 10}, {0, 6, 10}, {8, 1, 10}, {5, 3, 10}, {2, 5, 10}},
    {{7, 2, 10}, {4, 4, 10}, {1, 6, 10}, {9, 1, 10}, {6, 3, 10}},
    {{3, 5, 10}, {0, 7, 10}, {8, 2, 10}, {5, 4, 10}, {2, 6, 10}},
};

static const u32 sCards_Gfx[] = INCBIN_U32("graphics/voltorb_flip/cards.4bpp");
static const u16 sCards_Pal[] = INCBIN_U16("graphics/voltorb_flip/cards.gbapal");
static const u32 sCursor_Gfx[] = INCBIN_U32("graphics/voltorb_flip/cursor.4bpp");

static const struct BgTemplate sBgTemplates[] =
{
    {
        .bg = 0,                // text
        .charBaseIndex = 2,
        .mapBaseIndex = 31,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 0,
        .baseTile = 0,
    },
    {
        .bg = 1,                // the board
        .charBaseIndex = 0,
        .mapBaseIndex = 28,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 1,
        .baseTile = 0,
    },
};

static const struct WindowTemplate sWindowTemplates[] =
{
    [WIN_MESSAGE] = {
        .bg = 0, .tilemapLeft = 2, .tilemapTop = 15,
        .width = 26, .height = 4, .paletteNum = 15, .baseBlock = 0x001,
    },
    [WIN_HEADER] = {
        .bg = 0, .tilemapLeft = 1, .tilemapTop = 0,
        .width = 28, .height = 2, .paletteNum = 15, .baseBlock = 0x069,
    },
    [WIN_ROWS] = {
        .bg = 0, .tilemapLeft = 13, .tilemapTop = 2,
        .width = 4, .height = 10, .paletteNum = 15, .baseBlock = 0x0A1,
    },
    [WIN_COLS] = {
        .bg = 0, .tilemapLeft = 2, .tilemapTop = 12,
        .width = 10, .height = 2, .paletteNum = 15, .baseBlock = 0x0C9,
    },
    DUMMY_WIN_TEMPLATE,
};

static const struct OamData sOam_Cursor =
{
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(16x16),
    .x = 0,
    .size = SPRITE_SIZE(16x16),
    .priority = 0,
};

static const struct SpriteSheet sCursorSheet = {
    (const void *)sCursor_Gfx, 0x80, TAG_CURSOR
};

static const struct SpritePalette sCursorPalette = {
    sCards_Pal, TAG_CURSOR
};

static const struct SpriteTemplate sSpriteTemplate_Cursor =
{
    .tileTag = TAG_CURSOR,
    .paletteTag = TAG_CURSOR,
    .oam = &sOam_Cursor,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_Cursor,
};

static const u8 sText_VoltorbMark[] = _("V");
static const u8 sText_Marking[] = _("Marking. A cycles 1, 2, 3, VOLTORB,\nblank. SELECT to stop marking.");
static const u8 sText_Controls[] = _("A flips. SELECT marks a card.\nB gives up the round.");
static const u8 sText_Boom[] = _("A VOLTORB! The round is over.");
static const u8 sText_Cleared[] = _("Every 2 and 3 turned over!\nYou win {STR_VAR_1} coins!");
static const u8 sText_CaseFull[] = _("Your COIN CASE is full!");
static const u8 sText_AskQuit[] = _("Give up this round?\nA to quit, B to carry on.");
static const u8 sText_LevelUp[] = _("Up to level {STR_VAR_1}!");
static const u8 sText_LevelDown[] = _("Back to level {STR_VAR_1}.");
static const u8 sText_Header[] = _("Lv. {STR_VAR_1}   Round {STR_VAR_2}   Coins {STR_VAR_3}");

// ---------------------------------------------------------------------------
// dealing
// ---------------------------------------------------------------------------
static void DealBoard(void)
{
    const u8 *setup = sBoardSetups[sGame->level - 1][Random() % NUM_SETUPS];
    u8 i, j, swap, row, col;

    for (i = 0; i < NUM_CARDS; i++)
        sGame->card[i] = 1;
    for (i = 0; i < setup[0]; i++)
        sGame->card[i] = 2;
    for (; i < (u8)(setup[0] + setup[1]); i++)
        sGame->card[i] = 3;
    for (; i < (u8)(setup[0] + setup[1] + setup[2]); i++)
        sGame->card[i] = CARD_VOLTORB;

    // Fisher-Yates, so every arrangement of that many 2s, 3s and VOLTORB is
    // equally likely.
    for (i = NUM_CARDS - 1; i > 0; i--)
    {
        j = Random() % (i + 1);
        swap = sGame->card[i];
        sGame->card[i] = sGame->card[j];
        sGame->card[j] = swap;
    }

    for (i = 0; i < NUM_CARDS; i++)
    {
        sGame->flipped[i] = FALSE;
        sGame->memo[i] = MEMO_NONE;
    }

    // The hints are worked out from the board rather than stored with it, so
    // they cannot disagree with it.
    sGame->prizeCards = 0;
    for (row = 0; row < BOARD_SIZE; row++)
    {
        sGame->rowSum[row] = 0;
        sGame->rowVoltorbs[row] = 0;
    }
    for (col = 0; col < BOARD_SIZE; col++)
    {
        sGame->colSum[col] = 0;
        sGame->colVoltorbs[col] = 0;
    }
    for (i = 0; i < NUM_CARDS; i++)
    {
        row = i / BOARD_SIZE;
        col = i % BOARD_SIZE;
        if (sGame->card[i] == CARD_VOLTORB)
        {
            sGame->rowVoltorbs[row]++;
            sGame->colVoltorbs[col]++;
        }
        else
        {
            sGame->rowSum[row] += sGame->card[i];
            sGame->colSum[col] += sGame->card[i];
            if (sGame->card[i] > 1)
                sGame->prizeCards++;
        }
    }

    sGame->roundCoins = 0;
    sGame->cursor = 0;
    sGame->marking = FALSE;
}

static u8 PictureFor(u8 index)
{
    if (sGame->flipped[index])
    {
        if (sGame->card[index] == CARD_VOLTORB)
            return PIC_VOLTORB;
        return PIC_ONE + sGame->card[index] - 1;
    }
    switch (sGame->memo[index])
    {
    case MEMO_ONE:      return PIC_MEMO_ONE;
    case MEMO_TWO:      return PIC_MEMO_TWO;
    case MEMO_THREE:    return PIC_MEMO_THREE;
    case MEMO_VOLTORB:  return PIC_MEMO_VOLTORB;
    }
    return PIC_BACK;
}

// Card n lives in tiles 1 + 4n to 4 + 4n, laid out top-left, top-right,
// bottom-left, bottom-right, which is the order gbagfx cuts a 16x16 image in.
static void DrawCell(u8 index)
{
    u8 x = BOARD_LEFT + (index % BOARD_SIZE) * 2;
    u8 y = BOARD_TOP + (index / BOARD_SIZE) * 2;
    u16 base = 1 + PictureFor(index) * 4;

    FillBgTilemapBufferRect(1, base + 0, x,     y,     1, 1, CARD_PALETTE);
    FillBgTilemapBufferRect(1, base + 1, x + 1, y,     1, 1, CARD_PALETTE);
    FillBgTilemapBufferRect(1, base + 2, x,     y + 1, 1, 1, CARD_PALETTE);
    FillBgTilemapBufferRect(1, base + 3, x + 1, y + 1, 1, 1, CARD_PALETTE);
}

static void DrawBoard(void)
{
    u8 i;

    FillBgTilemapBufferRect_Palette0(1, 0, 0, 0, 32, 32);
    for (i = 0; i < NUM_CARDS; i++)
        DrawCell(i);
    CopyBgTilemapBufferToVram(1);
}

// ---------------------------------------------------------------------------
// the figures on the page
// ---------------------------------------------------------------------------
static void PrintHeader(void)
{
    ConvertIntToDecimalStringN(gStringVar1, sGame->level, STR_CONV_MODE_LEFT_ALIGN, 1);
    ConvertIntToDecimalStringN(gStringVar2, sGame->roundCoins, STR_CONV_MODE_LEFT_ALIGN, 4);
    ConvertIntToDecimalStringN(gStringVar3, GetCoins(), STR_CONV_MODE_LEFT_ALIGN, 4);
    StringExpandPlaceholders(gStringVar4, sText_Header);

    FillWindowPixelBuffer(WIN_HEADER, PIXEL_FILL(1));
    AddTextPrinterParameterized(WIN_HEADER, FONT_SMALL, gStringVar4, 0, 0, TEXT_SKIP_DRAW, NULL);
    CopyWindowToVram(WIN_HEADER, COPYWIN_GFX);
}

static void PrintHints(void)
{
    u8 i;
    u8 buffer[8];

    FillWindowPixelBuffer(WIN_ROWS, PIXEL_FILL(1));
    for (i = 0; i < BOARD_SIZE; i++)
    {
        ConvertIntToDecimalStringN(buffer, sGame->rowSum[i], STR_CONV_MODE_LEFT_ALIGN, 2);
        AddTextPrinterParameterized(WIN_ROWS, FONT_SMALL, buffer, 2, i * 16, TEXT_SKIP_DRAW, NULL);
        StringCopy(gStringVar4, sText_VoltorbMark);
        ConvertIntToDecimalStringN(buffer, sGame->rowVoltorbs[i], STR_CONV_MODE_LEFT_ALIGN, 1);
        StringAppend(gStringVar4, buffer);
        AddTextPrinterParameterized(WIN_ROWS, FONT_SMALL, gStringVar4, 2, i * 16 + 8,
                                    TEXT_SKIP_DRAW, NULL);
    }
    CopyWindowToVram(WIN_ROWS, COPYWIN_GFX);

    FillWindowPixelBuffer(WIN_COLS, PIXEL_FILL(1));
    for (i = 0; i < BOARD_SIZE; i++)
    {
        ConvertIntToDecimalStringN(buffer, sGame->colSum[i], STR_CONV_MODE_LEFT_ALIGN, 2);
        AddTextPrinterParameterized(WIN_COLS, FONT_SMALL, buffer, i * 16 + 1, 0,
                                    TEXT_SKIP_DRAW, NULL);
        StringCopy(gStringVar4, sText_VoltorbMark);
        ConvertIntToDecimalStringN(buffer, sGame->colVoltorbs[i], STR_CONV_MODE_LEFT_ALIGN, 1);
        StringAppend(gStringVar4, buffer);
        AddTextPrinterParameterized(WIN_COLS, FONT_SMALL, gStringVar4, i * 16 + 1, 8,
                                    TEXT_SKIP_DRAW, NULL);
    }
    CopyWindowToVram(WIN_COLS, COPYWIN_GFX);
}

static void ShowMessage(const u8 *str)
{
    FillWindowPixelBuffer(WIN_MESSAGE, PIXEL_FILL(1));
    AddTextPrinterParameterized(WIN_MESSAGE, FONT_NORMAL, str, 0, 1, TEXT_SKIP_DRAW, NULL);
    CopyWindowToVram(WIN_MESSAGE, COPYWIN_GFX);
}

// ---------------------------------------------------------------------------
// the cursor
// ---------------------------------------------------------------------------
static void SpriteCB_Cursor(struct Sprite *sprite)
{
    // A slow two-frame bob, so the cursor is findable on a board of 25 cards.
    sprite->data[0]++;
    sprite->y2 = (sprite->data[0] & 0x10) ? 1 : 0;
}

static void MoveCursorSprite(void)
{
    struct Sprite *sprite = &gSprites[sGame->spriteId];

    sprite->x = (BOARD_LEFT + (sGame->cursor % BOARD_SIZE) * 2) * 8 + 8;
    sprite->y = (BOARD_TOP + (sGame->cursor / BOARD_SIZE) * 2) * 8 + 8;
}

// ---------------------------------------------------------------------------
// playing
// ---------------------------------------------------------------------------
static void EndRoundLost(void)
{
    u8 i, revealed = 0;

    for (i = 0; i < NUM_CARDS; i++)
    {
        if (sGame->flipped[i] && sGame->card[i] > 1)
            revealed++;
        sGame->flipped[i] = TRUE;     // the board is turned face up
    }
    DrawBoard();

    // The drop is to however many 2s and 3s were already turned over, and
    // never upward, so a bad first guess costs more than a bad last one.
    if (revealed < sGame->level)
        sGame->level = revealed;
    if (sGame->level < 1)
        sGame->level = 1;

    sGame->roundCoins = 0;
    sGame->state = STATE_LOST;
    PlaySE(SE_FAILURE);
    ShowMessage(sText_Boom);
}

static void EndRoundWon(void)
{
    u8 i;

    for (i = 0; i < NUM_CARDS; i++)
        sGame->flipped[i] = TRUE;
    DrawBoard();

    ConvertIntToDecimalStringN(gStringVar1, sGame->roundCoins, STR_CONV_MODE_LEFT_ALIGN, 4);
    StringExpandPlaceholders(gStringVar4, sText_Cleared);
    if (!AddCoins(sGame->roundCoins))
        StringCopy(gStringVar4, sText_CaseFull);

    if (sGame->level < VF_MAX_LEVEL)
        sGame->level++;

    sGame->state = STATE_WON;
    PlayFanfare(MUS_OBTAIN_ITEM);
    ShowMessage(gStringVar4);
}

static void FlipCard(void)
{
    u8 index = sGame->cursor;

    if (sGame->flipped[index])
        return;

    sGame->flipped[index] = TRUE;
    sGame->memo[index] = MEMO_NONE;
    DrawCell(index);
    CopyBgTilemapBufferToVram(1);

    if (sGame->card[index] == CARD_VOLTORB)
    {
        EndRoundLost();
        return;
    }

    PlaySE(SE_SELECT);
    if (sGame->roundCoins == 0)
        sGame->roundCoins = sGame->card[index];
    else
        sGame->roundCoins *= sGame->card[index];

    if (sGame->card[index] > 1 && --sGame->prizeCards == 0)
        EndRoundWon();
    else
        PrintHeader();
}

static void CycleMemo(void)
{
    u8 index = sGame->cursor;

    if (sGame->flipped[index])
        return;
    sGame->memo[index]++;
    if (sGame->memo[index] >= MEMO_COUNT)
        sGame->memo[index] = MEMO_NONE;
    DrawCell(index);
    CopyBgTilemapBufferToVram(1);
    PlaySE(SE_SELECT);
}

static void HandleMovement(void)
{
    u8 row = sGame->cursor / BOARD_SIZE;
    u8 col = sGame->cursor % BOARD_SIZE;
    u8 moved = FALSE;

    if (JOY_NEW_OR_REPEAT(DPAD_UP) && row > 0)
    {
        row--;
        moved = TRUE;
    }
    else if (JOY_NEW_OR_REPEAT(DPAD_DOWN) && row < BOARD_SIZE - 1)
    {
        row++;
        moved = TRUE;
    }
    else if (JOY_NEW_OR_REPEAT(DPAD_LEFT) && col > 0)
    {
        col--;
        moved = TRUE;
    }
    else if (JOY_NEW_OR_REPEAT(DPAD_RIGHT) && col < BOARD_SIZE - 1)
    {
        col++;
        moved = TRUE;
    }

    if (moved)
    {
        sGame->cursor = row * BOARD_SIZE + col;
        MoveCursorSprite();
        PlaySE(SE_SELECT);
    }
}

static void Task_VoltorbFlipMain(u8 taskId)
{
    switch (sGame->state)
    {
    case STATE_PLAYING:
        HandleMovement();
        if (JOY_NEW(A_BUTTON))
        {
            if (sGame->marking)
                CycleMemo();
            else
                FlipCard();
        }
        else if (JOY_NEW(SELECT_BUTTON))
        {
            sGame->marking = !sGame->marking;
            PlaySE(SE_SELECT);
            ShowMessage(sGame->marking ? sText_Marking : sText_Controls);
        }
        else if (JOY_NEW(B_BUTTON))
        {
            sGame->state = STATE_ASK_QUIT;
            ShowMessage(sText_AskQuit);
        }
        break;
    case STATE_ASK_QUIT:
        if (JOY_NEW(A_BUTTON))
        {
            sGame->state = STATE_LEAVING;
            gTasks[taskId].func = Task_ExitVoltorbFlip;
            BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
        }
        else if (JOY_NEW(B_BUTTON))
        {
            sGame->state = STATE_PLAYING;
            ShowMessage(sText_Controls);
        }
        break;
    case STATE_LOST:
    case STATE_WON:
        if (JOY_NEW(A_BUTTON) || JOY_NEW(B_BUTTON))
        {
            ConvertIntToDecimalStringN(gStringVar1, sGame->level,
                                       STR_CONV_MODE_LEFT_ALIGN, 1);
            StringExpandPlaceholders(gStringVar4,
                                     sGame->state == STATE_WON ? sText_LevelUp
                                                               : sText_LevelDown);
            ShowMessage(gStringVar4);
            sGame->state = STATE_MESSAGE_THEN_PLAY;
        }
        break;
    case STATE_MESSAGE_THEN_PLAY:
        if (JOY_NEW(A_BUTTON) || JOY_NEW(B_BUTTON))
        {
            DealBoard();
            DrawBoard();
            MoveCursorSprite();
            PrintHeader();
            PrintHints();
            ShowMessage(sText_Controls);
            sGame->state = STATE_PLAYING;
        }
        break;
    }
}

// ---------------------------------------------------------------------------
// coming and going
// ---------------------------------------------------------------------------
static void VBlankCB_VoltorbFlip(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

static void CB2_RunVoltorbFlip(void)
{
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

static void CB2_InitVoltorbFlip(void)
{
    u8 taskId;

    SetVBlankCallback(NULL);
    ResetTasks();
    ResetSpriteData();
    FreeAllSpritePalettes();
    ResetPaletteFade();
    SetGpuReg(REG_OFFSET_DISPCNT, 0);
    ResetBgsAndClearDma3BusyFlags(FALSE);
    InitBgsFromTemplates(0, sBgTemplates, ARRAY_COUNT(sBgTemplates));
    InitWindows(sWindowTemplates);
    DeactivateAllTextPrinters();

    SetBgTilemapBuffer(0, sGame->textTilemap);
    SetBgTilemapBuffer(1, sGame->tilemap);
    FillBgTilemapBufferRect_Palette0(0, 0, 0, 0, 32, 32);

    LoadBgTiles(1, sCards_Gfx, sizeof(sCards_Gfx), 1);
    LoadPalette(sCards_Pal, BG_PLTT_ID(1), PLTT_SIZE_4BPP);
    // DrawStdWindowFrame draws from STD_WINDOW_BASE_TILE_NUM with the standard
    // palette, so load exactly what it expects rather than picking a tile base.
    LoadStdWindowFrameGfx();
    {
        u16 backdrop;
        SetBackdropColor(RGB(10, 14, 22), &backdrop);
    }

    LoadSpriteSheet(&sCursorSheet);
    LoadSpritePalette(&sCursorPalette);
    sGame->spriteId = CreateSprite(&sSpriteTemplate_Cursor, 0, 0, 0);

    DealBoard();
    DrawBoard();
    MoveCursorSprite();

    PutWindowTilemap(WIN_MESSAGE);
    PutWindowTilemap(WIN_HEADER);
    PutWindowTilemap(WIN_ROWS);
    PutWindowTilemap(WIN_COLS);
    // Only the message box is framed. A standard frame is drawn in the ring of
    // tiles *outside* its window, and there is no room for three more of those:
    // the header sits on row 0, and a frame round the column hints would land
    // on the bottom row of the board.
    DrawStdWindowFrame(WIN_MESSAGE, FALSE);
    PrintHeader();
    PrintHints();
    ShowMessage(sText_Controls);
    CopyBgTilemapBufferToVram(0);

    sGame->state = STATE_PLAYING;
    taskId = CreateTask(Task_VoltorbFlipMain, 0);
    (void)taskId;

    BlendPalettes(PALETTES_ALL, 16, RGB_BLACK);
    BeginNormalPaletteFade(PALETTES_ALL, 0, 16, 0, RGB_BLACK);
    SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_MODE_0 | DISPCNT_OBJ_1D_MAP | DISPCNT_OBJ_ON);
    ShowBg(0);
    ShowBg(1);
    SetVBlankCallback(VBlankCB_VoltorbFlip);
    SetMainCallback2(CB2_RunVoltorbFlip);
}

static void Task_ExitVoltorbFlip(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        VarSet(VAR_ORANGE_VOLTORB_LEVEL, sGame->level);
        SetVBlankCallback(NULL);
        ResetVramOamAndBgCntRegs();
        SetGpuReg(REG_OFFSET_BLDCNT, 0);
        SetGpuReg(REG_OFFSET_BLDALPHA, 0);
        SetGpuReg(REG_OFFSET_BLDY, 0);
        FreeAllWindowBuffers();
        FreeAllSpritePalettes();
        ResetPaletteFade();
        ResetSpriteData();
        Free(sGame);
        sGame = NULL;
        gFieldCallback = FieldCB_ContinueScriptHandleMusic;
        SetMainCallback2(CB2_ReturnToField);
        DestroyTask(taskId);
    }
}

static void Task_FadeToVoltorbFlip(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        DestroyTask(taskId);
        SetMainCallback2(CB2_InitVoltorbFlip);
    }
}

void PlayVoltorbFlip(void)
{
    u16 level = VarGet(VAR_ORANGE_VOLTORB_LEVEL);

    sGame = AllocZeroed(sizeof(*sGame));
    if (sGame == NULL)
        return;

    if (level < 1 || level > VF_MAX_LEVEL)
        level = 1;
    sGame->level = level;

    LockPlayerFieldControls();
    CreateTask(Task_FadeToVoltorbFlip, 0);
    BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
}
