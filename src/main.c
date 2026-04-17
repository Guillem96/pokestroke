#include <stdio.h>

#include "windows_fix.h"
#include "raylib.h"
#include "bg_input.h"
#include "resource_dir.h"
#include "game_manager.h"
#include "gameboy_frame.h"
#include "window_dragger.h"
#include "menu_bar_config.h"
#include "trainer_card_gui.h"
#include "ribbon_sheet.h"
#include "shiny_sparkle_sheet.h"
#include "pkmn_sprite_sheet.h"
#include "pkmn_battle_sprite_sheet.h"
#include "pokedex.h"
#include "pokedex_gui.h"
#include "shiny_charm_sheet.h"

static const char *GetUserHomeDir(void);
static const char *GetSaveFilePath(void);

static void SetupGameFilesystem(void);

int main()
{
	SetTraceLogLevel(LOG_ERROR);
	const char *saveFilePath = GetSaveFilePath();
	SetupGameFilesystem();

	// Behave as an overlay
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_TRANSPARENT | FLAG_WINDOW_HIGHDPI);
	InitWindow(256, 256, "PokeStroke");
	SetWindowState(FLAG_WINDOW_UNDECORATED | FLAG_WINDOW_TOPMOST);

	// Random seed
	SetRandomSeed((unsigned int)time(NULL));
	srand((unsigned int)time(NULL));

	// Font texture
	SetTextureFilter(GetFontDefault().texture, TEXTURE_FILTER_POINT);

	SetTargetFPS(60);

	// Screens
	PokedexGUI pokedexGUI;
	TrainerCardGUI tcg;
	GameManager manager;

	// Sheets
	ShinySparkleSheet sparkleSheet;
	RibbonSheet ribbonSheet;
	ShinySparkleAnim sparkleAnim;
	PokemonSpriteSheet pokemonSheet;
	PkmnBattleSpriteSheet battleSheet;
	ShinyCharmSheet shinyCharmSheet;

	GameState gameState;

	// Controls Init
	WindowDraggerInit();
	MenuBarConfigInit();
	InitBackgroundInput();

	// Savefile
	GameStateInit(&gameState, saveFilePath);

	// Trainer Card Init
	ShinyCharmSheetInit(&shinyCharmSheet, "shiny-charm.png");
	ShinySparkleSheetInit(&sparkleSheet, "shiny-sparkle.png");
	RibbonSheetInit(&ribbonSheet, "ribbons.png");
	TrainerCardGUIInit(&tcg, &ribbonSheet, &sparkleSheet, &shinyCharmSheet, &gameState);

	// Pokedex Init
	PkmnSpriteSheetInit(&pokemonSheet, "pokedex.png");
	PkmnBattleSpriteSheetInit(&battleSheet, "battle.png");
	PokedexGUIInit(&pokedexGUI, gameState.pokedex, &pokemonSheet, &battleSheet);

	// Game Init
	GameBoyFrameInit();
	GameManagerInit(&manager, &gameState, &pokemonSheet, &battleSheet);
	GameManagerUpdate(&manager); // Pre-run an update to initialize the first pokemon spawn

	// TraceLog(LOG_INFO, "Initialization complete, entering main loop.");

	while (!WindowShouldClose())
	{
		MenuBarConfigUpdate();

		int key = GetBackgroundAnyKeyPressed();

		if (key != -1)
		{
			GameManagerUpdate(&manager);
		}

		if (GetKeyPressed() == KEY_ESCAPE || g_menuBarConfig.shouldQuit)
		{
			TraceLog(LOG_INFO, "Quitting game", key);
			break;
		}

		WindowDraggerUpdate();

		if (g_menuBarConfig.showPokedex)
		{
			PokedexGUIUpdate(&pokedexGUI);
		}
		else if (g_menuBarConfig.showTrainerCard)
		{
			TrainerCardGUIUpdate(&tcg);
		}

		RenderTexture2D target = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
		SetTextureFilter(target.texture, TEXTURE_FILTER_POINT);
		BeginTextureMode(target);
		ClearBackground(BLANK);
		if (!g_menuBarConfig.showTrainerCard && !g_menuBarConfig.showPokedex)
		{
			GameBoyFrameDraw(256);
			GameManagerDraw(&manager);
		}
		else if (g_menuBarConfig.showTrainerCard)
		{
			TrainerCardGUIDraw(&tcg);
		}
		else if (g_menuBarConfig.showPokedex)
		{
			PokedexGUIDraw(&pokedexGUI);
		}
		EndTextureMode();

		BeginDrawing();
		ClearBackground(BLANK);
		DrawTexturePro(
			target.texture,
			(Rectangle){0.0f, 0.0f, (float)target.texture.width, (float)-(target.texture.height)},
			(Rectangle){0, 0, GetScreenWidth(), GetScreenHeight()}, (Vector2){0, 0},
			0.0f, WHITE);
		EndDrawing();
		UnloadRenderTexture(target);
	}

	// Sheets
	ShinySparkleSheetUnload(&sparkleSheet);
	RibbonSheetUnload(&ribbonSheet);
	PkmnSpriteSheetUnload(&pokemonSheet);
	PkmnBattleSpriteSheetUnload(&battleSheet);
	ShinyCharmSheetUnload(&shinyCharmSheet);
	GameBoyFrameUnload();

	// Screens
	GameManagerUnload(&manager);
	PokedexGUIUnload(&pokedexGUI);
	TrainerCardGUIUnload(&tcg);

	// Controls
	MenuBarConfigUnload();

	// Savefile
	GameStateUnload(&gameState);

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
#if defined(__APPLE__)
	// macOS Bundle Logic: resources are in ../Resources relative to the binary
	const char *appDir = GetApplicationDirectory();
	char resourcesPath[512];
	snprintf(resourcesPath, sizeof(resourcesPath), "%s../Resources", appDir);

	if (DirectoryExists(resourcesPath))
	{
		ChangeDirectory(resourcesPath);
		TraceLog(LOG_INFO, "Filesystem: macOS Bundle detected. Path: %s", resourcesPath);
	}
#elif defined(_WIN32)
	// Windows Bundle Logic: resources are in resources dir inside msix bundle
	const char *appDir = GetApplicationDirectory();
	char resourcesPath[512];
	snprintf(resourcesPath, sizeof(resourcesPath), "%sresources", appDir);

	if (DirectoryExists(resourcesPath))
	{
		ChangeDirectory(resourcesPath);
		TraceLog(LOG_INFO, "Filesystem: macOS Bundle detected. Path: %s", resourcesPath);
	}
#else
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
#endif
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

static const char *GetSaveFilePath(void)
{
#if defined(NDEBUG)
	const char *home = GetUserHomeDir();
	if (home == NULL)
	{
		TraceLog(LOG_ERROR, "Could not determine user home directory!");
		exit(1);
	}
	const char *saveDir = TextFormat("%s/.pokestroke", home);
	if (!DirectoryExists(saveDir))
	{
		MakeDirectory(saveDir);
	}

	const char *savePath = TextFormat("%s/.pokestroke/gamestate.dat", home);
	printf("Using save path: %s\n", savePath);
	return savePath;
#else
	return "gamestate.dat";
#endif
}