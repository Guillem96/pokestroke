#include "pkmn_battle_sprite_sheet.h"
#include <stdlib.h>
#include "raylib.h"
#include "pokedex.h"

void PkmnBattleSpriteSheetInit(PkmnBattleSpriteSheet *sheet, const char *filename)
{
    TraceLog(LOG_INFO, "Loading Pokemon Battle Sprite Sheet: %s", filename);
    // Load image first to process alpha
    Image img = LoadImage(filename);

    // Convert to RGBA if needed
    if (img.format != PIXELFORMAT_UNCOMPRESSED_R8G8B8A8)
    {
        Image temp = ImageCopy(img);
        ImageFormat(&temp, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
        UnloadImage(img);
        img = temp;
    }

    // Create alpha mask - set white pixels to transparent
    Color *pixels = (Color *)img.data;
    for (int i = 0; i < img.width * img.height; i++)
    {
        if (pixels[i].r == 248 && pixels[i].g == 248 && pixels[i].b == 248)
        {
            pixels[i].a = 0;
        }

        if (pixels[i].r == 0 && pixels[i].g == 162 && pixels[i].b == 232)
        {
            pixels[i].a = 0;
        }

        if (pixels[i].r == 255 && pixels[i].g == 127 && pixels[i].b == 39)
        {
            pixels[i].a = 0;
        }
        if (pixels[i].r == 255 && pixels[i].g == 13 && pixels[i].b == 19)
        {
            pixels[i].a = 0;
        }
        if (pixels[i].r == 168 && pixels[i].g == 0 && pixels[i].b == 32)
        {
            pixels[i].a = 0;
        }
    }
    sheet->texture = LoadTextureFromImage(img);
    UnloadImage(img);
    TraceLog(LOG_INFO, "Loaded Pokemon Sprite Sheet: %s (id: %d, width: %d, height: %d)", filename, sheet->texture.id, sheet->texture.width, sheet->texture.height);
    if (sheet->texture.id == 0)
    {
        TraceLog(LOG_ERROR, "Texture not loaded!");
        exit(1);
    }
}

void PkmnBattleSpriteSheetPokeballDraw(PkmnBattleSpriteSheet *sheet, unsigned short pokeballType, unsigned short status, int x, int y)
{
    TraceLog(LOG_DEBUG, "Drawing pokeball type %d status %d at position (%d, %d)", pokeballType, status, x, y);

    int height;
    int xOffset = POKEBALL_SHEET_OFFSET_X + (pokeballType * (POKEBALL_SIZE + POKEBALL_MARGIN));
    int yOffset;
    if (status == POKEBALL_STATUS_HALF_DOWN)
    {
        height = POKEBALL_SIZE / 2;
        yOffset = POKEBALL_SHEET_OFFSET_Y + (POKEBALL_STATUS_HALF_UP * (POKEBALL_SIZE + POKEBALL_MARGIN));
        yOffset += POKEBALL_SIZE / 2 + POKEBALL_MARGIN / 2;
    }
    else if (status == POKEBALL_STATUS_HALF_UP)
    {
        height = POKEBALL_SIZE / 2;
        yOffset = POKEBALL_SHEET_OFFSET_Y + (status * (POKEBALL_SIZE + POKEBALL_MARGIN));
    }
    else
    {
        height = POKEBALL_SIZE;
        yOffset = POKEBALL_SHEET_OFFSET_Y + (status * (POKEBALL_SIZE + POKEBALL_MARGIN));
    }

    Rectangle sourceRect = {
        xOffset,
        yOffset,
        POKEBALL_SIZE, height};

    Rectangle destRec = {x, y, POKEBALL_SIZE, height};
    Vector2 origin = {0, 0};
    DrawTexturePro(sheet->texture, sourceRect, destRec, origin, 0.0f, WHITE);
}

void PkmnBattleSpriteSheetPokeballExplosionDraw(PkmnBattleSpriteSheet *sheet, unsigned short status, int x, int y)
{
    int xOffset = POKEBALL_EXPLOSION_SHEET_OFFSET_X + ((status % POKEBALL_EXPLOSION_COLS) * (POKEBALL_EXPLOSION_WIDTH + POKEBALL_EXPLOSION_MARGIN));
    int yOffset = POKEBALL_EXPLOSION_SHEET_OFFSET_Y + ((status / POKEBALL_EXPLOSION_COLS) * (POKEBALL_EXPLOSION_HEIGHT + POKEBALL_EXPLOSION_MARGIN));

    Rectangle sourceRect = {
        xOffset,
        yOffset + POKEBALL_EXPLOSION_HEIGHT - POKEBALL_EXPLOSION_REAL_SIZE,
        POKEBALL_EXPLOSION_REAL_SIZE, POKEBALL_EXPLOSION_REAL_SIZE};

    Rectangle destRec = {x, y, POKEBALL_EXPLOSION_REAL_SIZE, POKEBALL_EXPLOSION_REAL_SIZE};
    Vector2 origin = {0, 0};
    DrawTexturePro(sheet->texture, sourceRect, destRec, origin, 0.0f, WHITE);
}

void PkmnBattleSpriteSheetDrawPkmnName(PkmnBattleSpriteSheet *sheet, const char *name, unsigned short *isCaught, int x, int y)
{
    Rectangle sourceRect = {
        PKMN_NAME_BAR_OFFSET_X,
        PKMN_NAME_BAR_OFFSET_Y,
        PKMN_NAME_BAR_WIDTH, PKMN_NAME_BAR_HEIGHT};

    Rectangle destRec = {x, y, PKMN_NAME_BAR_WIDTH, PKMN_NAME_BAR_HEIGHT};
    Vector2 origin = {0, 0};
    DrawTexturePro(sheet->texture, sourceRect, destRec, origin, 0.0f, WHITE);

    float textWidth = MeasureText(name, 10);
    DrawText(name, x + PKMN_NAME_OFFSET_X, y, 10, BLACK);

    float indicatorX = x + PKMN_NAME_OFFSET_X + textWidth + 4;
    Rectangle caughtIndicatorSourceRect = {
        PKMN_CAUGHT_INDIDATOR_OFFSET_X,
        PKMN_CAUGHT_INDIDATOR_OFFSET_Y,
        PKMN_INDICATOR_SIZE, PKMN_INDICATOR_SIZE};

    Rectangle notCaughtIndicatorSourceRect = {
        PKMN_NON_CAUGHT_INDIDATOR_OFFSET_X,
        PKMN_NON_CAUGHT_INDIDATOR_OFFSET_Y,
        PKMN_INDICATOR_SIZE, PKMN_INDICATOR_SIZE};

    for (int i = 0; i < 3; i++)
    {
        destRec = (Rectangle){indicatorX + (i * PKMN_INDICATOR_SIZE), y, PKMN_INDICATOR_SIZE, PKMN_INDICATOR_SIZE};
        if (isCaught[i] == POKEDEX_REGISTERED)
        {
            DrawTexturePro(sheet->texture, caughtIndicatorSourceRect, destRec, origin, 0.0f, WHITE);
        }
        else
        {
            DrawTexturePro(sheet->texture, notCaughtIndicatorSourceRect, destRec, origin, 0.0f, WHITE);
        }
    }
}
void PkmnBattleSpriteSheetUnload(PkmnBattleSpriteSheet *sheet)
{
    UnloadTexture(sheet->texture);
}