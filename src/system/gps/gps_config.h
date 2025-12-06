#ifndef GPS_CONFIG_H
#define GPS_CONFIG_H

#include "../gpios.h"
#include <HardwareSerial.h>

// GPS baud rate (common rates: 9600, 4800)
#define GPS_BAUD_RATE 9600

// GPS state
extern bool gpsEnabled;
extern HardwareSerial gpsSerial;

// Function declarations
void gpsSetup();
void enableGPS();
void disableGPS();
bool isGPSEnabled();
bool gpsAvailable();
String readGPSData();

#endif // GPS_CONFIG_H
