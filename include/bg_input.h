#ifndef BG_INPUT_H
#define BG_INPUT_H

#include <stdbool.h>
#include "raylib.h"

void InitBackgroundInput();
bool IsBackgroundKeyDown(int vKey);
bool IsBackgroundKeyPressed(int vKey);
Vector2 GetMouseScreenPosition();

// New: Returns the key code of the first key pressed this frame, or 0 if none.
int GetBackgroundAnyKeyPressed();

#endif