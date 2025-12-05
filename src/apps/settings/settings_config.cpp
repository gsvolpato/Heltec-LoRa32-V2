#include "settings_config.h"
#include "system/menu_config.h"

void settingsSetup() {
    Serial.println("[MENU] Settings submenu opened");
    subMenuIndex = 0;  // Reset cursor position
    displayMenu(countSettingsOptions, settingsOptions, true);
  }