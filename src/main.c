#include <stdio.h>

#include "windows_fix.h"
#include "raylib.h"
#include "bg_input.h"
#include "resource_dir.h"
#include "game_manager.h"
#include "gameboy_frame.h"
#include "window_dragger.h"
#include "menu_bar_config.h"

static const char *GetUserHomeDir(void);
static void SetupGameFilesystem(void);

int main()
{
	SetupGameFilesystem();

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
	MenuBarConfigInit();

	while (!WindowShouldClose())
	{
		int key = GetBackgroundAnyKeyPressed();

		if (key != -1)
		{
			GameManagerUpdate(manager);
		}

		if (GetKeyPressed() == KEY_ESCAPE || g_menuBarConfig.shouldQuit)
		{
			TraceLog(LOG_INFO, "Quitting game", key);
			break;
		}
		WindowDraggerUpdate();
		MenuBarConfigUpdate();

		BeginDrawing();
		ClearBackground(BLANK);
		GameBoyFrameDraw(256);
		GameManagerDraw(manager);
		EndDrawing();
	}

	GameBoyFrameUnload();
	GameManagerUnload(manager);
	MenuBarConfigUnload();
	CloseWindow();
	return 0;
}

static const char *GetUserHomeDir(void)
{
	const char *home;
#if defined(_WIN32)
	home = getenv("USERPROFILE"); // Windows: C:\Users\Name
#else
	home = getenv("HOME"); // macOS/Linux: /home/name or /Users/name
#endif
	return home;
}

static void SetupGameFilesystem(void)
{
	// Check if we are in Release mode.
	// Most compilers define NDEBUG for release builds.
#if defined(NDEBUG)
	const char *home = GetUserHomeDir();
	if (home != NULL)
	{
		const char *savePath = TextFormat("%s/.pokestroke/resources", home);

		if (!DirectoryExists(savePath))
		{
			MakeDirectory(savePath);
		}

		// Set the working directory to the user folder for Release
		ChangeDirectory(savePath);
		TraceLog(LOG_INFO, "Filesystem: Using User Directory [%s]", savePath);
	}
#else
	// Debug mode: Use your existing relative search logic
	if (SearchAndSetResourceDir("resources"))
	{
		TraceLog(LOG_INFO, "Filesystem: Using Local Resources [Debug Mode]");
	}
	else
	{
		TraceLog(LOG_WARNING, "Filesystem: Could not find local resources directory.");
	}
#endif
}
