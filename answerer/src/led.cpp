#include "led.h"

// ---------------- Pin Configuration ----------------

// Red LED pin
const int RED_PIN = 37;

// Green LED pin
const int GREEN_PIN = 38;

// ---------------- Shared Variable ----------------

// Command flag to control LED behavior
volatile int ledCommand = 0;

// ---------------- Setup Function ----------------

void ledsetup() {
    pinMode(RED_PIN, OUTPUT);
    pinMode(GREEN_PIN, OUTPUT);

    // Common-cathode: LOW = OFF
    digitalWrite(RED_PIN, LOW);
    digitalWrite(GREEN_PIN, LOW);
}

// ---------------- Main Loop ----------------

void ledloop() {
    switch (ledCommand) {

        case LED_GREEN:
            digitalWrite(GREEN_PIN, HIGH); // Turn green LED ON
            digitalWrite(RED_PIN, LOW);    // Turn red LED OFF
            break;

        case LED_RED:
            digitalWrite(RED_PIN, HIGH);   // Turn red LED ON
            digitalWrite(GREEN_PIN, LOW);  // Turn green LED OFF
            break;

        case LED_NONE:
        default:
            digitalWrite(RED_PIN, LOW);    // Turn red LED OFF
            digitalWrite(GREEN_PIN, LOW);  // Turn green LED OFF
            break;
    }
}

// ---------------- Trigger Helper ----------------

/**
 * @brief Sets the LED command
 */
void triggerLED(int cmd) {
    ledCommand = cmd;
}