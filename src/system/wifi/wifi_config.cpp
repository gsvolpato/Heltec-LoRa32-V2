#include "wifi_config.h"
#include "system/menu_config.h"
#include "system/display_config.h"
#include "system/freertos/utils.h"
#include "system/keyboard/keyboard_config.h"
#include "system/freertos/keyboard_queue.h"
#include "../gpios.h"
#include <WiFi.h>
#include <Arduino.h>
#include <cstring>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

// WiFi state tracking
static bool wifiEnabled = false;

// WiFi password input mode
bool inWiFiPasswordMode = false;
int wifiPasswordNetworkIndex = -1;

// Password input buffer
#define WIFI_PASSWORD_BUFFER_SIZE 64
static char wifiPasswordBuffer[WIFI_PASSWORD_BUFFER_SIZE];
static int wifiPasswordCursor = 0;

// Cursor blink state
static unsigned long lastWiFiPasswordBlinkTime = 0;
static bool wifiPasswordCursorVisible = true;
static const unsigned long WIFI_PASSWORD_BLINK_INTERVAL = 500; // milliseconds


// Scanned networks storage
static String scannedNetworks[MAX_NETWORKS];
static int32_t scannedRSSI[MAX_NETWORKS];
static wifi_auth_mode_t scannedEncryption[MAX_NETWORKS];
static int scannedNetworkCount = 0;

void disableWiFi() {
  WiFi.mode(WIFI_OFF);
  WiFi.disconnect(true);
  wifiEnabled = false;
  Serial.println("[WIFI] WiFi disabled");
}

void enableWiFi() {
  WiFi.mode(WIFI_STA);
  wifiEnabled = true;
  Serial.println("[WIFI] WiFi enabled");
  // Automatically scan networks when WiFi is enabled
  scanWiFiNetworks();
}

bool isWiFiEnabled() {
  return wifiEnabled;
}

void toggleWiFi() {
  if (wifiEnabled) {
    disableWiFi();
    scannedNetworkCount = 0;
  } else {
    enableWiFi();
  }
  // Reset last rebuilt count to force menu rebuild
  extern int lastRebuiltNetworkCount;
  lastRebuiltNetworkCount = -1;
}

void scanWiFiNetworks() {
  if (!wifiEnabled) {
    scannedNetworkCount = 0;
    return;
  }
  
  Serial.println("[WIFI] Scanning for networks...");
  WiFi.disconnect();
  vTaskDelay(pdMS_TO_TICKS(100));
  
  // Start scan and wait for it to complete
  int n = WiFi.scanNetworks();
  
  // If scan returns -1, it might still be scanning, wait a bit
  if (n == -1) {
    Serial.println("[WIFI] Scan in progress, waiting...");
    int attempts = 0;
    while (n == -1 && attempts < 50) { // Wait up to 5 seconds
      vTaskDelay(pdMS_TO_TICKS(100));
      n = WiFi.scanNetworks();
      attempts++;
    }
  }
  
  if (n == -1) {
    Serial.println("[WIFI] Scan failed or timed out");
    scannedNetworkCount = 0;
    return;
  }
  
  if (n > MAX_NETWORKS) {
    n = MAX_NETWORKS;
  }
  
  // Store networks immediately to preserve the data
  scannedNetworkCount = n;
  
  for (int i = 0; i < n; i++) {
    // Copy SSID string immediately to preserve it
    String ssid = WiFi.SSID(i);
    scannedNetworks[i] = ssid; // This creates a copy
    scannedRSSI[i] = WiFi.RSSI(i);
    scannedEncryption[i] = WiFi.encryptionType(i);
    Serial.print("[WIFI] Found: ");
    Serial.print(scannedNetworks[i]);
    Serial.print(" (");
    Serial.print(scannedRSSI[i]);
    Serial.println(" dBm)");
  }
  
  Serial.print("[WIFI] Scan complete. Found ");
  Serial.print(scannedNetworkCount);
  Serial.println(" networks");
}

int getWiFiNetworkCount() {
  return scannedNetworkCount;
}

const char* getWiFiNetworkSSID(int index) {
  if (index >= 0 && index < scannedNetworkCount && scannedNetworkCount > 0) {
    // Ensure the String is valid before returning c_str()
    if (scannedNetworks[index].length() > 0) {
      return scannedNetworks[index].c_str();
    }
  }
  return "";
}

int32_t getWiFiNetworkRSSI(int index) {
  if (index >= 0 && index < scannedNetworkCount) {
    return scannedRSSI[index];
  }
  return 0;
}

wifi_auth_mode_t getWiFiNetworkEncryption(int index) {
  if (index >= 0 && index < scannedNetworkCount) {
    return scannedEncryption[index];
  }
  return WIFI_AUTH_OPEN;
}

void wifiSetup() {
    Serial.println("[MENU] WiFi submenu opened");
    subMenuIndex = 0;  // Reset cursor position
    
    // If WiFi is enabled, ensure scan is done and wait if needed
    if (isWiFiEnabled()) {
      int networkCount = getWiFiNetworkCount();
      if (networkCount == 0) {
        Serial.println("[WIFI] WiFi enabled but no networks yet, waiting for scan...");
        // Wait for scan to complete (up to 5 seconds)
        for (int i = 0; i < 50; i++) {
          vTaskDelay(pdMS_TO_TICKS(100));
          networkCount = getWiFiNetworkCount();
          if (networkCount > 0) {
            Serial.print("[WIFI] Scan complete, found ");
            Serial.print(networkCount);
            Serial.println(" networks");
            break;
          }
        }
      }
    }
    
    // Rebuild menu options (toggle + networks)
    extern void rebuildWiFiMenuOptions();
    rebuildWiFiMenuOptions();
    displayMenu(countWifiOptions, wifiOptions, true);
  }

  
void displayNetworkInfo(int networkIndex) {
    if (networkIndex < 0 || networkIndex >= scannedNetworkCount) {
      return;
    }
    
    String ssid = scannedNetworks[networkIndex];
    int32_t rssi = scannedRSSI[networkIndex];
    String encType;
    switch(scannedEncryption[networkIndex]) {
      case WIFI_AUTH_OPEN: encType = "Open"; break;
      case WIFI_AUTH_WEP: encType = "WEP"; break;
      case WIFI_AUTH_WPA_PSK: encType = "WPA PSK"; break;
      case WIFI_AUTH_WPA2_PSK: encType = "WPA2 PSK"; break;
      case WIFI_AUTH_WPA_WPA2_PSK: encType = "WPA/WPA2"; break;
      default: encType = "Unknown";
    }
  
    lockDisplay();
    display.clearDisplay();
    display.drawRoundRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 4, WHITE);
    display.setCursor(8, 7);
    display.println("Network Info:");
    display.setCursor(8, 21);
    display.println(ssid);
    display.setCursor(8, 35);
    display.print("Signal: ");
    display.print(rssi);
    display.println("dB");
    display.setCursor(8, 49);
    display.print("Security: ");
    display.println(encType);
    display.display();
    unlockDisplay();
  }


void wifiPasswordMain(int networkIndex) {
  // Note: Display is already locked by taskMenu, so we don't lock/unlock here
  
  // Check for 'A' key to cancel
  const char* key = getKeyFromQueue();
  if (key != nullptr && strcmp(key, "A") == 0) {
    Serial.println("[WIFI] Password input cancelled");
    inWiFiPasswordMode = false;
    wifiPasswordNetworkIndex = -1;
    wifiPasswordCursor = 0;
    wifiPasswordBuffer[0] = '\0';
    resetPhoneStyleInput();
    // Return to WiFi menu
    extern void rebuildWiFiMenuOptions();
    rebuildWiFiMenuOptions();
    extern void displayMenu(int count, const char* list[], bool isSubmenu);
    extern int countWifiOptions;
    extern const char** wifiOptions;
    displayMenu(countWifiOptions, wifiOptions, true);
    return;
  }
  
  // Check for enter button to connect
  if (readEnterButton()) {
    Serial.println("[WIFI] Attempting to connect...");
    const char* ssid = getWiFiNetworkSSID(networkIndex);
    if (ssid != nullptr && strlen(ssid) > 0) {
      // Connect to WiFi
      bool connected = connectToWiFi(ssid, wifiPasswordBuffer);
      if (connected) {
        Serial.println("[WIFI] Connected successfully!");
        display.clearDisplay();
        display.drawRoundRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 4, WHITE);
        display.setCursor(8, 7);
        display.println("Connected!");
        display.setCursor(8, 21);
        display.println(ssid);
        display.setCursor(8, 35);
        display.print("IP: ");
        display.println(WiFi.localIP());
        display.display();
        vTaskDelay(pdMS_TO_TICKS(3000));
      } else {
        Serial.println("[WIFI] Connection failed!");
        display.clearDisplay();
        display.drawRoundRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 4, WHITE);
        display.setCursor(8, 7);
        display.println("Connection");
        display.setCursor(8, 21);
        display.println("failed!");
        display.display();
        vTaskDelay(pdMS_TO_TICKS(2000));
      }
    }
    // Return to WiFi menu
    inWiFiPasswordMode = false;
    wifiPasswordNetworkIndex = -1;
    wifiPasswordCursor = 0;
    wifiPasswordBuffer[0] = '\0';
    resetPhoneStyleInput();
    extern void rebuildWiFiMenuOptions();
    rebuildWiFiMenuOptions();
    extern void displayMenu(int count, const char* list[], bool isSubmenu);
    extern int countWifiOptions;
    extern const char** wifiOptions;
    displayMenu(countWifiOptions, wifiOptions, true);
    return;
  }
  
  // Handle phone-style text input
  char newChar = getPhoneStyleChar();
  if (newChar != '\0') {
    if (newChar == '\n') {
      newChar = ' ';
    }
    
    if (wifiPasswordCursor < WIFI_PASSWORD_BUFFER_SIZE - 1) {
      wifiPasswordBuffer[wifiPasswordCursor] = newChar;
      wifiPasswordCursor++;
      wifiPasswordBuffer[wifiPasswordCursor] = '\0';
      Serial.print("[WIFI] Password char: ");
      Serial.println(newChar);
    }
  }
  
  // Get preview of character being typed
  char previewChar = getPhoneStylePreview();
  
  // Get network info
  const char* ssid = getWiFiNetworkSSID(networkIndex);
  int32_t rssi = getWiFiNetworkRSSI(networkIndex);
  
  // Display password input screen (display already locked by taskMenu)
  display.clearDisplay();
  display.drawRoundRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 4, WHITE);
  
  // Line 1: SSID
  display.setCursor(8, 7);
  if (ssid != nullptr) {
    display.println(ssid);
  } else {
    display.println("Unknown");
  }
  
  // Line 2: Signal strength
  display.setCursor(8, 21);
  display.print("Signal: ");
  display.print(rssi);
  display.println(" dBm");
  
  // Line 3: Password field with blinking cursor
  display.setCursor(8, 35);
  display.print("Password: ");
  display.print(wifiPasswordBuffer);
  
  // Show preview if typing
  if (previewChar != '\0') {
    display.print(previewChar);
  }
  
  // Blinking cursor
  unsigned long currentTime = millis();
  if (currentTime - lastWiFiPasswordBlinkTime > WIFI_PASSWORD_BLINK_INTERVAL) {
    wifiPasswordCursorVisible = !wifiPasswordCursorVisible;
    lastWiFiPasswordBlinkTime = currentTime;
  }
  
  if (wifiPasswordCursorVisible) {
    display.print("_");
  }
  
  display.display();
}

void inputWiFiPassword(int networkIndex) {
  if (networkIndex < 0 || networkIndex >= scannedNetworkCount) {
    return;
  }
  
  Serial.print("[WIFI] Starting password input for network: ");
  Serial.println(getWiFiNetworkSSID(networkIndex));
  
  // Initialize password input
  inWiFiPasswordMode = true;
  wifiPasswordNetworkIndex = networkIndex;
  wifiPasswordCursor = 0;
  wifiPasswordBuffer[0] = '\0';
  wifiPasswordCursorVisible = true;
  lastWiFiPasswordBlinkTime = millis();
  resetPhoneStyleInput();
  
  // Clear keypad queue
  extern void clearKeyboardQueue();
  clearKeyboardQueue();
}

bool connectToWiFi(const char* ssid, const char* password) {
  if (ssid == nullptr || strlen(ssid) == 0) {
    return false;
  }
  
  Serial.print("[WIFI] Connecting to: ");
  Serial.println(ssid);
  
  // Ensure WiFi is enabled
  if (!isWiFiEnabled()) {
    enableWiFi();
  }
  
  // Connect
  WiFi.begin(ssid, password);
  
  // Wait for connection (up to 10 seconds)
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 100) {
    vTaskDelay(pdMS_TO_TICKS(100));
    attempts++;
    if (attempts % 10 == 0) {
      Serial.print(".");
    }
  }
  Serial.println();
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("[WIFI] Connected! IP: ");
    Serial.println(WiFi.localIP());
    return true;
  } else {
    Serial.println("[WIFI] Connection failed!");
    return false;
  }
  }
  