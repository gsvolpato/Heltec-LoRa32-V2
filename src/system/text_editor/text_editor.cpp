#include "text_editor.h"
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

// Helper function to find the start of the current line
int textEditorFindLineStart(const char* buffer, int cursorPos) {
  for (int i = cursorPos - 1; i >= 0; i--) {
    if (buffer[i] == '\n') {
      return i + 1;
    }
  }
  return 0;
}

// Helper function to find the end of the current line
int textEditorFindLineEnd(const char* buffer, int bufferSize, int cursorPos) {
  int len = strlen(buffer);
  for (int i = cursorPos; i < len; i++) {
    if (buffer[i] == '\n') {
      return i;
    }
  }
  return len;
}

// Helper function to get column position within a line (accounting for display wrapping)
int textEditorGetColumnPosition(const char* buffer, int lineStart, int cursorPos, int charsPerLine) {
  int column = 0;
  
  for (int i = lineStart; i < cursorPos; i++) {
    if (buffer[i] == '\n') {
      break;
    }
    column++;
    // Account for line wrapping in display
    if (column >= charsPerLine) {
      column = column % charsPerLine;
    }
  }
  return column;
}

void textEditorInit(TextEditorConfig* config, TextEditorState* state) {
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
  
  // Calculate chars per line if not set
  if (config->charsPerLine <= 0) {
    config->charsPerLine = (config->maxDisplayWidth) / 6; // Character width is ~6 pixels
  }
  
  // Reset phone-style input
  resetPhoneStyleInput();
}

bool textEditorProcess(TextEditorConfig* config, TextEditorState* state) {
  if (config == nullptr || state == nullptr) return false;
  if (config->buffer == nullptr || config->bufferSize <= 0) return false;
  
  bool modified = false;
  
  // Handle keyboard keys from queue
  const char* key = getKeyFromQueue();
  if (key != nullptr) {
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
    } else if (strcmp(key, "UP") == 0) {
      // Move cursor up one line
      int lineStart = textEditorFindLineStart(config->buffer, state->cursor);
      if (lineStart > 0) {
        // Not at first line, move to previous line
        int prevLineEnd = lineStart - 2; // Position before the newline
        if (prevLineEnd < 0) prevLineEnd = 0;
        int prevLineStart = textEditorFindLineStart(config->buffer, prevLineEnd);
        
        // Get current column position
        int column = textEditorGetColumnPosition(config->buffer, lineStart, state->cursor, config->charsPerLine);
        
        // Find position in previous line at same column
        int newPos = prevLineStart;
        int colCount = 0;
        
        while (newPos < prevLineEnd && colCount < column) {
          if (config->buffer[newPos] == '\n') {
            break;
          }
          newPos++;
          colCount++;
          if (colCount >= config->charsPerLine) {
            colCount = 0; // Wrapped line
          }
        }
        
        state->cursor = newPos;
        modified = true;
      }
    } else if (strcmp(key, "DOWN") == 0) {
      // Move cursor down one line
      int lineEnd = textEditorFindLineEnd(config->buffer, config->bufferSize, state->cursor);
      int textLen = strlen(config->buffer);
      
      if (lineEnd < textLen) {
        // Not at last line, move to next line
        int nextLineStart = lineEnd + 1; // Position after the newline
        if (nextLineStart >= textLen) {
          // At end of buffer
          state->cursor = textLen;
        } else {
          int nextLineEnd = textEditorFindLineEnd(config->buffer, config->bufferSize, nextLineStart);
          
          // Get current column position
          int lineStart = textEditorFindLineStart(config->buffer, state->cursor);
          int column = textEditorGetColumnPosition(config->buffer, lineStart, state->cursor, config->charsPerLine);
          
          // Find position in next line at same column
          int newPos = nextLineStart;
          int colCount = 0;
          
          while (newPos < nextLineEnd && colCount < column) {
            if (config->buffer[newPos] == '\n') {
              break;
            }
            newPos++;
            colCount++;
            if (colCount >= config->charsPerLine) {
              colCount = 0; // Wrapped line
            }
          }
          
          state->cursor = newPos;
        }
        modified = true;
      } else {
        // At end of last line, move to end of buffer
        state->cursor = textLen;
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
    } else if (strcmp(key, "D") == 0) {
      // Newline - insert line break at cursor position
      int textLen = strlen(config->buffer);
      if (textLen < config->bufferSize - 1) {
        // Shift characters to the right
        for (int i = textLen; i >= state->cursor; i--) {
          config->buffer[i + 1] = config->buffer[i];
        }
        config->buffer[state->cursor] = '\n';
        state->cursor++;
        config->buffer[textLen + 1] = '\0';
        modified = true;
      }
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
    
    // Handle newline from # key (convert to space, use D button for actual newline)
    if (newChar == '\n') {
      newChar = ' ';
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

void textEditorRender(TextEditorConfig* config, TextEditorState* state) {
  if (config == nullptr || state == nullptr) return;
  if (config->buffer == nullptr) return;
  
  int currentX = config->startX;
  int currentY = config->startY;
  
  // Display label if provided
  if (config->label != nullptr) {
    display.setCursor(currentX, currentY);
    display.print(config->label);
    // Move to next line for text editor
    currentY += 8;
    currentX = config->startX;
  }
  
  // Calculate cursor position while displaying text
  int cursorX = config->startX;
  int cursorY = config->startY;
  
  // Display all text with proper newline handling
  display.setCursor(currentX, currentY);
  
  int textLen = strlen(config->buffer);
  for (int i = 0; i < textLen; i++) {
    if (i == state->cursor) {
      // Store cursor position
      cursorX = currentX;
      cursorY = currentY;
    }
    
    if (config->buffer[i] == '\n') {
      // Handle newline - move to next line
      currentY += 8; // Line height for text size 1
      currentX = config->startX;  // Reset to left margin
      display.setCursor(currentX, currentY);
    } else {
      display.print(config->buffer[i]);
      currentX += 6; // Approximate character width
      
      // Handle line wrapping (screen width is 128, margin is config->startX on each side)
      if (currentX >= config->startX + config->maxDisplayWidth) {
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
    
    // Convert newline preview to space
    if (previewChar == '\n') {
      previewChar = ' ';
    }
    
    // Show preview character at cursor position (replaces cursor)
    if (cursorX < config->startX + config->maxDisplayWidth) {
      display.setCursor(cursorX, cursorY);
      display.print(previewChar);
    }
  } else {
    // Blinking cursor when not typing
    unsigned long currentTime = millis();
    if (currentTime - state->lastBlinkTime > TextEditorState::BLINK_INTERVAL) {
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

const char* textEditorGetText(TextEditorConfig* config) {
  if (config == nullptr || config->buffer == nullptr) return "";
  return config->buffer;
}

void textEditorSetText(TextEditorConfig* config, const char* text) {
  if (config == nullptr || config->buffer == nullptr || text == nullptr) return;
  
  int len = strlen(text);
  if (len >= config->bufferSize) {
    len = config->bufferSize - 1;
  }
  
  strncpy(config->buffer, text, len);
  config->buffer[len] = '\0';
}

void textEditorClear(TextEditorConfig* config, TextEditorState* state) {
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

int textEditorGetCursor(TextEditorState* state) {
  if (state == nullptr) return 0;
  return state->cursor;
}

void textEditorSetCursor(TextEditorState* state, int position) {
  if (state == nullptr) return;
  state->cursor = position;
}
