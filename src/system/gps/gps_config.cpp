#include "gps_config.h"
#include <Arduino.h>

// GPS state
bool gpsEnabled = false;
HardwareSerial gpsSerial(1); // Use UART1 for GPS

void enableGPS() {
  if (gpsEnabled) {
    return; // Already enabled
  }
  
  // Ensure VEXT is enabled (LOW = ON for Heltec boards)
  // This powers the GPS module
  pinMode(VEXT_CONTROL_PIN, OUTPUT);
  digitalWrite(VEXT_CONTROL_PIN, LOW);
  
  // Initialize GPS UART (UART1)
  // rxPin (GPIO 3): ESP32 receives data from GPS TX
  // txPin (GPIO 1): ESP32 transmits data to GPS RX
  // Note: GPIO 1 and 3 are UART0 pins, but we're using UART1 hardware
  // connected to these pins. Serial (USB) uses UART0 but can coexist.
  gpsSerial.begin(GPS_BAUD_RATE, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
  
  gpsEnabled = true;
  Serial.println("[GPS] GPS module enabled");
}

void disableGPS() {
  if (!gpsEnabled) {
    return; // Already disabled
  }
  
  gpsSerial.end();
  gpsEnabled = false;
  Serial.println("[GPS] GPS module disabled");
}

bool isGPSEnabled() {
  return gpsEnabled;
}

void gpsSetup() {
  Serial.println("[GPS] GPS setup called");
  // GPS is disabled by default
  disableGPS();
}

bool gpsAvailable() {
  if (!gpsEnabled) {
    return false;
  }
  return gpsSerial.available() > 0;
}

String readGPSData() {
  if (!gpsEnabled) {
    return "";
  }
  
  String data = "";
  while (gpsSerial.available() > 0) {
    char c = gpsSerial.read();
    if (c == '\n') {
      break;
    }
    data += c;
  }
  
  return data;
}
