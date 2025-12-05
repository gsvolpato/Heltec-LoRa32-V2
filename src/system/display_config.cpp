#include "display_config.h"
#include "skull_logo.h"
#include "gpios.h"

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET_PIN);

void startDisplay() {
    display.clearDisplay();
    display.drawRoundRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 4, WHITE);
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
}

void displaySkullLogo() {
    display.clearDisplay();
    display.drawBitmap(0, 0, skull_logo, SCREEN_WIDTH, SCREEN_HEIGHT, SSD1306_WHITE);
    display.display();
}
