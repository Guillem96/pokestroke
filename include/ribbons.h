#ifndef RIBBONS_H
#define RIBBONS_H

#include "game_state.h"
#include "types.h"

#define RIBBON_LOCKED_FLAG 0x0
#define RIBBON_REGULAR_FLAG 0x1
#define RIBBON_SHINY_FLAG 0x2
#define RIBBON_BW_FLAG 0x4

typedef u8 (*RibbonLogicFunc)(GameState *);

typedef struct
{
    char name[32];
    char description[512];
    RibbonLogicFunc checkStatus;
} Ribbon;

extern Ribbon RIBBONS[];

u32 RibbonGetCount();

bool RibbonShinyCharmAvailable(GameState *state);

#endif
