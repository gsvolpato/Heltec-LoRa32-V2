#ifndef TEXT_EDITOR_H
#define TEXT_EDITOR_H

#include "../display_config.h"
#include "../keyboard/keyboard_config.h"
#include "../freertos/keyboard_queue.h"
#include "../text_input/text_input.h"
#include <Arduino.h>

// Text editor configuration (extends text input)
struct TextEditorConfig {
  char* buffer;              // Text buffer
  int bufferSize;            // Maximum buffer size
  int maxDisplayWidth;       // Maximum display width in pixels
  int startX;                // Display start X position
  int startY;                // Display start Y position
  bool caseToggleEnabled;    // Enable case toggle with B key
  const char* label;         // Optional label to display
  int charsPerLine;          // Characters per line (for wrapping calculation)
};

// Text editor state (extends text input state)
struct TextEditorState {
  int cursor;                // Current cursor position
  bool uppercaseMode;        // Case mode (false = lowercase, true = uppercase)
  unsigned long lastBlinkTime;
  bool cursorVisible;
  static const unsigned long BLINK_INTERVAL = 500; // milliseconds
};

// Initialize text editor
void textEditorInit(TextEditorConfig* config, TextEditorState* state);

// Process keyboard input and update text buffer
// Returns true if text was modified
bool textEditorProcess(TextEditorConfig* config, TextEditorState* state);

// Render text editor on display
void textEditorRender(TextEditorConfig* config, TextEditorState* state);

// Get current text from buffer
const char* textEditorGetText(TextEditorConfig* config);

// Set text in buffer
void textEditorSetText(TextEditorConfig* config, const char* text);

// Clear text buffer
void textEditorClear(TextEditorConfig* config, TextEditorState* state);

// Get cursor position
int textEditorGetCursor(TextEditorState* state);

// Set cursor position
void textEditorSetCursor(TextEditorState* state, int position);

// Helper functions for line navigation
int textEditorFindLineStart(const char* buffer, int cursorPos);
int textEditorFindLineEnd(const char* buffer, int bufferSize, int cursorPos);
int textEditorGetColumnPosition(const char* buffer, int lineStart, int cursorPos, int charsPerLine);

#endif // TEXT_EDITOR_H
