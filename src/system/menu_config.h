#ifndef MENU_CONFIG_H
#define MENU_CONFIG_H

#include "display_config.h"

// Menu navigation variables
extern int mainMenuIndex;
extern int subMenuIndex;
extern bool inSubmenu;
extern int countMenu;
extern int countWifiOptions;
extern int countBluetoothOptions;
extern int countLoraOptions;
extern int countInfraredOptions;
extern int countSettingsOptions;
extern const int itemPerScreen;

// Current submenu tracking
extern const char** currentSubmenuList;
extern int currentSubmenuCount;

// Notepad mode tracking
extern bool inNotepadMode;

// Keyboard test mode tracking
extern bool inKeyboardTestMode;

// Homescreen mode tracking
extern bool inHomescreenMode;

// WiFi password input mode tracking
extern bool inWiFiPasswordMode;
extern int wifiPasswordNetworkIndex;

// Menu lists (extern declarations)
extern const char* menuList[];
extern const char** wifiOptions; // Dynamically built: toggle + networks + Back
extern const char* bluetoothOptions[];
extern const char* loraOptions[];
extern const char* infraredOptions[];
extern const char* settingsOptions[];

// Function declarations
void displayMenu(int count, const char* list[], bool isSubmenu);
void handleMenu(int& listIndex, int listCount, const char* listShow[], bool isSubmenu);
void vaiDisplay();
void handleKeyboardMenu();
void updateWiFiToggleText();
void rebuildWiFiMenuOptions();

// Setup functions (declared in other files)
void wifiSetup();
void bluetoothSetup();
void loraSetup();
void infraredSetup();
void settingsSetup();
void notepadSetup();
void enterDeepSleep();

#endif // MENU_CONFIG_H

