#ifndef POKEDEX_H
#define POKEDEX_H

extern const char *POKEMON_NAMES[];

#define POKEMON_COUNT 251
#define VARIANT_COUNT 3
#define POKEDEX_NEVER_SEEN 0
#define POKEDEX_SEEN 1
#define POKEDEX_REGISTERED 2

typedef struct
{
    unsigned short *registered;

} Pokedex;

void PokedexInit(Pokedex *pokedex);
void PokedexSeen(Pokedex *pokedex, unsigned int pokemonId, unsigned short variant);
void PokedexRegister(Pokedex *pokedex, unsigned int pokemonId, unsigned short variant);
void PokedexUnload(Pokedex *pokedex);

#endif // POKEDEX_H
