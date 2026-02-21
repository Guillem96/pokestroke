#ifndef DIALOG_BOX_H
#define DIALOG_BOX_H
#include "raylib.h"

typedef struct
{
    char text[512];
    unsigned int currentCharIndex;
    unsigned int totalChars;
    float framesPerChar;
    float frameCounter;
    unsigned short isComplete;
    Rectangle bounds;
} DialogBox;

void DialogBoxInit(DialogBox *box, const char *text, float framesPerChar, Rectangle bounds);
void DialogBoxClearAndUpdateText(DialogBox *box, const char *text);
void DialogBoxUpdate(DialogBox *box);
void DialogBoxDraw(const DialogBox *box);
void DialogBoxUnload(DialogBox *box);

#endif // DIALOG_BOX_H