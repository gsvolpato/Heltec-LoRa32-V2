#include "text_input.h"
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

void textInputInit(TextInputConfig* config, TextInputState* state) {
  if (config == nullptr || state == nullptr) return;
  
  // Initialize buffer
  if (config->buffer != nullptr && config->bufferSize > 0) {
    config->buffer[0] = '\0';
  }
  
  // Initialize state
  state->cursor = 0;
  state->uppercaseMode = false;
  state->lastBlinkTime = millis();
  state->cursorVisible = true;
  
  // Reset phone-style input
  resetPhoneStyleInput();
}

bool textInputProcess(TextInputConfig* config, TextInputState* state) {
  if (config == nullptr || state == nullptr) return false;
  if (config->buffer == nullptr || config->bufferSize <= 0) return false;
  
  bool modified = false;
  
  // Handle keyboard keys from queue
  const char* key = getKeyFromQueue();
  if (key != nullptr) {
    // Ignore "*" key - let the application handle it for cancel/exit
    if (strcmp(key, "*") == 0) {
      // Don't process "*", but it's already consumed from queue
      // The application should check for "*" before calling this function
      return false;
    }
    if (strcmp(key, "LEFT") == 0) {
      // Move cursor left
      if (state->cursor > 0) {
        state->cursor--;
        modified = true;
      }
    } else if (strcmp(key, "RIGHT") == 0) {
      // Move cursor right
      int textLen = strlen(config->buffer);
      if (state->cursor < textLen) {
        state->cursor++;
        modified = true;
      }
    } else if (strcmp(key, "A") == 0) {
      // A key is ESC/Exit - ignore here, let application handle it
      // Don't process, but it's already consumed from queue
      return false;
    } else if (strcmp(key, "B") == 0) {
      // Backspace - delete character before cursor
      if (state->cursor > 0) {
        int textLen = strlen(config->buffer);
        // Shift all characters after cursor position left by one
        for (int i = state->cursor - 1; i < textLen; i++) {
          config->buffer[i] = config->buffer[i + 1];
        }
        state->cursor--;
        modified = true;
      }
    } else if (strcmp(key, "C") == 0 && config->caseToggleEnabled) {
      // Shift - toggle between uppercase and lowercase
      state->uppercaseMode = !state->uppercaseMode;
      modified = true;
    }
  }
  
  // Handle phone-style text input
  char newChar = getPhoneStyleChar();
  if (newChar != '\0') {
    // Apply case transformation based on shift state
    if (state->uppercaseMode && config->caseToggleEnabled) {
      // Convert to uppercase if in uppercase mode
      if (newChar >= 'a' && newChar <= 'z') {
        newChar = newChar - 'a' + 'A';
      }
    } else {
      // Convert to lowercase if in lowercase mode
      if (newChar >= 'A' && newChar <= 'Z') {
        newChar = newChar - 'A' + 'a';
      }
    }
    
    // Handle newline
    if (newChar == '\n') {
      if (!config->allowNewlines) {
        // Convert newline to space if not allowed
        newChar = ' ';
      }
    }
    
    int textLen = strlen(config->buffer);
    if (textLen < config->bufferSize - 1) {
      // Insert character at cursor position
      // Shift characters to the right
      for (int i = textLen; i >= state->cursor; i--) {
        config->buffer[i + 1] = config->buffer[i];
      }
      config->buffer[state->cursor] = newChar;
      state->cursor++;
      config->buffer[textLen + 1] = '\0';
      modified = true;
    }
  }
  
  return modified;
}

void textInputRender(TextInputConfig* config, TextInputState* state) {
  if (config == nullptr || state == nullptr) return;
  if (config->buffer == nullptr) return;
  
  int currentX = config->startX;
  int currentY = config->startY;
  
  // Display label if provided
  if (config->label != nullptr) {
    display.setCursor(currentX, currentY);
    display.print(config->label);
    // Move to next line for text input
    currentY += 8;
    currentX = config->startX;
  }
  
  // Calculate cursor position while displaying text
  int cursorX = currentX;
  int cursorY = currentY;
  
  // Display text
  display.setCursor(currentX, currentY);
  int textLen = strlen(config->buffer);
  
  for (int i = 0; i < textLen; i++) {
    if (i == state->cursor) {
      // Store cursor position
      cursorX = currentX;
      cursorY = currentY;
    }
    
    char displayChar = config->buffer[i];
    
    // Handle password mode
    if (config->passwordMode && displayChar != '\n') {
      displayChar = '*';
    }
    
    // Handle newline
    if (displayChar == '\n') {
      if (config->allowNewlines) {
        currentY += 8; // Line height
        currentX = config->startX;
        display.setCursor(currentX, currentY);
      } else {
        displayChar = ' '; // Replace with space if newlines not allowed
        display.print(displayChar);
        currentX += 6; // Character width
      }
    } else {
      display.print(displayChar);
      currentX += 6; // Approximate character width
      
      // Handle line wrapping (if not password mode, as passwords are usually single line)
      if (!config->passwordMode && currentX >= config->startX + config->maxDisplayWidth) {
        currentY += 8;
        currentX = config->startX;
        display.setCursor(currentX, currentY);
      }
    }
  }
  
  // If cursor is at the end, position it after the last character
  if (state->cursor == textLen) {
    cursorX = currentX;
    cursorY = currentY;
  }
  
  // Get preview of character being typed (like old phones)
  char previewChar = getPhoneStylePreview();
  if (previewChar != '\0') {
    // Apply case transformation to preview
    if (state->uppercaseMode && config->caseToggleEnabled) {
      if (previewChar >= 'a' && previewChar <= 'z') {
        previewChar = previewChar - 'a' + 'A';
      }
    } else {
      if (previewChar >= 'A' && previewChar <= 'Z') {
        previewChar = previewChar - 'A' + 'a';
      }
    }
    
    // Convert newline preview to space if not allowed
    if (previewChar == '\n' && !config->allowNewlines) {
      previewChar = ' ';
    }
    
    // Show preview character at cursor position (replaces cursor)
    if (cursorX < config->startX + config->maxDisplayWidth) {
      display.setCursor(cursorX, cursorY);
      if (config->passwordMode && previewChar != '\n' && previewChar != ' ') {
        display.print('*');
      } else {
        display.print(previewChar);
      }
    }
  } else {
    // Blinking cursor when not typing
    unsigned long currentTime = millis();
    if (currentTime - state->lastBlinkTime > TextInputState::BLINK_INTERVAL) {
      state->cursorVisible = !state->cursorVisible;
      state->lastBlinkTime = currentTime;
    }
    
    if (state->cursorVisible) {
      // Make sure cursor doesn't go off screen
      if (cursorX < config->startX + config->maxDisplayWidth) {
        // Draw vertical bar cursor (8 pixels tall for text size 1)
        display.drawFastVLine(cursorX, cursorY, 8, WHITE);
      }
    }
  }
}

const char* textInputGetText(TextInputConfig* config) {
  if (config == nullptr || config->buffer == nullptr) return "";
  return config->buffer;
}

void textInputSetText(TextInputConfig* config, const char* text) {
  if (config == nullptr || config->buffer == nullptr || text == nullptr) return;
  
  int len = strlen(text);
  if (len >= config->bufferSize) {
    len = config->bufferSize - 1;
  }
  
  strncpy(config->buffer, text, len);
  config->buffer[len] = '\0';
}

void textInputClear(TextInputConfig* config, TextInputState* state) {
  if (config == nullptr || state == nullptr) return;
  
  if (config->buffer != nullptr) {
    config->buffer[0] = '\0';
  }
  state->cursor = 0;
  state->uppercaseMode = false;
  state->cursorVisible = true;
  state->lastBlinkTime = millis();
  resetPhoneStyleInput();
}

int textInputGetCursor(TextInputState* state) {
  if (state == nullptr) return 0;
  return state->cursor;
}

void textInputSetCursor(TextInputState* state, int position) {
  if (state == nullptr) return;
  state->cursor = position;
}
