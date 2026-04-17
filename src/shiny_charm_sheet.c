#include "shiny_charm_sheet.h"
#include "raylib.h"
#include <stdlib.h>

void ShinyCharmSheetInit(ShinyCharmSheet *sheet, const char *filename)
{
    TraceLog(LOG_INFO, "Loading Shiny Charm Sheet: %s", filename);
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
    Image sillhoutteImage = ImageCopy(img);
    Color *pixels = (Color *)sillhoutteImage.data;
    for (int i = 0; i < img.width * img.height; i++)
    {
        if (pixels[i].a != 0)
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

    sheet->texture = LoadTextureFromImage(img);
    if (sheet->texture.id == 0)
    {
        TraceLog(LOG_ERROR, "Failed to load texture for Shiny Charm Sheet: %s", filename);
        exit(1);
    }
    sheet->silhouetteTexture = LoadTextureFromImage(sillhoutteImage);
    if (sheet->silhouetteTexture.id == 0)
    {
        TraceLog(LOG_ERROR, "Failed to load silhouette texture for Shiny Charm Sheet: %s", filename);
        exit(1);
    }
    UnloadImage(img);
    UnloadImage(sillhoutteImage);
}

void ShinyCharmSheetDraw(ShinyCharmSheet *sheet, Vector2 position, unsigned char variant)
{
    Texture2D textureToUse = variant == SHINY_CHARM_REGULAR_VARIANT ? sheet->texture : sheet->silhouetteTexture;
    Rectangle src = {0, 0, 512, 512};
    Rectangle dest = {position.x, position.y, SHINY_CHARM_SIZE, SHINY_CHARM_SIZE};
    DrawTexturePro(textureToUse, src, dest, (Vector2){0, 0}, 0, WHITE);
}

void ShinyCharmSheetUnload(ShinyCharmSheet *sheet)
{
    UnloadTexture(sheet->texture);
    UnloadTexture(sheet->silhouetteTexture);
}
