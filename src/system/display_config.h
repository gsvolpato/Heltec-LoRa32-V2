#ifndef DISPLAY_CONFIG_H
#define DISPLAY_CONFIG_H

#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include "gpios.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

extern Adafruit_SSD1306 display;

void startDisplay();
void displaySkullLogo();

#endif // DISPLAY_CONFIG_H