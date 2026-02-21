#include "pkmn_grow_anim.h"
#include "pkmn_sprite_sheet.h"

void PkmnGrowAnimInit(PkmnGrowAnim *anim, PokemonSpriteSheet *sheet, int pokemon, unsigned short variant, Vector2 pkmnPosition)
{
    anim->sheet = sheet;
    anim->pokemon = pokemon;
    anim->variant = variant;
    anim->pkmnPosition = pkmnPosition;
    anim->currentScale = 0.0f;
    anim->frameCounter = 0;
    anim->active = 1;
}

void PkmnGrowAnimUpdate(PkmnGrowAnim *anim)
{
    if (!anim->active)
        return;

    if (anim->frameCounter > PKMN_GROW_FRAMES)
    {
        anim->active = 0;
        return;
    }

    anim->currentScale = (float)anim->frameCounter / (float)PKMN_GROW_FRAMES;
    anim->frameCounter++;
}

void PkmnGrowAnimDraw(PkmnGrowAnim *anim)
{
    PkmnSpriteSheetDrawScale(anim->sheet, anim->pokemon, anim->variant, anim->pkmnPosition, anim->currentScale);
}