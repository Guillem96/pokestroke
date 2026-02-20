#ifndef PKMN_BATTLE_SPRITE_SHEET_H
#define PKMN_BATTLE_SPRITE_SHEET_H

#include "raylib.h"

#define POKEBALL_TYPE_POKEBALL 0
#define POKEBALL_TYPE_GREATBALL 1
#define POKEBALL_TYPE_ULTRABALL 2
#define POKEBALL_TYPE_MASTERBALL 3

#define POKEBALL_STATUS_IDLE 0
#define POKEBALL_STATUS_LEFT 1
#define POKEBALL_STATUS_IDLE_MIDDLE 2
#define POKEBALL_STATUS_RIGHT 3
#define POKEBALL_STATUS_HALF_UP 4
#define POKEBALL_STATUS_HALF_DOWN 5

#define POKEBALL_SHEET_OFFSET_X 345
#define POKEBALL_SHEET_OFFSET_Y 312
#define POKEBALL_MARGIN 8
#define POKEBALL_SIZE 16

#define POKEBALL_EXPLOSION_SHEET_OFFSET_X 346
#define POKEBALL_EXPLOSION_SHEET_OFFSET_Y 160
#define POKEBALL_EXPLOSION_WIDTH 48
#define POKEBALL_EXPLOSION_HEIGHT 52
#define POKEBALL_EXPLOSION_REAL_SIZE 40
#define POKEBALL_EXPLOSION_MARGIN 8
#define POKEBALL_EXPLOSION_STATUS_1 0
#define POKEBALL_EXPLOSION_STATUS_2 1
#define POKEBALL_EXPLOSION_STATUS_3 2
#define POKEBALL_EXPLOSION_STATUS_4 3
#define POKEBALL_EXPLOSION_COLS 2
#define POKEBALL_EXPLOSION_ROWS 2

#define PKMN_NAME_BAR_OFFSET_X 184
#define PKMN_NAME_BAR_OFFSET_Y 24
#define PKMN_NAME_BAR_WIDTH 79
#define PKMN_NAME_BAR_HEIGHT 15

#define PKMN_CAUGHT_INDIDATOR_OFFSET_X 345
#define PKMN_CAUGHT_INDIDATOR_OFFSET_Y 8

#define PKMN_NON_CAUGHT_INDIDATOR_OFFSET_X 345
#define PKMN_NON_CAUGHT_INDIDATOR_OFFSET_Y 24

#define PKMN_INDICATOR_SIZE 8

#define PKMN_NAME_OFFSET_X 15

typedef struct
{
    Texture2D texture;
} PokemonBattleSpriteSheet;

void BattleSpriteSheetInit(PokemonBattleSpriteSheet *sheet, const char *filename);
void BattleSpriteSheetPokeballDraw(
    PokemonBattleSpriteSheet *sheet, unsigned short pokeballType, unsigned short status, int x, int y);
void BattleSpriteSheetPokeballExplosionDraw(
    PokemonBattleSpriteSheet *sheet, unsigned short status, int x, int y);
void BattleSpriteSheetDrawPkmnName(
    PokemonBattleSpriteSheet *sheet, const char *name, unsigned short *isCaught, int x, int y);
void BattleSpriteSheetUnload(PokemonBattleSpriteSheet *sheet);

#endif // PKMN_BATTLE_SPRITE_SHEET_H