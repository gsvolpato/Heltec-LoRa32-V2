#include "lora_config.h"
#include "system/menu_config.h"
#include <Arduino.h>

void disableLoRa() {
  // Keep LoRa reset pin HIGH (inactive state)
  // LoRa reset is active LOW, so HIGH keeps it disabled
  pinMode(LORA_RESET_PIN, OUTPUT);
  digitalWrite(LORA_RESET_PIN, HIGH);
}

void loraSetup() {
    Serial.println("[MENU] LoRa submenu opened");
    disableLoRa(); // Ensure LoRa is off
    subMenuIndex = 0;  // Reset cursor to first position
    displayMenu(countLoraOptions, loraOptions, true);
  }