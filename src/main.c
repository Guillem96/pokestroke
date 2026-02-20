#include <stdio.h>

#include "raylib.h"
#include "bg_input.h"
#include "resource_dir.h"
#include "game_manager.h"
#include "gameboy_frame.h"

int main()
{
	SearchAndSetResourceDir("resources");

	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI | FLAG_WINDOW_TRANSPARENT);
	InitWindow(512, 512, "Hello Raylib");
	SetWindowState(FLAG_WINDOW_UNDECORATED); // Hide border/titlebar; omit if you want them there.
	SetTargetFPS(16);

	InitBackgroundInput();

	GameManager *manager = malloc(sizeof(GameManager));
	GameManagerInit(manager, "gamestate.dat");

	GameBoyFrameInit();

	GameManagerUpdate(manager);
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
			TraceLog(LOG_INFO, "Global Key Pressed: %d", key);
			GameManagerUpdate(manager);
		}

		GameManagerUpdate(manager);

		BeginDrawing();

		ClearBackground((Color){248, 248, 248, 255});
		GameBoyFrameDraw(256);
		GameManagerDraw(manager);
		EndDrawing();
	}

	GameBoyFrameFree();

	CloseWindow();
	return 0;
}
