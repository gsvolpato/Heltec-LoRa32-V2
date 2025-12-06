# Text Input and Text Editor Usage Examples

This document shows how to use the reusable text input and text editor components.

## Text Input (Single-line)

Use `text_input` for simple single-line input like passwords, filenames, etc.

### Example: WiFi Password Input

```cpp
#include "system/text_input/text_input.h"

// Define buffer and configuration
#define PASSWORD_BUFFER_SIZE 64
char passwordBuffer[PASSWORD_BUFFER_SIZE];

TextInputConfig passwordConfig = {
  .buffer = passwordBuffer,
  .bufferSize = PASSWORD_BUFFER_SIZE,
  .maxDisplayWidth = SCREEN_WIDTH - 16,  // Leave margins
  .startX = 8,
  .startY = 35,
  .allowNewlines = false,
  .passwordMode = true,  // Hide characters
  .caseToggleEnabled = false,  // Passwords usually don't need case toggle
  .label = "Password: "
};

TextInputState passwordState;

void setupPasswordInput() {
  textInputInit(&passwordConfig, &passwordState);
}

void passwordInputMain() {
  // Process input
  textInputProcess(&passwordConfig, &passwordState);
  
  // Render
  display.clearDisplay();
  display.drawRoundRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 4, WHITE);
  textInputRender(&passwordConfig, &passwordState);
  display.display();
  
  // Check for enter button to confirm
  if (readEnterButton()) {
    const char* password = textInputGetText(&passwordConfig);
    // Use password...
  }
  
  // Check for cancel (A key = ESC/Exit, * key = also exit)
  const char* key = getKeyFromQueue();
  if (key != nullptr && (strcmp(key, "A") == 0 || strcmp(key, "*") == 0)) {
    // Cancel input
    textInputClear(&passwordConfig, &passwordState);
  }
}
```

### Example: Filename Input

```cpp
#include "system/text_input/text_input.h"

#define FILENAME_BUFFER_SIZE 32
char filenameBuffer[FILENAME_BUFFER_SIZE];

TextInputConfig filenameConfig = {
  .buffer = filenameBuffer,
  .bufferSize = FILENAME_BUFFER_SIZE,
  .maxDisplayWidth = SCREEN_WIDTH - 16,
  .startX = 8,
  .startY = 21,
  .allowNewlines = false,
  .passwordMode = false,
  .caseToggleEnabled = true,  // Allow case toggle for filenames
  .label = "Filename: "
};

TextInputState filenameState;

void setupFilenameInput() {
  textInputInit(&filenameConfig, &filenameState);
}

void filenameInputMain() {
  textInputProcess(&filenameConfig, &filenameState);
  
  display.clearDisplay();
  display.drawRoundRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 4, WHITE);
  textInputRender(&filenameConfig, &filenameState);
  display.display();
}
```

## Text Editor (Multi-line)

Use `text_editor` for full multi-line text editing like notepad.

### Example: Notepad Application

```cpp
#include "system/text_editor/text_editor.h"

#define NOTEPAD_BUFFER_SIZE 256
char notepadBuffer[NOTEPAD_BUFFER_SIZE];

TextEditorConfig notepadConfig = {
  .buffer = notepadBuffer,
  .bufferSize = NOTEPAD_BUFFER_SIZE,
  .maxDisplayWidth = SCREEN_WIDTH - 8,  // Leave small margins
  .startX = 4,
  .startY = 7,
  .caseToggleEnabled = true,
  .label = nullptr,  // No label for notepad
  .charsPerLine = 0  // Auto-calculate
};

TextEditorState notepadState;

void setupNotepad() {
  textEditorInit(&notepadConfig, &notepadState);
}

void notepadMain() {
  // Process input (handles all navigation, editing, etc.)
  textEditorProcess(&notepadConfig, &notepadState);
  
  // Render
  display.clearDisplay();
  display.drawRoundRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 4, WHITE);
  textEditorRender(&notepadConfig, &notepadState);
  display.display();
  
  // Check for exit (A key = ESC/Exit)
  const char* key = getKeyFromQueue();
  if (key != nullptr && strcmp(key, "A") == 0) {
    // Exit notepad
    // Handle exit logic...
  }
}
```

## Key Mappings

Both components use the same key mappings:

- **LEFT/RIGHT**: Move cursor left/right
- **UP/DOWN**: (Text Editor only) Move cursor up/down between lines
- **A**: ESC/Exit/Back (handled by application, not processed by component)
- **B**: Backspace (delete character before cursor)
- **C**: Toggle uppercase/lowercase (if `caseToggleEnabled` is true)
- **D**: (Text Editor only) Insert newline
- **#**: Enter/Confirm (handled by your application logic)
- **\***: Back/Cancel (handled by your application logic, alternative to A)
- **0-9, \*, #**: Phone-style text input

## Features

### Text Input Features:
- Single-line input
- Cursor navigation (LEFT/RIGHT)
- Backspace
- Case toggle (optional)
- Password mode (hide characters)
- Phone-style text input
- Cursor blinking
- Character preview (like old phones)

### Text Editor Features:
- Multi-line input
- Full cursor navigation (UP/DOWN/LEFT/RIGHT)
- Backspace
- Case toggle (optional)
- Newline insertion (D key)
- Phone-style text input
- Cursor blinking
- Character preview
- Line wrapping
- Complex line navigation

## Integration Notes

1. Both components use the keyboard queue system (`getKeyFromQueue()`)
2. Both use phone-style input from `keyboard_config.h`
3. Display should be locked/unlocked by your task (as done in `taskMenu`)
4. Call `textInputInit()` or `textEditorInit()` when entering input mode
5. Call `textInputProcess()` or `textEditorProcess()` in your main loop
6. Call `textInputRender()` or `textEditorRender()` to display
7. Use `textInputGetText()` or `textEditorGetText()` to get the final text
