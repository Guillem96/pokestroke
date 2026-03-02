#ifndef POKEDEX_GUI_H
#define POKEDEX_GUI_H

#include "pokedex.h"
#include "pkmn_sprite_sheet.h"
#include "pkmn_battle_sprite_sheet.h"

#define POKEDEX_PAGE_SIZE 8
#define POKEDEX_WINDOW_HEIGHT 670
#define POKEDEX_WINDOW_WIDTH 512

#define POKEDEX_MAIN_SCREEN_WIDTH POKEDEX_WINDOW_WIDTH * 0.75f
#define POKEDEX_MAIN_SCREEN_HEIGHT 260
#define POKEDEX_MAIN_SCREEN_X POKEDEX_WINDOW_WIDTH / 2 - POKEDEX_MAIN_SCREEN_WIDTH / 2
#define POKEDEX_MAIN_SCREEN_Y 150

typedef struct
{
    Pokedex *pokedex;
    PokemonSpriteSheet *sheet;
    PkmnBattleSpriteSheet *battleSheet;

    // State
    unsigned int selectedPkmnIndex;
    unsigned int page;
} PokedexGUI;

void PokedexGUIInit(PokedexGUI *gui, Pokedex *pokedex, PokemonSpriteSheet *sheet, PkmnBattleSpriteSheet *battleSheet);
void PokedexGUIUpdate(PokedexGUI *gui);
void PokedexGUIDraw(PokedexGUI *gui);
void PokedexGUIUnload(PokedexGUI *gui);

#endif