#include "dialog_box.h"
#include "raylib.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void DialogBoxInit(DialogBox *box, const char *text, float framesPerChar, Rectangle bounds)
{

    strcpy(box->text, text);
    box->currentCharIndex = 0;
    box->totalChars = strlen(text);
    box->framesPerChar = framesPerChar;
    box->bounds = bounds;
    box->frameCounter = 0;
    box->isComplete = 0;
}

void DialogBoxClearAndUpdateText(DialogBox *box, const char *text)
{
    strcpy(box->text, text);
    box->currentCharIndex = 0;
    box->totalChars = strlen(text);
    box->frameCounter = 0;
    box->isComplete = 0;
}

void DialogBoxUpdate(DialogBox *box)
{
    if (box->isComplete)
        return;

    box->frameCounter += 1;
    if (box->frameCounter >= box->framesPerChar)
    {
        box->frameCounter = 0;
        box->currentCharIndex++;
        if (box->currentCharIndex > box->totalChars)
        {
            box->currentCharIndex = box->totalChars;
            box->isComplete = 1;
        }
    }
}

void DialogBoxDraw(const DialogBox *box)
{
    char ss[512];
    strcpy(ss, box->text);
    ss[box->currentCharIndex] = '\0';

    DrawRectangleRounded((Rectangle){box->bounds.x - 2, box->bounds.y - 2, box->bounds.width + 4, box->bounds.height + 4}, 0.1f, 8, WHITE);
    DrawRectangleRoundedLinesEx(box->bounds, 0.1f, 8, 2, BLACK);
    DrawText(ss, box->bounds.x + 8, box->bounds.y + 8, 13, BLACK);
}

void DialogBoxUnload(DialogBox *box)
{
}
