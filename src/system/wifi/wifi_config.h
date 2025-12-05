#ifndef WIFI_CONFIG_H
#define WIFI_CONFIG_H

#include <WiFi.h>
#include <stdint.h>

#define MAX_NETWORKS 20

void wifiSetup();
void displayNetworkInfo(int networkIndex);
void inputWiFiPassword(int networkIndex);
void wifiPasswordMain(int networkIndex);
void disableWiFi();
void enableWiFi();
bool isWiFiEnabled();
void toggleWiFi();
void scanWiFiNetworks();
int getWiFiNetworkCount();
const char* getWiFiNetworkSSID(int index);
int32_t getWiFiNetworkRSSI(int index);
wifi_auth_mode_t getWiFiNetworkEncryption(int index);
bool connectToWiFi(const char* ssid, const char* password);

#endif // WIFI_CONFIG_H

