#include "notepad_config.h"
#include "system/menu_config.h"
#include "system/display_config.h"
#include "system/gpios.h"
#include "system/keyboard/keyboard_config.h"
#include "system/freertos/keyboard_queue.h"
#include <Arduino.h>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

// Notepad buffer
#define NOTEPAD_BUFFER_SIZE 256
char notepadBuffer[NOTEPAD_BUFFER_SIZE];
int notepadCursor = 0;

// Cursor blink state
static unsigned long lastBlinkTime = 0;
static bool cursorVisible = true;
static const unsigned long BLINK_INTERVAL = 500; // milliseconds

// Case toggle state (false = lowercase, true = uppercase)
static bool uppercaseMode = false;

// Notepad mode flag (extern in menu_config.h)
bool inNotepadMode = false;

// Helper function to find the start of the current line
static int findLineStart(int cursorPos) {
  for (int i = cursorPos - 1; i >= 0; i--) {
    if (notepadBuffer[i] == '\n') {
      return i + 1;
    }
  }
  return 0;
}

// Helper function to find the end of the current line
static int findLineEnd(int cursorPos) {
  int len = strlen(notepadBuffer);
  for (int i = cursorPos; i < len; i++) {
    if (notepadBuffer[i] == '\n') {
      return i;
    }
  }
  return len;
}

// Helper function to get column position within a line (accounting for display wrapping)
static int getColumnPosition(int lineStart, int cursorPos) {
  int column = 0;
  int charsPerLine = (SCREEN_WIDTH - 8) / 6; // Available width divided by char width
  
  for (int i = lineStart; i < cursorPos; i++) {
    if (notepadBuffer[i] == '\n') {
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

void notepadMain() {
  // Handle navigation and exit keys
  const char* key = getKeyFromQueue();
  if (key != nullptr) {
    if (strcmp(key, "A") == 0) {
    Serial.println("[NOTEPAD] A key pressed - returning to main menu");
    inNotepadMode = false;
    inSubmenu = false;
    currentSubmenuList = nullptr;
    currentSubmenuCount = 0;
    mainMenuIndex = 2;
    resetPhoneStyleInput();
    return;
    } else if (strcmp(key, "LEFT") == 0) {
      // Move cursor left
      if (notepadCursor > 0) {
        notepadCursor--;
        Serial.print("[NOTEPAD] Cursor moved left, position: ");
        Serial.println(notepadCursor);
      }
    } else if (strcmp(key, "RIGHT") == 0) {
      // Move cursor right
      int textLen = strlen(notepadBuffer);
      if (notepadCursor < textLen) {
        notepadCursor++;
        Serial.print("[NOTEPAD] Cursor moved right, position: ");
        Serial.println(notepadCursor);
      }
    } else if (strcmp(key, "UP") == 0) {
      // Move cursor up one line
      int lineStart = findLineStart(notepadCursor);
      if (lineStart > 0) {
        // Not at first line, move to previous line
        int prevLineEnd = lineStart - 2; // Position before the newline
        if (prevLineEnd < 0) prevLineEnd = 0;
        int prevLineStart = findLineStart(prevLineEnd);
        
        // Get current column position
        int column = getColumnPosition(lineStart, notepadCursor);
        
        // Find position in previous line at same column
        int newPos = prevLineStart;
        int charsPerLine = (SCREEN_WIDTH - 8) / 6;
        int colCount = 0;
        
        while (newPos < prevLineEnd && colCount < column) {
          if (notepadBuffer[newPos] == '\n') {
            break;
          }
          newPos++;
          colCount++;
          if (colCount >= charsPerLine) {
            colCount = 0; // Wrapped line
          }
        }
        
        notepadCursor = newPos;
        Serial.print("[NOTEPAD] Cursor moved up, position: ");
        Serial.println(notepadCursor);
      }
    } else if (strcmp(key, "DOWN") == 0) {
      // Move cursor down one line
      int lineEnd = findLineEnd(notepadCursor);
      int textLen = strlen(notepadBuffer);
      
      if (lineEnd < textLen) {
        // Not at last line, move to next line
        int nextLineStart = lineEnd + 1; // Position after the newline
        if (nextLineStart >= textLen) {
          // At end of buffer
          notepadCursor = textLen;
        } else {
          int nextLineEnd = findLineEnd(nextLineStart);
          
          // Get current column position
          int lineStart = findLineStart(notepadCursor);
          int column = getColumnPosition(lineStart, notepadCursor);
          
          // Find position in next line at same column
          int newPos = nextLineStart;
          int charsPerLine = (SCREEN_WIDTH - 8) / 6;
          int colCount = 0;
          
          while (newPos < nextLineEnd && colCount < column) {
            if (notepadBuffer[newPos] == '\n') {
              break;
            }
            newPos++;
            colCount++;
            if (colCount >= charsPerLine) {
              colCount = 0; // Wrapped line
            }
          }
          
          notepadCursor = newPos;
        }
        Serial.print("[NOTEPAD] Cursor moved down, position: ");
        Serial.println(notepadCursor);
      } else {
        // At end of last line, move to end of buffer
        notepadCursor = textLen;
        Serial.print("[NOTEPAD] Cursor moved to end, position: ");
        Serial.println(notepadCursor);
      }
    } else if (strcmp(key, "B") == 0) {
      // Backspace - delete character before cursor
      if (notepadCursor > 0) {
        int textLen = strlen(notepadBuffer);
        // Shift all characters after cursor position left by one
        for (int i = notepadCursor - 1; i < textLen; i++) {
          notepadBuffer[i] = notepadBuffer[i + 1];
        }
        notepadCursor--;
        Serial.print("[NOTEPAD] Backspace pressed, cursor position: ");
        Serial.println(notepadCursor);
      }
    } else if (strcmp(key, "C") == 0) {
      // Shift - toggle between uppercase and lowercase
      uppercaseMode = !uppercaseMode;
      Serial.print("[NOTEPAD] Shift toggled, uppercase mode: ");
      Serial.println(uppercaseMode ? "ON" : "OFF");
    } else if (strcmp(key, "D") == 0) {
      // Newline - insert line break at cursor position
      int textLen = strlen(notepadBuffer);
      if (textLen < NOTEPAD_BUFFER_SIZE - 1) {
        // Shift characters to the right
        for (int i = textLen; i >= notepadCursor; i--) {
          notepadBuffer[i + 1] = notepadBuffer[i];
        }
        notepadBuffer[notepadCursor] = '\n';
        notepadCursor++;
        notepadBuffer[textLen + 1] = '\0';
        Serial.println("[NOTEPAD] Newline inserted");
      }
    }
  }
  
  // Handle phone-style text input
  char newChar = getPhoneStyleChar();
  if (newChar != '\0') {
    // Apply case transformation based on shift state
    if (uppercaseMode) {
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
    
    int textLen = strlen(notepadBuffer);
    if (textLen < NOTEPAD_BUFFER_SIZE - 1) {
      // Insert character at cursor position
      // Shift characters to the right
      for (int i = textLen; i >= notepadCursor; i--) {
        notepadBuffer[i + 1] = notepadBuffer[i];
      }
      notepadBuffer[notepadCursor] = newChar;
      notepadCursor++;
      notepadBuffer[textLen + 1] = '\0';
      Serial.print("[NOTEPAD] Inserted: ");
      Serial.print(newChar);
      Serial.print(" at position: ");
      Serial.println(notepadCursor - 1);
    }
  }
  
  // Get preview of character being typed (like old phones)
  char previewChar = getPhoneStylePreview();
  // Apply case transformation to preview
  if (previewChar != '\0') {
    if (uppercaseMode) {
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
  }
  
  // Display notepad - clean and simple
  display.clearDisplay();
  display.drawRoundRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 4, WHITE);
  
  // Display all text with proper newline handling
  int currentX = 4;
  int currentY = 7;
  display.setCursor(currentX, currentY);
  
  // Calculate cursor position while displaying text
  int cursorX = 4;
  int cursorY = 7;
  
  for (int i = 0; i < strlen(notepadBuffer); i++) {
    if (i == notepadCursor) {
      // Store cursor position
      cursorX = currentX;
      cursorY = currentY;
    }
    
    if (notepadBuffer[i] == '\n') {
      // Handle newline - move to next line
      currentY += 8; // Line height for text size 1
      currentX = 4;  // Reset to left margin
      display.setCursor(currentX, currentY);
    } else {
      display.print(notepadBuffer[i]);
      currentX += 6; // Approximate character width
      
      // Handle line wrapping (screen width is 128, margin is 4 on each side)
      if (currentX >= SCREEN_WIDTH - 4) {
        currentY += 8;
        currentX = 4;
        display.setCursor(currentX, currentY);
      }
    }
  }
  
  // If cursor is at the end, position it after the last character
  if (notepadCursor == strlen(notepadBuffer)) {
    cursorX = currentX;
    cursorY = currentY;
  }
  
  // Show preview character if typing (like old phones)
  if (previewChar != '\0') {
    // Show preview character at cursor position (replaces cursor)
    if (cursorX < SCREEN_WIDTH - 4) {
      display.setCursor(cursorX, cursorY);
      display.print(previewChar);
    }
  } else {
    // Blinking cursor when not typing
    unsigned long currentTime = millis();
    if (currentTime - lastBlinkTime > BLINK_INTERVAL) {
      cursorVisible = !cursorVisible;
      lastBlinkTime = currentTime;
    }
    
    if (cursorVisible) {
      // Make sure cursor doesn't go off screen
      if (cursorX < SCREEN_WIDTH - 4) {
        // Draw vertical bar cursor (8 pixels tall for text size 1)
        display.drawFastVLine(cursorX, cursorY, 8, WHITE);
      }
    }
  }
  
  display.display();
}

void notepadSetup() {
  Serial.println("[NOTEPAD] Notepad opened");
  // Initialize notepad buffer if needed
  if (notepadBuffer[0] == '\0') {
    notepadCursor = 0;
  } else {
    // If buffer has content, place cursor at the end
    notepadCursor = strlen(notepadBuffer);
  }
  resetPhoneStyleInput();
  cursorVisible = true;
  lastBlinkTime = millis();
  uppercaseMode = false; // Start in lowercase mode
  inNotepadMode = true;
  // Note: notepadMain() will be called from the main loop
}

