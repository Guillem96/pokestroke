#ifndef POKEMON_SPRITE_SHEET_H
#define POKEMON_SPRITE_SHEET_H

#include "raylib.h"

#define PKMN_PER_ROW 10
#define PKMN_ROW_INITIAL_OFFSET_PX 18
#define PKMN_ROW_HEIGHT_PX (56 * 3 + 19)

#define PKMN_TOTAL 256

#define PKMN_HEIGHT 56
#define PKMN_WIDTH 56

#define PKMN_VARIANTS 3
#define PKMN_VARIANT_REGULAR 0
#define PKMN_VARIANT_SHINY 1
#define PKMN_VARIANT_BW 2

typedef struct
{
    Texture2D texture;
} PokemonSpriteSheet;

void PkmnSpriteSheetLoad(PokemonSpriteSheet *sheet, const char *filename);
void PkmnSpriteSheetDraw(PokemonSpriteSheet *sheet, int pokemon, unsigned short variant, Vector2 pos);
void PkmnSpriteSheetDrawScale(PokemonSpriteSheet *sheet, int pokemon, unsigned short variant, Vector2 pos, float scale);
void PkmnSpriteSheetUnload(PokemonSpriteSheet *sheet);

#endif // POKEMON_SPRITE_SHEET_H