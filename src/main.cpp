#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <WiFi.h>
#include "system/gpios.h"
#include "system/display_config.h"
#include "system/skull_logo.h"
#include "system/menu_config.h"
#include "system/homescreen_config.h"
#include "system/keyboard/keyboard_config.h"
#include "system/lora/lora_config.h"
#include "apps/notepad/notepad_config.h"
#include "apps/settings/keyboard_test.h"
#include "system/freertos/tasks_config.h"
#include "system/freertos/utils.h"

void setup() {
  Serial.begin(115200);
  delay(100);
  Serial.println("\n[INIT] System starting with FreeRTOS...");
  
  // Disable WiFi
  Serial.println("[INIT] Disabling WiFi");
  WiFi.mode(WIFI_OFF);
  WiFi.disconnect(true);
  
  // Disable Bluetooth (if available)
  #ifdef ESP32
    Serial.println("[INIT] Disabling Bluetooth");
    btStop();
  #endif
  
  // Disable LoRa
  Serial.println("[INIT] Disabling LoRa");
  disableLoRa();
  
  // Initialize GPIOs
  Serial.println("[INIT] Initializing GPIOs");
  gpios_setup();
  
  // Initialize I2C for OLED
  Serial.println("[INIT] Initializing I2C");
  Wire.begin(OLED_SDA_PIN, OLED_SCL_PIN);
  
  // Initialize display
  Serial.println("[INIT] Initializing display");
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("[ERROR] SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }
  
  // Display skull logo for 3 seconds
  Serial.println("[INIT] Displaying skull logo");
  lockDisplay();
  displaySkullLogo();
  unlockDisplay();
  delay(3000);
  
  // Initialize keyboard
  Serial.println("[INIT] Initializing keyboard");
  keyboardInit();
  
  // Initialize FreeRTOS
  Serial.println("[INIT] Initializing FreeRTOS");
  initFreeRTOS();
  
  // Initialize menu system
  Serial.println("[INIT] Initializing menu system");
  startDisplay();
  
  // Create FreeRTOS tasks
  Serial.println("[INIT] Creating FreeRTOS tasks");
  createTasks();
  
  // Show homescreen
  Serial.println("[INIT] System ready - FreeRTOS tasks running");
  sendSerialMessage("[INIT] System ready - showing homescreen\n");
  
  // Initialize homescreen
  homescreenSetup();
}

void loop() {
  // FreeRTOS scheduler handles everything
  // This loop should be empty or minimal
  vTaskDelay(pdMS_TO_TICKS(1000));
}
