#ifndef BUZZER_H
#define BUZZER_H

#include <Arduino.h>

// ---------------- Command Constants ----------------

#define BUZZ_NONE  0
#define BUZZ_RIGHT 1
#define BUZZ_WRONG 2

// ---------------- Shared Variable ----------------

// Shared command used by main loop to trigger buzzer actions
extern volatile int buzzerCommand;

// ---------------- Function Declarations ----------------

// Setup buzzer hardware
void buzzersetup();

// Main buzzer loop handler
void buzzerloop();

// Sound effects
void playRight();
void playWrong();

#endif