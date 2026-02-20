#include "gameboy_frame.h"

#include "raylib.h"

static Font nintendoFont;
static Font gameBoyFont;

void GameBoyFrameInit()
{
    nintendoFont = LoadFont("Pretendo.ttf");
    gameBoyFont = LoadFont("gb.ttf");
}

Rectangle GameBoyFrameDraw(int size)
{
    DrawRectangleRounded((Rectangle){0, 0, size, size}, 0.08f, 8, (Color){235, 235, 235, 255});

    int screenBorderX = size * 0.08f;
    int screenBorderY = size * 0.15f;
    int screenBorderWidth = size - screenBorderX * 2;
    int screenBorderHeight = size - (int)(size * 0.25f);

    // Notches
    DrawRectangle(0, screenBorderX, size, 3, (Color){207, 207, 207, 255});
    DrawRectangle(screenBorderX, 0, 3, screenBorderX, (Color){207, 207, 207, 255});
    DrawRectangle(size - screenBorderX, 0, 3, screenBorderX, (Color){207, 207, 207, 255});

    // Screen border
    DrawRectangleRounded(
        (Rectangle){screenBorderX, screenBorderY, screenBorderWidth, screenBorderWidth / 1.5f},
        .1f, 8, (Color){175, 170, 176, 255});
    DrawRectangleRounded(
        (Rectangle){screenBorderX, screenBorderY, screenBorderWidth / 1.5f, screenBorderHeight},
        .1f, 8, (Color){175, 170, 176, 255});
    DrawRectangleRounded(
        (Rectangle){
            screenBorderX + screenBorderWidth / 2,
            screenBorderY + screenBorderHeight / 2,
            screenBorderWidth / 2,
            screenBorderHeight / 2},
        .7f, 8,
        (Color){175, 170, 176, 255});

    // Top stright lines
    DrawRectangle(
        screenBorderX + 10, screenBorderY + 10, screenBorderWidth / 4,
        3, (Color){127, 83, 107, 255});
    DrawRectangle(
        screenBorderX + 10, screenBorderY + 15, screenBorderWidth / 4,
        3, (Color){54, 56, 86, 255});

    DrawRectangle(
        screenBorderX + screenBorderWidth - screenBorderWidth / 6 - 10,
        screenBorderY + 10, screenBorderWidth / 6,
        3, (Color){127, 83, 107, 255});
    DrawRectangle(screenBorderX + screenBorderWidth - screenBorderWidth / 6 - 10,
                  screenBorderY + 15, screenBorderWidth / 6, 3, (Color){54, 56, 86, 255});

    // Batttery LED indicator
    DrawCircle(screenBorderX + 8, screenBorderY + screenBorderHeight / 2, 4, (Color){255, 0, 0, 255});
    DrawCircleGradient(screenBorderX + 8, screenBorderY + screenBorderHeight / 2, 5, (Color){255, 0, 0, 255}, (Color){255, 0, 0, 100});

    // Screen
    int screenX = screenBorderX + screenBorderWidth * 0.15f;
    int screenY = screenBorderY + screenBorderHeight * 0.16f;
    int screenWidth = screenBorderWidth * 0.7f;
    int screenHeight = screenBorderHeight * 0.75f;
    DrawRectangle(screenX, screenY, screenWidth, screenHeight, (Color){248, 248, 248, 255});

    // Text
    int fontSize = size * 0.04f;
    int fontY = screenBorderHeight + screenBorderY + fontSize;
    DrawTextEx(
        nintendoFont,
        "Nintendo",
        (Vector2){screenBorderX, fontY + fontSize * 0.5f},
        fontSize,
        0.5f,
        (Color){28, 47, 119, 255});

    DrawTextEx(
        gameBoyFont,
        "GAME BOY",
        (Vector2){screenBorderX + 6 * fontSize, fontY},
        fontSize * 2,
        0.5f,
        (Color){28, 47, 119, 255});

    return (Rectangle){screenX, screenY, screenWidth, screenHeight};
}

void GameBoyFrameFree()
{
    UnloadFont(nintendoFont);
    UnloadFont(gameBoyFont);
}
