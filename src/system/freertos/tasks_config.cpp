#include "tasks_config.h"
#include "../gpios.h"
#include "../display_config.h"
#include "../keyboard/keyboard_config.h"
#include "../menu_config.h"
#include "../homescreen_config.h"
#include "utils.h"
#include "keyboard_queue.h"
#include "apps/settings/keyboard_test.h"
#include "apps/notepad/notepad_config.h"
#include "../wifi/wifi_config.h"

// Forward declarations
extern const char** wifiOptions;
extern int countWifiOptions;
extern bool inWiFiPasswordMode;
extern int wifiPasswordNetworkIndex;

// Task handles
TaskHandle_t taskKeyboardHandle = NULL;
TaskHandle_t taskMenuHandle = NULL;
TaskHandle_t taskSerialHandle = NULL;

// Queues
QueueHandle_t queueKeypad = NULL;
QueueHandle_t queueSerial = NULL;

// Mutexes
SemaphoreHandle_t mutexDisplay = NULL;
SemaphoreHandle_t mutexSerial = NULL;

void initFreeRTOS() {
  Serial.println("[FREERTOS] Initializing FreeRTOS...");
  
  // Create queues
  queueKeypad = xQueueCreate(QUEUE_SIZE_KEYPAD, sizeof(KeypadMessage_t));
  queueSerial = xQueueCreate(QUEUE_SIZE_SERIAL, sizeof(SerialMessage_t));
  
  if (queueKeypad == NULL || queueSerial == NULL) {
    Serial.println("[FREERTOS] ERROR: Failed to create queues!");
    return;
  }
  
  // Create mutexes
  mutexDisplay = xSemaphoreCreateMutex();
  mutexSerial = xSemaphoreCreateMutex();
  
  if (mutexDisplay == NULL || mutexSerial == NULL) {
    Serial.println("[FREERTOS] ERROR: Failed to create mutexes!");
    return;
  }
  
  Serial.println("[FREERTOS] FreeRTOS initialized successfully");
}

void createTasks() {
  Serial.println("[FREERTOS] Creating tasks...");
  
  // Task 1: Keyboard Input (Core 0, High Priority)
  xTaskCreatePinnedToCore(
    taskKeyboard,
    "TaskKeyboard",
    TASK_STACK_SIZE_MEDIUM,
    NULL,
    TASK_PRIORITY_HIGH,
    &taskKeyboardHandle,
    0  // Core 0
  );
  
  // Task 2: Menu/UI (Core 0, Normal Priority)
  xTaskCreatePinnedToCore(
    taskMenu,
    "TaskMenu",
    TASK_STACK_SIZE_LARGE,
    NULL,
    TASK_PRIORITY_NORMAL,
    &taskMenuHandle,
    0  // Core 0
  );
  
  // Task 3: Serial/Debug (Core 1, Low Priority)
  xTaskCreatePinnedToCore(
    taskSerial,
    "TaskSerial",
    TASK_STACK_SIZE_SMALL,
    NULL,
    TASK_PRIORITY_LOW,
    &taskSerialHandle,
    1  // Core 1
  );
  
  Serial.println("[FREERTOS] All tasks created");
}

void taskKeyboard(void* parameter) {
  char serialBuf[50];
  snprintf(serialBuf, sizeof(serialBuf), "[TASK] Keyboard task started on Core %d\n", xPortGetCoreID());
  sendSerialMessage(serialBuf);
  
  KeypadMessage_t keyMsg;
  char pressedKey = '\0';
  int pressedRow = -1;
  int pressedCol = -1;
  unsigned long pressStartTime = 0;
  
  // Use centralized key mapping from keyboard_config
  extern const int rowPins[4];
  extern const int colPins[5];
  extern const char* keypadMap[4][5];
  
  const char* pressedKeyStr = nullptr;
  
  while (true) {
    if (pressedKeyStr == nullptr) {
      // Waiting for key press
      for (int col = 0; col < 5; col++) {
        digitalWrite(colPins[col], LOW);
        delayMicroseconds(10);
        
        for (int row = 0; row < 4; row++) {
          if (digitalRead(rowPins[row]) == LOW) {
            vTaskDelay(pdMS_TO_TICKS(20)); // Debounce
            if (digitalRead(rowPins[row]) == LOW) {
              pressedKeyStr = keypadMap[row][col];
              pressedRow = row;
              pressedCol = col;
              pressStartTime = millis();
              break;
            }
          }
        }
        
        digitalWrite(colPins[col], HIGH);
        if (pressedKeyStr != nullptr) break;
      }
    } else {
      // Waiting for key release
      digitalWrite(colPins[pressedCol], LOW);
      delayMicroseconds(10);
      
      if (digitalRead(rowPins[pressedRow]) == HIGH) {
        // Key released
        vTaskDelay(pdMS_TO_TICKS(20)); // Debounce
        if (digitalRead(rowPins[pressedRow]) == HIGH) {
          digitalWrite(colPins[pressedCol], HIGH);
          
          // Send key to queue (store full key name)
          strncpy(keyMsg.key, pressedKeyStr, 9);
          keyMsg.key[9] = '\0';
          keyMsg.timestamp = millis();
          
          if (xQueueSend(queueKeypad, &keyMsg, 0) != pdTRUE) {
            // Queue full
            char errBuf[50];
            snprintf(errBuf, sizeof(errBuf), "[TASK] Keyboard queue full!\n");
            sendSerialMessage(errBuf);
          }
          
          pressedKeyStr = nullptr;
        }
      } else {
        digitalWrite(colPins[pressedCol], HIGH);
      }
    }
    
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void taskMenu(void* parameter) {
  char serialBuf[50];
  snprintf(serialBuf, sizeof(serialBuf), "[TASK] Menu task started on Core %d\n", xPortGetCoreID());
  sendSerialMessage(serialBuf);
  
  while (true) {
    // Handle menu navigation based on current mode
    lockDisplay();
    if (inHomescreenMode) {
      homescreenMain();
    } else if (inKeyboardTestMode) {
      keyboardTestMain();
    } else if (inNotepadMode) {
      notepadMain();
    } else if (inWiFiPasswordMode) {
      extern void wifiPasswordMain(int networkIndex);
      wifiPasswordMain(wifiPasswordNetworkIndex);
    } else {
      // Handle menu navigation - handleMenu will read from queue internally
      if (inSubmenu && currentSubmenuList != nullptr) {
        // For WiFi menu, always use current countWifiOptions to ensure it's up to date
        if (currentSubmenuList == wifiOptions) {
          handleMenu(subMenuIndex, countWifiOptions, currentSubmenuList, true);
        } else {
        handleMenu(subMenuIndex, currentSubmenuCount, currentSubmenuList, true);
        }
      } else {
        handleMenu(mainMenuIndex, countMenu, menuList, false);
      }
    }
    unlockDisplay();
    
    vTaskDelay(pdMS_TO_TICKS(50));
  }
}

void taskSerial(void* parameter) {
  char serialBuf[50];
  snprintf(serialBuf, sizeof(serialBuf), "[TASK] Serial task started on Core %d\n", xPortGetCoreID());
  lockSerial();
  Serial.print(serialBuf);
  unlockSerial();
  
  SerialMessage_t serialMsg;
  
  while (true) {
    // Check for serial messages
    if (xQueueReceive(queueSerial, &serialMsg, pdMS_TO_TICKS(100)) == pdTRUE) {
      // Take serial mutex
      if (xSemaphoreTake(mutexSerial, pdMS_TO_TICKS(1000)) == pdTRUE) {
        if (serialMsg.message != NULL) {
          Serial.print(serialMsg.message);
          free(serialMsg.message); // Free allocated memory
        }
        xSemaphoreGive(mutexSerial);
      }
    }
    
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

