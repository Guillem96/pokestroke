#include "dialog_box.h"
#include "raylib.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void DialogBoxInit(DialogBox *box, const char *text, float framesPerChar, Rectangle bounds, unsigned int fontSize, unsigned int padding, bool alignTextRight)
{
    strcpy(box->text, text);
    box->currentCharIndex = 0;
    box->fontSize = fontSize;
    box->padding = padding;
    box->totalChars = strlen(text);
    box->alignTextRight = alignTextRight;
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

    if (box->framesPerChar <= 0)
    {
        box->currentCharIndex = box->totalChars;
        box->isComplete = 1;
        return;
    }

    box->frameCounter += 1;
    box->currentCharIndex = box->frameCounter / box->framesPerChar;
    if (box->currentCharIndex > box->totalChars)
    {
        box->currentCharIndex = box->totalChars;
        box->isComplete = 1;
    }
}

void DialogBoxDraw(const DialogBox *box)
{
    char ss[512];
    strcpy(ss, box->text);
    ss[box->currentCharIndex] = '\0';

    DrawRectangleRounded((Rectangle){box->bounds.x - box->padding, box->bounds.y - box->padding, box->bounds.width + box->padding * 2, box->bounds.height + box->padding * 2}, 0.1f, 8, WHITE);
    DrawRectangleRoundedLinesEx(box->bounds, 0.1f, 8, 2, BLACK);

    if (box->alignTextRight)
    {
        int textSize = MeasureText(ss, box->fontSize);
        DrawText(ss, box->bounds.x + box->bounds.width - textSize - box->padding * 4, box->bounds.y + box->padding * 2, box->fontSize, BLACK);
        return;
    }
    DrawText(ss, box->bounds.x + box->padding * 4, box->bounds.y + box->padding * 2, box->fontSize, BLACK);
}

void DialogBoxUnload(DialogBox *box)
{
}
