
#include "deepsleep_config.h"
#include "system/display_config.h"
#include "system/skull_logo.h"
#include <Arduino.h>
#include <esp_sleep.h>

void enterDeepSleep() {
    displaySkullLogo();
    delay(200);
    esp_deep_sleep_start();
}
  