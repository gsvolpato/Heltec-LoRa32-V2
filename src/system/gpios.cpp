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

float readBatteryVoltage() {
  // Note: GPIO 13 is also used for COL5_PIN
  // Temporarily configure as analog input for battery reading
  // Column pins are normally OUTPUT for keypad scanning
  
  // Configure pin as input for analog reading
  pinMode(VBAT_PIN, INPUT);
  
  // Small delay to let pin stabilize after mode change
  delay(10);
  
  // Read ADC value (ESP32 ADC is 12-bit: 0-4095)
  // For better accuracy, take multiple samples and average
  uint32_t adcSum = 0;
  const int samples = 5;
  for (int i = 0; i < samples; i++) {
    adcSum += analogRead(VBAT_PIN);
    delay(5);
  }
  uint32_t adcReading = adcSum / samples;
  
  // Restore pin configuration (back to OUTPUT for keypad column)
  pinMode(VBAT_PIN, OUTPUT);
  digitalWrite(VBAT_PIN, HIGH);
  
  // Convert ADC reading to battery voltage
  // Formula from gpios.h: VBAT = (ADC_reading / 4095.0) * 3.3 * 3.2
  // Voltage divider ratio: 100/(220+100) = 0.3125
  // So: VBAT = (ADC_reading / 4095.0) * 3.3 / 0.3125 = (ADC_reading / 4095.0) * 3.3 * 3.2
  float voltage = (adcReading / 4095.0) * 3.3 * 3.2;
  
  return voltage;
}

uint8_t getBatteryPercentage() {
  float voltage = readBatteryVoltage();
  
  // Li-ion battery voltage range
  // 3.0V = 0% (empty/cutoff)
  // 4.2V = 100% (full)
  const float minVoltage = 3.0;
  const float maxVoltage = 4.2;
  
  // Clamp voltage to valid range
  if (voltage < minVoltage) {
    voltage = minVoltage;
  } else if (voltage > maxVoltage) {
    voltage = maxVoltage;
  }
  
  // Calculate percentage (linear interpolation)
  float percentage = ((voltage - minVoltage) / (maxVoltage - minVoltage)) * 100.0;
  
  // Clamp to 0-100 range
  if (percentage < 0) {
    percentage = 0;
  } else if (percentage > 100) {
    percentage = 100;
  }
  
  return (uint8_t)percentage;
}
