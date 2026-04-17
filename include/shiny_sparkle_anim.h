#ifndef SHINY_SPARKLE_ANIM_H
#define SHINY_SPARKLE_ANIM_H

#define SHINY_SPARKLE_SIZE 48
#define SHINY_SPARKLE_START 1
#define SHINY_SPARKLE_PADDING 1

#define SHINY_SPARKLE_COLS 10
#define SHINY_SPARKLE_ROWS 4
#define SHINY_SPARKLE_TOTAL_STATES 48

#include "raylib.h"
#include "shiny_sparkle_sheet.h"

typedef struct
{
    unsigned int frameDuration;
    unsigned short loop;
    ShinySparkleSheet *sheet;
    unsigned int elapsedFrames;
    unsigned short isActive;
} ShinySparkleAnim;

void ShinySparkleAnimInit(ShinySparkleAnim *anim, unsigned int frameDuration, unsigned short loop, ShinySparkleSheet *sheet);
void ShinySparkleAnimUpdate(ShinySparkleAnim *anim);
void ShinySparkleAnimDraw(ShinySparkleAnim *anim, Vector2 position);
void ShinySparkleAnimUnload(ShinySparkleAnim *anim);

#endif