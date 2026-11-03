#ifndef FOURDIGIT_H
#define FOURDIGIT_H

#include <Arduino.h>

// =====================================================
// SHARED GAME STATE
// =====================================================

// Time remaining in game (shared with IoT/game logic)
extern volatile int timeLeft;   // in seconds

// =====================================================
// FUNCTION DECLARATIONS
// =====================================================

// 4-digit display lifecycle
void fourdigitsetup();
void fourdigitloop();

#endif