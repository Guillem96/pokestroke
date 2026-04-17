#include "shiny_sparkle_anim.h"
#include "shiny_sparkle_sheet.h"

void ShinySparkleAnimInit(ShinySparkleAnim *anim, unsigned int frameDuration, unsigned short loop, ShinySparkleSheet *sheet)
{
    anim->frameDuration = frameDuration;
    anim->loop = loop;
    anim->sheet = sheet;
    anim->elapsedFrames = 0;
    anim->isActive = true;
}
void ShinySparkleAnimUpdate(ShinySparkleAnim *anim)
{
    if (!anim->isActive)
        return;

    unsigned int framesPerState = anim->frameDuration / SHINY_SPARKLE_TOTAL_STATES;
    unsigned short currentState = anim->elapsedFrames / framesPerState;

    anim->elapsedFrames++;

    if (anim->elapsedFrames >= anim->frameDuration)
    {
        if (anim->loop)
        {
            anim->elapsedFrames = 0;
        }
        else
        {
            anim->isActive = false;
        }
    }
}

void ShinySparkleAnimDraw(ShinySparkleAnim *anim, Vector2 position)
{
    unsigned int framesPerState = anim->frameDuration / SHINY_SPARKLE_TOTAL_STATES;
    unsigned short currentState = anim->elapsedFrames / framesPerState;
    ShinySparkleSheetDraw(anim->sheet, position, currentState);
}

void ShinySparkleAnimUnload(ShinySparkleAnim *anim)
{
}
