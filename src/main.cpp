#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <IRrecv.h>
#include <IRsend.h>
#include <IRremoteESP8266.h>
#include <IRutils.h>

// OLED display settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    16  // Reset pin is not connected
#define OLED_SDA 4
#define OLED_SCL 15

// IR receiver and VEXT control
#define IR_RECEIVER_PIN 23
#define VEXT_CONTROL_PIN 21
#define IR_LED_PIN 32
#define ROTARY_BUTTON_PIN 13

// IR receiver settings
const uint32_t kBaudRate = 115200;
const uint16_t kCaptureBufferSize = 1024;
const uint8_t kTimeout = 15;
const uint8_t kTolerancePercentage = kTolerance;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
IRrecv irrecv(IR_RECEIVER_PIN, kCaptureBufferSize, kTimeout, true);
IRsend irsend(IR_LED_PIN);
decode_results results;

uint16_t rawData[kCaptureBufferSize];  // Array to store raw data
uint16_t rawDataLength = 0;  // Length of the captured raw data

void setup() {
  // Initialize Serial for debugging
  Serial.begin(kBaudRate);
  while (!Serial) delay(50);

  // Power on the module
  pinMode(VEXT_CONTROL_PIN, OUTPUT);
  digitalWrite(VEXT_CONTROL_PIN, LOW);  // Assuming LOW turns on the module

  // Initialize I2C for the OLED
  Wire.begin(OLED_SDA, OLED_SCL);

  // Initialize the display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }

  display.setRotation(2);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.clearDisplay();
  display.display();

  // Initialize the IR receiver
  irrecv.setTolerance(kTolerancePercentage);
  irrecv.enableIRIn();

  // Initialize the IR sender
  irsend.begin();

  // Initialize the rotary button
  pinMode(ROTARY_BUTTON_PIN, INPUT_PULLUP);
}

void displayIRInfo(const String &info) {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print(info);
  display.display();
}

void loop() {
  // Check if the rotary button is pressed
  if (digitalRead(ROTARY_BUTTON_PIN) == LOW) {
    // Turn off the IR receiver
    irrecv.disableIRIn();

    // Transmit the captured raw data repeatedly while the button is held
    while (digitalRead(ROTARY_BUTTON_PIN) == LOW) {
      if (rawDataLength > 0) {
        Serial.println("Transmitting captured raw data...");
        irsend.sendRaw(rawData, rawDataLength, 38);  // 38 kHz frequency
        delay(100);  // Delay to control transmission rate
      }
    }

    // Re-enable the IR receiver after the button is released
    irrecv.enableIRIn();
  }

  // Check for IR code reception
  if (irrecv.decode(&results)) {
    // Check for non-repeat codes
    if (results.value != 0xFFFFFFFF && results.value != 0x0) {
      uint32_t now = millis();
      Serial.printf("Timestamp : %06u.%03u\n", now / 1000, now % 1000);
      if (results.overflow)
        Serial.printf("Warning: Buffer full\n");

      // Get human-readable information
      String info = resultToHumanReadableBasic(&results);
      Serial.println(info);

      
      displayIRInfo(info);

      // Store the raw data
      rawDataLength = results.rawlen - 1;
      memcpy(rawData, (const void*)results.rawbuf, rawDataLength * sizeof(uint16_t));

      // Wait for the button to be released
      while (irrecv.decode(&results) && (results.value == 0xFFFFFFFF || results.value == 0x0)) {
        irrecv.resume();
      }
    } else {
      Serial.println("Repeat code detected");
    }

    irrecv.resume();  // Receive the next value
  }
} 