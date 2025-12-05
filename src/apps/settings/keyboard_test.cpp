#include "keyboard_test.h"
#include "system/menu_config.h"
#include "system/display_config.h"
#include "system/gpios.h"
#include "system/keyboard/keyboard_config.h"
#include "settings_config.h"
#include "system/freertos/keyboard_queue.h"
#include <Arduino.h>

// Keyboard test mode flag
bool inKeyboardTestMode = false;

// Last pressed key for display
char lastPressedKey = '\0';
unsigned long lastKeyTime = 0;
const unsigned long KEY_DISPLAY_TIME = 2000;

void keyboardTestMain() {
  unsigned long currentTime = millis();
  
  // Check for 'A' key to exit
  const char* key = getKeyFromQueue();
  if (key != nullptr && strcmp(key, "A") == 0) {
    Serial.println("[KEYBOARD_TEST] A key pressed - returning to Settings menu");
    inKeyboardTestMode = false;
    inSubmenu = true;
    currentSubmenuList = settingsOptions;
    currentSubmenuCount = countSettingsOptions;
    subMenuIndex = 3;
    settingsSetup();
    return;
  }
  
  // Handle keyboard key presses/releases (all keys are valid for testing)
  if (key != nullptr && strcmp(key, "A") != 0) {
    lastPressedKey = key[0]; // Store first char for display
    lastKeyTime = currentTime;
    Serial.print("[KEYBOARD_TEST] Key pressed: ");
    Serial.println(key);
  }
  
  // Display on OLED
  display.clearDisplay();
  display.drawRoundRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 4, WHITE);
  display.setCursor(8, 7);
  display.println("Keyboard Test");
  
  // Show last pressed key
  if (lastPressedKey != '\0' && (currentTime - lastKeyTime) < KEY_DISPLAY_TIME) {
    display.setCursor(8, 21);
    display.print("Key: ");
    if (lastPressedKey == '*') {
      display.print("*");
    } else if (lastPressedKey == '#') {
      display.print("#");
    } else {
      display.print((char)lastPressedKey);
    }
    
    display.setCursor(8, 49);
    display.print("Press A to exit");
  } else {
    display.setCursor(8, 21);
    display.print("Press any key");
    display.setCursor(8, 35);
    display.print("Press A to exit");
  }
  
  display.display();
}

void keyboardTestSetup() {
  Serial.println("[KEYBOARD_TEST] Keyboard test started");
  inKeyboardTestMode = true;
  lastPressedKey = '\0';
  lastKeyTime = 0;
  resetPhoneStyleInput(); // Reset any phone-style input state
}

