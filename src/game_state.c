#include "game_state.h"

#include <stdio.h>

#include "raylib.h"
#include "pokedex.h"

#define SECOND_IN_DAY 86400

static void InitializeGameStateFromFile(FILE *file, GameState *state);
static void InitializeBrandNewGameState(GameState *state);
static void LogGameState(const GameState *state);

void GameStateInit(GameState *state, const char *filePath)
{
    FILE *file = fopen(filePath, "rb");
    if (file == NULL)
    {
        TraceLog(LOG_INFO, "No existing game state found, starting fresh.");
        InitializeBrandNewGameState(state);
        LogGameState(state);
        return;
    }
    InitializeGameStateFromFile(file, state);
    LogGameState(state);
}

void GameStateRecordKeyStroke(GameState *state)
{
    time_t now = time(NULL);
    double secondsSinceStart = difftime(now, state->startTime);
    unsigned int dayIndex = (unsigned int)(secondsSinceStart / SECOND_IN_DAY);

    if (dayIndex < state->currentCapacity)
    {
        state->keyStrokesByDay[dayIndex]++;
        return;
    }

    unsigned int newCapacity = state->currentCapacity * 2;
    while (newCapacity <= dayIndex)
    {
        newCapacity *= 2;
    }
    unsigned long long *newArray = realloc(state->keyStrokesByDay, sizeof(unsigned long long) * newCapacity);
    if (newArray == NULL)
    {
        fprintf(stderr, "Error reallocating memory for keyStrokesByDay\n");
        exit(1);
    }
    for (unsigned int i = state->currentCapacity; i < newCapacity; i++)
    {
        newArray[i] = NO_DAY_DATA;
    }
    state->keyStrokesByDay = newArray;
    state->currentCapacity = newCapacity;
}

unsigned long long GameStateGetTotalKeyStrokes(const GameState *state)
{
    unsigned long long total = 0;
    for (unsigned int i = 0; i < state->currentCapacity; i++)
    {
        if (state->keyStrokesByDay[i] == NO_DAY_DATA)
        {
            return total;
        }
        total += state->keyStrokesByDay[i];
    }
    return total;
}

unsigned int GameStateGetNumDays(const GameState *state)
{
    for (unsigned int i = 0; i < state->currentCapacity; i++)
    {
        if (state->keyStrokesByDay[i] == NO_DAY_DATA)
        {
            return i;
        }
    }
    return state->currentCapacity;
}

void GameStateSave(const GameState *state, const char *filePath)
{
    FILE *file = fopen(filePath, "wb");
    if (file == NULL)
    {
        fprintf(stderr, "Error opening file for writing: %s\n", filePath);
        return;
    }

    fwrite(&state->currentCapacity, sizeof(unsigned int), 1, file);
    fwrite(state->keyStrokesByDay, sizeof(unsigned long long), state->currentCapacity, file);
    fwrite(&state->startTime, sizeof(time_t), 1, file);
    fwrite(state->pokedex->registered, sizeof(unsigned short), POKEMON_COUNT * VARIANT_COUNT, file);

    fclose(file);
}

void GameStateFree(GameState *state)
{
    LogGameState(state);
    free(state->keyStrokesByDay);
    state->keyStrokesByDay = NULL;

    PokedexUnload(state->pokedex);
    free(state->pokedex);
    state->pokedex = NULL;
}

static void InitializeGameStateFromFile(FILE *file, GameState *state)
{
    fread(&state->currentCapacity, sizeof(unsigned int), 1, file);
    state->keyStrokesByDay = malloc(sizeof(unsigned long long) * state->currentCapacity);
    fread(state->keyStrokesByDay, sizeof(unsigned long long), state->currentCapacity, file);
    fread(&state->startTime, sizeof(time_t), 1, file);

    state->pokedex = malloc(sizeof(Pokedex));
    PokedexInit(state->pokedex);
    fread(state->pokedex->registered, sizeof(unsigned short), POKEMON_COUNT * VARIANT_COUNT, file);

    fclose(file);
}

static void InitializeBrandNewGameState(GameState *state)
{
    state->currentCapacity = INITIAL_CAPACITY;
    state->keyStrokesByDay = malloc(sizeof(unsigned long long) * INITIAL_CAPACITY);
    for (int i = 0; i < INITIAL_CAPACITY; i++)
    {
        state->keyStrokesByDay[i] = NO_DAY_DATA;
    }
    state->startTime = time(NULL);
    state->pokedex = malloc(sizeof(Pokedex));
    PokedexInit(state->pokedex);
}

static inline void LogGameState(const GameState *state)
{
    printf("GameState:\n");
    printf("  nDays: %u\n", GameStateGetNumDays(state));
    printf("  currentCapacity: %u\n", state->currentCapacity);
    printf("  keyStrokesByDay: [");
    for (unsigned int i = 0; i < GameStateGetNumDays(state); i++)
    {
        printf("%llu", state->keyStrokesByDay[i]);
        if (i < GameStateGetNumDays(state) - 1)
            printf(", ");
    }
    printf("]\n");
    printf("  startTime: %ld\n", state->startTime);
}