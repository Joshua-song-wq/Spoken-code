#ifndef STEPPER_H
#define STEPPER_H

#include <Arduino.h>

// ---------------- Shared Control Flag ----------------

// Trigger flag used to start motor movement from other tasks
extern volatile bool triggerMotor;

#endif