#ifndef FOURDIGIT_H
#define FOURDIGIT_H

#include <Arduino.h>

// ---------------- Shared Variables ----------------

// Shared time variable (comes from main game logic)
// Represents remaining time in seconds
extern volatile int timeLeft;

// ---------------- Function Declarations ----------------

// Initialize the 4-digit display
void fourdigitsetup();

// Update the display (called repeatedly in loop)
void fourdigitloop();

#endif