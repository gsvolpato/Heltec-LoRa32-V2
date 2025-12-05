#include "utils.h"
#include "tasks_config.h"
#include <string.h>
#include <stdio.h>

void sendKeyToQueue(char key) {
  KeypadMessage_t keyMsg;
  keyMsg.key[0] = key;
  keyMsg.key[1] = '\0';
  keyMsg.timestamp = millis();
  
  if (queueKeypad != NULL) {
    xQueueSend(queueKeypad, &keyMsg, 0);
  }
}

void sendSerialMessage(const char* message) {
  if (queueSerial == NULL || message == NULL) {
    return;
  }
  
  SerialMessage_t serialMsg;
  // Allocate memory for the message
  size_t len = strlen(message) + 1;
  serialMsg.message = (char*)malloc(len);
  
  if (serialMsg.message != NULL) {
    strcpy(serialMsg.message, message);
    serialMsg.timestamp = millis();
    
    if (xQueueSend(queueSerial, &serialMsg, 0) != pdTRUE) {
      // Queue full, free memory
      free(serialMsg.message);
    }
  }
}

void lockDisplay() {
  if (mutexDisplay != NULL) {
    xSemaphoreTake(mutexDisplay, portMAX_DELAY);
  }
}

void unlockDisplay() {
  if (mutexDisplay != NULL) {
    xSemaphoreGive(mutexDisplay);
  }
}

void lockSerial() {
  if (mutexSerial != NULL) {
    xSemaphoreTake(mutexSerial, portMAX_DELAY);
  }
}

void unlockSerial() {
  if (mutexSerial != NULL) {
    xSemaphoreGive(mutexSerial);
  }
}

