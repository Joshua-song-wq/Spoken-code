#include <Arduino.h>
#include <TM1637Display.h>
#include "4digit.h"

// Pin definitions for TM1637 display
#define CLK 2
#define DIO 1

// Create display object
TM1637Display display(CLK, DIO);

// Display buffer (4 digits)
uint8_t displayData[4];

// Track last shown time (to avoid unnecessary refresh)
int lastShownTime = -1;

// Countdown time in seconds (initialized to 5 minutes)
volatile int timeLeft = 5 * 60;

/**
 * @brief Updates the display buffer with current time (MM:SS)
 */
void updateDisplayBuffer() {
    int minutes = timeLeft / 60;
    int seconds = timeLeft % 60;

    displayData[0] = display.encodeDigit(minutes / 10);             // Tens place (minutes)
    displayData[1] = display.encodeDigit(minutes % 10) | 0x80;       // Ones place (minutes) + colon
    displayData[2] = display.encodeDigit(seconds / 10);              // Tens place (seconds)
    displayData[3] = display.encodeDigit(seconds % 10);              // Ones place (seconds)
}

/**
 * @brief Initializes the 4-digit display
 */
void fourdigitsetup() {
    display.setBrightness(2);       // Set brightness level
    updateDisplayBuffer();          // Prepare initial display data
    display.setSegments(displayData); // Push data to display
}

/**
 * @brief Updates the display only when time changes
 */
void fourdigitloop() {

    // Only update when time actually changes
    if (timeLeft != lastShownTime) {
        lastShownTime = timeLeft;

        updateDisplayBuffer();        // Refresh buffer
        display.setSegments(displayData); // Update display
    }
}