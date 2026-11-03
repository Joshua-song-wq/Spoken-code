#include <Arduino.h>
#include "genstage.h"
#include "joystick.h"
#include "iot_proto.h"

// =====================================================
// EXTERNAL VARIABLES (FROM OTHER MODULES)
// =====================================================

// Mask system (used for image reveal)
extern String mask;

// Cursor position (used by TFT display)
extern int cursorX;
extern int cursorY;

// =====================================================
// PIN DEFINITIONS
// =====================================================
const int joystickXPin = 4;
const int joystickYPin = 5;
const int buttonPin    = 6;

// =====================================================
// JOYSTICK SETTINGS
// =====================================================
const int deadzone = 50;        // Ignore small movements
const int moveThreshold = 200;  // Minimum movement to register direction
const int moveDelay = 200;      // Delay between moves (ms)

// =====================================================
// INTERNAL STATE VARIABLES
// =====================================================
int centerX = 0;   // Calibrated center X
int centerY = 0;   // Calibrated center Y

volatile int joyX = 0;   // Processed X value
volatile int joyY = 0;   // Processed Y value
volatile int joyBtn = 1; // Button state (1 = not pressed)

unsigned long lastMoveTime = 0; // Movement cooldown timer
bool lastBtnState = 1;          // Previous button state

// =====================================================
// SETUP FUNCTION
// Calibrates joystick center position
// =====================================================
void joysticksetup() {

  pinMode(buttonPin, INPUT_PULLUP);

  long sumX = 0, sumY = 0;
  const int samples = 100;

  // Average multiple readings for stable center calibration
  for (int i = 0; i < samples; i++) {
    sumX += analogRead(joystickXPin);
    sumY += analogRead(joystickYPin);
    delay(5);
  }

  centerX = sumX / samples;
  centerY = sumY / samples;

  Serial.println("Joystick calibrated");
}

// =====================================================
// GET DIRECTION
// Returns: 'U', 'D', 'L', 'R', or 0 (no movement)
// =====================================================
char getDirection() {

  if (joyY > moveThreshold)  return 'D';
  if (joyY < -moveThreshold) return 'U';
  if (joyX > moveThreshold)  return 'R';
  if (joyX < -moveThreshold) return 'L';

  return 0;
}

// =====================================================
// REVEAL MASK PIXEL
// Updates mask to reveal pixel at (x, y)
// =====================================================
void revealMaskPixel(int x, int y){
    int idx = y * 8 + x;

    if(idx >= 0 && idx < mask.length()){
        mask.setCharAt(idx, '1');
    }
}

// =====================================================
// MAIN LOOP
// Handles joystick movement and button actions
// =====================================================
void joystickloop() {

  // ---------------- READ JOYSTICK ----------------
  int rawX = analogRead(joystickXPin) - centerX;
  int rawY = analogRead(joystickYPin) - centerY;

  // Apply deadzone filtering
  joyX = (abs(rawX) < deadzone) ? 0 : rawX;
  joyY = (abs(rawY) < deadzone) ? 0 : rawY;

  joyBtn = digitalRead(buttonPin);

  // ---------------- BUTTON (RESET GAME) ----------------
  if(lastBtnState == 1 && joyBtn == 0) {

      Serial.println("Game ended -> resetting...");
      delay(200);
      ESP.restart();
      return;
  }

  lastBtnState = joyBtn;

  // ---------------- ONLY ACTIVE IN STAGE 4 ----------------
  if(currentStage != 4) return;

  unsigned long now = millis();
  char dir = getDirection();

  // ---------------- MOVEMENT HANDLING ----------------
  if(dir != 0 && (now - lastMoveTime > moveDelay)) {

    lastMoveTime = now;

    // Update cursor position within bounds (0–7)
    if(dir == 'U' && cursorY > 0) cursorY--;
    if(dir == 'D' && cursorY < 7) cursorY++;
    if(dir == 'L' && cursorX > 0) cursorX--;
    if(dir == 'R' && cursorX < 7) cursorX++;

    // Reveal corresponding pixel in mask
    revealMaskPixel(cursorX, cursorY);

    // Debug output
    Serial.printf("Move: %c -> (%d,%d)\n", dir, cursorX, cursorY);
  }
}