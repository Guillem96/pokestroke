#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include "game_state.h"
#include "pkmn_sprite_sheet.h"
#include "pkmn_battle_sprite_sheet.h"
#include "pokeball_anim.h"
#include "pkmn_spawn_anim.h"
#include "dialog_box.h"
#include "pkmn_shrink_anim.h"
#include "pkmn_grow_anim.h"

#define POKEMON_POSITION (Vector2){140, 90}
#define POKEBALL_THROW_START_POSITION (Vector2){60, 180}
#define POKEMON_NAME_POSITION (Vector2){60, 75}

#define PKMN_SHYNY_PROBABILITY 0.01f
#define PKMN_BW_PROBABILITY 0.005f
#define PKMN_CATCH_SUCCESS_PROBABILITY 0.15f

#define ULTRABALL_MULTIPLIER 2.f
#define GREATBALL_MULTIPLIER 1.5f
#define POKEBALL_MULTIPLIER 1.0f
#define MASTERBALL_MULTIPLIER 255.0f

#define MASTERBALL_PROBABILITY 0.01f
#define ULTRABALL_PROBABILITY 0.1f
#define GREATBALL_PROBABILITY 0.3f

#define PKMN_FLEE_PROBABILITY 0.1f

#define GAME_MANAGER_STATE_SPAWN_POKEMON 0
#define GAME_MANAGER_STATE_ROLL_CATCH_RESULT 1
#define GAME_MANAGER_STATE_THROWING_POKEBALL 2
#define GAME_MANAGER_STATE_REGISTERING_CATCH 3
#define GAME_MANAGER_STATE_CATCH_FAILURE 4
#define GAME_MANAGER_STATE_POKEMON_FLEEING 5

typedef struct
{
    unsigned short nShakes;
    unsigned short isSuccessful;
    unsigned short pokeballType;
} PokemonCatchResult;

typedef struct
{
    unsigned int pokemonId;
    unsigned short variant;
} SpawnedPokemon;

typedef struct
{
    GameState *gameState;
    PokemonSpriteSheet *sheet;
    PokemonBattleSpriteSheet *battleSheet;

    // Animations
    PokeballAnim *pokeballAnim;
    PokemonSpawnAnimation *spawnAnim;
    PkmnShrinkAnim *shrinkAnim;
    PkmnGrowAnim *growAnim;

    // Dialog boxes
    DialogBox *bottomDialog;

    // State
    SpawnedPokemon *spawnedPokemon;
    PokemonCatchResult *catchResult;
    unsigned short currentState;

} GameManager;

void GameManagerInit(GameManager *manager, const char *filePath);
void GameManagerUpdate(GameManager *manager);
void GameManagerDraw(const GameManager *manager);
void GameManagerUnload(GameManager *manager);

#endif // GAME_MANAGER_H