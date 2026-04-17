#include "trainer_card_gui.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "ribbon_sheet.h"
#include "shiny_sparkle_anim.h"
#include "shiny_charm_sheet.h"
#include "game_state.h"
#include "utils.h"
#include "dialog_box.h"
#include "raylib.h"
#include "ribbons.h"

#define ITEM_WIDTH 100
#define ITEM_HEIGHT 75
#define ITEM_GRID_MARGIN_X 21
#define ITEM_GRID_MARGIN_Y 55
#define RIBBONS_PER_PAGE 12
#define RIBBONS_PER_ROW 4

#define RIBBON_NAME_FONT_SIZE 20
#define TRAINER_CARD_SELECTION_ANIM_DURATION 30

static void DrawSkeleton();

static void DrawRibbonBox(TrainerCardGUI *tcg, Vector2 position, bool isSelected, Color, Color);
static void DrawRibbonName(const char *name, Vector2 position);
static void DrawSparkle(Vector2 pos, float scale, Color color);

static void DrawRibbon(TrainerCardGUI *tcg, Ribbon *ribbon, Vector2 position, bool isSelected);

static void DrawRibbonGrid(TrainerCardGUI *tcg, Vector2 startPos, float marginX, float marginY);
static void DrawPlayerStats(TrainerCardGUI *tcg, Vector2 position);
static void DrawShinyCharmProgress(TrainerCardGUI *tcg, Vector2 position);

static unsigned long long GetPlayedDays(const GameState *state);
static unsigned long long GetPeakDayKs(const GameState *state);

void TrainerCardGUIInit(TrainerCardGUI *tcg, RibbonSheet *ribbonSheet, ShinySparkleSheet *shinySparkleSheet, ShinyCharmSheet *shinyCharmSheet, GameState *gameState)
{
    tcg->gameState = gameState;
    tcg->ribbonSheet = ribbonSheet;
    tcg->shinyCharmSheet = shinyCharmSheet;

    tcg->shinySparkleAnim = malloc(sizeof(ShinySparkleAnim));
    if (tcg->shinySparkleAnim == NULL)
    {
        TraceLog(LOG_ERROR, "Failed to allocate memory for ShinySparkleAnim");
        exit(1);
    }
    ShinySparkleAnimInit(tcg->shinySparkleAnim, 60, true, shinySparkleSheet);

    tcg->descriptionBox = malloc(sizeof(DialogBox));
    if (tcg->descriptionBox == NULL)
    {
        TraceLog(LOG_ERROR, "Failed to allocate memory for DialogBox in TrainerCard GUI");
        exit(1);
    }
    DialogBoxInit(
        tcg->descriptionBox,
        "",
        0, (Rectangle){25, 550, TRAINER_CARD_WINDOW_WIDTH - 50, 100},
        20, 4, 0);

    tcg->page = 0;
    tcg->selectedRibbon = 0;
    tcg->elapsedFrames = 0;
    tcg->selectionEnabled = true;
}

void TrainerCardGUIUpdate(TrainerCardGUI *tcg)
{
    unsigned int nRibbons = min(RIBBONS_PER_PAGE, RibbonGetCount() - tcg->page * RIBBONS_PER_PAGE);
    unsigned int totalPages = ceil(RibbonGetCount() / (float)RIBBONS_PER_PAGE);
    unsigned int totalRows = RIBBONS_PER_PAGE / RIBBONS_PER_ROW;

    if (IsKeyPressed(KEY_RIGHT))
    {
        if ((tcg->selectedRibbon + 1) % RIBBONS_PER_ROW == 0 && tcg->page < totalPages - 1)
        {
            tcg->page++;
            tcg->selectedRibbon = 0;
            tcg->elapsedFrames = 0;
            tcg->selectionEnabled = true;
        }
        else if (tcg->selectedRibbon < nRibbons - 1)
        {
            tcg->selectedRibbon++;
            tcg->elapsedFrames = 0;
            tcg->selectionEnabled = true;
        }
    }
    else if (IsKeyPressed(KEY_LEFT))
    {
        if (tcg->selectedRibbon % RIBBONS_PER_ROW == 0 && tcg->page > 0)
        {
            tcg->page--;
            tcg->selectedRibbon = 0;

            tcg->elapsedFrames = 0;
            tcg->selectionEnabled = true;
        }
        else if (tcg->selectedRibbon % RIBBONS_PER_ROW != 0)
        {
            tcg->selectedRibbon--;
            tcg->elapsedFrames = 0;
            tcg->selectionEnabled = true;
        }
    }
    else if (IsKeyPressed(KEY_DOWN))
    {
        unsigned int currentRow = tcg->selectedRibbon / RIBBONS_PER_ROW;
        if (currentRow < totalRows - 1)
        {
            tcg->selectedRibbon += RIBBONS_PER_ROW;
            tcg->elapsedFrames = 0;
            tcg->selectionEnabled = true;
        }
    }
    else if (IsKeyPressed(KEY_UP))
    {
        unsigned int currentRow = tcg->selectedRibbon / RIBBONS_PER_ROW;
        if (currentRow > 0)
        {
            tcg->selectedRibbon -= RIBBONS_PER_ROW;
            tcg->elapsedFrames = 0;
            tcg->selectionEnabled = true;
        }
    }

    ShinySparkleAnimUpdate(tcg->shinySparkleAnim);
    DialogBoxClearAndUpdateText(tcg->descriptionBox,
                                TextFitting(
                                    RIBBONS[tcg->page * RIBBONS_PER_PAGE + tcg->selectedRibbon].description,
                                    TRAINER_CARD_WINDOW_WIDTH - 100, NULL, 20, 2));
    DialogBoxUpdate(tcg->descriptionBox);

    if (tcg->elapsedFrames > TRAINER_CARD_SELECTION_ANIM_DURATION)
    {
        tcg->elapsedFrames = 0;
        tcg->selectionEnabled = !tcg->selectionEnabled;
    }
    tcg->elapsedFrames++;
}

void TrainerCardGUIDraw(TrainerCardGUI *tcg)
{
    unsigned int totalPages = ceil(RibbonGetCount() / (float)RIBBONS_PER_PAGE);

    ClearBackground(BLANK);
    DrawSkeleton();

    DrawShinyCharmProgress(tcg, (Vector2){370, 30});
    DrawPlayerStats(tcg, (Vector2){30, 30});
    DrawRibbonGrid(tcg, (Vector2){27, 150}, ITEM_GRID_MARGIN_X, ITEM_GRID_MARGIN_Y);
    DialogBoxDraw(tcg->descriptionBox);
    DrawText(TextFormat("%d/%d", tcg->page + 1, totalPages), TRAINER_CARD_WINDOW_WIDTH - 60, TRAINER_CARD_WINDOW_HEIGHT - 40, 20, BLACK);
}

void TrainerCardGUIUnload(TrainerCardGUI *tcg)
{
    DialogBoxUnload(tcg->descriptionBox);
    free(tcg->descriptionBox);

    ShinySparkleAnimUnload(tcg->shinySparkleAnim);
    free(tcg->shinySparkleAnim);
}

void DrawSkeleton()
{
    SetWindowSize(TRAINER_CARD_WINDOW_WIDTH, TRAINER_CARD_WINDOW_HEIGHT);
    DrawRectangleRounded(
        (Rectangle){0, 0, TRAINER_CARD_WINDOW_WIDTH, TRAINER_CARD_WINDOW_HEIGHT}, 0.05, 10, (Color){67, 49, 100, 255});

    DrawRectangleRounded(
        (Rectangle){5, 5, TRAINER_CARD_WINDOW_WIDTH - 10, TRAINER_CARD_WINDOW_HEIGHT - 10}, 0.05, 10, (Color){48, 30, 100, 255});

    DrawRectangleRounded(
        (Rectangle){8, 8, TRAINER_CARD_WINDOW_WIDTH - 16, TRAINER_CARD_WINDOW_HEIGHT - 16}, 0.05f, 10, (Color){141, 142, 218, 255});

    DrawRectangleRounded(
        (Rectangle){11, 11, TRAINER_CARD_WINDOW_WIDTH - 22, TRAINER_CARD_WINDOW_HEIGHT - 22}, 0.05, 10, (Color){48, 30, 100, 255});

    DrawRectangleRounded(
        (Rectangle){14, 14, TRAINER_CARD_WINDOW_WIDTH - 28, TRAINER_CARD_WINDOW_HEIGHT - 28}, 0.05, 10, (Color){210, 240, 234, 255});
}

void DrawRibbonGrid(TrainerCardGUI *tcg, Vector2 startPos, float marginX, float marginY)
{
    unsigned int nRibbons = min(RIBBONS_PER_PAGE, RibbonGetCount() - tcg->page * RIBBONS_PER_PAGE);

    for (int i = 0; i < nRibbons; i++)
    {
        // Calculate current row and column
        int column = i % RIBBONS_PER_ROW;
        int row = i / RIBBONS_PER_ROW;

        // Calculate the specific position for this ribbon
        Vector2 currentPos = {
            startPos.x + (column * (ITEM_WIDTH + marginX)),
            startPos.y + (row * (ITEM_HEIGHT + marginY))};

        DrawRibbon(tcg, &RIBBONS[tcg->page * RIBBONS_PER_PAGE + i], currentPos, i == tcg->selectedRibbon);
    }
}

void DrawRibbonBox(TrainerCardGUI *tcg, Vector2 position, bool isSelected, Color borderColor, Color bgColor)
{
    unsigned int borderThickness = 4;

    if (tcg->selectionEnabled && isSelected)
    {
        borderColor = RED;
    }
    DrawRectangleRounded(
        (Rectangle){position.x, position.y, ITEM_WIDTH, ITEM_HEIGHT}, 0.1f, 4, borderColor);

    DrawRectangleRoundedLinesEx(
        (Rectangle){position.x, position.y, ITEM_WIDTH, ITEM_HEIGHT}, 0.1f, 4, 2, BLACK);

    DrawRectangleRounded(
        (Rectangle){position.x + borderThickness, position.y + borderThickness, ITEM_WIDTH - borderThickness * 2, ITEM_HEIGHT - borderThickness * 2}, 0.1f, 4, bgColor);

    DrawRectangleRoundedLinesEx(
        (Rectangle){position.x + borderThickness, position.y + borderThickness, ITEM_WIDTH - borderThickness * 2, ITEM_HEIGHT - borderThickness * 2}, 0.1f, 4, 2, BLACK);
}

void DrawRibbonName(const char *name, Vector2 position)
{
    const char *fittedName = TextFitting(name, ITEM_WIDTH, NULL, RIBBON_NAME_FONT_SIZE, RIBBON_NAME_FONT_SIZE / 10.0f);
    DrawText(fittedName, position.x, position.y, RIBBON_NAME_FONT_SIZE, BLACK);
}

void DrawSparkle(Vector2 pos, float scale, Color color)
{
    int s = (int)(4 * scale); // Base size
    // Horizontal bar
    DrawRectangle(pos.x - s, pos.y - 1, s * 2, 2, color);
    // Vertical bar
    DrawRectangle(pos.x - 1, pos.y - s, 2, s * 2, color);
}

void DrawRibbon(TrainerCardGUI *tcg, Ribbon *ribbon, Vector2 position, bool isSelected)
{
    float scale = 1.8f;
    unsigned char variant;
    unsigned char status = ribbon->checkStatus(tcg->gameState);
    bool drawSparklesAnin = false;
    bool drawBgSparkles = false;

    Color border = WHITE;
    Color bg = WHITE;

    if (status & BW_RIBBON_VARIANT)
    {
        drawSparklesAnin = true;
        drawBgSparkles = true;
        variant = BW_RIBBON_VARIANT;
        border = (Color){184, 73, 198, 255};
        bg = (Color){188, 133, 234, 255};
    }
    else if (status & RIBBON_SHINY_FLAG)
    {
        drawSparklesAnin = true;
        variant = SHINY_RIBBON_VARIANT;
        border = (Color){208, 172, 134, 255};
        bg = (Color){118, 171, 189, 255};
    }
    else if (status & RIBBON_REGULAR_FLAG)
    {
        variant = REGULAR_RIBBON_VARIANT;
    }
    else
    {
        variant = SILHOUETTE_RIBBON_VARIANT;
    }

    DrawRibbonBox(tcg, position, isSelected, border, bg);
    RibbonSheetDraw(
        tcg->ribbonSheet,
        (Vector2){
            position.x + ITEM_WIDTH / 2 - SHINY_RIBBON_WIDTH * scale / 2,
            position.y + ITEM_HEIGHT / 2 - RIBBON_HEIGHT * scale / 2},
        variant, scale);

    if (drawSparklesAnin)
    {
        ShinySparkleAnimDraw(tcg->shinySparkleAnim,
                             (Vector2){
                                 position.x + ITEM_WIDTH / 2 - SHINY_SPARKLE_SIZE / 2,
                                 position.y + ITEM_HEIGHT / 2 - SHINY_SPARKLE_SIZE / 2});
    }

    if (drawBgSparkles)
    {
        DrawSparkle(
            (Vector2){position.x + 15, position.y + 30},
            1.2f,
            (Color){154, 118, 185, 255});

        DrawSparkle(
            (Vector2){position.x + 30, position.y + 10},
            0.8f,
            (Color){154, 118, 185, 255});

        DrawSparkle(
            (Vector2){position.x + 80, position.y + 30},
            1.5f,
            (Color){154, 118, 185, 255});

        DrawSparkle(
            (Vector2){position.x + 85, position.y + 60},
            0.8f,
            (Color){154, 118, 185, 255});

        DrawSparkle(
            (Vector2){position.x + 18, position.y + 70},
            0.6f,
            (Color){154, 118, 185, 255});
    }

    DrawRibbonName(ribbon->name, (Vector2){position.x, position.y + ITEM_HEIGHT + 5});
}

void DrawPlayerStats(TrainerCardGUI *tcg, Vector2 position)
{
    int fontSize = 20;
    int lineHeight = fontSize * 1.2f;
    DrawText("# PLAYED DAYS:", position.x, position.y, fontSize, BLACK);
    DrawText("# STROKES:", position.x, position.y + lineHeight, fontSize, BLACK);
    DrawText("# PEAK DAY KS:", position.x, position.y + lineHeight * 2, fontSize, BLACK);
    DrawText("# POKEDEX:", position.x, position.y + lineHeight * 3, fontSize, BLACK);

    int maxLength = MeasureText("# PEAK DAY KS:", fontSize);
    unsigned long long totalKs = GameStateGetTotalKeyStrokes(tcg->gameState);
    unsigned long long playedDays = GetPlayedDays(tcg->gameState);
    unsigned long long peakDayKs = GetPeakDayKs(tcg->gameState);
    unsigned int pokedexCount = PokedexNumRegistered(tcg->gameState->pokedex, 0);

    DrawText(TextFormat("%s day(s).", FormatNumber(playedDays)), position.x + maxLength + 20, position.y, fontSize, BLACK);
    DrawText(TextFormat("%s ks", FormatNumber(totalKs)), position.x + maxLength + 20, position.y + lineHeight, fontSize, BLACK);
    DrawText(TextFormat("%s ks", FormatNumber(peakDayKs)), position.x + maxLength + 20, position.y + lineHeight * 2, fontSize, BLACK);

    float pbarWidth = 100.0f;
    float pbarHeight = lineHeight / 2;
    float pbarX = position.x + maxLength + 15;
    float pbarY = position.y + lineHeight * 3 + lineHeight / 2 - pbarHeight / 2;
    float pbarProgress = (float)pokedexCount / 251;
    float pbarInset = 2.0f;
    DrawRectangleRoundedLinesEx((Rectangle){pbarX, pbarY, 100, pbarHeight}, 0.1f, 4, 2, BLACK);
    DrawRectangleRounded(
        (Rectangle){
            pbarX + pbarInset,
            pbarY + pbarInset,
            (100.0f * pokedexCount / 251) - pbarInset * 2,
            pbarHeight - pbarInset * 2},
        0.1f, 4, (Color){48, 30, 100, 255});

    char formattedPokedexCount[128];
    strcpy(formattedPokedexCount, TextFormat("%s/251", FormatNumber(pokedexCount)));
    float pokedexTextWidth = MeasureText(formattedPokedexCount, fontSize / 1.5f);
    DrawText(formattedPokedexCount, pbarX + pbarWidth - pokedexTextWidth, pbarHeight + pbarY + 5, fontSize / 1.5f, BLACK);
}

void DrawShinyCharmProgress(TrainerCardGUI *tcg, Vector2 position)
{
    int width = 120;
    int height = 100;
    int borderThickness = 4;
    DrawRectangleRounded(
        (Rectangle){position.x, position.y, width, height}, 0.1f, 4, WHITE);

    DrawRectangleRoundedLinesEx(
        (Rectangle){position.x, position.y, width, height}, 0.1f, 4, 2, BLACK);

    DrawRectangleRoundedLinesEx(
        (Rectangle){position.x + borderThickness, position.y + borderThickness, width - borderThickness * 2, height - borderThickness * 2}, 0.1f, 4, 2, BLACK);

    bool isShinyCharmCompleted = RibbonShinyCharmAvailable(tcg->gameState);
    Vector2 shinyCharmPos = (Vector2){position.x + width / 2 - SHINY_CHARM_SIZE / 2, position.y + height / 2 - SHINY_CHARM_SIZE / 2};
    ShinyCharmSheetDraw(tcg->shinyCharmSheet, shinyCharmPos, !isShinyCharmCompleted);
    if (isShinyCharmCompleted)
    {
        ShinySparkleAnimDraw(tcg->shinySparkleAnim,
                             (Vector2){
                                 position.x + width / 2 - SHINY_SPARKLE_SIZE / 2,
                                 position.y + height / 2 - SHINY_SPARKLE_SIZE / 2});
    }
}

unsigned long long GetPlayedDays(const GameState *state)
{
    unsigned long long playedDays = 0;
    for (unsigned int i = 0; i < state->currentCapacity; i++)
    {
        if (state->keyStrokesByDay[i] != NO_DAY_DATA)
        {
            playedDays++;
        }
    }
    return playedDays;
}

unsigned long long GetPeakDayKs(const GameState *state)
{
    unsigned long long peakDayKs = 0;
    for (unsigned int i = 0; i < state->currentCapacity; i++)
    {
        if (state->keyStrokesByDay[i] != NO_DAY_DATA && state->keyStrokesByDay[i] > peakDayKs)
        {
            peakDayKs = state->keyStrokesByDay[i];
        }
    }
    return peakDayKs;
}