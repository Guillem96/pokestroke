#include <stdio.h>

#include "raylib.h"
#include "bg_input.h"
#include "resource_dir.h"
#include "game_manager.h"
#include "gameboy_frame.h"
#include "window_dragger.h"

int main()
{
	SearchAndSetResourceDir("resources");

	// Behave as an overlay
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_TRANSPARENT | FLAG_WINDOW_HIGHDPI);
	InitWindow(256, 256, "PokeStroke");
	SetWindowState(FLAG_WINDOW_UNDECORATED | FLAG_WINDOW_TOPMOST);

	SetTargetFPS(30);

	InitBackgroundInput();

	GameManager *manager = malloc(sizeof(GameManager));
	GameManagerInit(manager, "gamestate.dat");

	GameBoyFrameInit();

	GameManagerUpdate(manager);

	WindowDraggerInit();

	while (!WindowShouldClose())
	{
		int key = GetBackgroundAnyKeyPressed();
		if (key == 53)
		{
			TraceLog(LOG_INFO, "ESC pressed, quitting game", key);
			break;
		}

		if (key != -1)
		{
			GameManagerUpdate(manager);
		}

		WindowDraggerUpdate();

		BeginDrawing();
		ClearBackground(BLANK);
		GameBoyFrameDraw(256);
		GameManagerDraw(manager);
		EndDrawing();
	}

	GameBoyFrameUnload();
	GameManagerUnload(manager);
	CloseWindow();
	return 0;
}
