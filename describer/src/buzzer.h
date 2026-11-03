#ifndef BUZZER_H
#define BUZZER_H

#include <Arduino.h>

// =====================================================
// BUZZER COMMAND DEFINITIONS
// =====================================================

#define BUZZ_NONE  0
#define BUZZ_RIGHT 1
#define BUZZ_WRONG 2

// =====================================================
// SHARED CONTROL VARIABLE
// =====================================================

extern volatile int buzzerCommand;   // Current buzzer action state

// =====================================================
// FUNCTION DECLARATIONS
// =====================================================

// Lifecycle
void buzzersetup();
void buzzerloop();

// Sound effects
void playRight();
void playWrong();

#endif