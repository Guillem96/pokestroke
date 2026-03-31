
#include "pokedex_gui.h"
#include "menu_bar_config.h"
#include <stdlib.h>

#include "raylib.h"
#include "utils.h"

#define POKEDEX_COLOR_RED (Color){193, 44, 69, 255}
#define POKEDEX_COLOR_DARK_RED (Color){131, 27, 46, 255}
#define POKEDEX_COLOR_SKY_BLUE (Color){87, 186, 207, 255}
#define POKEDEX_COLOR_SKY_BLUE_2 (Color){109, 231, 246, 255}
#define POKEDEX_COLOR_SKY_BLUE_3 (Color){180, 242, 246, 255}

#define POKEDEX_COLOR_DARK_GRAY (Color){50, 50, 50, 255}
#define POKEDEX_COLOR_DARK_GRAY_2 (Color){20, 20, 20, 255}

#define POKEDEX_COLOR_SCREEN_GREEN (Color){122, 194, 134, 255}

static Font *font = NULL;

static void DrawFrame();
static void DrawNames(PokedexGUI *gui, Pokedex *pokedex);
static void DrawPokemon(PokedexGUI *gui, Pokedex *pokedex);
static void DrawIndicators(PokedexGUI *gui, Pokedex *pokedex);

void PokedexGUIInit(PokedexGUI *gui, Pokedex *pokedex, PokemonSpriteSheet *sheet, PkmnBattleSpriteSheet *battleSheet)
{
    gui->pokedex = pokedex;
    gui->sheet = sheet;
    gui->battleSheet = battleSheet;
    gui->selectedPkmnIndex = 0;
    gui->page = 0;
}

void PokedexGUIUpdate(PokedexGUI *gui)
{

    if (IsKeyPressed(KEY_DOWN))
    {
        if (gui->selectedPkmnIndex < POKEDEX_PAGE_SIZE - 1 && gui->page * POKEDEX_PAGE_SIZE + gui->selectedPkmnIndex < POKEMON_COUNT - 1)
        {
            gui->selectedPkmnIndex++;
        }
        else if (gui->page * POKEDEX_PAGE_SIZE + gui->selectedPkmnIndex < POKEMON_COUNT - 1)
        {
            gui->page++;
            gui->selectedPkmnIndex = 0;
        }
    }
    else if (IsKeyPressed(KEY_UP))
    {
        if (gui->selectedPkmnIndex > 0)
        {
            gui->selectedPkmnIndex--;
        }
        else if (gui->page > 0)
        {
            gui->page--;
            gui->selectedPkmnIndex = POKEDEX_PAGE_SIZE - 1;
        }
    }
    else if (IsKeyPressed(KEY_RIGHT))
    {
        if ((gui->page + 1) * POKEDEX_PAGE_SIZE < POKEMON_COUNT)
        {
            gui->page++;
            gui->selectedPkmnIndex = 0;
        }
    }
    else if (IsKeyPressed(KEY_LEFT))
    {
        if (gui->page > 0)
        {
            gui->page--;
            gui->selectedPkmnIndex = 0;
        }
    }
}

void PokedexGUIDraw(PokedexGUI *gui)
{
    DrawFrame();
    DrawNames(gui, gui->pokedex);
    DrawPokemon(gui, gui->pokedex);
    DrawIndicators(gui, gui->pokedex);
}

static void DrawLight(Color color, Vector2 pos, int rad)
{
    float sr = rad * .7f;
    DrawCircleV(pos, rad, color);
    DrawCircle(
        pos.x - sr * .25f, pos.y - sr * 0.25f,
        sr,
        (Color){min(color.r + 50, 255), min(color.g + 50, 255), min(color.b + 50, 255), 255});
}

static void DrawCross(Vector2 pos, Color color)
{
    int size = 95;
    int height = 25;
    DrawRectangleRounded((Rectangle){pos.x - size / 2, pos.y - height / 2, size, height}, 0.6f, 8, color);
    DrawRectangleRounded((Rectangle){pos.x - height / 2, pos.y - size / 2, height, size}, 0.6f, 8, color);
    DrawCircle(pos.x, pos.y, 8, BLACK);
}

static void DrawFrame()
{
    DrawRectangleRounded(
        (Rectangle){0, 0, POKEDEX_WINDOW_WIDTH, POKEDEX_WINDOW_HEIGHT},
        .04f, 8, POKEDEX_COLOR_DARK_RED);

    DrawRectangleRounded(
        (Rectangle){10, 0, POKEDEX_WINDOW_WIDTH - 10, POKEDEX_WINDOW_HEIGHT - 10},
        .04f, 8, POKEDEX_COLOR_RED);

    // Left Blue light
    Vector2 circleCenter = (Vector2){60, 60};
    DrawCircleV(circleCenter, 35, WHITE);
    DrawCircleV(
        (Vector2){circleCenter.x - .5f, circleCenter.y - 4}, 28, POKEDEX_COLOR_SKY_BLUE_2);
    DrawCircleV(
        (Vector2){circleCenter.x + 1.5f, circleCenter.y - 2}, 28, POKEDEX_COLOR_SKY_BLUE);
    DrawCircleV(
        (Vector2){circleCenter.x - 4.5f, circleCenter.y - 9}, 20, POKEDEX_COLOR_SKY_BLUE_2);
    DrawCircleV(
        (Vector2){circleCenter.x - 6.5f, circleCenter.y - 14}, 8, POKEDEX_COLOR_SKY_BLUE_3);

    // Top three LEDs
    DrawLight(RED, (Vector2){120, 40}, 10);
    DrawLight(YELLOW, (Vector2){145, 40}, 10);
    DrawLight(GREEN, (Vector2){170, 40}, 10);

    // Screen Margin
    int screenMarginWidth = POKEDEX_WINDOW_WIDTH - 70;
    int screenMarginHeight = 360;
    DrawRectangleRounded((Rectangle){35, 110, screenMarginWidth, screenMarginHeight},
                         0.08f, 8, GRAY);
    DrawRectangleRounded((Rectangle){40, 110, screenMarginWidth - 5, screenMarginHeight - 5}, 0.08f, 8, LIGHTGRAY);
    DrawLight(RED, (Vector2){(screenMarginWidth + 70) / 2 - 10, 135}, 5);
    DrawLight(RED, (Vector2){(screenMarginWidth + 70) / 2 + 10, 135}, 5);

    for (int i = 0; i < 4; i++)
    {
        DrawRectangle(POKEDEX_WINDOW_WIDTH - 160, 430 + i * 8, 100, 3, GRAY);
    }

    DrawLight(RED, (Vector2){90, 440}, 15);

    // Screen
    DrawRectangleRounded((Rectangle){POKEDEX_MAIN_SCREEN_X, POKEDEX_MAIN_SCREEN_Y, POKEDEX_MAIN_SCREEN_WIDTH, POKEDEX_MAIN_SCREEN_HEIGHT}, 0.08f, 8, (Color){239, 137, 65, 255});

    // Cross control
    DrawCross((Vector2){417, 533}, POKEDEX_COLOR_DARK_GRAY_2);
    DrawCross((Vector2){420, 530}, POKEDEX_COLOR_DARK_GRAY);

    // Smaller green screen
    DrawRectangleRounded(
        (Rectangle){40, 485, 300, 150}, 0.06f, 8, POKEDEX_COLOR_SCREEN_GREEN);
    DrawRectangleRoundedLinesEx(
        (Rectangle){40, 485, 300, 150}, 0.06f, 8, 2, BLACK);

    // Controls
    if (font == NULL)
    {
        font = (Font *)malloc(sizeof(Font));
        *font = LoadFont("Pretendo.ttf");
    }
    DrawTextEx(*font, "ARROW KEYS to navigate", (Vector2){POKEDEX_WINDOW_WIDTH - 230, 15}, 12.0f, 2.0f, POKEDEX_COLOR_DARK_RED);
}

static void DrawNames(PokedexGUI *gui, Pokedex *pokedex)
{
    int x = POKEDEX_MAIN_SCREEN_X + POKEDEX_MAIN_SCREEN_WIDTH * 0.3f;
    int y = POKEDEX_MAIN_SCREEN_Y + 10;
    int height = POKEDEX_MAIN_SCREEN_HEIGHT - 20;

    int nameHeight = height / POKEDEX_PAGE_SIZE;

    DrawRectangle(x, y, POKEDEX_MAIN_SCREEN_WIDTH * 0.70f - 10, height, WHITE);
    DrawRectangleLinesEx((Rectangle){x, y, POKEDEX_MAIN_SCREEN_WIDTH * 0.70f - 10, height}, 2, BLACK);

    for (int i = 0; i < POKEDEX_PAGE_SIZE; i++)
    {
        int pkmnIndex = gui->page * POKEDEX_PAGE_SIZE + i;
        if (pkmnIndex >= POKEMON_COUNT)
        {
            break;
        }

        bool isSeen = false;
        for (int j = 0; j < VARIANT_COUNT; j++)
        {
            if (pokedex->registered[pkmnIndex * VARIANT_COUNT + j] == POKEDEX_SEEN || pokedex->registered[pkmnIndex * VARIANT_COUNT + j] == POKEDEX_REGISTERED)
            {
                isSeen = true;
                break;
            }
        }

        int nameX = x + 10;
        int nameMargin = 12;
        float nameY = y + i * nameHeight + nameMargin / 2.f;
        const char *nameToDraw = TextFormat("%03d. %s", pkmnIndex + 1, isSeen ? POKEMON_NAMES[pkmnIndex] : "?????");
        int nameWidth = MeasureText(nameToDraw, nameHeight - nameMargin);
        DrawText(nameToDraw, nameX, nameY, nameHeight - nameMargin, BLACK);
        PkmnBattleSpriteSheeDrawCaughtIndicator(
            gui->battleSheet,
            &gui->pokedex->registered[pkmnIndex * VARIANT_COUNT],
            (Vector2){nameX + nameWidth + 8, nameY}, 1.4f);

        if (i == gui->selectedPkmnIndex)
        {
            DrawRectangleLinesEx(
                (Rectangle){x + 2, y + i * nameHeight, POKEDEX_MAIN_SCREEN_WIDTH * 0.70f - 10 - 4, nameHeight},
                3, POKEDEX_COLOR_SCREEN_GREEN);
        }
    }
}

static void DrawPokemon(PokedexGUI *gui, Pokedex *pokedex)
{
    int pkmnIndex = gui->page * POKEDEX_PAGE_SIZE + gui->selectedPkmnIndex;
    int x = POKEDEX_MAIN_SCREEN_X + 20;
    int y = POKEDEX_MAIN_SCREEN_Y + 8;
    int margin = 5;
    float scale = 1.4f;

    for (int j = 0; j < VARIANT_COUNT; j++)
    {
        int pkmnY = y + j * (PKMN_HEIGHT * scale + margin);
        if (pokedex->registered[pkmnIndex * VARIANT_COUNT + j] == POKEDEX_NEVER_SEEN)
        {
            DrawRectangle(x, pkmnY, PKMN_WIDTH * scale, PKMN_HEIGHT * scale, GRAY);
            DrawText("???", x + (PKMN_WIDTH * scale - MeasureText("???", 20)) / 2, pkmnY + (PKMN_HEIGHT * scale - 20) / 2, 20, BLACK);
        }
        else if (pokedex->registered[pkmnIndex * VARIANT_COUNT + j] == POKEDEX_SEEN)
        {
            PkmnSpriteSheetDrawPro(gui->sheet, pkmnIndex, j, (Vector2){x, pkmnY}, scale, false, DARKGRAY);
        }
        else
        {
            PkmnSpriteSheetDrawPro(gui->sheet, pkmnIndex, j, (Vector2){x, pkmnY}, scale, false, WHITE);
        }

        DrawRectangleLinesEx((Rectangle){x, pkmnY, PKMN_WIDTH * scale, PKMN_HEIGHT * scale}, 2, BLACK);
    }
}

static void DrawIndicators(PokedexGUI *gui, Pokedex *pokedex)
{
    int x = 40;
    int y = 485;

    DrawText(
        TextFormat("Page: %d / %d", gui->page + 1, (POKEMON_COUNT + POKEDEX_PAGE_SIZE - 1) / POKEDEX_PAGE_SIZE),
        x + 10, y + 10, 20, BLACK);

    // Two way table to indicate which variants (Normal, Shiny, Black & White) have been seen/caught
    DrawText("Seen/Caught:", x + 10, y + 40, 20, BLACK);
    // Count all seen/caught normal variants
    int normalSeen = 0;
    int normalCaught = 0;
    for (int i = 0; i < POKEMON_COUNT; i++)
    {
        if (pokedex->registered[i * VARIANT_COUNT] == POKEDEX_SEEN)
        {
            normalSeen++;
        }
        if (pokedex->registered[i * VARIANT_COUNT] == POKEDEX_REGISTERED)
        {
            normalCaught++;
            normalSeen++;
        }
    }
    DrawText(TextFormat("Normal: %d/%d", normalSeen, normalCaught), x + 30, y + 70, 18, BLACK);

    // Count all seen/caught shiny variants
    int shinySeen = 0;
    int shinyCaught = 0;
    for (int i = 0; i < POKEMON_COUNT; i++)
    {
        if (pokedex->registered[i * VARIANT_COUNT + 1] == POKEDEX_SEEN)
        {
            shinySeen++;
        }
        if (pokedex->registered[i * VARIANT_COUNT + 1] == POKEDEX_REGISTERED)
        {
            shinyCaught++;
            shinySeen++;
        }
    }
    DrawText(TextFormat("Shiny: %d/%d", shinySeen, shinyCaught), x + 30, y + 90, 18, BLACK);

    // Count all seen/caught black & white variants
    int bwSeen = 0;
    int bwCaught = 0;
    for (int i = 0; i < POKEMON_COUNT; i++)
    {
        if (pokedex->registered[i * VARIANT_COUNT + 2] == POKEDEX_SEEN)
        {
            bwSeen++;
        }
        if (pokedex->registered[i * VARIANT_COUNT + 2] == POKEDEX_REGISTERED)
        {
            bwCaught++;
            bwSeen++;
        }
    }
    DrawText(TextFormat("Black & White: %d/%d", bwSeen, bwCaught), x + 30, y + 110, 18, BLACK);
}

void PokedexGUIUnload(PokedexGUI *gui)
{
    if (font != NULL)
    {
        UnloadFont(*font);
        free(font);
        font = NULL;
    }
}
