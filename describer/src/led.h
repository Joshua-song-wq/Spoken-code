#ifndef LED_H
#define LED_H

#include <Arduino.h>

// =====================================================
// LED COMMAND DEFINITIONS
// =====================================================

#define LED_NONE  0
#define LED_GREEN 1
#define LED_RED   2

// =====================================================
// SHARED CONTROL VARIABLE
// =====================================================

extern volatile int ledCommand;   // Current LED state command

// =====================================================
// FUNCTION DECLARATIONS
// =====================================================

void ledsetup();                  // Initialize LED pins
void ledloop();                   // Update LED state
void triggerLED(int cmd);        // Set LED command

#endif