#include "keyboard_queue.h"
#include <string.h>

// Helper to get key as string
static char keyBuffer[10] = {'\0'};

const char* getKeyFromQueue() {
  KeypadMessage_t keyMsg;
  if (xQueueReceive(queueKeypad, &keyMsg, 0) == pdTRUE) {
    strncpy(keyBuffer, keyMsg.key, 9);
    keyBuffer[9] = '\0';
    return keyBuffer;
  }
  return nullptr;
}

const char* getKeyFromQueueTimeout(TickType_t timeout) {
  KeypadMessage_t keyMsg;
  if (xQueueReceive(queueKeypad, &keyMsg, timeout) == pdTRUE) {
    strncpy(keyBuffer, keyMsg.key, 9);
    keyBuffer[9] = '\0';
    return keyBuffer;
  }
  return nullptr;
}

void clearKeyboardQueue() {
  KeypadMessage_t keyMsg;
  // Remove all items from the queue
  while (xQueueReceive(queueKeypad, &keyMsg, 0) == pdTRUE) {
    // Just discard the messages
  }
}

