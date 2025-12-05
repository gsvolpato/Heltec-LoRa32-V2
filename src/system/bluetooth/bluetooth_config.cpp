#include "bluetooth_config.h"
#include "system/menu_config.h"
#include "system/display_config.h"
#include <Arduino.h>

#ifdef ESP32
#include "esp_bt.h"
#endif

void disableBluetooth() {
  #ifdef ESP32
    btStop();
  #endif
}

void bluetoothSetup() {
    Serial.println("[MENU] Bluetooth submenu opened");
    disableBluetooth(); // Ensure Bluetooth is off
    subMenuIndex = 0;  // Reset cursor to first position
    displayMenu(countBluetoothOptions, bluetoothOptions, true);
  }

  
void BTscan() {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Scanning for Bluetooth devices...");
    display.display();
  }
  