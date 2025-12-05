#include "menu_config.h"
#include "display_config.h"
#include "keyboard/keyboard_config.h"
#include "apps/settings/deepsleep_config.h"
#include "apps/notepad/notepad_config.h"
#include "apps/settings/keyboard_test.h"
#include "homescreen_config.h" // For homescreenSetup
#include "freertos/keyboard_queue.h"
#include "system/wifi/wifi_config.h"
#include "gpios.h"
#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <cstring>

// Menu navigation variables
int mainMenuIndex = 0;
int subMenuIndex = 0;
bool inSubmenu = false;
int countMenu = 3;
int countWifiOptions = 1; // Will be updated dynamically
// WiFi options array (dynamically built: toggle + networks)
char wifiOptionToggle[20] = "WiFi: OFF";
// Store SSID strings directly to avoid pointer invalidation
static char wifiNetworkSSIDs[21][33]; // Max 32 chars per SSID + null terminator
static const char* wifiOptionsStatic[21]; // Toggle (1) + max networks (20) = 21
const char** wifiOptions = (const char**)wifiOptionsStatic;
// Track last network count to avoid unnecessary rebuilds
int lastRebuiltNetworkCount = -1; // Made non-static so wifi_config can reset it
int countBluetoothOptions = 3;
int countLoraOptions = 3;
int countInfraredOptions = 3;
int countSettingsOptions = 6;

// Current submenu tracking
const char** currentSubmenuList = nullptr;
int currentSubmenuCount = 0;

// Global variables
const int itemPerScreen = 4;


// Define function pointer type
typedef void (*FunctionPointer)();

// Forward declarations
void wifiSetup();
void bluetoothSetup();
void loraSetup();
void infraredSetup();
void settingsSetup();
void notepadSetup();

// Function to update WiFi toggle text
void updateWiFiToggleText() {
  if (isWiFiEnabled()) {
    strcpy(wifiOptionToggle, "WiFi: ON");
  } else {
    strcpy(wifiOptionToggle, "WiFi: OFF");
  }
}

// Function to rebuild WiFi menu options dynamically
void rebuildWiFiMenuOptions() {
  updateWiFiToggleText();
  wifiOptionsStatic[0] = wifiOptionToggle;
  
  int networkCount = getWiFiNetworkCount();
  if (networkCount == lastRebuiltNetworkCount && networkCount >= 0) {
    return;
  }
  
  if (networkCount > 0) {
    for (int i = 0; i < networkCount; i++) {
      const char* ssid = getWiFiNetworkSSID(i);
      if (ssid != nullptr && strlen(ssid) > 0) {
        strncpy(wifiNetworkSSIDs[i], ssid, 32);
        wifiNetworkSSIDs[i][32] = '\0';
        wifiOptionsStatic[i + 1] = wifiNetworkSSIDs[i];
      }
    }
    countWifiOptions = networkCount + 1;
    lastRebuiltNetworkCount = networkCount;
  } else if (!isWiFiEnabled()) {
    countWifiOptions = 1;
    lastRebuiltNetworkCount = 0;
  }
  
  if (currentSubmenuList == wifiOptions) {
    currentSubmenuCount = countWifiOptions;
  }
}

// Define function pointer list (must be before handleMenu)
FunctionPointer functionList[] = {
    infraredSetup,
    notepadSetup,
    settingsSetup
};

void displayMenu(int count, const char* list[], bool isSubmenu) {
  display.clearDisplay();
  display.drawRoundRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 4, WHITE);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  
  int currentIndex = isSubmenu ? subMenuIndex : mainMenuIndex;
  int startIndex = currentIndex / itemPerScreen * itemPerScreen;
  int endIndex = startIndex + itemPerScreen;
  if (endIndex > count) {
    endIndex = count;
  }
  
  for (int i = startIndex; i < endIndex; i++) {
    display.setCursor(8, 14 * (i - startIndex) + 7);
    if (i == currentIndex) {
      display.print("> ");
    } else {
      display.print("  ");
    }
    display.println(list[i]);
  }
  display.display();
}

  
void handleMenu(int& listIndex, int listCount, const char* listShow[], bool isSubmenu) {
    // Display the menu
    displayMenu(listCount, listShow, isSubmenu);
  
    // Handle navigation buttons (physical GPIO buttons)
    // Check physical buttons first
    if (readEnterButton()) {
      // Enter button pressed
      Serial.println("[ACTION] ENTER button pressed");
      if (inSubmenu) {
        // Handle submenu selection logic (no Back option - handled by back button)
        // Handle submenu item selection
        Serial.print("[SUB] Selected option: ");
        Serial.print(listIndex);
        Serial.print(" - ");
        Serial.println(listShow[listIndex]);
        
        // Check if we're in WiFi submenu and handle toggle/networks
        if (currentSubmenuList == wifiOptions) {
          if (listIndex == 0) {
            // WiFi toggle selected
            Serial.println("[WIFI] Toggling WiFi");
            bool wasEnabled = isWiFiEnabled();
            toggleWiFi();
            
            if (!wasEnabled && isWiFiEnabled()) {
              for (int i = 0; i < 50; i++) {
                vTaskDelay(pdMS_TO_TICKS(100));
                if (getWiFiNetworkCount() > 0 || i >= 49) break;
              }
            }
            
            rebuildWiFiMenuOptions();
            subMenuIndex = 0;
            displayMenu(countWifiOptions, wifiOptions, true);
            return;
          } else {
            int networkIndex = listIndex - 1;
            extern void inputWiFiPassword(int networkIndex);
            inputWiFiPassword(networkIndex);
            return;
          }
        } else if (currentSubmenuList == settingsOptions) {
          if (listIndex == 0) {
            currentSubmenuList = wifiOptions;
            currentSubmenuCount = countWifiOptions;
            subMenuIndex = 0;
            wifiSetup();
            return;
          } else if (listIndex == 1) {
            currentSubmenuList = bluetoothOptions;
            currentSubmenuCount = countBluetoothOptions;
            subMenuIndex = 0;
            bluetoothSetup();
            return;
          } else if (listIndex == 2) {
            currentSubmenuList = loraOptions;
            currentSubmenuCount = countLoraOptions;
            subMenuIndex = 0;
            loraSetup();
            return;
          } else if (listIndex == 3) {
            keyboardTestSetup();
            return;
          } else {
            displayMenu(listCount, listShow, true);
          }
        } else {
          // Other submenus (Infrared, Bluetooth, LoRa) - placeholder
          displayMenu(listCount, listShow, true);
        }
        } else {
          if (listIndex >= 0 && listIndex < sizeof(functionList) / sizeof(FunctionPointer)) {
            inSubmenu = true;
            subMenuIndex = 0;
            switch(listIndex) {
              case 0: currentSubmenuList = infraredOptions; currentSubmenuCount = countInfraredOptions; break;
              case 1: currentSubmenuList = nullptr; currentSubmenuCount = 0; break;
              case 2: currentSubmenuList = settingsOptions; currentSubmenuCount = countSettingsOptions; break;
            }
            functionList[listIndex]();
          }
        }
    }
    
    const char* key = getKeyFromQueue();
    if (key != nullptr) {
      if (strcmp(key, "UP") == 0) {
        if (isSubmenu) {
          subMenuIndex = (subMenuIndex - 1 + listCount) % listCount;
          listIndex = subMenuIndex;
        } else {
          mainMenuIndex = (mainMenuIndex - 1 + listCount) % listCount;
          listIndex = mainMenuIndex;
        }
      } else if (strcmp(key, "DOWN") == 0) {
        if (isSubmenu) {
          subMenuIndex = (subMenuIndex + 1) % listCount;
          listIndex = subMenuIndex;
        } else {
          mainMenuIndex = (mainMenuIndex + 1) % listCount;
          listIndex = mainMenuIndex;
        }
      }
    }
  }

// Define your menu items
const char* menuList[] = {"Infrared", "Notepad", "Settings"};
const char* bluetoothOptions[] = {"Option 1", "Option 2", "Option 3"};
const char* loraOptions[] = {"Option 1", "Option 2", "Option 3"};
const char* infraredOptions[] = {"Option 1", "Option 2", "Option 3"};
const char* settingsOptions[] = {"WiFi", "Bluetooth", "LoRa", "Keyboard Test", "Option 1", "Option 2"};



