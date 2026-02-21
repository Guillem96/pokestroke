#ifndef POKEBALL_ANIM_H
#define POKEBALL_ANIM_H

#include "pkmn_battle_sprite_sheet.h"
#include "raylib.h"

#define POKEBALL_ANIM_STATUS_THROW 0
#define POKEBALL_ANIM_STATUS_OPENING 1
#define POKEBALL_ANIM_STATUS_EXPLODING 2
#define POKEBALL_ANIM_STATUS_CLOSING 3
#define POKEBALL_ANIM_STATUS_BOUNCING 4
#define POKEBALL_ANIM_STATUS_SHAKING 5
#define POKEBALL_ANIM_STATUS_BREAK_FREE 6

#define POKEBALL_ANIM_THORW_FRAMES 16
#define POKEBALL_ANIM_OPENING_FRAMES 8
#define POKEBALL_ANIM_EXPLODING_FRAMES 8
#define POKEBALL_ANIM_CLOSING_FRAMES 6
#define POKEBALL_ANIM_BOUNCING_FRAMES 32
#define POKEBALL_ANIM_SHAKE_FRAME 6
#define POKEBALL_ANIM_BREAK_FEE_FRAMES 8

typedef struct
{
    // Needed to render the animation
    PkmnBattleSpriteSheet *sheet;

    // Animation parameters
    int pokeballType;
    int status;
    int nShakes;
    int isSuccessfulCatch;
    Vector2 throwingStartPosition;
    Vector2 pokemonPosition;

    // Animation state
    Vector2 position;
    Vector2 otherPosition; // Used for half of the pokeball when it's opening/closing
    int frameCounter;
    int pokeballStatus;
    int explosionStatus;
    int active;
} PokeballAnim;

void PokeballAnimInit(
    PokeballAnim *anim,
    PkmnBattleSpriteSheet *sheet,
    Vector2 throwingStartPosition,
    Vector2 pokemonPosition,
    int pokeballType, int nShakes, int isSuccessfulCatch);
void PokeballAnimUpdate(PokeballAnim *anim);
void PokeballAnimDraw(PokeballAnim *anim);
void PokeballAnimUnload(PokeballAnim *anim);

#endif // POKEBALL_ANIM_H