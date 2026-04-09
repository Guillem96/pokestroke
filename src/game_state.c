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
        TraceLog(LOG_ERROR, "Error reallocating memory for keyStrokesByDay\n");
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
            total += 0;
        }
        else
        {
            total += state->keyStrokesByDay[i];
        }
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
        TraceLog(LOG_ERROR, "Error opening file for writing: %s\n", filePath);
        return;
    }

    unsigned short pokedexVersionMarker = 0xA; // Marker to identify pokedex v2

    fwrite(&state->currentCapacity, sizeof(unsigned int), 1, file);
    fwrite(state->keyStrokesByDay, sizeof(unsigned long long), state->currentCapacity, file);
    fwrite(&state->startTime, sizeof(time_t), 1, file);
    fwrite(&pokedexVersionMarker, sizeof(unsigned short), 1, file); // Marker to identify pokedex v2
    fwrite(state->pokedex, sizeof(Pokedex), 1, file);

    fclose(file);
}

void GameStateUnload(GameState *state)
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

    unsigned short pokedexVersionMarker;
    fread(&pokedexVersionMarker, sizeof(unsigned short), 1, file);
    if (pokedexVersionMarker != 0xA)
    {
        // Update old pokedex format to new format
        unsigned short oldPokedexData[POKEMON_COUNT * VARIANT_COUNT];
        oldPokedexData[0] = pokedexVersionMarker;
        fread(&oldPokedexData[1], sizeof(unsigned short), POKEMON_COUNT * VARIANT_COUNT - 1, file);
        for (unsigned int i = 0; i < POKEMON_COUNT; i++)
        {
            for (unsigned int j = 0; j < VARIANT_COUNT; j++)
            {
                state->pokedex->registered[i].variantStatus[j] = oldPokedexData[i * VARIANT_COUNT + j];
                state->pokedex->registered[i].caughtCount[j] = oldPokedexData[i * VARIANT_COUNT + j] == POKEDEX_REGISTERED ? 1 : 0;
            }
        }
    }
    else
    {
        fread(state->pokedex, sizeof(Pokedex), 1, file);
    }

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