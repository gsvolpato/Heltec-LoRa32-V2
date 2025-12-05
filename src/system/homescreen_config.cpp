#include "homescreen_config.h"
#include "display_config.h"
#include "menu_config.h"
#include "gpios.h"
#include "freertos/keyboard_queue.h"
#include <Arduino.h>

// Homescreen mode flag
bool inHomescreenMode = true;


void homescreenMain() {
  // Consume any keyboard keys pressed on homescreen (they should be ignored)
  const char* key = getKeyFromQueue();
  if (key != nullptr) {
    // Keys ignored on homescreen
  }

  // Check for enter button to open main menu
  if (readEnterButton()) {
    Serial.println("[HOMESCREEN] Enter button pressed - opening main menu");
    // Clear any remaining keys in queue before opening menu
    clearKeyboardQueue();
    inHomescreenMode = false;
    // Main menu will be displayed by taskMenu
    return;
  }

  // Display homescreen - empty with just outline
  display.clearDisplay();
  display.drawRoundRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 4, WHITE);
  
  // Future: Add clock, battery percentage, etc. here
  
  display.display();
}

void homescreenSetup() {
  Serial.println("[HOMESCREEN] Homescreen opened");
  inHomescreenMode = true;
}

