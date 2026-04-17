#include "ribbon_sheet.h"
#include "raylib.h"
#include <stdlib.h>

void RibbonSheetInit(RibbonSheet *sheet, const char *filename)
{
    TraceLog(LOG_INFO, "Loading Ribbon Sheet: %s", filename);
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
        if (pixels[i].r == 255 && pixels[i].g == 255 && pixels[i].b == 255)
        {
            pixels[i].a = 0;
        }
    }

    sheet->texture = LoadTextureFromImage(img);

    Image grayScaleImage = ImageCopy(img);
    ImageColorGrayscale(&grayScaleImage);
    ImageFormat(&grayScaleImage, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    pixels = (Color *)grayScaleImage.data;
    for (int i = 0; i < img.width * img.height; i++)
    {
        if (pixels[i].r == 255 && pixels[i].g == 255 && pixels[i].b == 255)
        {
            pixels[i].a = 0;
        }
    }
    sheet->grayScaleTexture = LoadTextureFromImage(grayScaleImage);

    Image silhouetteImage = ImageCopy(img);
    pixels = (Color *)silhouetteImage.data;
    for (int i = 0; i < img.width * img.height; i++)
    {
        if (pixels[i].r != 255 && pixels[i].g != 255 && pixels[i].b != 255)
        {
            pixels[i].r = LIGHTGRAY.r;
            pixels[i].g = LIGHTGRAY.g;
            pixels[i].b = LIGHTGRAY.b;
            pixels[i].a = 255;
        }
        else
        {
            pixels[i].a = 0;
        }
    }
    sheet->silhouetteTexture = LoadTextureFromImage(silhouetteImage);

    UnloadImage(img);
    UnloadImage(grayScaleImage);
    UnloadImage(silhouetteImage);

    if (sheet->texture.id == 0)
    {
        TraceLog(LOG_ERROR, "Texture not loaded!");
        exit(1);
    }
}

void RibbonSheetDraw(RibbonSheet *sheet, Vector2 position, unsigned short variant, float scale)
{
    if (variant > 3)
    {
        TraceLog(LOG_WARNING, "Invalid ribbon variant: %d", variant);
        return;
    }

    Rectangle src;
    Rectangle dst;
    Texture2D textureToUse;

    if (variant == REGULAR_RIBBON_VARIANT || variant == SILHOUETTE_RIBBON_VARIANT)
    {
        textureToUse = variant == REGULAR_RIBBON_VARIANT ? sheet->texture : sheet->silhouetteTexture;
        src = (Rectangle){0, 0, REGULAR_RIBBON_WIDTH, RIBBON_HEIGHT};
        dst = (Rectangle){position.x + (1 * scale), position.y, REGULAR_RIBBON_WIDTH * scale, RIBBON_HEIGHT * scale};
    }
    else if (variant == SHINY_RIBBON_VARIANT || variant == BW_RIBBON_VARIANT)
    {
        textureToUse = variant == SHINY_RIBBON_VARIANT ? sheet->texture : sheet->grayScaleTexture;
        src = (Rectangle){SHINY_RIBBON_START_X, 0, SHINY_RIBBON_WIDTH, RIBBON_HEIGHT};
        dst = (Rectangle){position.x, position.y, SHINY_RIBBON_WIDTH * scale, RIBBON_HEIGHT * scale};
    }

    DrawTexturePro(
        textureToUse,
        src,
        dst,
        (Vector2){0, 0},
        0.0f, WHITE);
}

void RibbonSheetUnload(RibbonSheet *sheet)
{
    UnloadTexture(sheet->texture);
    UnloadTexture(sheet->grayScaleTexture);
}