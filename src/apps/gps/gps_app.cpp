#include "gps_app.h"
#include "../../system/menu_config.h"
#include "../../system/display_config.h"
#include "../../system/gps/gps_config.h"
#include "../../system/freertos/keyboard_queue.h"
#include "../../system/gpios.h"
#include <Arduino.h>
#include <cstring>

// GPS app mode tracking
bool inGPSAppMode = false;
bool inGPSSelftestMode = false;

void gpsAppSetup() {
    Serial.println("[MENU] GPS app submenu opened");
    subMenuIndex = 0;  // Reset cursor position
    inGPSAppMode = true;
    inGPSSelftestMode = false;
    
    // Display GPS submenu
    extern const char* gpsOptions[];
    extern int countGPSOptions;
    displayMenu(countGPSOptions, gpsOptions, true);
}

void gpsSelftestMain() {
    // Consume any keyboard keys pressed (they should be ignored during selftest)
    const char* key = getKeyFromQueue();
    if (key != nullptr) {
        // Keys ignored during selftest
    }

    // Check for enter button to return to GPS menu
    if (readEnterButton()) {
        Serial.println("[GPS_SELFTEST] Enter button pressed - returning to GPS menu");
        clearKeyboardQueue();
        inGPSSelftestMode = false;
        inGPSAppMode = true;
        extern const char* gpsOptions[];
        extern int countGPSOptions;
        subMenuIndex = 0;
        displayMenu(countGPSOptions, gpsOptions, true);
        return;
    }

    // Display selftest screen
    display.clearDisplay();
    display.drawRoundRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 4, WHITE);
    display.setTextSize(1);
    display.setTextColor(WHITE);
    
    display.setCursor(4, 8);
    display.print("GPS Selftest");
    
    display.setCursor(4, 20);
    if (isGPSEnabled()) {
        display.print("Status: Enabled");
    } else {
        display.print("Status: Disabled");
    }
    
    display.setCursor(4, 32);
    if (gpsAvailable()) {
        display.print("Data: Available");
    } else {
        display.print("Data: Waiting...");
    }
    
    // Read and display GPS data
    if (gpsAvailable()) {
        String gpsData = readGPSData();
        if (gpsData.length() > 0) {
            // Display first line of GPS data (truncate if too long)
            display.setCursor(4, 44);
            if (gpsData.length() > 20) {
                String truncated = gpsData.substring(0, 17) + "...";
                display.print(truncated);
            } else {
                display.print(gpsData);
            }
            
            // Display second line if available
            if (gpsAvailable()) {
                String gpsData2 = readGPSData();
                if (gpsData2.length() > 0) {
                    display.setCursor(4, 56);
                    if (gpsData2.length() > 20) {
                        String truncated2 = gpsData2.substring(0, 17) + "...";
                        display.print(truncated2);
                    } else {
                        display.print(gpsData2);
                    }
                }
            }
        }
    } else {
        display.setCursor(4, 44);
        display.print("No data received");
    }
    
    display.display();
}

void gpsAppMain() {
    if (inGPSSelftestMode) {
        gpsSelftestMain();
        return;
    }
    
    // Normal GPS app menu navigation is handled by menu system
}
