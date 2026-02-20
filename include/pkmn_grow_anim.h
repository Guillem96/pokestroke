#ifndef PKMN_GROW_H
#define PKMN_GROW_H

#include "pkmn_sprite_sheet.h"
#include "pokeball_anim.h"

#define PKMN_GROW_FRAMES 8

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
} PkmnGrowAnim;

void PkmnGrowAnimInit(PkmnGrowAnim *anim, PokemonSpriteSheet *sheet, int pokemon, unsigned short variant, Vector2 pkmnPosition);
void PkmnGrowAnimUpdate(PkmnGrowAnim *anim);
void PkmnGrowAnimDraw(PkmnGrowAnim *anim);

#endif