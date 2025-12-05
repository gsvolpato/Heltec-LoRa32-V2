#define DISABLE_ALL_LIBRARY_WARNINGS
#ifndef GPIOS_H
#define GPIOS_H

// 5X4 Matrix Keypad Pins
#define COL1_PIN        3
#define COL2_PIN        23
#define COL3_PIN        2
#define COL4_PIN        17
#define COL5_PIN        13

#define ROW1_PIN        22
#define ROW2_PIN        1
#define ROW3_PIN        32
#define ROW4_PIN        12


// Navigation Buttons
#define ENTER_BUTTON_PIN 0  // Enter button (GPIO 0 - Boot button)

// LoRa Pins
#define LORA_SCK_PIN    5
#define LORA_RESET_PIN  14
#define LORA_CS_PIN     18
#define LORA_MISO_PIN   19
#define LORA_DIO0_PIN   26
#define LORA_MOSI_PIN   27
#define LORA_DIO2_PIN   34
#define LORA_DIO1_PIN   35

// OLED Pins
#define OLED_SCL_PIN    15
#define OLED_SDA_PIN    4
#define OLED_RESET_PIN  16

// VEXT Control Pin
#define VEXT_CONTROL_PIN 21

// Onboard LED Pin
#define ONBOARD_LED_PIN 25

// VBAT Pin
#define VBAT_PIN 1

void gpios_setup();
bool readEnterButton(); // Returns true on button release (debounced)

#endif // GPIOS_H