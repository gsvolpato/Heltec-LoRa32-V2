#include "keyboard_config.h"
#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <string.h>

// Keypad mapping: [row][col] = key
const char* keypadMap[4][5] = {
  {"UP", "1", "2", "3", "A"},      // Row 1
  {"RIGHT", "4", "5", "6", "B"},   // Row 2
  {"LEFT", "7", "8", "9", "C"},    // Row 3
  {"DOWN", "*", "0", "#", "D"}     // Row 4
};

// Pin arrays (centralized, exported for use in tasks)
const int rowPins[4] = {ROW1_PIN, ROW2_PIN, ROW3_PIN, ROW4_PIN};
const int colPins[5] = {COL1_PIN, COL2_PIN, COL3_PIN, COL4_PIN, COL5_PIN};

// Phone-style text input mapping
// Each key maps to a string of characters that cycle on repeated presses
// Layout matches standard phone keypad:
// 1: _, @, , (comma), 1
// 2: A, B, C, 2
// 3: D, E, F, 3
// 4: G, H, I, 4
// 5: J, K, L, 5
// 6: M, N, O, 6
// 7: P, Q, R, S, 7
// 8: T, U, V, 8
// 9: W, X, Y, Z, 9
// *: _, (, )
// 0: 0, space
// #: #, newline
const char* phoneKeyMap[12] = {
  "_,@1",    // Key '1': underscore, comma, @, 1
  "ABC2",    // Key '2': A, B, C, 2
  "DEF3",    // Key '3': D, E, F, 3
  "GHI4",    // Key '4': G, H, I, 4
  "JKL5",    // Key '5': J, K, L, 5
  "MNO6",    // Key '6': M, N, O, 6
  "PQRS7",   // Key '7': P, Q, R, S, 7
  "TUV8",    // Key '8': T, U, V, 8
  "WXYZ9",   // Key '9': W, X, Y, Z, 9
  "_()",     // Key '*': underscore, open paren, close paren
  "0 ",      // Key '0': 0, space
  "#\n"      // Key '#': #, newline
};

// Phone-style input state
static char lastKey = '\0';
static int pressCount = 0;
static unsigned long lastPressTime = 0;
static const unsigned long INPUT_TIMEOUT = 800; // milliseconds to wait before confirming character

// Forward declaration
static char getCharForKey(char key, int count);

void keyboardInit() {
  // Already initialized in gpios_setup()
}

// Helper function to convert key string to char code
static char getKeyCode(const char* keyStr) {
  if (keyStr == nullptr) return '\0';
  if (strcmp(keyStr, "UP") == 0) return 'U';
  if (strcmp(keyStr, "DOWN") == 0) return 'D';
  if (strcmp(keyStr, "LEFT") == 0) return 'L';
  if (strcmp(keyStr, "RIGHT") == 0) return 'R';
  if (strcmp(keyStr, "A") == 0) return 'A';
  if (strcmp(keyStr, "B") == 0) return 'B';
  if (strcmp(keyStr, "C") == 0) return 'C';
  if (strcmp(keyStr, "D") == 0) return 'X'; // Use 'X' for action button D to avoid conflict with DOWN
  if (strlen(keyStr) == 1) return keyStr[0]; // Single char keys (numbers, symbols)
  return '\0';
}

char getKeypadKey() {
  // Scan each column
  for (int col = 0; col < 5; col++) {
    // Set current column LOW
    digitalWrite(colPins[col], LOW);
    delayMicroseconds(10); // Small delay for settling
    
    // Check each row
    for (int row = 0; row < 4; row++) {
      if (digitalRead(rowPins[row]) == LOW) {
        // Key pressed, wait for debounce
        vTaskDelay(pdMS_TO_TICKS(50)); // FreeRTOS compatible
        // Verify key is still pressed
        if (digitalRead(rowPins[row]) == LOW) {
          // Set column back HIGH
          digitalWrite(colPins[col], HIGH);
          return getKeyCode(keypadMap[row][col]);
        }
      }
    }
    
    // Set column back HIGH
    digitalWrite(colPins[col], HIGH);
  }
  
  return '\0'; // No key pressed
}

char getKeypadKeyRelease() {
  // First, detect when a key is pressed
  char pressedKey = '\0';
  int pressedRow = -1;
  int pressedCol = -1;
  
  // Wait for a key to be pressed
  while (pressedKey == '\0') {
    for (int col = 0; col < 5; col++) {
      digitalWrite(colPins[col], LOW);
      delayMicroseconds(10);
      
      for (int row = 0; row < 4; row++) {
        if (digitalRead(rowPins[row]) == LOW) {
          vTaskDelay(pdMS_TO_TICKS(20)); // Debounce (FreeRTOS compatible)
          if (digitalRead(rowPins[row]) == LOW) {
            pressedKey = getKeyCode(keypadMap[row][col]);
            pressedRow = row;
            pressedCol = col;
            break;
          }
        }
      }
      
      digitalWrite(colPins[col], HIGH);
      if (pressedKey != '\0') break;
    }
    
    if (pressedKey == '\0') {
      vTaskDelay(pdMS_TO_TICKS(10)); // Small delay before next scan (FreeRTOS compatible)
    }
  }
  
  // Now wait for the key to be released
  while (true) {
    digitalWrite(colPins[pressedCol], LOW);
    delayMicroseconds(10);
    
    if (digitalRead(rowPins[pressedRow]) == HIGH) {
      // Key released
      vTaskDelay(pdMS_TO_TICKS(20)); // Debounce (FreeRTOS compatible)
      if (digitalRead(rowPins[pressedRow]) == HIGH) {
        digitalWrite(colPins[pressedCol], HIGH);
        return pressedKey; // Return the key that was released
      }
    }
    
    digitalWrite(colPins[pressedCol], HIGH);
    vTaskDelay(pdMS_TO_TICKS(10)); // Small delay before next check (FreeRTOS compatible)
  }
}

void resetPhoneStyleInput() {
  lastKey = '\0';
  pressCount = 0;
  lastPressTime = 0;
}

// Non-blocking key detection for phone-style input
static char checkKeyPress() {
  // Quick scan for any pressed key
  for (int col = 0; col < 5; col++) {
    digitalWrite(colPins[col], LOW);
    delayMicroseconds(10);
    
    for (int row = 0; row < 4; row++) {
      if (digitalRead(rowPins[row]) == LOW) {
        digitalWrite(colPins[col], HIGH);
        return getKeyCode(keypadMap[row][col]);
      }
    }
    
    digitalWrite(colPins[col], HIGH);
  }
  return '\0';
}

// Non-blocking key press check (exported for use in notepad and other modules)
char checkKeyPressNonBlocking() {
  return checkKeyPress();
}

char getPhoneStyleChar() {
  unsigned long currentTime = millis();
  
  // Check if we have a pending character that timed out
  if (lastKey != '\0' && (currentTime - lastPressTime) > INPUT_TIMEOUT) {
    // Timeout reached, confirm the last character
    char confirmedChar = getCharForKey(lastKey, pressCount);
    resetPhoneStyleInput();
    // Serial output handled by serial task
    return confirmedChar;
  }
  
  // Check for key press (non-blocking)
  char pressedKey = checkKeyPress();
  
  // Ignore navigation and action keys for phone-style input
  // U=UP, D=DOWN, L=LEFT, R=RIGHT, A=Back, B=Shift, C=Action C, X=Action D
  if (pressedKey == 'U' || pressedKey == 'D' || pressedKey == 'L' || pressedKey == 'R' || 
      pressedKey == 'A' || pressedKey == 'B' || pressedKey == 'C' || pressedKey == 'X') {
    return '\0';
  }
  
  if (pressedKey != '\0') {
    // Key is currently pressed - wait for release
    static char lastPressedKey = '\0';
    static unsigned long pressStartTime = 0;
    
    if (pressedKey != lastPressedKey) {
      // New key pressed
      lastPressedKey = pressedKey;
      pressStartTime = currentTime;
    }
    
    // Wait for key release
    while (checkKeyPress() == pressedKey) {
      vTaskDelay(pdMS_TO_TICKS(10)); // FreeRTOS compatible delay
      currentTime = millis();
      
      // Check timeout on pending character
      if (lastKey != '\0' && pressedKey != lastKey && (currentTime - lastPressTime) > INPUT_TIMEOUT) {
        // Previous character timed out while waiting for new key release
        char confirmedChar = getCharForKey(lastKey, pressCount);
        resetPhoneStyleInput();
        lastKey = pressedKey;
        pressCount = 0;
        lastPressTime = currentTime;
        return confirmedChar;
      }
    }
    
    // Key released
    char releasedKey = lastPressedKey;
    lastPressedKey = '\0';
    
    // Serial output handled by serial task
    
    // Map key to index (for phoneKeyMap array)
    int keyIndex = -1;
    if (releasedKey >= '1' && releasedKey <= '9') {
      keyIndex = releasedKey - '1';
    } else if (releasedKey == '*') {
      keyIndex = 9;
    } else if (releasedKey == '0') {
      keyIndex = 10;
    } else if (releasedKey == '#') {
      keyIndex = 11;
    }
    
    if (keyIndex >= 0) {
      // Check if it's the same key as last time
      if (releasedKey == lastKey) {
        // Same key pressed again - cycle to next character
        pressCount++;
        lastPressTime = millis();
        // Serial output handled by serial task
      } else {
        // Different key pressed
        if (lastKey != '\0') {
          // Confirm previous character first
          char confirmedChar = getCharForKey(lastKey, pressCount);
          lastKey = releasedKey;
          pressCount = 0;
          lastPressTime = millis();
          return confirmedChar;
        } else {
          // First key press
          lastKey = releasedKey;
          pressCount = 0;
          lastPressTime = millis();
        }
      }
    } else {
      // Special handling for other keys
      if (lastKey != '\0') {
        // Confirm previous character
        char confirmedChar = getCharForKey(lastKey, pressCount);
        resetPhoneStyleInput();
        return confirmedChar;
      }
    }
  }
  
  return '\0'; // No character ready yet
}

char getPhoneStylePreview() {
  // Return the current character that would be inserted (before confirmation)
  // This allows showing a preview like old phones
  if (lastKey != '\0') {
    return getCharForKey(lastKey, pressCount);
  }
  return '\0';
}

char getCharForKey(char key, int count) {
  int keyIndex = -1;
  if (key >= '1' && key <= '9') {
    keyIndex = key - '1';
  } else if (key == '*') {
    keyIndex = 9;
  } else if (key == '0') {
    keyIndex = 10;
  } else if (key == '#') {
    keyIndex = 11;
  } else {
    return '\0';
  }
  
  if (keyIndex >= 0 && keyIndex < 12) {
    const char* charSet = phoneKeyMap[keyIndex];
    int charSetLen = strlen(charSet);
    if (charSetLen > 0) {
      return charSet[count % charSetLen];
    }
  }
  
  return '\0';
}

