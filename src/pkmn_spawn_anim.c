#include "pkmn_spawn_anim.h"
#include <math.h>
#define STARTING_OFFSET_X 55

static void DrawSparkle(Vector2 pos, float scale, Color color)
{
    int s = (int)(4 * scale); // Base size
    // Horizontal bar
    DrawRectangle(pos.x - s, pos.y - 1, s * 2, 2, color);
    // Vertical bar
    DrawRectangle(pos.x - 1, pos.y - s, 2, s * 2, color);
    // Center glow
    DrawRectangle(pos.x - 2, pos.y - 2, 4, 4, WHITE);
}

void PokemonSpawnAnimationInit(PokemonSpawnAnimation *anim, PokemonSpriteSheet *sheet, Vector2 targetPosition, int pokemonId, int pokemonVariant)
{
    anim->sheet = sheet;
    anim->targetPosition = targetPosition;
    anim->position = (Vector2){STARTING_OFFSET_X, targetPosition.y};
    anim->frameCounter = 0;
    anim->pokemonId = pokemonId;
    anim->pokemonVariant = pokemonVariant;
    anim->active = 1;
    anim->drawSparkles = false;
}

void PokemonSpawnAnimationUpdate(PokemonSpawnAnimation *anim)
{
    if (!anim->active)
        return;

    if (anim->frameCounter >= POKEMON_SPAWN_ANIMATION_FRAMES && anim->pokemonVariant == PKMN_VARIANT_REGULAR)
    {
        anim->active = 0;
        anim->drawSparkles = false;
        return;
    }
    else if (anim->frameCounter >= POKEMON_SPAWN_ANIMATION_FRAMES + PKMN_SPARKLES_FRAMES)
    {
        anim->active = 0;
        anim->drawSparkles = false;
        return;
    }

    if (anim->frameCounter <= POKEMON_SPAWN_ANIMATION_FRAMES)
    {
        float progress = (float)anim->frameCounter / POKEMON_SPAWN_ANIMATION_FRAMES;
        anim->position.x = STARTING_OFFSET_X + (anim->targetPosition.x - STARTING_OFFSET_X) * progress;
    }
    else
    {
        anim->drawSparkles = true;
        float progress = (float)(anim->frameCounter - POKEMON_SPAWN_ANIMATION_FRAMES) / (float)PKMN_SPARKLES_FRAMES;
        float radius = progress * PKMN_HEIGHT / 1.5f; // Expands outward

        for (int i = 0; i < PKMN_SPAWN_NUM_SPARKLES; i++)
        {
            // Calculate angle for each sparkle
            float angle = (i * (360.0f / PKMN_SPAWN_NUM_SPARKLES)) * DEG2RAD;

            Vector2 sparklePos = {
                anim->position.x + PKMN_WIDTH / 2 + cosf(angle) * radius,
                anim->position.y + PKMN_WIDTH / 2 + sinf(angle) * radius};

            float scale = 1.f + progress;
            anim->sparkles[i].position = sparklePos;
            anim->sparkles[i].scale = scale;
        }
    }

    anim->frameCounter++;
}

void PokemonSpawnAnimationDraw(PokemonSpawnAnimation *anim)
{
    PkmnSpriteSheetDraw(anim->sheet, anim->pokemonId, anim->pokemonVariant, anim->position);

    if (!anim->drawSparkles)
        return;

    for (int i = 0; i < PKMN_SPAWN_NUM_SPARKLES; i++)
        DrawSparkle(anim->sparkles[i].position, anim->sparkles[i].scale, GOLD);
}
