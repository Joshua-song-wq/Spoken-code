#include <Arduino.h>
#include <TM1637Display.h>
#include "4digit.h"

// =====================================================
// PIN DEFINITIONS (TM1637 DISPLAY)
// =====================================================
#define CLK 13
#define DIO 14

// =====================================================
// DISPLAY OBJECT
// =====================================================
TM1637Display display(CLK, DIO);

// =====================================================
// GLOBAL VARIABLES
// =====================================================

// Display buffer for 4 digits
uint8_t displayData[4];

// Tracks last displayed time (prevents unnecessary refresh)
int lastShownTime = -1;

// Countdown timer (in seconds) - initialized to 5 minutes
volatile int timeLeft = 5 * 60;

// =====================================================
// UPDATE DISPLAY BUFFER
// Converts timeLeft into MM:SS format
// =====================================================
void updateDisplayBuffer() {
    int minutes = timeLeft / 60;
    int seconds = timeLeft % 60;

    // Encode each digit
    displayData[0] = display.encodeDigit(minutes / 10);
    displayData[1] = display.encodeDigit(minutes % 10) | 0x80; // Add colon (dot)
    displayData[2] = display.encodeDigit(seconds / 10);
    displayData[3] = display.encodeDigit(seconds % 10);
}

// =====================================================
// SETUP FUNCTION
// Initializes display and shows initial time
// =====================================================
void fourdigitsetup() {
    display.setBrightness(2);   // Brightness range: 0–7

    updateDisplayBuffer();      // Prepare initial display data
    display.setSegments(displayData);
}

// =====================================================
// LOOP FUNCTION
// Updates display only when time changes
// =====================================================
void fourdigitloop() {

    // Avoid redundant updates (reduces flicker and CPU usage)
    if (timeLeft != lastShownTime) {
        lastShownTime = timeLeft;

        updateDisplayBuffer();      // Refresh buffer
        display.setSegments(displayData); // Update display
    }
}