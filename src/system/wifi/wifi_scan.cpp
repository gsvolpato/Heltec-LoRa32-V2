#include "wifi_scan.h"
#include "wifi_config.h"
#include "../menu_config.h"
#include "../display_config.h"
#include <WiFi.h>
#include <Arduino.h>

void networkScan(bool isConnectMode) {
    display.clearDisplay();
    display.drawRoundRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 4, WHITE);
    display.setCursor(8, 7);
    display.println("Scanning...");
    display.display();
    
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);
    
    int n = WiFi.scanNetworks();
    display.clearDisplay();
    
    if (n == 0) {
      display.drawRoundRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 4, WHITE);
      display.setCursor(8, 7);
      display.println("No networks found!");
      display.display();
      delay(2000);
      displayMenu(countWifiOptions, wifiOptions, true);
    } else {
      String* networks = new String[n + 1];
      const char** networkList = new const char*[n + 1];
      
      for (int i = 0; i < n; i++) {
        networks[i] = WiFi.SSID(i);
        networkList[i] = networks[i].c_str();
      }
      
      networks[n] = "Back";
      networkList[n] = networks[n].c_str();
      int countNetworks = n + 1;
      
      // TODO: Implement numpad-based network selection
      // For now, just show the networks and return
      display.clearDisplay();
      display.drawRoundRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 4, WHITE);
      display.setCursor(8, 7);
      display.println("Networks found:");
      display.setCursor(8, 21);
      display.print("Count: ");
      display.println(n);
      display.setCursor(8, 35);
      display.println("(Numpad nav TODO)");
      display.display();
      delay(3000);
      displayMenu(countWifiOptions, wifiOptions, true);
      
      delete[] networks;
      delete[] networkList;
    }
  }