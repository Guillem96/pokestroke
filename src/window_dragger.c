#include "window_dragger.h"
#include "raylib.h"
#include "bg_input.h"

static Vector2 offset;

static bool isDrag;

void WindowDraggerInit()
{
    isDrag = false;
    offset = (Vector2){0, 0};
}

void WindowDraggerUpdate()
{
    Vector2 mousePosition = GetMouseScreenPosition();
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !isDrag)
    {
        if (IsKeyDown(KEY_LEFT_SUPER) || IsKeyDown(KEY_LEFT_CONTROL))
        {
            Vector2 windowPosition = GetWindowPosition();
            isDrag = true;
            offset.x = mousePosition.x - windowPosition.x;
            offset.y = mousePosition.y - windowPosition.y;
        }
    }

    if (isDrag)
    {
        SetWindowPosition(
            (int)(mousePosition.x - offset.x),
            (int)(mousePosition.y - offset.y));

        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
            isDrag = false;
    }
}
