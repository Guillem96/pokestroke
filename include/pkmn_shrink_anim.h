#ifndef PKMN_SHRINK_H
#define PKMN_SHRINK_H

#include "pkmn_sprite_sheet.h"
#include "pokeball_anim.h"

#define PKMN_SHRINK_FRAMES_OFFSET (POKEBALL_ANIM_THORW_FRAMES + POKEBALL_ANIM_OPENING_FRAMES)
#define PKMN_SHRINK_FRAMES 8

typedef struct
{
    PokemonSpriteSheet *sheet;
    Vector2 pkmnPosition;
    int pokemon;
    unsigned short int variant;

    // Animation state
    float currentScale;
    int frameCounter;
    unsigned short active;
} PkmnShrinkAnim;

void PkmnShrinkAnimInit(PkmnShrinkAnim *anim, PokemonSpriteSheet *sheet, int pokemon, unsigned short variant, Vector2 pkmnPosition);
void PkmnShrinkAnimUpdate(PkmnShrinkAnim *anim);
void PkmnShrinkAnimDraw(PkmnShrinkAnim *anim);

#endif