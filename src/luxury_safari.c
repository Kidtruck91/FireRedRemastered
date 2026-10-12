// Six Island Luxury Safari: lets the player pick which habitat (encounter set) to hunt in.
//
// The habitats are the consecutive MAP_SIX_ISLAND_SAFARI_1 entries in src/data/wild_encounters.json
// (Forest, Meadow, Desert, ...). VAR_LUXURY_SAFARI_HABITAT picks which one is active; see
// GetCurrentMapWildMonHeaderId() in wild_encounter.c. The names below must stay in the same order.

#include "global.h"
#include "data.h"
#include "event_data.h"
#include "list_menu.h"
#include "main.h"
#include "malloc.h"
#include "menu.h"
#include "new_menu_helpers.h"
#include "script.h"
#include "sound.h"
#include "string_util.h"
#include "strings.h"
#include "task.h"
#include "text.h"
#include "text_window.h"
#include "wild_encounter.h"
#include "window.h"
#include "constants/songs.h"

#define LIST_WINDOW_TOP    1
#define LIST_WINDOW_HEIGHT 14
#define PREVIEW_LEFT       11
#define PREVIEW_WIDTH      18
#define PREVIEW_HEIGHT     18
#define PREVIEW_LINE       10
#define PREVIEW_COLUMN     72
#define MAX_PREVIEW_MONS   16

static const u8 sText_Forest[]  = _("FOREST");
static const u8 sText_Meadow[]  = _("MEADOW");
static const u8 sText_Desert[]  = _("DESERT");
static const u8 sText_Volcano[] = _("VOLCANO");
static const u8 sText_Jungle[]  = _("JUNGLE");
static const u8 sText_Coast[]   = _("COAST");
static const u8 sText_Meteor[]  = _("METEOR");
static const u8 sText_Water[]   = _("ON THE WATER");
static const u8 sText_AreaGrounds[] = _("{LEFT_ARROW} SAFARI GROUNDS {RIGHT_ARROW}");
static const u8 sText_AreaDesert[]  = _("{LEFT_ARROW} DESERT {RIGHT_ARROW}");
static const u8 sText_AreaVolcano[] = _("{LEFT_ARROW} VOLCANO CAVE {RIGHT_ARROW}");
static const u8 sText_AreaBeach[]   = _("{LEFT_ARROW} BEACH {RIGHT_ARROW}");
static const u8 sText_AreaIce[]     = _("{LEFT_ARROW} ICE CAVE {RIGHT_ARROW}");
static const u8 sText_LandGrass[]   = _("IN THE GRASS");
static const u8 sText_LandSand[]    = _("ON THE DEEP SAND");
static const u8 sText_LandCave[]    = _("IN THE CAVE");

// Same order as sLuxurySafariAreaMaps in wild_encounter.c
static const u8 *const sAreaNames[NUM_LUXURY_SAFARI_AREAS] =
{
    sText_AreaGrounds, sText_AreaDesert, sText_AreaVolcano, sText_AreaBeach, sText_AreaIce,
};
static const u8 *const sAreaLandNames[NUM_LUXURY_SAFARI_AREAS] =
{
    sText_LandGrass, sText_LandSand, sText_LandCave, sText_LandGrass, sText_LandCave,
};
static const u8 sText_Cancel[]  = _("CANCEL");
static const u8 sText_NoMons[]  = _("-");
static const u8 sText_CancelHint[] = _("Leave without\nchoosing a habitat.");

static const u8 *const sHabitatNames[NUM_LUXURY_SAFARI_HABITATS] =
{
    sText_Forest,
    sText_Meadow,
    sText_Desert,
    sText_Volcano,
    sText_Jungle,
    sText_Coast,
    sText_Meteor,
};

static const struct ListMenuItem sHabitatItems[NUM_LUXURY_SAFARI_HABITATS + 1] =
{
    { sText_Forest,  0 },
    { sText_Meadow,  1 },
    { sText_Desert,  2 },
    { sText_Volcano, 3 },
    { sText_Jungle,  4 },
    { sText_Coast,   5 },
    { sText_Meteor,  6 },
    { sText_Cancel,  LIST_CANCEL },
};

static const u8 sColor_Name[]   = { TEXT_COLOR_TRANSPARENT, TEXT_COLOR_DARK_GRAY, TEXT_COLOR_LIGHT_GRAY };
static const u8 sColor_Grass[]  = { TEXT_COLOR_TRANSPARENT, TEXT_COLOR_GREEN, TEXT_COLOR_LIGHT_GREEN };
static const u8 sColor_Water[]  = { TEXT_COLOR_TRANSPARENT, TEXT_COLOR_BLUE, TEXT_COLOR_LIGHT_BLUE };

struct LuxurySafariMenu
{
    s32 habitat;
    u8 area;
    u8 listWindowId;
    u8 previewWindowId;
    u8 listTaskId;
};

static struct LuxurySafariMenu *sMenu;

static void MoveCursorFunc(s32 itemIndex, bool8 onInit, struct ListMenu *list);
static void Task_HandleHabitatMenu(u8 taskId);

static const struct ListMenuTemplate sHabitatListTemplate =
{
    .items = sHabitatItems,
    .moveCursorFunc = MoveCursorFunc,
    .itemPrintFunc = NULL,
    .totalItems = NUM_LUXURY_SAFARI_HABITATS + 1,
    .maxShowed = NUM_LUXURY_SAFARI_HABITATS + 1,
    .windowId = 0,
    .header_X = 0,
    .item_X = 8,
    .cursor_X = 0,
    .upText_Y = 0,
    .cursorPal = 2,
    .fillValue = 1,
    .cursorShadowPal = 3,
    .lettersSpacing = 1,
    .itemVerticalPadding = 0,
    .scrollMultiple = 0,
    .fontId = FONT_NORMAL,
    .cursorKind = 0,
};

static u8 AddUniqueSpecies(u16 *list, u8 count, u16 species)
{
    u8 i;

    if (species == SPECIES_NONE || count >= MAX_PREVIEW_MONS)
        return count;
    for (i = 0; i < count; i++)
    {
        if (list[i] == species)
            return count;
    }
    list[count] = species;
    return count + 1;
}

// Prints a section header and its species in two columns; returns the y below it.
static u8 PrintSection(u8 windowId, u8 y, const u8 *header, const u8 *headerColor, const u16 *species, u8 count)
{
    u8 i;

    AddTextPrinterParameterized3(windowId, FONT_SMALL, 0, y, headerColor, TEXT_SKIP_DRAW, header);
    y += PREVIEW_LINE + 1;
    if (count == 0)
    {
        AddTextPrinterParameterized3(windowId, FONT_SMALL, 4, y, sColor_Name, TEXT_SKIP_DRAW, sText_NoMons);
        return y + PREVIEW_LINE;
    }
    for (i = 0; i < count; i++)
        AddTextPrinterParameterized3(windowId, FONT_SMALL, 4 + (i % 2) * PREVIEW_COLUMN, y + (i / 2) * PREVIEW_LINE,
                                     sColor_Name, TEXT_SKIP_DRAW, gSpeciesNames[species[i]]);
    return y + ((count + 1) / 2) * PREVIEW_LINE;
}

static void DrawHabitatPreview(s32 habitat)
{
    u8 area = sMenu->area;
    u8 windowId = sMenu->previewWindowId;
    const struct WildPokemonHeader *header;
    u16 land[MAX_PREVIEW_MONS];
    u16 water[MAX_PREVIEW_MONS];
    u8 landCount = 0;
    u8 waterCount = 0;
    u8 i, y;

    FillWindowPixelBuffer(windowId, PIXEL_FILL(1));
    if (habitat < 0 || habitat >= NUM_LUXURY_SAFARI_HABITATS)
    {
        AddTextPrinterParameterized3(windowId, FONT_SMALL, 0, 0, sColor_Name, TEXT_SKIP_DRAW, sText_CancelHint);
        CopyWindowToVram(windowId, COPYWIN_GFX);
        return;
    }

    header = GetLuxurySafariHeader(area, habitat);
    if (header != NULL)
    {
        if (header->landMonsInfo != NULL)
        {
            for (i = 0; i < LAND_WILD_COUNT; i++)
                landCount = AddUniqueSpecies(land, landCount, header->landMonsInfo->wildPokemon[i].species);
        }
        if (header->waterMonsInfo != NULL)
        {
            for (i = 0; i < WATER_WILD_COUNT; i++)
                waterCount = AddUniqueSpecies(water, waterCount, header->waterMonsInfo->wildPokemon[i].species);
        }
        if (header->fishingMonsInfo != NULL)
        {
            for (i = 0; i < FISH_WILD_COUNT; i++)
                waterCount = AddUniqueSpecies(water, waterCount, header->fishingMonsInfo->wildPokemon[i].species);
        }
    }

    AddTextPrinterParameterized3(windowId, FONT_SMALL, 0, 0, sColor_Name, TEXT_SKIP_DRAW, sAreaNames[area]);
    y = PREVIEW_LINE + 4;
    if (landCount != 0)
        y = PrintSection(windowId, y, sAreaLandNames[area], sColor_Grass, land, landCount) + 2;
    if (waterCount != 0)
        PrintSection(windowId, y, sText_Water, sColor_Water, water, waterCount);
    CopyWindowToVram(windowId, COPYWIN_GFX);
}

static void MoveCursorFunc(s32 itemIndex, bool8 onInit, struct ListMenu *list)
{
    if (!onInit)
        PlaySE(SE_SELECT);
    sMenu->habitat = itemIndex;
    DrawHabitatPreview(itemIndex);
}

// callnative LuxurySafari_ChooseHabitat, then waitstate.
// VAR_RESULT = chosen habitat (also stored in VAR_LUXURY_SAFARI_HABITAT, name in STR_VAR_1), or 0x7F if cancelled.
void LuxurySafari_ChooseHabitat(void)
{
    struct WindowTemplate template;
    struct ListMenuTemplate listTemplate;

    sMenu = AllocZeroed(sizeof(*sMenu));

    template = SetWindowTemplateFields(0, 1, LIST_WINDOW_TOP, 8, LIST_WINDOW_HEIGHT, 15, 0x038);
    sMenu->listWindowId = AddWindow(&template);
    template = SetWindowTemplateFields(0, PREVIEW_LEFT, 1, PREVIEW_WIDTH, PREVIEW_HEIGHT, 15, 0x038 + 8 * LIST_WINDOW_HEIGHT);
    sMenu->previewWindowId = AddWindow(&template);

    SetStdWindowBorderStyle(sMenu->listWindowId, FALSE);
    SetStdWindowBorderStyle(sMenu->previewWindowId, FALSE);
    FillWindowPixelBuffer(sMenu->previewWindowId, PIXEL_FILL(1));

    listTemplate = sHabitatListTemplate;
    listTemplate.windowId = sMenu->listWindowId;
    sMenu->listTaskId = ListMenuInit(&listTemplate, 0, 0);

    PutWindowTilemap(sMenu->listWindowId);
    PutWindowTilemap(sMenu->previewWindowId);
    CopyWindowToVram(sMenu->listWindowId, COPYWIN_FULL);
    CopyWindowToVram(sMenu->previewWindowId, COPYWIN_FULL);
    CreateTask(Task_HandleHabitatMenu, 8);
}

static void CloseHabitatMenu(u8 taskId)
{
    DestroyListMenuTask(sMenu->listTaskId, NULL, NULL);
    ClearStdWindowAndFrameToTransparent(sMenu->previewWindowId, FALSE);
    ClearStdWindowAndFrameToTransparent(sMenu->listWindowId, TRUE);
    RemoveWindow(sMenu->previewWindowId);
    RemoveWindow(sMenu->listWindowId);
    FREE_AND_SET_NULL(sMenu);
    DestroyTask(taskId);
    ScriptContext_Enable();
}

static void Task_HandleHabitatMenu(u8 taskId)
{
    s32 input;

    // Left/Right flip the preview between the safari areas
    if (JOY_NEW(DPAD_LEFT) || JOY_NEW(DPAD_RIGHT))
    {
        if (JOY_NEW(DPAD_LEFT))
            sMenu->area = (sMenu->area + NUM_LUXURY_SAFARI_AREAS - 1) % NUM_LUXURY_SAFARI_AREAS;
        else
            sMenu->area = (sMenu->area + 1) % NUM_LUXURY_SAFARI_AREAS;
        PlaySE(SE_SELECT);
        DrawHabitatPreview(sMenu->habitat);
        return;
    }
    input = ListMenu_ProcessInput(sMenu->listTaskId);
    if (input == LIST_NOTHING_CHOSEN)
        return;

    PlaySE(SE_SELECT);
    if (input == LIST_CANCEL || input < 0 || input >= NUM_LUXURY_SAFARI_HABITATS)
    {
        gSpecialVar_Result = 0x7F;
    }
    else
    {
        gSpecialVar_Result = input;
        VarSet(VAR_LUXURY_SAFARI_HABITAT, input);
        StringCopy(gStringVar1, sHabitatNames[input]);
    }
    CloseHabitatMenu(taskId);
}

// callnative LuxurySafari_BufferHabitatName: STR_VAR_1 = name of the current habitat.
void LuxurySafari_BufferHabitatName(void)
{
    u16 habitat = VarGet(VAR_LUXURY_SAFARI_HABITAT);

    if (habitat >= NUM_LUXURY_SAFARI_HABITATS)
        habitat = 0;
    StringCopy(gStringVar1, sHabitatNames[habitat]);
}
