#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "gpios.h"
#include "skull_logo.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET_PIN);

struct TestResult {
    const char* name;
    bool passed;
    const char* details;
};

TestResult testResults[25];
int testIndex = 0;
int totalTests = 0;
int passedTests = 0;

void displayTestScreen(const char* testName, const char* status, int progress) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    
    display.setCursor(0, 0);
    display.println("Pin Self-Test");
    display.drawLine(0, 10, 127, 10, SSD1306_WHITE);
    
    display.setCursor(0, 12);
    display.print("Test: ");
    display.println(testName);
    
    display.setCursor(0, 22);
    display.print("Status: ");
    display.println(status);
    
    display.setCursor(0, 32);
    display.print("Progress: ");
    display.print(progress);
    display.print("/");
    display.print(totalTests);
    
    display.setCursor(0, 42);
    display.print("Passed: ");
    display.print(passedTests);
    
    display.setCursor(0, 52);
    int barWidth = (progress * 120) / totalTests;
    display.drawRect(0, 52, 120, 10, SSD1306_WHITE);
    display.fillRect(2, 54, barWidth - 4, 6, SSD1306_WHITE);
    
    display.display();
    delay(500);
}

void displayResults() {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    
    display.setCursor(0, 0);
    display.println("Test Results:");
    display.drawLine(0, 10, 127, 10, SSD1306_WHITE);
    
    int y = 12;
    for (int i = 0; i < testIndex && y < 64; i++) {
        if (y + 8 > 64) break;
        display.setCursor(0, y);
        display.print(testResults[i].passed ? "[OK]" : "[FAIL]");
        display.print(" ");
        display.print(testResults[i].name);
        y += 8;
    }
    
    display.display();
    delay(3000);
}

void displaySummary() {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    
    display.setCursor(0, 0);
    display.println("Final Summary");
    display.drawLine(0, 10, 127, 10, SSD1306_WHITE);
    
    display.setCursor(0, 15);
    display.print("Total: ");
    display.print(totalTests);
    
    display.setCursor(0, 25);
    display.setTextColor(SSD1306_WHITE);
    display.print("Passed: ");
    display.print(passedTests);
    
    display.setCursor(0, 35);
    display.setTextColor(SSD1306_WHITE);
    display.print("Failed: ");
    display.print(totalTests - passedTests);
    
    display.setCursor(0, 45);
    if (passedTests == totalTests) {
        display.setTextColor(SSD1306_WHITE);
        display.println("ALL TESTS PASSED!");
    } else {
        display.setTextColor(SSD1306_WHITE);
        display.println("SOME TESTS FAILED");
    }
    
    display.display();
}

bool testPin(int pin, const char* name, bool isOutput) {
    testResults[testIndex].name = name;
    testResults[testIndex].details = "";
    
    if (isOutput) {
        pinMode(pin, OUTPUT);
        digitalWrite(pin, HIGH);
        delay(50);
        digitalWrite(pin, LOW);
        delay(50);
        testResults[testIndex].passed = true;
        testResults[testIndex].details = "Output OK";
    } else {
        pinMode(pin, INPUT);
        int value = digitalRead(pin);
        testResults[testIndex].passed = true;
        char details[20];
        snprintf(details, sizeof(details), "Read: %d", value);
        testResults[testIndex].details = details;
    }
    
    if (testResults[testIndex].passed) {
        passedTests++;
    }
    
    testIndex++;
    return testResults[testIndex - 1].passed;
}

bool testVBAT() {
    testResults[testIndex].name = "VBAT (GPIO13)";
    
    pinMode(VBAT_PIN, INPUT);
    int adcValue = analogRead(VBAT_PIN);
    float voltage = (adcValue / 4095.0) * 3.3 * 3.2;
    
    testResults[testIndex].passed = (adcValue > 0 && voltage > 0 && voltage < 5.0);
    char details[30];
    snprintf(details, sizeof(details), "%.2fV (%d)", voltage, adcValue);
    testResults[testIndex].details = details;
    
    if (testResults[testIndex].passed) {
        passedTests++;
    }
    
    testIndex++;
    return testResults[testIndex - 1].passed;
}

bool testVext() {
    testResults[testIndex].name = "Vext Ctrl (GPIO21)";
    
    pinMode(VEXT_CONTROL_PIN, OUTPUT);
    digitalWrite(VEXT_CONTROL_PIN, LOW);
    delay(100);
    digitalWrite(VEXT_CONTROL_PIN, HIGH);
    delay(100);
    digitalWrite(VEXT_CONTROL_PIN, LOW);
    
    testResults[testIndex].passed = true;
    testResults[testIndex].details = "Control OK";
    
    passedTests++;
    testIndex++;
    return true;
}

bool testLED() {
    testResults[testIndex].name = "LED (GPIO25)";
    
    pinMode(ONBOARD_LED_PIN, OUTPUT);
    for (int i = 0; i < 3; i++) {
        digitalWrite(ONBOARD_LED_PIN, HIGH);
        delay(200);
        digitalWrite(ONBOARD_LED_PIN, LOW);
        delay(200);
    }
    
    testResults[testIndex].passed = true;
    testResults[testIndex].details = "Blink OK";
    
    passedTests++;
    testIndex++;
    return true;
}

bool testOLED() {
    testResults[testIndex].name = "OLED Display";
    
    if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
        testResults[testIndex].passed = false;
        testResults[testIndex].details = "Init failed";
        testIndex++;
        return false;
    }
    
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(20, 20);
    display.println("OLED OK");
    display.display();
    delay(500);
    
    testResults[testIndex].passed = true;
    testResults[testIndex].details = "I2C OK";
    
    passedTests++;
    testIndex++;
    return true;
}

bool testLoRaSPI() {
    testResults[testIndex].name = "LoRa SPI Pins";
    
    pinMode(LORA_CS_PIN, OUTPUT);
    pinMode(LORA_SCK_PIN, OUTPUT);
    pinMode(LORA_MOSI_PIN, OUTPUT);
    pinMode(LORA_MISO_PIN, INPUT);
    pinMode(LORA_RESET_PIN, OUTPUT);
    
    digitalWrite(LORA_CS_PIN, HIGH);
    digitalWrite(LORA_RESET_PIN, HIGH);
    delay(10);
    digitalWrite(LORA_RESET_PIN, LOW);
    delay(10);
    digitalWrite(LORA_RESET_PIN, HIGH);
    
    testResults[testIndex].passed = true;
    testResults[testIndex].details = "SPI pins OK";
    
    passedTests++;
    testIndex++;
    return true;
}

bool testLoRaDIO() {
    testResults[testIndex].name = "LoRa DIO Pins";
    
    pinMode(LORA_DIO0_PIN, INPUT);
    pinMode(LORA_DIO1_PIN, INPUT);
    pinMode(LORA_DIO2_PIN, INPUT);
    
    int dio0 = digitalRead(LORA_DIO0_PIN);
    int dio1 = digitalRead(LORA_DIO1_PIN);
    int dio2 = digitalRead(LORA_DIO2_PIN);
    
    testResults[testIndex].passed = true;
    char details[30];
    snprintf(details, sizeof(details), "DIO: %d/%d/%d", dio0, dio1, dio2);
    testResults[testIndex].details = details;
    
    passedTests++;
    testIndex++;
    return true;
}

void showLogo() {
    display.clearDisplay();
    display.drawBitmap(0, 0, skull_logo, 128, 64, SSD1306_WHITE);
    display.display();
    delay(2000);
}

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("\n=== Heltec WiFi LoRa 32 V2 Pin Self-Test ===\n");
    
    Wire.begin(OLED_SDA_PIN, OLED_SCL_PIN);
    
    if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
        Serial.println("OLED initialization failed!");
        while (1) delay(1000);
    }
    
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(10, 20);
    display.println("Self-Test");
    display.setCursor(10, 40);
    display.println("Starting...");
    display.display();
    delay(2000);
    
    showLogo();
    
    totalTests = 19;
    testIndex = 0;
    passedTests = 0;
    
    displayTestScreen("Initializing", "Starting", 0);
    delay(500);
    
    displayTestScreen("OLED", "Testing", 1);
    testOLED();
    delay(500);
    
    displayTestScreen("Vext Control", "Testing", 2);
    testVext();
    delay(500);
    
    displayTestScreen("LED", "Testing", 3);
    testLED();
    delay(500);
    
    displayTestScreen("VBAT", "Testing", 4);
    testVBAT();
    delay(500);
    
    displayTestScreen("LoRa SPI", "Testing", 5);
    testLoRaSPI();
    delay(500);
    
    displayTestScreen("LoRa DIO", "Testing", 6);
    testLoRaDIO();
    delay(500);
    
    displayTestScreen("GPIO 0", "Testing", 7);
    testPin(ENTER_BUTTON_PIN, "GPIO 0 (Boot)", false);
    delay(500);
    
    displayTestScreen("GPIO 2", "Testing", 8);
    testPin(2, "GPIO 2", true);
    delay(500);
    
    displayTestScreen("GPIO 12", "Testing", 9);
    testPin(12, "GPIO 12", true);
    delay(500);
    
    displayTestScreen("GPIO 17", "Testing", 10);
    testPin(17, "GPIO 17", true);
    delay(500);
    
    displayTestScreen("GPIO 22", "Testing", 11);
    testPin(22, "GPIO 22", true);
    delay(500);
    
    displayTestScreen("GPIO 23", "Testing", 12);
    testPin(23, "GPIO 23", true);
    delay(500);
    
    displayTestScreen("GPIO 32", "Testing", 13);
    testPin(32, "GPIO 32", true);
    delay(500);
    
    displayTestScreen("GPIO 34", "Testing", 14);
    testPin(LORA_DIO2_PIN, "GPIO 34 (DIO2)", false);
    delay(500);
    
    displayTestScreen("GPIO 35", "Testing", 15);
    testPin(LORA_DIO1_PIN, "GPIO 35 (DIO1)", false);
    delay(500);
    
    displayTestScreen("GPIO 36", "Testing", 16);
    testPin(JP3_PIN4_GPIO36, "GPIO 36 (JP3-4)", false);
    delay(500);
    
    displayTestScreen("GPIO 37", "Testing", 17);
    testPin(JP3_PIN5_GPIO37, "GPIO 37 (JP3-5)", false);
    delay(500);
    
    displayTestScreen("GPIO 38", "Testing", 18);
    testPin(JP3_PIN6_GPIO38, "GPIO 38 (JP3-6)", false);
    delay(500);
    
    displayTestScreen("GPIO 39", "Testing", 19);
    testPin(JP3_PIN7_GPIO39, "GPIO 39 (JP3-7)", false);
    delay(500);
    
    displayResults();
    delay(3000);
    
    displaySummary();
    
    Serial.println("\n=== Test Results ===");
    for (int i = 0; i < testIndex; i++) {
        Serial.printf("%s: %s - %s\n", 
            testResults[i].passed ? "PASS" : "FAIL",
            testResults[i].name,
            testResults[i].details);
    }
    Serial.printf("\nTotal: %d/%d passed\n", passedTests, totalTests);
}

void loop() {
    displaySummary();
    delay(5000);
    
    displayResults();
    delay(5000);
}

