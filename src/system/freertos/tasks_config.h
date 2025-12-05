#ifndef TASKS_CONFIG_H
#define TASKS_CONFIG_H

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>

// Task priorities
#define TASK_PRIORITY_HIGH     5
#define TASK_PRIORITY_NORMAL   3
#define TASK_PRIORITY_LOW      1

// Task stack sizes
#define TASK_STACK_SIZE_LARGE  4096
#define TASK_STACK_SIZE_MEDIUM 2048
#define TASK_STACK_SIZE_SMALL  1024

// Queue sizes
#define QUEUE_SIZE_KEYPAD      10
#define QUEUE_SIZE_SERIAL      20

// Task handles
extern TaskHandle_t taskKeyboardHandle;
extern TaskHandle_t taskMenuHandle;
extern TaskHandle_t taskSerialHandle;

// Queues
extern QueueHandle_t queueKeypad;
extern QueueHandle_t queueSerial;

// Mutexes
extern SemaphoreHandle_t mutexDisplay;
extern SemaphoreHandle_t mutexSerial;

// Message structures
typedef struct {
  char key[10];  // Store key name as string (UP, DOWN, LEFT, RIGHT, A, B, C, D, or single char)
  unsigned long timestamp;
} KeypadMessage_t;

typedef struct {
  char* message;
  unsigned long timestamp;
} SerialMessage_t;

// Function declarations
void initFreeRTOS();
void createTasks();
void taskKeyboard(void* parameter);
void taskMenu(void* parameter);
void taskSerial(void* parameter);

#endif // TASKS_CONFIG_H

