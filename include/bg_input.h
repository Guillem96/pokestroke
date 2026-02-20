#ifndef BG_INPUT_H
#define BG_INPUT_H

#include <stdbool.h>

void InitBackgroundInput();
bool IsBackgroundKeyDown(int vKey);
bool IsBackgroundKeyPressed(int vKey);

// New: Returns the key code of the first key pressed this frame, or 0 if none.
int GetBackgroundAnyKeyPressed();

#endif