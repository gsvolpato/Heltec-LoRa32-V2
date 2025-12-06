# Heltec-LoRa32-V2

Heltec LoRa 32 v2 Projects. Each branch a different project.

## Current Branch: lora_pager

A feature-rich menu-driven interface system for the Heltec LoRa32 V2 board built on FreeRTOS with comprehensive hardware integration.

## System Architecture

- **FreeRTOS-based**: Multi-tasking system with three main tasks (keyboard, menu, serial)
- **Queue-based communication**: Inter-task communication via FreeRTOS queues
- **Mutex-protected resources**: Display and serial access are thread-safe
- **Non-blocking operations**: All I/O operations are non-blocking for responsive UI

## Hardware Components

- **Board**: Heltec LoRa32 V2 (ESP32-based)
- **Display**: SSD1306 OLED (128x64)
- **Input**: 5x4 matrix keypad (20 keys)
- **Communication**: WiFi, Bluetooth, LoRa modules
- **Additional**: Physical enter button (GPIO 0 - Boot button)

## Keypad Layout

```
UP    1 2 3 A
RIGHT 4 5 6 B
LEFT  7 8 9 C
DOWN  * 0 # D
```

### Navigation Controls

- **UP/DOWN**: Navigate menu items
- **LEFT/RIGHT**: Cursor movement in text input
- **# (Enter)**: Select menu item / Confirm input
- **\* (Back)**: Go back / Exit
- **A**: Back/Delete in text input
- **B**: Toggle uppercase/lowercase in text input
- **Physical Enter Button (GPIO 0)**: Enter/Select

## Features

### Homescreen
- Simple outline display
- Accessible from main menu
- Press Enter button to open main menu

### Main Menu
Three main options:
1. **Infrared** - Infrared module interface (placeholder options)
2. **Notepad** - Full-featured text editor
3. **Settings** - System configuration submenu

### Notepad Application
- **Phone-style text input**: Multi-tap input like classic phones
- **256-character buffer**: Text storage with cursor navigation
- **Cursor control**: UP/DOWN for line navigation, LEFT/RIGHT for character navigation
- **Case toggle**: Button B toggles between uppercase and lowercase
- **Visual feedback**: Blinking cursor indicator
- **Text wrapping**: Automatic line wrapping on display

### Settings Submenu
1. **WiFi** - Full WiFi management
2. **Bluetooth** - Bluetooth interface (placeholder options)
3. **LoRa** - LoRa module interface (placeholder options)
4. **Keyboard Test** - Interactive keypad testing utility
5. **Option 1** - Placeholder
6. **Option 2** - Placeholder

### WiFi Module (Fully Implemented)
- **Toggle ON/OFF**: Enable/disable WiFi functionality
- **Network scanning**: Automatic scan when WiFi is enabled (up to 20 networks)
- **Network selection**: Browse and select from scanned networks
- **Password input**: Phone-style text input for WiFi passwords
- **Connection management**: Connect to selected networks with password
- **Network information**: Display SSID, RSSI, and encryption type
- **Dynamic menu**: Menu updates automatically with available networks

### Keyboard Test Utility
- **Real-time key detection**: Displays pressed keys on screen
- **All keys testable**: Tests all 20 keypad keys
- **Visual feedback**: Shows last pressed key with timestamp
- **Exit**: Press A to return to Settings menu

### Deep Sleep
- **Deep sleep function**: Implemented for power saving
- **Skull logo display**: Shows logo before entering deep sleep

### Bluetooth Module
- **Basic setup**: Module structure in place
- **Placeholder options**: Currently shows placeholder menu items

### LoRa Module
- **Basic setup**: Module structure in place
- **Disabled by default**: LoRa is disabled on system startup
- **Placeholder options**: Currently shows placeholder menu items

### Infrared Module
- **Basic setup**: Module structure in place
- **Placeholder options**: Currently shows placeholder menu items

## System Initialization

On startup, the system:
1. Disables WiFi, Bluetooth, and LoRa by default
2. Initializes GPIO pins for keypad and display
3. Initializes I2C for OLED display
4. Displays skull logo splash screen for 3 seconds
5. Initializes keyboard system
6. Sets up FreeRTOS tasks and queues
7. Shows homescreen

## Phone-Style Text Input

The system implements classic phone-style multi-tap text input:
- **Key 1**: `_`, `@`, `,`
- **Key 2**: `A`, `B`, `C`
- **Key 3**: `D`, `E`, `F`
- **Key 4**: `G`, `H`, `I`
- **Key 5**: `J`, `K`, `L`
- **Key 6**: `M`, `N`, `O`
- **Key 7**: `P`, `Q`, `R`, `S`
- **Key 8**: `T`, `U`, `V`
- **Key 9**: `W`, `X`, `Y`, `Z`
- **Key \***: `_`, `(`, `)`
- **Key 0**: `0`, ` ` (space)
- **Key #**: `#`, `\n` (newline)

Characters cycle on repeated presses with 800ms timeout for confirmation.

## Serial Debugging

Comprehensive serial output (115200 baud) for:
- System initialization steps
- Menu navigation actions
- User input events
- WiFi operations
- Task status and core assignments
- Error messages

## Technical Details

- **Framework**: Arduino (PlatformIO)
- **RTOS**: FreeRTOS (1000Hz tick rate)
- **Display Library**: Adafruit SSD1306
- **Graphics Library**: Adafruit GFX
- **Task Priorities**: High (5), Normal (3), Low (1)
- **Stack Sizes**: Large (4096), Medium (2048), Small (1024)

## Build Configuration

- **Platform**: ESP32 (espressif32)
- **Board**: esp32doit-devkit-v1
- **Debug Level**: 3 (CORE_DEBUG_LEVEL)
- **FreeRTOS Frequency**: 1000Hz

## Status Summary

| Module | Status | Notes |
|--------|--------|-------|
| Homescreen | ✅ Implemented | Basic outline display |
| Main Menu | ✅ Implemented | 3 main options |
| Notepad | ✅ Fully Functional | Complete text editor |
| WiFi | ✅ Fully Functional | Complete WiFi management |
| Keyboard Test | ✅ Fully Functional | Interactive testing |
| Deep Sleep | ✅ Implemented | Power saving mode |
| Bluetooth | ⚠️ Placeholder | Structure ready, options pending |
| LoRa | ⚠️ Placeholder | Structure ready, options pending |
| Infrared | ⚠️ Placeholder | Structure ready, options pending |
