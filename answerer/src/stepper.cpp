#include <Arduino.h>
#include "stepper.h"

// ---------------- Pin Definitions ----------------

#define IN1 19
#define IN2 20
#define IN3 21
#define IN4 47

// ---------------- Shared State ----------------

// Trigger flag for motor movement
volatile bool triggerMotor = false;

// ---------------- Step Sequence ----------------

// Half-step sequence for smoother motion
const uint8_t halfStepSeq[8][4] = {
    {1,0,0,0},
    {1,1,0,0},
    {0,1,0,0},
    {0,1,1,0},
    {0,0,1,0},
    {0,0,1,1},
    {0,0,0,1},
    {1,0,0,1}
};

// Current step index
int stepIndex = 0;

// Track if motor has run (currently unused)
bool hasRun = false;

// ---------------- Motor Control ----------------

/**
 * @brief Perform one step in given direction
 */
void stepMotor(int direction) {
    stepIndex += direction;

    if (stepIndex > 7) stepIndex = 0;
    if (stepIndex < 0) stepIndex = 7;

    digitalWrite(IN1, halfStepSeq[stepIndex][0]);
    digitalWrite(IN2, halfStepSeq[stepIndex][1]);
    digitalWrite(IN3, halfStepSeq[stepIndex][2]);
    digitalWrite(IN4, halfStepSeq[stepIndex][3]);
}

/**
 * @brief Release motor coils (stop holding torque)
 */
void releaseMotor() {
    digitalWrite(IN1, 0);
    digitalWrite(IN2, 0);
    digitalWrite(IN3, 0);
    digitalWrite(IN4, 0);
}

// ---------------- Setup ----------------

void steppersetup() {
    Serial.begin(115200);

    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);

    Serial.println("Stepper starting...");
}

// ---------------- Loop ----------------

void stepperloop() {

    /*
    // Continuous rotation test (disabled)
    stepMotor(1);
    delay(50);
    */

    if (triggerMotor) {

        Serial.println("Motor triggered!");

        // -------- Move Forward --------
        for (int i = 0; i < 1024; i++) {
            stepMotor(1);
            delay(2);
        }

        Serial.println("Holding position...");
        delay(10000); // Wait 10 seconds

        // -------- Move Backward --------
        for (int i = 0; i < 1024; i++) {
            stepMotor(-1);
            delay(2);
        }

        // Release motor
        releaseMotor();

        Serial.println("Motor returned to start.");

        // Reset trigger
        triggerMotor = false;
    }

    delay(10);
}