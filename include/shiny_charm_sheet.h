#ifndef SHINY_CHARM_SHEET_H
#define SHINY_CHARM_SHEET_H

#include "raylib.h"

#define SHINY_CHARM_SIZE 96
#define SHINY_CHARM_REGULAR_VARIANT 0
#define SHINY_CHARM_SILHOUETTE_VARIANT 1

typedef struct
{
    Texture2D texture;
    Texture2D silhouetteTexture;
} ShinyCharmSheet;

void ShinyCharmSheetInit(ShinyCharmSheet *sheet, const char *filename);
void ShinyCharmSheetDraw(ShinyCharmSheet *sheet, Vector2 position, unsigned char variant);
void ShinyCharmSheetUnload(ShinyCharmSheet *sheet);

#endif