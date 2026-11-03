#ifndef LED_H
#define LED_H

#include <Arduino.h>

// ---------------- Command Definitions ----------------

#define LED_NONE  0
#define LED_GREEN 1
#define LED_RED   2

// ---------------- Shared Variable ----------------

// Shared command used to control LED state
extern volatile int ledCommand;

// ---------------- Function Declarations ----------------

// Initialize LED pins
void ledsetup();

// Main LED loop handler
void ledloop();

// Helper to set LED command
void triggerLED(int cmd);

#endif