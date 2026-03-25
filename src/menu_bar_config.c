#include "menu_bar_config.h"
#include <string.h>
#include <stdio.h>
#include "raylib.h"

#if defined(__clang__)
#include <libc.h>
#else
#include <unistd.h>
#endif

#if defined(_WIN32) || defined(_WIN64)
#define TRAY_WINAPI 1
#elif defined(__linux__) || defined(linux) || defined(__linux)
#define TRAY_QT 1
#elif defined(__APPLE__) || defined(__MACH__)
#define TRAY_APPKIT 1
#endif

#include "tray.h"

#if TRAY_QT
#define TRAY_ICON "icon-24px.png"
#elif TRAY_APPKIT
#define TRAY_ICON "icon.png"
#elif TRAY_WINAPI
#define TRAY_ICON "icon.ico"
#endif

MenuBarConfig g_menuBarConfig = {
    .showPokedex = 0,
    .showConfig = 0,
    .minimizedView = 1,
    .shouldQuit = 0,
};

static void toggle_pokedex_cb(struct tray_menu_item *item);
static void toggle_minimized_view_cb(struct tray_menu_item *item);
static void quit_cb(struct tray_menu_item *item);
static void move_window_cb(struct tray_menu_item *item);

// Test tray init
struct tray tray = {
    .icon_filepath = TRAY_ICON,
    .tooltip = "PokeStroke",
    .menu =
        (struct tray_menu_item[]){
            {.text = "Show Pokedex", .checked = 0, .cb = toggle_pokedex_cb},
            {.text = "Minimized View", .checked = 1, .cb = toggle_minimized_view_cb},
            {.text = "-"},
            {.text = "Window Position",
             .submenu =
                 (struct tray_menu_item[]){
                     {.text = "Bottom Left", .cb = move_window_cb},
                     {.text = "Bottom Right", .cb = move_window_cb},
                     {.text = NULL}},
             .cb = NULL},

            {.text = "-"},
            {.text = "Quit", .cb = quit_cb},
            {.text = NULL}},
};

void toggle_pokedex_cb(struct tray_menu_item *item)
{
    item->checked = !item->checked;
    g_menuBarConfig.showPokedex = item->checked;
    struct tray *ct = tray_get_instance();
    if (ct != NULL)
        tray_update(ct);
}

void toggle_minimized_view_cb(struct tray_menu_item *item)
{
    printf("toggle minimized view cb\n");
    item->checked = !item->checked;
    g_menuBarConfig.minimizedView = item->checked;
    struct tray *ct = tray_get_instance();
    if (ct != NULL)
        tray_update(ct);
}

void quit_cb(struct tray_menu_item *item)
{
    (void)item;
    g_menuBarConfig.shouldQuit = 1;
    tray_exit();
}

void move_window_cb(struct tray_menu_item *item)
{
    (void)item;
    int minimizedOffsetX = g_menuBarConfig.minimizedView ? 45 : -10;
    int minimizedOffsetY = g_menuBarConfig.minimizedView ? 30 : -10;
    if (strcmp(item->text, "Bottom Left") == 0)
    {
        SetWindowMonitor(GetCurrentMonitor());
        SetWindowPosition(-minimizedOffsetX, GetMonitorHeight(GetCurrentMonitor()) - GetScreenHeight() + minimizedOffsetY);
    }
    else if (strcmp(item->text, "Bottom Right") == 0)
    {
        printf("Moving window to bottom right\n");
        SetWindowMonitor(GetCurrentMonitor());
        SetWindowPosition(GetMonitorWidth(GetCurrentMonitor()) - GetScreenWidth() + minimizedOffsetX, GetMonitorHeight(GetCurrentMonitor()) - GetScreenHeight() + minimizedOffsetY);
    }
}

void MenuBarConfigInit()
{
    if (tray_init(&tray) < 0)
    {
        TraceLog(LOG_ERROR, "Failed to initialize tray");
        exit(1);
    }
}
void MenuBarConfigUpdate()
{
    tray_loop(0);
}

void MenuBarConfigUnload()
{
    tray_exit();
}