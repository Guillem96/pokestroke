#include "pokeball_anim.h"
#include "raylib.h"
#include "pkmn_battle_sprite_sheet.h"
#include <math.h>

static void PlayOpeningAnimation(PokeballAnim *anim);
static void PlayClosingAnimation(PokeballAnim *anim);
static void PlayShakingAnimation(PokeballAnim *anim);
static void PlayThrowingAnimation(PokeballAnim *anim);
static void PlayBouncingAnimation(PokeballAnim *anim);
static void PlayExplodingAnimation(PokeballAnim *anim, unsigned short isBreakFree);
static void ShakePokeballEveryN(PokeballAnim *anim, int frame, int nFrames);

static Vector2 QuadraticBezier(Vector2 p0, Vector2 p1, Vector2 p2, float t);

void PokeballAnimInit(
    PokeballAnim *anim, PokemonBattleSpriteSheet *sheet,
    Vector2 throwingStartPosition,
    Vector2 pokemonPosition,
    int pokeballType, int nShakes, int isSuccessfulCatch)
{
    anim->pokeballType = pokeballType;
    anim->status = POKEBALL_ANIM_STATUS_THROW;
    anim->frameCounter = 0;
    anim->throwingStartPosition = throwingStartPosition;
    anim->pokemonPosition = pokemonPosition;
    anim->pokeballStatus = POKEBALL_STATUS_IDLE;
    anim->nShakes = nShakes;
    anim->isSuccessfulCatch = isSuccessfulCatch;
    anim->position = throwingStartPosition;
    anim->sheet = sheet;
    anim->active = 1;
}

void PokeballAnimUpdate(PokeballAnim *anim)
{
    if (anim->active == 0)
        return;

    if (anim->status == POKEBALL_ANIM_STATUS_OPENING)
    {
        PlayOpeningAnimation(anim);
    }
    else if (anim->status == POKEBALL_ANIM_STATUS_EXPLODING || anim->status == POKEBALL_ANIM_STATUS_BREAK_FREE)
    {
        PlayExplodingAnimation(anim, anim->status == POKEBALL_ANIM_STATUS_BREAK_FREE);
    }
    else if (anim->status == POKEBALL_ANIM_STATUS_CLOSING)
    {
        PlayClosingAnimation(anim);
    }
    else if (anim->status == POKEBALL_ANIM_STATUS_SHAKING)
    {
        PlayShakingAnimation(anim);
    }
    else if (anim->status == POKEBALL_ANIM_STATUS_THROW)
    {
        PlayThrowingAnimation(anim);
    }
    else if (anim->status == POKEBALL_ANIM_STATUS_BOUNCING)
    {
        PlayBouncingAnimation(anim);
    }
}

void PokeballAnimDraw(PokeballAnim *anim)
{
    if (anim->status == POKEBALL_ANIM_STATUS_BREAK_FREE)
    {
        BattleSpriteSheetPokeballExplosionDraw(anim->sheet, anim->explosionStatus, anim->pokemonPosition.x - POKEBALL_EXPLOSION_REAL_SIZE / 2, anim->pokemonPosition.y - POKEBALL_EXPLOSION_REAL_SIZE / 2);
        return;
    }

    if (anim->status == POKEBALL_ANIM_STATUS_OPENING || anim->status == POKEBALL_ANIM_STATUS_CLOSING || anim->status == POKEBALL_ANIM_STATUS_EXPLODING)
    {
        BattleSpriteSheetPokeballDraw(anim->sheet, anim->pokeballType, POKEBALL_STATUS_HALF_DOWN, anim->pokemonPosition.x - POKEBALL_SIZE / 2, anim->pokemonPosition.y - POKEBALL_SIZE / 4);
        BattleSpriteSheetPokeballDraw(anim->sheet, anim->pokeballType, POKEBALL_STATUS_HALF_UP, anim->position.x - POKEBALL_SIZE / 2, anim->position.y - POKEBALL_SIZE / 2);
    }
    else
    {
        BattleSpriteSheetPokeballDraw(anim->sheet, anim->pokeballType, anim->pokeballStatus, anim->position.x - POKEBALL_SIZE / 2, anim->position.y - POKEBALL_SIZE);
    }

    if (anim->status == POKEBALL_ANIM_STATUS_EXPLODING)
    {
        BattleSpriteSheetPokeballExplosionDraw(anim->sheet, anim->explosionStatus, anim->pokemonPosition.x - POKEBALL_EXPLOSION_REAL_SIZE / 2, anim->pokemonPosition.y - POKEBALL_EXPLOSION_REAL_SIZE / 2);
    }
}

void PokeballAnimUnload(PokeballAnim *anim)
{
}

static void PlayOpeningAnimation(PokeballAnim *anim)
{
    int height = 42;
    float progress = (float)anim->frameCounter / POKEBALL_ANIM_OPENING_FRAMES;
    if (anim->frameCounter > POKEBALL_ANIM_OPENING_FRAMES)
    {
        TraceLog(LOG_DEBUG, "Pokeball finished opening animation, starting exploding animation");
        anim->status = POKEBALL_ANIM_STATUS_EXPLODING;
        anim->frameCounter = 0;
        return;
    }

    anim->position.y = anim->pokemonPosition.y - height * progress;
    anim->frameCounter++;
}

static void PlayClosingAnimation(PokeballAnim *anim)
{
    int height = 42;
    float progress = (float)anim->frameCounter / POKEBALL_ANIM_CLOSING_FRAMES;
    if (anim->frameCounter >= POKEBALL_ANIM_CLOSING_FRAMES)
    {
        TraceLog(LOG_DEBUG, "Pokeball finished closing animation, starting bouncing animation");
        anim->status = POKEBALL_ANIM_STATUS_BOUNCING;
        anim->frameCounter = 0;
        return;
    }

    anim->position.y = anim->pokemonPosition.y - height * (1 - progress);
    anim->frameCounter++;
}

static void PlayShakingAnimation(PokeballAnim *anim)
{
    TraceLog(LOG_INFO, "Playing shaking animation, frameCounter: %d, nShakes: %d pokeball status: %d", anim->frameCounter, anim->nShakes, anim->pokeballStatus);
    if (anim->frameCounter > POKEBALL_ANIM_SHAKE_FRAME)
    {
        // End of the animation
        // Remove one shake and reset the frame counter to loop the animation if there are shakes left
        anim->frameCounter = 0;
        anim->nShakes--;
        if (anim->nShakes <= 0 && anim->isSuccessfulCatch)
        {
            anim->status = 999;
            anim->active = 0;
            return;
        }
        else if (anim->nShakes <= 0 && !anim->isSuccessfulCatch)
        {
            anim->status = POKEBALL_ANIM_STATUS_BREAK_FREE;
            return;
        }
    }
    anim->frameCounter++;
    ShakePokeballEveryN(anim, anim->frameCounter, POKEBALL_ANIM_SHAKE_FRAME / 2);
}

static void PlayThrowingAnimation(PokeballAnim *anim)
{
    if (anim->frameCounter >= POKEBALL_ANIM_THORW_FRAMES)
    {
        TraceLog(LOG_DEBUG, "Pokeball finished throwing animation, starting opening animation");
        anim->status = POKEBALL_ANIM_STATUS_OPENING;
        anim->frameCounter = 0;
        return;
    }

    ShakePokeballEveryN(anim, anim->frameCounter, 4);

    anim->frameCounter++;
    float t = (float)anim->frameCounter / POKEBALL_ANIM_THORW_FRAMES;
    TraceLog(LOG_INFO, "Playing throwing animation, frameCounter: %d, t: %f", anim->frameCounter, t);
    anim->position = QuadraticBezier(
        anim->throwingStartPosition,
        (Vector2){
            (anim->throwingStartPosition.x + anim->pokemonPosition.x - POKEBALL_SIZE) * 0.5f,
            (anim->throwingStartPosition.y + anim->pokemonPosition.y - POKEBALL_SIZE) * 0.5f - 100},
        (Vector2){anim->pokemonPosition.x, anim->pokemonPosition.y},
        t);
}

static void PlayBouncingAnimation(PokeballAnim *anim)
{
    int singleBounceFrames = POKEBALL_ANIM_BOUNCING_FRAMES / 4;
    float decay = 0.55f;
    int initialHeight = 50;

    if (anim->frameCounter > POKEBALL_ANIM_BOUNCING_FRAMES)
    {
        TraceLog(LOG_DEBUG, "Pokeball finished bouncing animation, starting shaking animation");
        anim->status = POKEBALL_ANIM_STATUS_SHAKING;
        anim->frameCounter = 0;
        return;
    }

    float currentBounce = anim->frameCounter / singleBounceFrames;
    float t = (float)(anim->frameCounter % singleBounceFrames) / singleBounceFrames;
    float height = initialHeight * powf(decay, currentBounce);
    anim->frameCounter++;

    ShakePokeballEveryN(anim, anim->frameCounter, 2);

    anim->position = QuadraticBezier(
        (Vector2){anim->pokemonPosition.x, anim->pokemonPosition.y},
        (Vector2){anim->pokemonPosition.x, anim->pokemonPosition.y - height},
        (Vector2){anim->pokemonPosition.x, anim->pokemonPosition.y},
        t);
}

static void PlayExplodingAnimation(PokeballAnim *anim, unsigned short isBreakFree)
{
    float progress = (float)anim->frameCounter / POKEBALL_ANIM_EXPLODING_FRAMES;

    if (anim->frameCounter >= POKEBALL_ANIM_EXPLODING_FRAMES)
    {
        TraceLog(LOG_DEBUG, "Pokeball finished exploding animation, starting closing animation");
        anim->status = isBreakFree ? 999 : POKEBALL_ANIM_STATUS_CLOSING;
        anim->active = !isBreakFree;
        anim->frameCounter = 0;
        return;
    }

    int nAnimFrames = POKEBALL_EXPLOSION_ROWS * POKEBALL_EXPLOSION_COLS;
    anim->explosionStatus = (anim->frameCounter * nAnimFrames) / POKEBALL_ANIM_EXPLODING_FRAMES;
    anim->frameCounter++;
}

Vector2 QuadraticBezier(Vector2 p0, Vector2 p1, Vector2 p2, float t)
{
    float u = 1.0f - t;
    return (Vector2){
        u * u * p0.x + 2 * u * t * p1.x + t * t * p2.x,
        u * u * p0.y + 2 * u * t * p1.y + t * t * p2.y};
}

void ShakePokeballEveryN(PokeballAnim *anim, int frame, int nFrames)
{
    if (anim->frameCounter % nFrames == 0 && anim->pokeballStatus == POKEBALL_STATUS_IDLE)
    {
        anim->pokeballStatus = POKEBALL_STATUS_LEFT;
    }
    else if (anim->frameCounter % nFrames == 0 && anim->pokeballStatus == POKEBALL_STATUS_LEFT)
    {
        anim->pokeballStatus = POKEBALL_STATUS_IDLE_MIDDLE;
    }
    else if (anim->frameCounter % nFrames == 0 && anim->pokeballStatus == POKEBALL_STATUS_IDLE_MIDDLE)
    {
        anim->pokeballStatus = POKEBALL_STATUS_RIGHT;
    }
    else if (anim->frameCounter % nFrames == 0 && anim->pokeballStatus == POKEBALL_STATUS_RIGHT)
    {
        anim->pokeballStatus = POKEBALL_STATUS_IDLE;
    }
}