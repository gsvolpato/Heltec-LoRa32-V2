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

// VBAT Pin - Connected to voltage divider (R10=220K, R12=100K)
// VBAT = ADC_reading * 3.2 (voltage divider ratio: 100/(220+100) = 0.3125)
#define VBAT_PIN 13

// ============================================================================
// HEADER JP2 PIN DEFINITIONS (Left Header - 18 pins)
// ============================================================================
// Pin 18: GPIO 16 (OLED_RST) - See OLED_RESET_PIN
// Pin 17: GPIO 17
#define JP2_PIN17_GPIO17    17
// Pin 16: GPIO 4 (SDA) - See OLED_SDA_PIN
// Pin 15: GPIO 2
#define JP2_PIN15_GPIO2     2
// Pin 14: GPIO 15 (SCL) - See OLED_SCL_PIN
// Pin 13: SCK (GPIO 5) - See LORA_SCK_PIN
// Pin 12: CS (GPIO 18) - See LORA_CS_PIN
// Pin 11: GPIO 23
#define JP2_PIN11_GPIO23    23
// Pin 10: MISO (GPIO 19) - See LORA_MISO_PIN
// Pin 9: GPIO 22
#define JP2_PIN9_GPIO22     22
// Pin 8: GPIO 0 - See ENTER_BUTTON_PIN
// Pin 7: CHIP_PU (Reset, not a GPIO)
// Pin 6: U0TXD (GPIO 1)
#define JP2_PIN6_U0TXD      1
// Pin 5: U0RXD (GPIO 3)
#define JP2_PIN5_U0RXD      3
// Pin 4: Vext (Power, not GPIO)
// Pin 3: Vext (Power, not GPIO)
// Pin 2: +5V (Power, not GPIO)
// Pin 1: GND (Ground, not GPIO)

// ============================================================================
// HEADER JP3 PIN DEFINITIONS (Right Header - 18 pins)
// ============================================================================
// Pin 18: GPIO 21 - See VEXT_CONTROL_PIN
// Pin 17: GPIO 13 - See VBAT_PIN
// Pin 16: GPIO 12
#define JP3_PIN16_GPIO12    12
// Pin 15: RST_127814 (GPIO 14) - See LORA_RESET_PIN
// Pin 14: MOSI 27 (GPIO 27) - See LORA_MOSI_PIN
// Pin 13: DIO0 (GPIO 26) - See LORA_DIO0_PIN
// Pin 12: GPIO 25 - See ONBOARD_LED_PIN
// Pin 11: 32K_XN (Crystal, not GPIO)
// Pin 10: 32K_XP (Crystal, not GPIO)
// Pin 9: DIO1 (GPIO 35) - See LORA_DIO1_PIN
// Pin 8: DIO2 (GPIO 34) - See LORA_DIO2_PIN
// Pin 7: GPIO 39
#define JP3_PIN7_GPIO39     39
// Pin 6: GPIO 38
#define JP3_PIN6_GPIO38     38
// Pin 5: GPIO 37
#define JP3_PIN5_GPIO37     37
// Pin 4: GPIO 36
#define JP3_PIN4_GPIO36     36
// Pin 3: 3.3V (Power, not GPIO)
// Pin 2: 3.3V (Power, not GPIO)
// Pin 1: GND (Ground, not GPIO)

// ============================================================================
// ALL AVAILABLE GPIO PINS SUMMARY
// ============================================================================
// Header JP2 GPIOs: 0, 1, 2, 3, 4, 5, 15, 16, 17, 18, 19, 22, 23
// Header JP3 GPIOs: 12, 13, 14, 21, 25, 26, 27, 34, 35, 36, 37, 38, 39
// Other GPIOs (not on headers): 6 (FLASH_SCK - internal), 32 (available but not on headers)
// Total Available GPIOs: 0, 1, 2, 3, 4, 5, 6, 12, 13, 14, 15, 16, 17, 18, 19,
//                         21, 22, 23, 25, 26, 27, 32, 34, 35, 36, 37, 38, 39

void gpios_setup();
bool readEnterButton(); // Returns true on button release (debounced)

/*
 * SCHEMATIC VERIFIED PIN ASSIGNMENTS (WiFi LoRa 32 V2)
 * 
 * All pin assignments verified from actual board schematics.
 * 
 * UART0 (CP2102 USB-to-Serial):
 *   - GPIO 1: U0TXD
 *   - GPIO 3: U0RXD
 * 
 * OLED Display (I2C):
 *   - GPIO 4:  OLED_SDA
 *   - GPIO 15: OLED_SCL
 *   - GPIO 16: OLED_RST
 * 
 * LoRa Module (SPI):
 *   - GPIO 5:  SCK
 *   - GPIO 18: CS
 *   - GPIO 19: MISO
 *   - GPIO 27: MOSI
 *   - GPIO 14: RST
 *   - GPIO 26: DIO0
 *   - GPIO 34: DIO2
 *   - GPIO 35: DIO1
 * 
 * Power Management:
 *   - GPIO 21: Vext Control (via R13 1K to Q3 MOSFET gate)
 *              LOW = Vext ON, HIGH = Vext OFF
 *   - GPIO 13: VBAT Reading (voltage divider: R10=220K, R12=100K)
 *              VBAT = (ADC_reading / 4095.0) * 3.3 * 3.2
 *              Voltage divider ratio: 100/(220+100) = 0.3125
 * 
 * Status Indicators:
 *   - GPIO 25: Onboard LED
 * 
 * Flash Memory:
 *   - GPIO 6:  FLASH_SCK (internal, not user-accessible)
 */

#endif // GPIOS_H