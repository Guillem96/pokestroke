#include "shiny_sparkle_sheet.h"
#include "raylib.h"
#include <stdlib.h>

void ShinySparkleSheetInit(ShinySparkleSheet *sheet, const char *filename)
{
    TraceLog(LOG_INFO, "Loading Shiny Sparkle Sheet: %s", filename);
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
        if (pixels[i].r == 238 && pixels[i].g == 82 && pixels[i].b == 247)
        {
            pixels[i].a = 0;
        }
    }

    sheet->texture = LoadTextureFromImage(img);
    UnloadImage(img);

    if (sheet->texture.id == 0)
    {
        TraceLog(LOG_ERROR, "Texture not loaded!");
        exit(1);
    }
}

void ShinySparkleSheetDraw(ShinySparkleSheet *sheet, Vector2 position, unsigned short state)
{
    if (state >= SHINY_SPARKLE_TOTAL_STATES)
    {
        TraceLog(LOG_WARNING, "Invalid shiny sparkle state: %d", state);
        return;
    }

    unsigned int col = state % SHINY_SPARKLE_COLS;
    unsigned int row = state / SHINY_SPARKLE_COLS;
    unsigned int x = SHINY_SPARKLE_START + col * (SHINY_SPARKLE_SIZE + SHINY_SPARKLE_PADDING);
    unsigned int y = SHINY_SPARKLE_START + row * (SHINY_SPARKLE_SIZE + SHINY_SPARKLE_PADDING);

    DrawTexturePro(
        sheet->texture,
        (Rectangle){x, y, SHINY_SPARKLE_SIZE, SHINY_SPARKLE_SIZE},
        (Rectangle){position.x, position.y, SHINY_SPARKLE_SIZE, SHINY_SPARKLE_SIZE},
        (Vector2){0, 0},
        0.0f, WHITE);
}

void ShinySparkleSheetUnload(ShinySparkleSheet *sheet)
{
    UnloadTexture(sheet->texture);
}