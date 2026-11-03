#include "led.h"

// =====================================================
// PIN DEFINITIONS
// =====================================================
const int RED_PIN = 38;
const int GREEN_PIN = 37;

// =====================================================
// SHARED CONTROL VARIABLE
// =====================================================
volatile int ledCommand = 0;

// =====================================================
// SETUP FUNCTION
// Initializes LED pins and ensures LEDs start OFF
// =====================================================
void ledsetup() {
    pinMode(RED_PIN, OUTPUT);
    pinMode(GREEN_PIN, OUTPUT);

    digitalWrite(RED_PIN, LOW);
    digitalWrite(GREEN_PIN, LOW);
}

// =====================================================
// MAIN LOOP
// Controls LED state based on ledCommand
// =====================================================
void ledloop() {
    switch(ledCommand) {

        case LED_GREEN:
            digitalWrite(GREEN_PIN, HIGH); // Green ON
            digitalWrite(RED_PIN, LOW);    // Red OFF
            break;

        case LED_RED:
            digitalWrite(RED_PIN, HIGH);   // Red ON
            digitalWrite(GREEN_PIN, LOW);  // Green OFF
            break;

        case LED_NONE:
        default:
            digitalWrite(RED_PIN, LOW);    // Both OFF
            digitalWrite(GREEN_PIN, LOW);
            break;
    }
}

// =====================================================
// HELPER FUNCTION
// Sets LED command state
// =====================================================
void triggerLED(int cmd) {
    ledCommand = cmd;
}