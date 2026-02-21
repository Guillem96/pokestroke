#include "pkmn_shrink_anim.h"
#include "pkmn_sprite_sheet.h"

void PkmnShrinkAnimInit(PkmnShrinkAnim *anim, PokemonSpriteSheet *sheet, int pokemon, unsigned short variant, Vector2 pkmnPosition)
{
    anim->sheet = sheet;
    anim->pokemon = pokemon;
    anim->variant = variant;
    anim->pkmnPosition = pkmnPosition;
    anim->currentScale = 1.0f;
    anim->frameCounter = 0;
    anim->active = 1;
}

void PkmnShrinkAnimUpdate(PkmnShrinkAnim *anim)
{
    if (anim->frameCounter > PKMN_SHRINK_FRAMES + PKMN_SHRINK_FRAMES_OFFSET)
    {
        anim->active = 0;
        return;
    }

    if (anim->frameCounter < PKMN_SHRINK_FRAMES_OFFSET)
    {
        anim->frameCounter++;
        return;
    }

    int frames = anim->frameCounter - PKMN_SHRINK_FRAMES_OFFSET;
    anim->currentScale = 1 - ((float)frames / (float)PKMN_SHRINK_FRAMES);
    anim->frameCounter++;
}

void PkmnShrinkAnimDraw(PkmnShrinkAnim *anim)
{
    PkmnSpriteSheetDrawScale(anim->sheet, anim->pokemon, anim->variant, anim->pkmnPosition, anim->currentScale);
}