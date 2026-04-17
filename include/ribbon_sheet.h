#ifndef RIBBONS_SHEET_H
#define RIBBONS_SHEET_H

#define RIBBON_HEIGHT 30
#define REGULAR_RIBBON_WIDTH 22
#define SHINY_RIBBON_WIDTH 24
#define SHINY_RIBBON_START_X 32

#define REGULAR_RIBBON_VARIANT 0
#define SHINY_RIBBON_VARIANT 1
#define BW_RIBBON_VARIANT 2
#define SILHOUETTE_RIBBON_VARIANT 3

#include "raylib.h"

typedef struct
{
    Texture2D texture;
    Texture2D grayScaleTexture;
    Texture2D silhouetteTexture;
} RibbonSheet;

void RibbonSheetInit(RibbonSheet *sheet, const char *filename);
void RibbonSheetDraw(RibbonSheet *sheet, Vector2 position, unsigned short variant, float scale);
void RibbonSheetUnload(RibbonSheet *sheet);

#endif