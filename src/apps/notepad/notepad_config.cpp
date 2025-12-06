#include "notepad_config.h"
#include "system/menu_config.h"
#include "system/display_config.h"
#include "system/gpios.h"
#include "system/keyboard/keyboard_config.h"
#include "system/freertos/keyboard_queue.h"
#include "system/text_editor/text_editor.h"
#include <Arduino.h>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

// Notepad buffer and configuration using text_editor component
#define NOTEPAD_BUFFER_SIZE 256
static char notepadBuffer[NOTEPAD_BUFFER_SIZE];
static TextEditorConfig notepadConfig = {
  .buffer = notepadBuffer,
  .bufferSize = NOTEPAD_BUFFER_SIZE,
  .maxDisplayWidth = SCREEN_WIDTH - 8,
  .startX = 4,
  .startY = 7,
  .caseToggleEnabled = true,
  .label = nullptr,
  .charsPerLine = 0  // Auto-calculate
};
static TextEditorState notepadState;

// Notepad mode flag (extern in menu_config.h)
bool inNotepadMode = false;

void notepadMain() {
  // Handle exit key (A key = ESC/Exit)
  const char* key = getKeyFromQueue();
  if (key != nullptr && strcmp(key, "A") == 0) {
    Serial.println("[NOTEPAD] A key (ESC) pressed - returning to main menu");
    inNotepadMode = false;
    inSubmenu = false;
    currentSubmenuList = nullptr;
    currentSubmenuCount = 0;
    mainMenuIndex = 1; // Notepad is at index 1
    return;
  }
  
  // Process text editing using reusable text_editor component
  // This handles all navigation, editing, cursor movement, etc.
  // B = backspace, C = shift, D = newline
  textEditorProcess(&notepadConfig, &notepadState);
  
  // Display notepad using text_editor component
  display.clearDisplay();
  display.drawRoundRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 4, WHITE);
  textEditorRender(&notepadConfig, &notepadState);
  display.display();
}

void notepadSetup() {
  Serial.println("[NOTEPAD] Notepad opened");
  
  // Initialize text editor component
  textEditorInit(&notepadConfig, &notepadState);
  
  // If buffer has content, place cursor at the end
  if (strlen(notepadBuffer) > 0) {
    textEditorSetCursor(&notepadState, strlen(notepadBuffer));
  }
  
  inNotepadMode = true;
  // Note: notepadMain() will be called from the main loop
}

