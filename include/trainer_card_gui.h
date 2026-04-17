#ifndef TRAINER_CARD_GUI_H
#define TRAINER_CARD_GUI_H

#include "raylib.h"
#include "ribbon_sheet.h"
#include "shiny_sparkle_anim.h"
#include "game_state.h"
#include "shiny_charm_sheet.h"
#include "dialog_box.h"

#define TRAINER_CARD_WINDOW_WIDTH 512
#define TRAINER_CARD_WINDOW_HEIGHT 700

typedef struct
{
    GameState *gameState;
    RibbonSheet *ribbonSheet;
    ShinyCharmSheet *shinyCharmSheet;
    ShinySparkleAnim *shinySparkleAnim;
    DialogBox *descriptionBox;

    // State
    unsigned int selectedRibbon;
    unsigned int page;
    unsigned int elapsedFrames;
    bool selectionEnabled;
} TrainerCardGUI;

void TrainerCardGUIInit(TrainerCardGUI *tcg, RibbonSheet *ribbonSheet, ShinySparkleSheet *shinySparkleSheet, ShinyCharmSheet *shinyCharmSheet, GameState *gameState);
void TrainerCardGUIUpdate(TrainerCardGUI *tcg);
void TrainerCardGUIDraw(TrainerCardGUI *tcg);
void TrainerCardGUIUnload(TrainerCardGUI *tcg);

#endif
