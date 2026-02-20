#include "pkmn_spawn_anim.h"

#define STARTING_OFFSET_X 55
void PokemonSpawnAnimationInit(PokemonSpawnAnimation *anim, PokemonSpriteSheet *sheet, Vector2 targetPosition, int pokemonId, int pokemonVariant)
{
    anim->sheet = sheet;
    anim->targetPosition = targetPosition;
    anim->position = (Vector2){STARTING_OFFSET_X, targetPosition.y};
    anim->frameCounter = 0;
    anim->pokemonId = pokemonId;
    anim->pokemonVariant = pokemonVariant;
    anim->active = 1;
}

void PokemonSpawnAnimationUpdate(PokemonSpawnAnimation *anim)
{
    if (!anim->active)
        return;

    if (anim->frameCounter >= POKEMON_SPAWN_ANIMATION_FRAMES)
    {
        anim->active = 0;
        return;
    }
    float progress = (float)anim->frameCounter / POKEMON_SPAWN_ANIMATION_FRAMES;
    anim->position.x = STARTING_OFFSET_X + (anim->targetPosition.x - STARTING_OFFSET_X) * progress;
    anim->frameCounter++;
}

void PokemonSpawnAnimationDraw(PokemonSpawnAnimation *anim)
{
    PkmnSpriteSheetDraw(anim->sheet, anim->pokemonId, anim->pokemonVariant, anim->position);
}
