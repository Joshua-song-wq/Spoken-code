#ifndef ONEDIGIT_H
#define ONEDIGIT_H

#include <Arduino.h>

// =====================================================
// SHARED GAME STATE
// =====================================================

// Remaining chances (shared with main game logic)
extern volatile int chancesLeft;

// =====================================================
// FUNCTION DECLARATIONS
// =====================================================

// 1-digit display lifecycle
void onedigitsetup();
void onedigitloop();

#endif