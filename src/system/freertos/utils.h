#ifndef FREERTOS_UTILS_H
#define FREERTOS_UTILS_H

#include "tasks_config.h"

// Utility functions for FreeRTOS communication
void sendKeyToQueue(char key);
void sendSerialMessage(const char* message);
void lockDisplay();
void unlockDisplay();
void lockSerial();
void unlockSerial();

#endif // FREERTOS_UTILS_H

