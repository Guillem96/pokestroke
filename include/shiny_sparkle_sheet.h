#ifndef SHINY_SPARKLE_SHEET_H
#define SHINY_SPARKLE_SHEET_H

#define SHINY_SPARKLE_SIZE 48
#define SHINY_SPARKLE_START 1
#define SHINY_SPARKLE_PADDING 1

#define SHINY_SPARKLE_COLS 10
#define SHINY_SPARKLE_ROWS 4
#define SHINY_SPARKLE_TOTAL_STATES 48

#include "raylib.h"

typedef struct
{
    Texture2D texture;
} ShinySparkleSheet;

void ShinySparkleSheetInit(ShinySparkleSheet *sheet, const char *filename);
void ShinySparkleSheetDraw(ShinySparkleSheet *sheet, Vector2 position, unsigned short state);
void ShinySparkleSheetUnload(ShinySparkleSheet *sheet);

#endif