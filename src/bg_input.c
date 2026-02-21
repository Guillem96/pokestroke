#include "bg_input.h"
#include "raylib.h"

#ifdef _WIN32
#include <windows.h>
static bool keyState[256] = {0};

void InitBackgroundInput() {}

bool IsBackgroundKeyDown(int vKey)
{
    // 0x8000 checks if the key is currently held down
    return (GetAsyncKeyState(vKey) & 0x8000) != 0;
}

int GetBackgroundAnyKeyPressed()
{
    // Windows VK codes range from 0x01 to 0xFF (255)
    // We start at 0x07 to skip mouse buttons but catch ALL keys
    for (int i = 0x07; i <= 0xFF; i++)
    {
        if (IsBackgroundKeyPressed(i))
        {
            return i;
        }
    }
    return 0;
}
Vector2 GetMouseScreenPosition()
{
    POINT p;
    GetCursorPos(&p);
    return (Vector2){(float)p.x, (float)p.y};
}

#elif __APPLE__
#include <ApplicationServices/ApplicationServices.h>
static bool keyState[256] = {0};

void InitBackgroundInput()
{
    // Force the accessibility prompt to appear
    const void *keys[] = {kAXTrustedCheckOptionPrompt};
    const void *values[] = {kCFBooleanTrue};
    CFDictionaryRef options = CFDictionaryCreate(NULL, keys, values, 1, NULL, NULL);
    bool trusted = AXIsProcessTrustedWithOptions(options);
    CFRelease(options);

    if (!trusted)
    {
        printf("WARNING: Accessibility permissions NOT granted. Letters will not be detected.\n");
    }
}

bool IsBackgroundKeyDown(int vKey)
{
    // Use kCGEventSourceStateHIDSystemState for better background reliability
    return CGEventSourceKeyState(kCGEventSourceStateHIDSystemState, (CGKeyCode)vKey);
}

// This function MUST be called once per frame to update the state
int GetBackgroundAnyKeyPressed()
{
    for (int i = 0; i <= 127; i++)
    {
        bool currentlyDown = IsBackgroundKeyDown(i);
        bool wasDown = keyState[i];
        keyState[i] = currentlyDown;

        if (currentlyDown && !wasDown)
        {
            return i; // Found a key that was JUST pressed
        }
    }
    return -1; // Using -1 for 'none' because 0 is the letter 'A' on Mac!
}

Vector2 GetMouseScreenPosition()
{
    // Queries the global mouse position from the Quartz window server
    CGEventRef event = CGEventCreate(NULL);
    CGPoint cursor = CGEventGetLocation(event);
    CFRelease(event);
    return (Vector2){(float)cursor.x, (float)cursor.y};
}

#endif

// This handles the "Pressed" logic for ALL platforms
bool IsBackgroundKeyPressed(int vKey)
{
    if (vKey < 0 || vKey >= 256)
        return false;

    bool currentlyDown = IsBackgroundKeyDown(vKey);
    bool wasDown = keyState[vKey];

    // Update the state table
    keyState[vKey] = currentlyDown;

    // Return true only on the frame it transitions from UP to DOWN
    return (currentlyDown && !wasDown);
}
