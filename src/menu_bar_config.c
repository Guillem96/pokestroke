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

#include "pokedex_gui.h"
#include "utils.h"

#define TRAY_POKEDEX_ITEM_INDEX 0
#define TRAY_TRAINER_CARD_ITEM_INDEX 1
#define TRAY_BATTLE_ITEM_INDEX 2

MenuBarConfig g_menuBarConfig = {
    .showPokedex = 0,
    .showTrainerCard = 0,
    .showConfig = 0,
    .minimizedView = 1,
    .shouldQuit = 0,
};

static void toggle_pokedex_cb(struct tray_menu_item *item);
static void toggle_trainer_card_cb(struct tray_menu_item *item);
static void toggle_battle_cb(struct tray_menu_item *item);
static void toggle_minimized_view_cb(struct tray_menu_item *item);
static void quit_cb(struct tray_menu_item *item);
static void move_window_cb(struct tray_menu_item *item);
static void update_checked_states();

struct tray tray = {
    .icon_filepath = TRAY_ICON,
    .tooltip = "PokeStroke",
    .menu =
        (struct tray_menu_item[]){
            {.text = "Pokedex", .checked = 0, .cb = toggle_pokedex_cb},
            {.text = "Trainer Card", .checked = 0, .cb = toggle_trainer_card_cb},
            {.text = "Battle", .disabled = 1, .checked = 1, .cb = toggle_battle_cb},
            {.text = "-"},
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
    g_menuBarConfig.showPokedex = 1;
    g_menuBarConfig.showTrainerCard = 0;

    Vector2 winPos = GetWindowPosition();
    Vector2 screenSize = (Vector2){GetScreenWidth(), GetScreenHeight()};
    Vector2 monitorSize = (Vector2){GetMonitorWidth(GetCurrentMonitor()), GetMonitorHeight(GetCurrentMonitor())};
    Vector2 newPos = {
        clamp(winPos.x - (POKEDEX_WINDOW_WIDTH - screenSize.x), 0, monitorSize.x - POKEDEX_WINDOW_WIDTH),
        clamp(winPos.y, -(POKEDEX_WINDOW_HEIGHT + screenSize.y), monitorSize.y - POKEDEX_WINDOW_HEIGHT)};

    SafeSetWindowSize(POKEDEX_WINDOW_WIDTH, POKEDEX_WINDOW_HEIGHT);
    SetWindowPosition(newPos.x, newPos.y);
    update_checked_states();
}

static void toggle_battle_cb(struct tray_menu_item *item)
{
    g_menuBarConfig.showPokedex = 0;
    g_menuBarConfig.showTrainerCard = 0;
    SafeSetWindowSize(256, 256);
    update_checked_states();
}

void toggle_trainer_card_cb(struct tray_menu_item *item)
{
    g_menuBarConfig.showPokedex = 0;
    g_menuBarConfig.showTrainerCard = 1;
    update_checked_states();

    // Vector2 winPos = GetWindowPosition();
    // Vector2 screenSize = (Vector2){GetScreenWidth(), GetScreenHeight()};
    // Vector2 monitorSize = (Vector2){GetMonitorWidth(GetCurrentMonitor()), GetMonitorHeight(GetCurrentMonitor())};
    // Vector2 newPos = {
    //     clamp(winPos.x - (TRAINER_CARD_WINDOW_WIDTH - screenSize.x), 0, monitorSize.x - TRAINER_CARD_WINDOW_WIDTH),
    //     clamp(winPos.y, -(TRAINER_CARD_WINDOW_HEIGHT + screenSize.y), monitorSize.y - TRAINER_CARD_WINDOW_HEIGHT)};

    // SafeSetWindowSize(TRAINER_CARD_WINDOW_WIDTH, TRAINER_CARD_WINDOW_HEIGHT);
    // SetWindowPosition(newPos.x, newPos.y);
}

void toggle_minimized_view_cb(struct tray_menu_item *item)
{
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

static void update_checked_states()
{
    struct tray *ct = tray_get_instance();
    if (ct == NULL)
    {
        TraceLog(LOG_ERROR, "Tray instance is NULL");
        return;
    }

    ct->menu[TRAY_POKEDEX_ITEM_INDEX].checked = g_menuBarConfig.showPokedex;
    ct->menu[TRAY_POKEDEX_ITEM_INDEX].disabled = g_menuBarConfig.showPokedex;
    ct->menu[TRAY_TRAINER_CARD_ITEM_INDEX].checked = g_menuBarConfig.showTrainerCard;
    ct->menu[TRAY_TRAINER_CARD_ITEM_INDEX].disabled = g_menuBarConfig.showTrainerCard;

    ct->menu[TRAY_BATTLE_ITEM_INDEX].checked = !g_menuBarConfig.showPokedex && !g_menuBarConfig.showTrainerCard;
    ct->menu[TRAY_BATTLE_ITEM_INDEX].disabled = !g_menuBarConfig.showPokedex && !g_menuBarConfig.showTrainerCard;
    tray_update(ct);
}