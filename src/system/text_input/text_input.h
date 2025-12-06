#ifndef TEXT_INPUT_H
#define TEXT_INPUT_H

#include "../display_config.h"
#include "../keyboard/keyboard_config.h"
#include "../freertos/keyboard_queue.h"
#include <Arduino.h>

// Text input configuration
struct TextInputConfig {
  char* buffer;              // Text buffer
  int bufferSize;            // Maximum buffer size
  int maxDisplayWidth;       // Maximum display width in pixels
  int startX;                // Display start X position
  int startY;                // Display start Y position
  bool allowNewlines;        // Allow newline characters
  bool passwordMode;         // Hide characters (for passwords)
  bool caseToggleEnabled;    // Enable case toggle with B key
  const char* label;         // Optional label to display
};

// Text input state
struct TextInputState {
  int cursor;                // Current cursor position
  bool uppercaseMode;        // Case mode (false = lowercase, true = uppercase)
  unsigned long lastBlinkTime;
  bool cursorVisible;
  static const unsigned long BLINK_INTERVAL = 500; // milliseconds
};

// Initialize text input
void textInputInit(TextInputConfig* config, TextInputState* state);

// Process keyboard input and update text buffer
// Returns true if text was modified
bool textInputProcess(TextInputConfig* config, TextInputState* state);

// Render text input on display
void textInputRender(TextInputConfig* config, TextInputState* state);

// Get current text from buffer
const char* textInputGetText(TextInputConfig* config);

// Set text in buffer
void textInputSetText(TextInputConfig* config, const char* text);

// Clear text buffer
void textInputClear(TextInputConfig* config, TextInputState* state);

// Get cursor position
int textInputGetCursor(TextInputState* state);

// Set cursor position
void textInputSetCursor(TextInputState* state, int position);

#endif // TEXT_INPUT_H
