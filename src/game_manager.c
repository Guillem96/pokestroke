#include "game_manager.h"

#include <stdio.h>
#include <stdlib.h>

#include "game_state.h"
#include "pokedex.h"
#include "pkmn_sprite_sheet.h"
#include "pkmn_battle_sprite_sheet.h"
#include "pokeball_anim.h"
#include "pkmn_spawn_anim.h"
#include "dialog_box.h"

// "Business" logic functions
static void SpawnRandomPokemon(GameManager *manager);
static void RollCatchResult(GameManager *manager);
static void RollPokemonFlee(GameManager *manager);

// Hooks to finalize complex states
static void FinalizeCatchFailureState(GameManager *manager);
static void FinalizePokedexRegisterState(GameManager *manager);
static void FinalizePokemonFleeState(GameManager *manager);

// Hooks to initialize complex states
static void InitPokeballThrowState(GameManager *manager);
static void InitPokemonSpawnState(GameManager *manager);

// Start animations
static void StartSpawnAnimation(GameManager *manager);
static void StartPokeballAnimation(GameManager *manager);
static void StartShrinkAnimation(GameManager *manager);
static void StartGrowAnimation(GameManager *manager);

// Sub-update functions
static void SpawnPokemonPhaseUpdate(GameManager *manager);
static void RegisterCatchResultPhaseUpdate(GameManager *manager);
static void ThrowPokeballPhaseUpdate(GameManager *manager);
static void CatchFailurePhaseUpdate(GameManager *manager);

// Helpers
static void ClearContextForNextRound(GameManager *manager, unsigned short clearSpawnedPokemon, unsigned short clearCatchResult);

void GameManagerInit(GameManager *manager, const char *filePath)
{
    SetRandomSeed((unsigned int)time(NULL));
    srand((unsigned int)time(NULL));

    manager->gameState = (GameState *)malloc(sizeof(GameState));
    if (manager->gameState == NULL)
    {
        TraceLog(LOG_ERROR, "Failed to allocate memory for GameState.\n");
        exit(1);
    }

    manager->sheet = (PokemonSpriteSheet *)malloc(sizeof(PokemonSpriteSheet));
    if (manager->sheet == NULL)
    {
        TraceLog(LOG_ERROR, "Failed to allocate memory for PokemonSpriteSheet.\n");
        exit(1);
    }

    manager->battleSheet = (PkmnBattleSpriteSheet *)malloc(sizeof(PkmnBattleSpriteSheet));
    if (manager->battleSheet == NULL)
    {
        TraceLog(LOG_ERROR, "Failed to allocate memory for PkmnBattleSpriteSheet.\n");
        exit(1);
    }

    PkmnSpriteSheetLoad(manager->sheet, "pokedex.png");
    PkmnBattleSpriteSheetInit(manager->battleSheet, "battle.png");
    GameStateInit(manager->gameState, filePath);

    manager->pokeballAnim = NULL;
    manager->spawnAnim = NULL;
    manager->shrinkAnim = NULL;
    manager->growAnim = NULL;

    manager->spawnedPokemon = NULL;
    manager->catchResult = NULL;

    manager->bottomDialog = (DialogBox *)malloc(sizeof(DialogBox));
    if (manager->bottomDialog == NULL)
    {
        TraceLog(LOG_ERROR, "Failed to allocate memory for DialogBox.\n");
        exit(1);
    }
    DialogBoxInit(manager->bottomDialog, "Guillem", 1, (Rectangle){55, 170, 145, 40});

    manager->currentState = GAME_MANAGER_STATE_SPAWN_POKEMON;
}

void GameManagerUpdate(GameManager *manager)
{
    // Record keystrokes
    GameStateRecordKeyStroke(manager->gameState);

    DialogBoxUpdate(manager->bottomDialog);
    if (manager->currentState == GAME_MANAGER_STATE_POKEMON_CAUGHT_WAIT && manager->bottomDialog->isComplete)
    {
        manager->currentState = GAME_MANAGER_STATE_SPAWN_POKEMON;
        ClearContextForNextRound(manager, 1, 1);
    }
    if (manager->currentState == GAME_MANAGER_STATE_REGISTERING_CATCH && manager->bottomDialog->isComplete)
    {
        FinalizePokedexRegisterState(manager);
        DialogBoxClearAndUpdateText(manager->bottomDialog, "      ");
    }
    else if (manager->currentState == GAME_MANAGER_STATE_CATCH_FAILURE && manager->bottomDialog->isComplete)
    {
        FinalizeCatchFailureState(manager);
    }
    else if (manager->currentState == GAME_MANAGER_STATE_POKEMON_FLEEING && manager->bottomDialog->isComplete)
    {
        FinalizePokemonFleeState(manager);
    }
    else if (manager->currentState == GAME_MANAGER_STATE_REGISTERING_CATCH || manager->currentState == GAME_MANAGER_STATE_POKEMON_FLEEING || manager->currentState == GAME_MANAGER_STATE_POKEMON_CAUGHT_WAIT)
    {
        // Waiting for dialog to complete, do not update anything else
        return;
    }
    else if (manager->currentState == GAME_MANAGER_STATE_CATCH_FAILURE)
    {
        PkmnGrowAnimUpdate(manager->growAnim);
        return;
    }

    if (manager->currentState == GAME_MANAGER_STATE_SPAWN_POKEMON && manager->spawnedPokemon == NULL)
    {
        InitPokemonSpawnState(manager);
    }
    else if (manager->currentState == GAME_MANAGER_STATE_SPAWN_POKEMON)
    {
        SpawnPokemonPhaseUpdate(manager);
    }
    else if (manager->currentState == GAME_MANAGER_STATE_THROWING_POKEBALL)
    {
        ThrowPokeballPhaseUpdate(manager);
    }
}

void GameManagerDraw(const GameManager *manager)
{
    if (manager->currentState == GAME_MANAGER_STATE_SPAWN_POKEMON)
    {
        PokemonSpawnAnimationDraw(manager->spawnAnim);
    }

    if (manager->currentState != GAME_MANAGER_STATE_SPAWN_POKEMON && manager->spawnedPokemon != NULL)
    {
        PkmnBattleSpriteSheetDrawPkmnName(
            manager->battleSheet, POKEMON_NAMES[manager->spawnedPokemon->pokemonId],
            &manager->gameState->pokedex->registered[manager->spawnedPokemon->pokemonId * PKMN_VARIANTS],
            POKEMON_NAME_POSITION.x, POKEMON_NAME_POSITION.y);
    }

    if (manager->currentState == GAME_MANAGER_STATE_THROWING_POKEBALL)
    {
        PkmnShrinkAnimDraw(manager->shrinkAnim);
        PokeballAnimDraw(manager->pokeballAnim);
    }

    if (manager->currentState == GAME_MANAGER_STATE_CATCH_FAILURE)
    {
        PkmnGrowAnimDraw(manager->growAnim);
    }

    if (manager->currentState == GAME_MANAGER_STATE_REGISTERING_CATCH || manager->currentState == GAME_MANAGER_STATE_POKEMON_CAUGHT_WAIT)
    {
        PkmnBattleSpriteSheetPokeballDraw(
            manager->battleSheet,
            manager->catchResult->pokeballType,
            POKEBALL_STATUS_IDLE,
            POKEMON_POSITION.x + PKMN_WIDTH / 2 - POKEBALL_SIZE / 2, POKEMON_POSITION.y + PKMN_HEIGHT - POKEBALL_SIZE);
    }

    DialogBoxDraw(manager->bottomDialog);
}

void GameManagerUnload(GameManager *manager)
{
    GameStateSave(manager->gameState, "gamestate.dat");
    GameStateUnload(manager->gameState);
    free(manager->gameState);

    PkmnSpriteSheetUnload(manager->sheet);
    free(manager->sheet);

    PkmnBattleSpriteSheetUnload(manager->battleSheet);
    free(manager->battleSheet);

    DialogBoxUnload(manager->bottomDialog);
    free(manager->bottomDialog);

    if (manager->spawnedPokemon != NULL)
    {
        free(manager->spawnedPokemon);
    }

    if (manager->pokeballAnim != NULL)
    {
        free(manager->pokeballAnim);
    }

    if (manager->shrinkAnim != NULL)
    {
        free(manager->shrinkAnim);
    }

    if (manager->spawnAnim != NULL)
    {
        free(manager->spawnAnim);
    }

    if (manager->growAnim != NULL)
    {
        free(manager->growAnim);
    }
}

static void SpawnRandomPokemon(GameManager *manager)
{
    if (manager->spawnedPokemon != NULL)
    {
        TraceLog(LOG_ERROR, "SpawnRandomPokemon called but spawnedPokemon is not NULL");
        exit(1);
    }

    unsigned int pokemonId = GetRandomValue(0, POKEMON_COUNT - 1);
    float variantRoll = (float)rand() / (float)RAND_MAX;
    unsigned short variant;
    if (variantRoll < PKMN_BW_PROBABILITY)
    {
        variant = PKMN_VARIANT_BW;
    }
    else if (variantRoll < PKMN_SHINY_PROBABILITY)
    {
        variant = PKMN_VARIANT_SHINY;
    }
    else
    {
        variant = PKMN_VARIANT_REGULAR;
    }

    manager->spawnedPokemon = (SpawnedPokemon *)malloc(sizeof(SpawnedPokemon));
    if (manager->spawnedPokemon == NULL)
    {
        TraceLog(LOG_ERROR, "Failed to allocate memory for SpawnedPokemon.\n");
        exit(1);
    }

    manager->spawnedPokemon->pokemonId = pokemonId;
    manager->spawnedPokemon->variant = variant;

    char msg[256];
    snprintf(msg, 256, "Wild %s\nappeared!", POKEMON_NAMES[pokemonId]);
    DialogBoxClearAndUpdateText(manager->bottomDialog, msg);
    PokedexSeen(manager->gameState->pokedex, pokemonId, variant);
}

static void SpawnPokemonPhaseUpdate(GameManager *manager)
{
    PokemonSpawnAnimationUpdate(manager->spawnAnim);
    if (!manager->spawnAnim->active)
    {
        free(manager->spawnAnim);
        manager->spawnAnim = NULL;

        InitPokeballThrowState(manager);
    }
}

static void RegisterCatchResultPhaseUpdate(GameManager *manager)
{
    char msg[256];
    TraceLog(LOG_INFO, "Catch successful, registering %d with variant %d in pokedex", manager->spawnedPokemon->pokemonId, manager->spawnedPokemon->variant);

    snprintf(msg, 256, "Gotcha! %s\nwas caught!      ", POKEMON_NAMES[manager->spawnedPokemon->pokemonId]);
    DialogBoxClearAndUpdateText(manager->bottomDialog, msg);
}

static void ThrowPokeballPhaseUpdate(GameManager *manager)
{
    PokeballAnimUpdate(manager->pokeballAnim);
    PkmnShrinkAnimUpdate(manager->shrinkAnim);
    if (manager->pokeballAnim->active)
    {
        // Still playing the pokeball animation, do not update anything else
        return;
    }

    if (manager->catchResult->isSuccessful)
    {
        // When the pokeball animation finishes, and the catch was successful,
        // we want to show the "Gotcha!" dialog and register the catch in the pokedex
        manager->currentState = GAME_MANAGER_STATE_REGISTERING_CATCH;
        RegisterCatchResultPhaseUpdate(manager);
    }
    else if (manager->pokeballAnim->active == 0 && !manager->catchResult->isSuccessful)
    {
        // When the pokeball animation finishes, and the catch was unsuccessful,
        // we want to show the "Oh no!" dialog and then prepare for the next spawn
        manager->currentState = GAME_MANAGER_STATE_CATCH_FAILURE;
        CatchFailurePhaseUpdate(manager);
        StartGrowAnimation(manager);
    }

    free(manager->pokeballAnim);
    manager->pokeballAnim = NULL;

    free(manager->shrinkAnim);
    manager->shrinkAnim = NULL;
}

static void CatchFailurePhaseUpdate(GameManager *manager)
{
    // Pokemon animation finished and catch was unsuccessful, show different dialog based on number of shakes
    if (manager->catchResult->nShakes == 0)
    {
        DialogBoxClearAndUpdateText(manager->bottomDialog, "Oh no! The POKéMON\nbroke free!");
    }
    else
    {
        DialogBoxClearAndUpdateText(manager->bottomDialog, "Aww! It appeared to\nbe caught!");
    }

    ClearContextForNextRound(manager, 0, 1);
}

static void ClearContextForNextRound(GameManager *manager, unsigned short clearSpawnedPokemon, unsigned short clearCatchResult)
{
    if (clearSpawnedPokemon && manager->spawnedPokemon != NULL)
    {
        free(manager->spawnedPokemon);
        manager->spawnedPokemon = NULL;
    }

    if (clearCatchResult && manager->catchResult != NULL)
    {
        free(manager->catchResult);
        manager->catchResult = NULL;
    }
}

static void RollCatchResult(GameManager *manager)
{
    if (manager->catchResult != NULL)
    {
        TraceLog(LOG_ERROR, "RollCatchResult called but catchResult is not NULL");
        exit(1);
    }

    float multiplier;
    manager->catchResult = (PokemonCatchResult *)malloc(sizeof(PokemonCatchResult));
    if (manager->catchResult == NULL)
    {
        TraceLog(LOG_ERROR, "Failed to allocate memory for PokemonCatchResult.\n");
        exit(1);
    }

    float ballRoll = (float)rand() / (float)RAND_MAX;
    if (ballRoll < MASTERBALL_PROBABILITY)
    {
        multiplier = MASTERBALL_MULTIPLIER;
        manager->catchResult->pokeballType = POKEBALL_TYPE_MASTERBALL;
    }
    else if (ballRoll < ULTRABALL_PROBABILITY)
    {
        multiplier = ULTRABALL_MULTIPLIER;
        manager->catchResult->pokeballType = POKEBALL_TYPE_ULTRABALL;
    }
    else if (ballRoll < GREATBALL_PROBABILITY)
    {
        multiplier = GREATBALL_MULTIPLIER;
        manager->catchResult->pokeballType = POKEBALL_TYPE_GREATBALL;
    }
    else
    {
        multiplier = POKEBALL_MULTIPLIER;
        manager->catchResult->pokeballType = POKEBALL_TYPE_POKEBALL;
    }

    float catchRoll = (float)rand() / (float)RAND_MAX;
    catchRoll /= multiplier;
    manager->catchResult->isSuccessful = catchRoll < PKMN_CATCH_SUCCESS_PROBABILITY;
    manager->catchResult->nShakes = manager->catchResult->isSuccessful ? 4 : GetRandomValue(0, 3);
}

static void RollPokemonFlee(GameManager *manager)
{
    if (manager->spawnedPokemon == NULL)
    {
        TraceLog(LOG_ERROR, "RollPokemonFlee called but spawnedPokemon is NULL");
        exit(1);
    }

    float fleeRoll = (float)rand() / (float)RAND_MAX;
    if (fleeRoll < PKMN_FLEE_PROBABILITY)
    {
        // Pokemon flees, show dialog and then clean up and prepare for next spawn
        DialogBoxClearAndUpdateText(manager->bottomDialog, "Oh no! The POKéMON\nfled!  ");
        manager->currentState = GAME_MANAGER_STATE_POKEMON_FLEEING;
    }
}

static void StartSpawnAnimation(GameManager *manager)
{
    manager->spawnAnim = (PokemonSpawnAnimation *)malloc(sizeof(PokemonSpawnAnimation));
    if (manager->spawnAnim == NULL)
    {
        TraceLog(LOG_ERROR, "Failed to allocate memory for PokemonSpawnAnimation.\n");
        exit(1);
    }
    PokemonSpawnAnimationInit(
        manager->spawnAnim, manager->sheet, POKEMON_POSITION,
        manager->spawnedPokemon->pokemonId, manager->spawnedPokemon->variant);
}

void StartPokeballAnimation(GameManager *manager)
{
    manager->pokeballAnim = (PokeballAnim *)malloc(sizeof(PokeballAnim));
    if (manager->pokeballAnim == NULL)
    {
        TraceLog(LOG_ERROR, "Failed to allocate memory for PokeballAnim.\n");
        exit(1);
    }
    PokeballAnimInit(
        manager->pokeballAnim,
        manager->battleSheet,
        POKEBALL_THROW_START_POSITION,
        (Vector2){POKEMON_POSITION.x + PKMN_WIDTH / 2, POKEMON_POSITION.y + PKMN_HEIGHT},
        manager->catchResult->pokeballType,
        manager->catchResult->nShakes, manager->catchResult->isSuccessful);
}

static void StartShrinkAnimation(GameManager *manager)
{
    manager->shrinkAnim = (PkmnShrinkAnim *)malloc(sizeof(PkmnShrinkAnim));
    if (manager->shrinkAnim == NULL)
    {
        TraceLog(LOG_ERROR, "Failed to allocate memory for PkmnShrinkAnim.\n");
        exit(1);
    }
    PkmnShrinkAnimInit(
        manager->shrinkAnim,
        manager->sheet,
        manager->spawnedPokemon->pokemonId,
        manager->spawnedPokemon->variant,
        POKEMON_POSITION);
}

static void StartGrowAnimation(GameManager *manager)
{
    manager->growAnim = (PkmnGrowAnim *)malloc(sizeof(PkmnGrowAnim));
    if (manager->growAnim == NULL)
    {
        TraceLog(LOG_ERROR, "Failed to allocate memory for PkmnGrowAnim.\n");
        exit(1);
    }
    PkmnGrowAnimInit(
        manager->growAnim,
        manager->sheet,
        manager->spawnedPokemon->pokemonId,
        manager->spawnedPokemon->variant,
        POKEMON_POSITION);
}

static void InitPokemonSpawnState(GameManager *manager)
{
    SpawnRandomPokemon(manager);
    StartSpawnAnimation(manager);
}

static void InitPokeballThrowState(GameManager *manager)
{
    RollCatchResult(manager);
    StartPokeballAnimation(manager);
    StartShrinkAnimation(manager);
    manager->currentState = GAME_MANAGER_STATE_THROWING_POKEBALL;
}

static void FinalizeCatchFailureState(GameManager *manager)
{
    RollPokemonFlee(manager);

    free(manager->growAnim);
    manager->growAnim = NULL;

    if (manager->currentState != GAME_MANAGER_STATE_POKEMON_FLEEING)
    {
        // If pokemon doesn't run away, restart pokeball throwing
        InitPokeballThrowState(manager);
    }
}

static void FinalizePokedexRegisterState(GameManager *manager)
{
    PokedexRegister(manager->gameState->pokedex, manager->spawnedPokemon->pokemonId, manager->spawnedPokemon->variant);
    GameStateSave(manager->gameState, "gamestate.dat");
    manager->currentState = GAME_MANAGER_STATE_POKEMON_CAUGHT_WAIT;
}

static void FinalizePokemonFleeState(GameManager *manager)
{
    ClearContextForNextRound(manager, 1, 1);
    manager->currentState = GAME_MANAGER_STATE_SPAWN_POKEMON;
}