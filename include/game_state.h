#ifndef GAME_STATE_H
#define GAME_STATE_H
#include <time.h>
#include <stdlib.h>

#include "pokedex.h"

#define INITIAL_CAPACITY 365
#define NO_DAY_DATA -1

typedef struct GameState
{
    unsigned int currentCapacity;
    unsigned long long *keyStrokesByDay;
    Pokedex *pokedex;
    time_t startTime;
} GameState;

void GameStateInit(GameState *state, const char *filePath);
void GameStateRecordKeyStroke(GameState *state);
unsigned long long GameStateGetTotalKeyStrokes(const GameState *state);
unsigned int GameStateGetNumDays(const GameState *state);
void GameStateSave(const GameState *state, const char *filePath);
void GameStateFree(GameState *state);

#endif // GAME_STATE_H