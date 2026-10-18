# Run from the repo root, after orange_art_split.py, with the badge sheets already in place:
#   graphics/trainer_card/johto_badges.png   (8 badges, 128x16, same palette as badges.png)
#   graphics/trainer_card/orange_badges.png  (4 badges + empty space, 128x16)
# Trainer card badge pages: LEFT/RIGHT on the card front flips the badge row between KANTO,
# JOHTO (8) and ORANGE (4). A page only appears once the player has a badge from it; with only
# Kanto badges the card looks exactly like before.
#  - Johto badges: beating that gym's leader (any of the A/B/C teams) counts as the badge,
#    since the Johto gym scripts don't set badge flags.
#  - Orange badges: FLAG_BADGE_CORAL_EYE / SPIKE_SHELL / SEA_RUBY / JADE_STAR.
def rd(p): return open(p, encoding='utf-8').read()
def wr(p, s): open(p, 'w', newline='\n', encoding='utf-8').write(s)
def sub1(t, old, new, label):
    assert old in t, label
    return t.replace(old, new, 1)

p = 'src/trainer_card.c'; t = rd(p)
if 'BADGE_PAGE_ORANGE' not in t:
    t = sub1(t, '#include "constants/trainers.h"\n', '#include "constants/trainers.h"\n#include "constants/opponents.h"\n', 'inc')
    t = sub1(t, '    bool8 timeColonNeedDraw;\n', '    bool8 timeColonNeedDraw;\n    u8 badgePage;\n', 'struct')
    t = sub1(t, 'static const u32 sKantoTrainerCardBadges_Gfx[] = INCBIN_U32("graphics/trainer_card/badges.4bpp.lz");\n',
'''static const u32 sKantoTrainerCardBadges_Gfx[] = INCBIN_U32("graphics/trainer_card/badges.4bpp.lz");
static const u32 sJohtoTrainerCardBadges_Gfx[] = INCBIN_U32("graphics/trainer_card/johto_badges.4bpp.lz");
static const u32 sOrangeTrainerCardBadges_Gfx[] = INCBIN_U32("graphics/trainer_card/orange_badges.4bpp.lz");

// Badge pages (LEFT/RIGHT on the card front)
enum {
    BADGE_PAGE_KANTO,
    BADGE_PAGE_JOHTO,
    BADGE_PAGE_ORANGE,
    BADGE_PAGE_COUNT
};

static const u32 *const sBadgePageGfx[BADGE_PAGE_COUNT] = {
    sKantoTrainerCardBadges_Gfx,
    sJohtoTrainerCardBadges_Gfx,
    sOrangeTrainerCardBadges_Gfx,
};

static const u8 sBadgePageSize[BADGE_PAGE_COUNT] = {8, 8, 4};

static const u8 sText_BadgePageKanto[] = _("{LEFT_ARROW}KANTO{RIGHT_ARROW}");
static const u8 sText_BadgePageJohto[] = _("{LEFT_ARROW}JOHTO{RIGHT_ARROW}");
static const u8 sText_BadgePageOrange[] = _("{LEFT_ARROW}ORANGE{RIGHT_ARROW}");
static const u8 *const sBadgePageNames[BADGE_PAGE_COUNT] = {
    sText_BadgePageKanto,
    sText_BadgePageJohto,
    sText_BadgePageOrange,
};

// Johto: a badge = that gym's leader beaten (the gyms pick one of three teams)
static const u16 sJohtoLeaders[8][3] = {
    {TRAINER_LEADER_WALKERA,  TRAINER_LEADER_WALKERB,  TRAINER_LEADER_WALKERC},  // ZEPHYR, Violet
    {TRAINER_LEADER_BUGSYA,   TRAINER_LEADER_BUGSYB,   TRAINER_LEADER_BUGSYC},   // HIVE, Azalea
    {TRAINER_LEADER_WHITNEYA, TRAINER_LEADER_WHITNEYB, TRAINER_LEADER_WHITNEYC}, // PLAIN, Goldenrod
    {TRAINER_LEADER_MORTYA,   TRAINER_LEADER_MORTYB,   TRAINER_LEADER_MORTYC},   // FOG, Ecruteak
    {TRAINER_LEADER_CHUCKA,   TRAINER_LEADER_CHUCKB,   TRAINER_LEADER_CHUCKC},   // STORM, Cianwood
    {TRAINER_LEADER_JASMINEA, TRAINER_LEADER_JASMINEB, TRAINER_LEADER_JASMINEC}, // MINERAL, Olivine
    {TRAINER_LEADER_PRYCEA,   TRAINER_LEADER_PRYCEB,   TRAINER_LEADER_PRYCEC},   // GLACIER, Mahogany
    {TRAINER_LEADER_CLAIRA,   TRAINER_LEADER_CLAIRB,   TRAINER_LEADER_CLAIRC},   // RISING, Blackthorn
};

static const u16 sOrangeBadgeFlags[4] = {
    FLAG_BADGE_CORAL_EYE, FLAG_BADGE_SPIKE_SHELL, FLAG_BADGE_SEA_RUBY, FLAG_BADGE_JADE_STAR,
};
''', 'gfx')

    # helpers, placed before DrawStarsAndBadgesOnCard
    t = sub1(t, 'static void DrawStarsAndBadgesOnCard(void)\n{', '''static bool8 HasPageBadge(u8 page, u8 i)
{
    u8 j;

    switch (page)
    {
    case BADGE_PAGE_KANTO:
        return sTrainerCardDataPtr->hasBadge[i];
    case BADGE_PAGE_JOHTO:
        for (j = 0; j < 3; j++)
            if (FlagGet(TRAINER_FLAGS_START + sJohtoLeaders[i][j]))
                return TRUE;
        return FALSE;
    case BADGE_PAGE_ORANGE:
        return FlagGet(sOrangeBadgeFlags[i]);
    }
    return FALSE;
}

// Kanto is always there; another page shows up once it has a badge. Never on a link card.
static bool8 IsBadgePageAvailable(u8 page)
{
    u8 i;

    if (page == BADGE_PAGE_KANTO)
        return TRUE;
    if (sTrainerCardDataPtr->isLink)
        return FALSE;
    for (i = 0; i < sBadgePageSize[page]; i++)
        if (HasPageBadge(page, i))
            return TRUE;
    return FALSE;
}

static u8 CountBadgePages(void)
{
    u8 page, count = 0;

    for (page = 0; page < BADGE_PAGE_COUNT; page++)
        if (IsBadgePageAvailable(page))
            count++;
    return count;
}

static void PrintBadgePageLabel(void)
{
    const u8 *str = sBadgePageNames[sTrainerCardDataPtr->badgePage];
    s32 x;

    if (sTrainerCardDataPtr->isLink || sTrainerCardDataPtr->cardType == CARD_TYPE_RSE || CountBadgePages() < 2)
        return;
    FillWindowPixelRect(1, PIXEL_FILL(0), 136, 107, 80, 13);
    x = 212 - GetStringWidth(FONT_SMALL, str, 0);
    AddTextPrinterParameterized3(1, FONT_SMALL, x, 107, sTrainerCardTextColors, TEXT_SKIP_DRAW, str);
    CopyWindowToVram(1, COPYWIN_GFX);
}

// LEFT/RIGHT on the card front: next/previous page that has a badge
static bool8 ChangeBadgePage(s8 delta)
{
    u8 page = sTrainerCardDataPtr->badgePage;
    u8 i;

    if (sTrainerCardDataPtr->isLink || sTrainerCardDataPtr->cardType == CARD_TYPE_RSE || CountBadgePages() < 2)
        return FALSE;
    for (i = 0; i < BADGE_PAGE_COUNT; i++)
    {
        page = (page + BADGE_PAGE_COUNT + delta) % BADGE_PAGE_COUNT;
        if (IsBadgePageAvailable(page))
            break;
    }
    if (page == sTrainerCardDataPtr->badgePage)
        return FALSE;
    sTrainerCardDataPtr->badgePage = page;
    LZ77UnCompWram(sBadgePageGfx[page], sTrainerCardDataPtr->badgeTiles);
    LoadBgTiles(3, sTrainerCardDataPtr->badgeTiles, NELEMS(sTrainerCardDataPtr->badgeTiles), 0);
    DrawStarsAndBadgesOnCard();
    return TRUE;
}

static void DrawStarsAndBadgesOnCard(void)
{''', 'helpers')
    t = sub1(t, 'static void DrawStarsAndBadgesOnCard(void);\n',
             'static void DrawStarsAndBadgesOnCard(void);\nstatic bool8 ChangeBadgePage(s8 delta);\nstatic void PrintBadgePageLabel(void);\n', 'proto')

    # draw the current page
    t = sub1(t, '''        x = 4;
        for (i = 0; i < NUM_BADGES; i++, tileNum += 2, x += 3)
        {
            if (sTrainerCardDataPtr->hasBadge[i])
            {''', '''        x = 4;
        FillBgTilemapBufferRect_Palette0(3, 0, 4, 16, 24, 2); // clear the badge row
        for (i = 0; i < sBadgePageSize[sTrainerCardDataPtr->badgePage]; i++, tileNum += 2, x += 3)
        {
            if (HasPageBadge(sTrainerCardDataPtr->badgePage, i))
            {''', 'draw')
    t = sub1(t, '''    CopyBgTilemapBufferToVram(3);
}

static void DrawCardBackStats(void)''', '''    CopyBgTilemapBufferToVram(3);
    if (!sTrainerCardDataPtr->isLink && sTrainerCardDataPtr->cardType != CARD_TYPE_RSE)
    {
        // Pages with fewer than 8 badges: cover the unused slot boxes with the plain strip
        // (column 6 is a gap between boxes); other pages get the card's boxes back.
        u16 *dst = sTrainerCardDataPtr->cardTilemapBuffer;
        const u16 *src = sTrainerCardDataPtr->frontTilemap;
        s16 row, col;

        for (row = 16; row <= 17; row++)
        {
            for (col = 4 + 3 * sBadgePageSize[BADGE_PAGE_ORANGE]; col < 4 + 3 * NUM_BADGES; col++)
            {
                if (col >= 4 + 3 * sBadgePageSize[sTrainerCardDataPtr->badgePage])
                    dst[32 * row + col] = src[30 * row + 6];
                else
                    dst[32 * row + col] = src[30 * row + col];
            }
        }
        CopyBgTilemapBufferToVram(0);
    }
    PrintBadgePageLabel();
}

static void DrawCardBackStats(void)''', 'draw end')

    # input on the card front
    t = sub1(t, '''        if (JOY_NEW(A_BUTTON))
        {
            SetHelpContext(HELPCONTEXT_TRAINER_CARD_BACK);''', '''        if (JOY_NEW(DPAD_LEFT | DPAD_RIGHT))
        {
            if (ChangeBadgePage(JOY_NEW(DPAD_RIGHT) ? 1 : -1))
                PlaySE(SE_SELECT);
        }
        else if (JOY_NEW(A_BUTTON))
        {
            SetHelpContext(HELPCONTEXT_TRAINER_CARD_BACK);''', 'input')
    wr(p, t)
print('badge pages ok')
