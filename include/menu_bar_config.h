#ifndef MENU_BAR_CONFIG_H
#define MENU_BAR_CONFIG_H

#if defined(_WIN32) || defined(_WIN64)
#define TRAY_WINAPI 1
#elif defined(__linux__) || defined(linux) || defined(__linux)
#define TRAY_APPINDICATOR 1
#elif defined(__APPLE__) || defined(__MACH__)
#define TRAY_APPKIT 1
#endif

#include "tray.h"

typedef struct
{
    int showPokedex;
    int showConfig;
    int shouldQuit;
    int minimizedView;
} MenuBarConfig;

extern MenuBarConfig g_menuBarConfig;

void MenuBarConfigInit();
void MenuBarConfigUpdate();
void MenuBarConfigUnload();

#endif