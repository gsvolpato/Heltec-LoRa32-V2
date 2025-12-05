#ifndef KEYBOARD_CONFIG_H
#define KEYBOARD_CONFIG_H

#include "../gpios.h"

// Keyboard layout (5x4):
// UP    1 2 3 A
// RIGHT 4 5 6 B
// LEFT  7 8 9 C
// DOWN  * 0 # D

// Key mapping constants (centralized)
#define KEY_UP "UP"
#define KEY_DOWN "DOWN"
#define KEY_LEFT "LEFT"
#define KEY_RIGHT "RIGHT"
#define KEY_BACK "A"      // Back/Delete button
#define KEY_SHIFT "B"     // Caps lock/Shift button
#define KEY_ACTION_C "C"  // Action button C
#define KEY_ACTION_D "D"  // Action button D (mapped to 'X' internally to avoid conflict with DOWN)
#define KEY_EXIT '*'
#define KEY_ENTER '#'

// Phone-style text input functions
char getKeypadKey();
char getKeypadKeyRelease();
void keyboardInit();

// Phone-style text input (like old phones)
// Returns the character to insert, or '\0' if no character ready
// Call this repeatedly in your main loop
char getPhoneStyleChar();

// Get preview of current character being typed (before confirmation)
// Returns the character that would be inserted, or '\0' if no key is being typed
// Use this to show preview like old phones
char getPhoneStylePreview();

// Reset phone-style input state (call when entering/exiting text input mode)
void resetPhoneStyleInput();

// Non-blocking key press check (for detecting key hold)
char checkKeyPressNonBlocking();

// Exported key mapping (centralized)
extern const char* keypadMap[4][5];
extern const int rowPins[4];
extern const int colPins[5];

#endif // KEYBOARD_CONFIG_H

