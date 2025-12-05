#include "gpios.h"
#include <Arduino.h>

void gpios_setup() {
  // Setup matrix keypad pins
  // Columns as outputs (initially HIGH)
  pinMode(COL1_PIN, OUTPUT);
  pinMode(COL2_PIN, OUTPUT);
  pinMode(COL3_PIN, OUTPUT);
  pinMode(COL4_PIN, OUTPUT);
  pinMode(COL5_PIN, OUTPUT);
  digitalWrite(COL1_PIN, HIGH);
  digitalWrite(COL2_PIN, HIGH);
  digitalWrite(COL3_PIN, HIGH);
  digitalWrite(COL4_PIN, HIGH);
  digitalWrite(COL5_PIN, HIGH);
  
  // Rows as inputs with pull-up
  pinMode(ROW1_PIN, INPUT_PULLUP);
  pinMode(ROW2_PIN, INPUT_PULLUP);
  pinMode(ROW3_PIN, INPUT_PULLUP);
  pinMode(ROW4_PIN, INPUT_PULLUP);
  
  // Navigation button as input with pull-up
  pinMode(ENTER_BUTTON_PIN, INPUT_PULLUP);
  
  // OLED pins are handled by Wire library
  // VEXT control pin
  pinMode(VEXT_CONTROL_PIN, OUTPUT);
  digitalWrite(VEXT_CONTROL_PIN, LOW); // Enable VEXT (LOW = ON for Heltec boards)
  
  // LoRa reset pin - keep HIGH to disable LoRa (reset is active LOW)
  pinMode(LORA_RESET_PIN, OUTPUT);
  digitalWrite(LORA_RESET_PIN, HIGH); // Keep LoRa disabled
  
  // Onboard LED
  pinMode(ONBOARD_LED_PIN, OUTPUT);
  digitalWrite(ONBOARD_LED_PIN, LOW);
}

// Button state tracking for debouncing
static bool lastEnterButtonState = HIGH;
static unsigned long lastEnterButtonTime = 0;
static const unsigned long BUTTON_DEBOUNCE_TIME = 50;

bool readEnterButton() {
  bool currentState = digitalRead(ENTER_BUTTON_PIN);
  unsigned long currentTime = millis();
  
  if (currentState == LOW && lastEnterButtonState == HIGH) {
    if (currentTime - lastEnterButtonTime > BUTTON_DEBOUNCE_TIME) {
      lastEnterButtonState = LOW;
      lastEnterButtonTime = currentTime;
    }
  } else if (currentState == HIGH && lastEnterButtonState == LOW) {
    if (currentTime - lastEnterButtonTime > BUTTON_DEBOUNCE_TIME) {
      lastEnterButtonState = HIGH;
      lastEnterButtonTime = currentTime;
      return true;
    }
  }
  
  return false;
}
