#ifndef POKEMON_SPAWN_ANIMATION_H
#define POKEMON_SPAWN_ANIMATION_H

#include "pkmn_sprite_sheet.h"

#define POKEMON_SPAWN_ANIMATION_FRAMES 32

typedef struct
{
    PokemonSpriteSheet *sheet;
    Vector2 targetPosition;
    Vector2 position;
    int frameCounter;
    int pokemonId;
    int pokemonVariant;
    int active;
} PokemonSpawnAnimation;

void PokemonSpawnAnimationInit(PokemonSpawnAnimation *anim, PokemonSpriteSheet *sheet, Vector2 targetPosition, int pokemonId, int pokemonVariant);
void PokemonSpawnAnimationUpdate(PokemonSpawnAnimation *anim);
void PokemonSpawnAnimationDraw(PokemonSpawnAnimation *anim);

#endif // POKEMON_SPAWN_ANIMATION_H