#include <Arduino.h>
#include "1digit.h"

// =====================================================
// PIN DEFINITIONS (7-SEGMENT DISPLAY)
// =====================================================
#define SEG_A 8
#define SEG_B 3
#define SEG_C 46
#define SEG_D 9
#define SEG_E 10
#define SEG_F 11
#define SEG_G 12

// =====================================================
// DIGIT SEGMENT MAP (COMMON ANODE)
// LOW  = Segment ON
// HIGH = Segment OFF
// Each row represents a digit (0–9)
// Order: A, B, C, D, E, F, G
// =====================================================
const uint8_t digits[10][7] = {
  {LOW, LOW, LOW, LOW, LOW, LOW, HIGH},     // 0
  {HIGH, LOW, LOW, HIGH, HIGH, HIGH, HIGH}, // 1
  {LOW, LOW, HIGH, LOW, LOW, HIGH, LOW},    // 2
  {LOW, LOW, LOW, LOW, HIGH, HIGH, LOW},    // 3
  {HIGH, LOW, LOW, HIGH, HIGH, LOW, LOW},   // 4
  {LOW, HIGH, LOW, LOW, HIGH, LOW, LOW},    // 5
  {LOW, HIGH, LOW, LOW, LOW, LOW, LOW},     // 6
  {LOW, LOW, LOW, HIGH, HIGH, HIGH, HIGH},  // 7
  {LOW, LOW, LOW, LOW, LOW, LOW, LOW},      // 8
  {LOW, LOW, LOW, LOW, HIGH, LOW, LOW}      // 9
};

// =====================================================
// GLOBAL VARIABLES
// =====================================================
int lastDisplayed = -1;       // Stores last shown digit (prevents flicker)
volatile int chancesLeft = 5; // Remaining chances (shared across modules)

// =====================================================
// SETUP FUNCTION
// Initializes all segment pins as OUTPUT
// =====================================================
void onedigitsetup() {
  pinMode(SEG_A, OUTPUT);
  pinMode(SEG_B, OUTPUT);
  pinMode(SEG_C, OUTPUT);
  pinMode(SEG_D, OUTPUT);
  pinMode(SEG_E, OUTPUT);
  pinMode(SEG_F, OUTPUT);
  pinMode(SEG_G, OUTPUT);
}

// =====================================================
// DISPLAY FUNCTION
// Displays a single digit (0–9) on 7-segment
// =====================================================
void displayDigit(uint8_t num) {
  digitalWrite(SEG_A, digits[num][0]);
  digitalWrite(SEG_B, digits[num][1]);
  digitalWrite(SEG_C, digits[num][2]);
  digitalWrite(SEG_D, digits[num][3]);
  digitalWrite(SEG_E, digits[num][4]);
  digitalWrite(SEG_F, digits[num][5]);
  digitalWrite(SEG_G, digits[num][6]);
}

// =====================================================
// LOOP FUNCTION
// Updates display only when chances change
// =====================================================
void onedigitloop() {
  // Prevent unnecessary updates (avoids flickering)
  if (chancesLeft != lastDisplayed) {
    lastDisplayed = chancesLeft;

    // Display only valid single digits (0–9)
    if (chancesLeft >= 0 && chancesLeft <= 9) {
      displayDigit(chancesLeft);
    }
  }
}