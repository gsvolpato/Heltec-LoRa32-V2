#ifndef KEYBOARD_QUEUE_H
#define KEYBOARD_QUEUE_H

#include "tasks_config.h"

// Function to get key from queue (non-blocking)
// Returns key name as string (UP, DOWN, LEFT, RIGHT, A, B, C, D, or single char like "1", "*", "#")
// Returns nullptr if no key available
const char* getKeyFromQueue();

// Function to get key from queue (blocking with timeout)
// Returns key name as string or nullptr if timeout
const char* getKeyFromQueueTimeout(TickType_t timeout);

// Function to clear all keys from queue
void clearKeyboardQueue();

#endif // KEYBOARD_QUEUE_H

