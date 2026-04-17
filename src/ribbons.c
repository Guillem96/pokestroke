#include "ribbons.h"
#include <string.h>

#include "pkmn_sprite_sheet.h"

static bool AreAllCaught(Pokedex *dex, const int *ids, int count, u8 variant)
{
    for (int i = 0; i < count; i++)
    {
        // IDs are usually 1-indexed in Pokemon, adjusting to 0-index for array
        if (!PokedexIsRegistered(dex, ids[i] - 1, variant))
            return false;
    }
    return true;
}

u8 CheckEeveeFanatic(GameState *state)
{
    u8 mask = RIBBON_LOCKED_FLAG;
    int ids[] = {133, 134, 135, 136, 196, 197}; // Eevee + Vaporeon, Jolteon, Flareon, Espeon, Umbreon
    for (u8 v = 0; v < VARIANT_COUNT; v++)
    {
        if (AreAllCaught(state->pokedex, ids, 6, v))
            mask |= (1 << v);
        else
            break;
    }
    return mask;
}

u8 CheckNostalgicFingers(GameState *state)
{
    u8 mask = RIBBON_LOCKED_FLAG;
    for (u8 v = 0; v < VARIANT_COUNT; v++)
    {
        bool complete = true;
        for (int i = 0; i < 151; i++)
        {
            if (!PokedexIsRegistered(state->pokedex, i, v))
            {
                complete = false;
                break;
            }
        }
        if (complete)
            mask |= (1 << v);
        else
            break;
    }
    return mask;
}

u8 CheckBurnedFingers(GameState *state)
{
    u8 mask = RIBBON_LOCKED_FLAG;
    for (u8 v = 0; v < VARIANT_COUNT; v++)
    {
        bool complete = true;
        for (int i = 151; i < 251; i++)
        {
            if (!PokedexIsRegistered(state->pokedex, i, v))
            {
                complete = false;
                break;
            }
        }
        if (complete)
            mask |= (1 << v);
        else
            break;
    }
    return mask;
}

u8 CheckBrokenFingers(GameState *state)
{
    u8 mask = RIBBON_LOCKED_FLAG;
    for (u8 v = 0; v < VARIANT_COUNT; v++)
    {
        if (PokedexNumRegistered(state->pokedex, v) >= 251)
            mask |= (1 << v);
        else
            break;
    }
    return mask;
}

u8 CheckRookie(GameState *state)
{
    u8 mask = RIBBON_LOCKED_FLAG;
    for (u8 v = 0; v < VARIANT_COUNT; v++)
    {
        if (PokedexNumRegistered(state->pokedex, v) >= 1)
            mask |= (1 << v);
        else
            break;
    }
    return mask;
}

u8 CheckTimeTraveler(GameState *state)
{
    u8 mask = RIBBON_LOCKED_FLAG;
    for (u8 v = 0; v < VARIANT_COUNT; v++)
    {
        if (PokedexIsRegistered(state->pokedex, 250, v))
            mask |= (1 << v); // Celebi (251)
        else
            break;
    }
    return mask;
}

u8 CheckMadScientist(GameState *state)
{
    u8 mask = RIBBON_LOCKED_FLAG;
    int ids[] = {150, 151};
    for (u8 v = 0; v < VARIANT_COUNT; v++)
    {
        if (AreAllCaught(state->pokedex, ids, 2, v))
            mask |= (1 << v);
        else
            break;
    }
    return mask;
}

u8 CheckDogWhisperer(GameState *state)
{
    u8 mask = RIBBON_LOCKED_FLAG;
    int ids[] = {243, 244, 245}; // Raikou, Entei, Suicune
    for (u8 v = 0; v < VARIANT_COUNT; v++)
    {
        if (AreAllCaught(state->pokedex, ids, 3, v))
            mask |= (1 << v);
        else
            break;
    }
    return mask;
}

u8 CheckSkylineOverload(GameState *state)
{
    u8 mask = RIBBON_LOCKED_FLAG;
    int ids[] = {144, 145, 146}; // Articuno, Zapdos, Moltres
    for (u8 v = 0; v < VARIANT_COUNT; v++)
    {
        if (AreAllCaught(state->pokedex, ids, 3, v))
            mask |= (1 << v);
        else
            break;
    }
    return mask;
}

u8 CheckStarterMania(GameState *state)
{
    u8 mask = RIBBON_LOCKED_FLAG;
    int ids[] = {1, 4, 7, 152, 155, 158}; // Bulbasaur, Charmander, Squirtle, Chikorita, Cyndaquil, Totodile
    for (u8 v = 0; v < VARIANT_COUNT; v++)
    {
        if (AreAllCaught(state->pokedex, ids, 6, v))
            mask |= (1 << v);
        else
            break;
    }
    return mask;
}

u8 CheckHistoryBuff(GameState *state)
{
    u8 mask = RIBBON_LOCKED_FLAG;
    int ids[] = {138, 140, 142}; // Omanyte, Kabuto, Aerodactyl
    for (u8 v = 0; v < VARIANT_COUNT; v++)
    {
        if (AreAllCaught(state->pokedex, ids, 3, v))
            mask |= (1 << v);
        else
            break;
    }
    return mask;
}

u8 CheckHeavyLifter(GameState *state)
{
    u8 mask = RIBBON_LOCKED_FLAG;
    int ids[] = {143, 68, 76}; // Snorlax, Machamp, Golem
    for (u8 v = 0; v < VARIANT_COUNT; v++)
    {
        if (AreAllCaught(state->pokedex, ids, 3, v))
            mask |= (1 << v);
        else
            break;
    }
    return mask;
}

u8 CheckCodeDebugger(GameState *state)
{
    u8 mask = RIBBON_LOCKED_FLAG;
    // Simple subset of bugs across Gen I & II
    int ids[] = {
        10, 11, 12,
        13, 14, 15,
        46, 47,
        48, 49,
        123,
        127,
        165, 166, 167, 168,
        193,
        204, 205,
        212, 213, 214};
    for (u8 v = 0; v < VARIANT_COUNT; v++)
    {
        if (AreAllCaught(state->pokedex, ids, 22, v))
            mask |= (1 << v);
        else
            break;
    }
    return mask;
}

u8 CheckKeystrokeMilestones(GameState *state)
{
    unsigned long long total = GameStateGetTotalKeyStrokes(state);
    u8 mask = RIBBON_LOCKED_FLAG;
    if (total >= 100000)
        mask |= RIBBON_REGULAR_FLAG;
    if (total >= 500000)
        mask |= RIBBON_SHINY_FLAG;
    if (total >= 1000000)
        mask |= RIBBON_BW_FLAG;
    return mask;
}

// --- Ribbon Definitions Array ---

Ribbon RIBBONS[] = {
    {"Rookie", "Catch your very first Pokemon.", CheckRookie},
    {"Non-stop typing", "Reach 100k, 500k, and 1M keystrokes.", CheckKeystrokeMilestones},
    {"Starter Mania", "Catch all starters from both generations.", CheckStarterMania},
    {"History Buff", "Catch Omanyte, Kabuto, and Aerodactyl.", CheckHistoryBuff},
    {"Code Debugger", "Catch all Bug types, including the evolutions.", CheckCodeDebugger},
    {"Heavy Lifter", "Catch Snorlax, Machamp, and Golem.", CheckHeavyLifter},
    {"Eevee Fanatic", "Catch Eevee and all Eevee evolutions.", CheckEeveeFanatic},
    {"Mad Scientist", "Capture Mew and Mewtwo.", CheckMadScientist},
    {"Dog Whisperer", "Capture the legendary beasts (Raikou, Entei & Suicune).", CheckDogWhisperer},
    {"Time Traveler", "Capture the mythical Celebi.", CheckTimeTraveler},
    {"Nostalgic Fingers", "Complete the Kanto Pokedex.", CheckNostalgicFingers},
    {"Burned Fingers", "Complete the Johto Pokedex.", CheckBurnedFingers},
    {"Broken Fingers", "Complete the entire Pokedex.", CheckBrokenFingers},
    {"Skyline Overload", "Capture the legendary birds (Moltres, Zapdos & Articuno).", CheckSkylineOverload},
    {"", "", NULL} // Sentinel
};

u32 RibbonGetCount()
{
    u32 count = 0;
    while (RIBBONS[count].checkStatus != NULL)
    {
        count++;
    }
    return count;
}

bool RibbonShinyCharmAvailable(GameState *state)
{
    return PokedexNumRegistered(state->pokedex, PKMN_VARIANT_REGULAR) >= 251;
}