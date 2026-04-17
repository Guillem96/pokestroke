#include "trainer_card_gui.h"

#include <stdlib.h>
#include <string.h>

#include "ribbon_sheet.h"
#include "shiny_sparkle_anim.h"
#include "shiny_charm_sheet.h"
#include "game_state.h"
#include "utils.h"
#include "dialog_box.h"
#include "raylib.h"

#define ITEM_WIDTH 100
#define ITEM_HEIGHT 80

static void DrawSkeleton();

static void DrawRibbonBox(TrainerCardGUI *tcg, Vector2 position, Color, Color);
static void DrawRibbonName(const char *name, Vector2 position);

static void DrawNonCompletedRibbon(TrainerCardGUI *tcg, Vector2 position);
static void DrawCompletedRibbon(TrainerCardGUI *tcg, Vector2 position);
static void DrawShinyRibbon(TrainerCardGUI *tcg, Vector2 position);

static void DrawBWRibbon(TrainerCardGUI *tcg, Vector2 position);
static void DrawRibbonGrid(TrainerCardGUI *tcg, Vector2 startPos, int columns, int totalElements, float marginX, float marginY);
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
        TextFitting("this is guillem speaking, and this is a long description",
                    TRAINER_CARD_WINDOW_WIDTH - 50, NULL, 20, 2),
        0, (Rectangle){25, 500, TRAINER_CARD_WINDOW_WIDTH - 50, 100},
        20, 4, 0);

    tcg->page = 0;
    tcg->selectedRibbon = 0;
}

void TrainerCardGUIUpdate(TrainerCardGUI *tcg)
{
    ShinySparkleAnimUpdate(tcg->shinySparkleAnim);
    DialogBoxUpdate(tcg->descriptionBox);
}

void TrainerCardGUIDraw(TrainerCardGUI *tcg)
{
    ClearBackground(BLANK);
    DrawSkeleton();

    DrawShinyCharmProgress(tcg, (Vector2){370, 30});
    DrawPlayerStats(tcg, (Vector2){30, 30});
    DrawRibbonGrid(tcg, (Vector2){27, 150}, 4, 12, 20, 40);
    DialogBoxDraw(tcg->descriptionBox);
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

static int hiddenCounter = 0;
void DrawRibbonGrid(TrainerCardGUI *tcg, Vector2 startPos, int columns, int totalElements, float marginX, float marginY)
{

    hiddenCounter = 0;
    for (int i = 0; i < totalElements; i++)
    {
        // Calculate current row and column
        int column = i % columns;
        int row = i / columns;

        // Calculate the specific position for this ribbon
        Vector2 currentPos = {
            startPos.x + (column * (ITEM_WIDTH + marginX)),
            startPos.y + (row * (ITEM_HEIGHT + marginY))};

        // Call your existing function
        hiddenCounter++;
        int variant = hiddenCounter % 4;
        if (variant == 0)
        {
            DrawNonCompletedRibbon(tcg, currentPos);
        }
        else if (variant == 1)
        {
            DrawCompletedRibbon(tcg, currentPos);
        }
        else if (variant == 2)
        {
            DrawShinyRibbon(tcg, currentPos);
        }
        else
        {
            DrawBWRibbon(tcg, currentPos);
        }
    }
}

void DrawRibbonBox(TrainerCardGUI *tcg, Vector2 position, Color borderColor, Color bgColor)
{
    unsigned int borderThickness = 4;

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
    const char *fittedName = TextFitting(name, ITEM_WIDTH, NULL, 16, 16 / 10.0f);
    DrawText(fittedName, position.x, position.y, 16, BLACK);
}

void DrawNonCompletedRibbon(TrainerCardGUI *tcg, Vector2 position)
{
    float scale = 2.0f;
    DrawRibbonBox(tcg, position, WHITE, WHITE);

    RibbonSheetDraw(
        tcg->ribbonSheet,
        (Vector2){
            position.x + ITEM_WIDTH / 2 - SHINY_RIBBON_WIDTH * scale / 2,
            position.y + ITEM_HEIGHT / 2 - RIBBON_HEIGHT * scale / 2},
        SILHOUETTE_RIBBON_VARIANT, scale);

    DrawRibbonName("NOSTALGIA", (Vector2){position.x, position.y + ITEM_HEIGHT + 5});
}

void DrawCompletedRibbon(TrainerCardGUI *tcg, Vector2 position)
{
    float scale = 2.0f;
    DrawRibbonBox(tcg, position, WHITE, WHITE);

    RibbonSheetDraw(
        tcg->ribbonSheet,
        (Vector2){
            position.x + ITEM_WIDTH / 2 - SHINY_RIBBON_WIDTH * scale / 2,
            position.y + ITEM_HEIGHT / 2 - RIBBON_HEIGHT * scale / 2},
        REGULAR_RIBBON_VARIANT, scale);

    DrawRibbonName("NOSTALGIA", (Vector2){position.x, position.y + ITEM_HEIGHT + 5});
}

void DrawShinyRibbon(TrainerCardGUI *tcg, Vector2 position)
{
    float scale = 2.0f;
    DrawRibbonBox(tcg, position, (Color){208, 172, 134, 255}, (Color){118, 171, 189, 255});

    RibbonSheetDraw(
        tcg->ribbonSheet,
        (Vector2){
            position.x + ITEM_WIDTH / 2 - SHINY_RIBBON_WIDTH * scale / 2,
            position.y + ITEM_HEIGHT / 2 - RIBBON_HEIGHT * scale / 2},
        SHINY_RIBBON_VARIANT, scale);

    ShinySparkleAnimDraw(tcg->shinySparkleAnim,
                         (Vector2){
                             position.x + ITEM_WIDTH / 2 - SHINY_SPARKLE_SIZE / 2,
                             position.y + ITEM_HEIGHT / 2 - SHINY_SPARKLE_SIZE / 2});

    DrawRibbonName("NOSTALGIA", (Vector2){position.x, position.y + ITEM_HEIGHT + 5});
}

void DrawBWRibbon(TrainerCardGUI *tcg, Vector2 position)
{
    float scale = 2.0f;
    DrawRibbonBox(tcg, position, (Color){184, 73, 198, 255}, (Color){188, 133, 234, 255});

    RibbonSheetDraw(
        tcg->ribbonSheet,
        (Vector2){
            position.x + ITEM_WIDTH / 2 - SHINY_RIBBON_WIDTH * scale / 2,
            position.y + ITEM_HEIGHT / 2 - RIBBON_HEIGHT * scale / 2},
        BW_RIBBON_VARIANT, scale);

    ShinySparkleAnimDraw(tcg->shinySparkleAnim,
                         (Vector2){
                             position.x + ITEM_WIDTH / 2 - SHINY_SPARKLE_SIZE / 2,
                             position.y + ITEM_HEIGHT / 2 - SHINY_SPARKLE_SIZE / 2});

    DrawRibbonName("NOSTALGIA", (Vector2){position.x, position.y + ITEM_HEIGHT + 5});
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

    ShinyCharmSheetDraw(tcg->shinyCharmSheet, (Vector2){position.x + width / 2 - SHINY_CHARM_SIZE / 2, position.y + height / 2 - SHINY_CHARM_SIZE / 2}, 1);
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