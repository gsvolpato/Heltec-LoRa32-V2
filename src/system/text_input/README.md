# Reusable Text Input System

This directory contains reusable text input and editing components that eliminate code duplication across the project.

## Problem Solved

Previously, text input functionality was duplicated in multiple places:
- **Notepad**: Full multi-line editor with complex cursor navigation
- **WiFi Password**: Simple single-line input with basic features
- **Future SD Card**: Will need filename editing

Each implementation had its own code for:
- Cursor management
- Backspace handling
- Case toggle
- Phone-style input
- Cursor blinking
- Display rendering

## Solution

Two reusable components:

### 1. `text_input` (Single-line)
For simple input fields like passwords, filenames, etc.

**Location**: `src/system/text_input/`

**Features**:
- Single-line text input
- LEFT/RIGHT cursor navigation
- Backspace (A key)
- Case toggle (B key, optional)
- Password mode (hide characters)
- Phone-style text input
- Cursor blinking
- Character preview

**Use Cases**:
- WiFi passwords
- Filename input (SD card)
- Any single-line text field

### 2. `text_editor` (Multi-line)
For full text editing like notepad.

**Location**: `src/system/text_editor/`

**Features**:
- Multi-line text editing
- Full cursor navigation (UP/DOWN/LEFT/RIGHT)
- Backspace (A key)
- Case toggle (B key, optional)
- Newline insertion (D key)
- Phone-style text input
- Cursor blinking
- Character preview
- Line wrapping
- Complex line navigation

**Use Cases**:
- Notepad application
- Multi-line text editing
- Any full text editor

## Architecture

Both components use a configuration/state pattern:

```cpp
// Configuration (what the component needs)
TextInputConfig config = {
  .buffer = buffer,
  .bufferSize = size,
  .maxDisplayWidth = width,
  // ... other settings
};

// State (internal state managed by component)
TextInputState state;

// Initialize
textInputInit(&config, &state);

// Process input (in your main loop)
textInputProcess(&config, &state);

// Render (in your display loop)
textInputRender(&config, &state);
```

## Benefits

1. **No Code Duplication**: Write once, use everywhere
2. **Consistent Behavior**: All text inputs work the same way
3. **Easy to Maintain**: Fix bugs in one place
4. **Easy to Extend**: Add features once, available everywhere
5. **Type Safety**: Configuration struct ensures correct setup
6. **Flexible**: Configure for different use cases (password mode, case toggle, etc.)

## Migration Path

To migrate existing code:

1. **WiFi Password**: Replace `wifiPasswordMain()` with `text_input` component
2. **Notepad**: Replace `notepadMain()` with `text_editor` component
3. **Future SD Card**: Use `text_input` for filename editing

See `USAGE_EXAMPLES.md` for detailed migration examples.

## Files

- `text_input.h` / `text_input.cpp`: Single-line text input component
- `text_editor.h` / `text_editor.cpp`: Multi-line text editor component
- `USAGE_EXAMPLES.md`: Detailed usage examples
- `README.md`: This file
