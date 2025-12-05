# Heltec-LoRa32-V2
Heltec LoRa 32 v2 Projects. Each branch a different project.

## Current Branch: lora_pager

This branch implements a menu-driven interface for the Heltec LoRa32 V2 board with a 3x4 matrix numpad for navigation. The system features a skull logo splash screen on startup, followed by a main menu with submenus for WiFi, Bluetooth, LoRa, Infrared, and Settings. All modules (WiFi, Bluetooth, LoRa) are disabled by default. Navigation uses the numpad with key release detection: buttons 2/0 for up/down, 6/# for enter, and 4/* for back. The system includes comprehensive serial feedback for all user actions and menu navigation.