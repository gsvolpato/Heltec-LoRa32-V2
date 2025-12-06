#include "ir_config.h"
#include "system/menu_config.h"

void infraredSetup() {
    Serial.println("[MENU] Infrared submenu opened");
    subMenuIndex = 0;  // Reset cursor position
    displayMenu(countInfraredOptions, infraredOptions, true);
}
