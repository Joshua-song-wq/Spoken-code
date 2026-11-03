#ifndef JOYSTICK_H
#define JOYSTICK_H

#include <Arduino.h>

// =====================================================
// SHARED JOYSTICK STATE (READ-ONLY EXTERNAL ACCESS)
// =====================================================

extern volatile int joyX;     // Processed X-axis value
extern volatile int joyY;     // Processed Y-axis value
extern volatile int joyBtn;   // Button state

// =====================================================
// FUNCTION DECLARATIONS
// =====================================================

// Main joystick lifecycle
void joysticksetup();
void joystickloop();

// Direction helper (U, D, L, R, or 0)
char getDirection();

// Mask system interaction (8x8 grid reveal)
void revealMaskPixel(int x, int y);

#endif