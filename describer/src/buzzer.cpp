#include "buzzer.h"

// =====================================================
// BUZZER PIN CONFIGURATION
// =====================================================
const int BUZZER_PIN = 17;

// =====================================================
// TONE FREQUENCIES
// =====================================================
const int TONE_RIGHT = 1000;  // Frequency for correct action
const int TONE_WRONG = 400;   // Frequency for incorrect action

// =====================================================
// PWM CONFIGURATION (ESP32 LEDC)
// =====================================================
const int BUZZER_CHANNEL = 0;     // PWM channel
const int PWM_RESOLUTION = 8;     // Resolution (bits)
const int PWM_MAX = 127;          // Max duty cycle value

// =====================================================
// SHARED CONTROL VARIABLE
// =====================================================
volatile int buzzerCommand = 0;   // Command trigger for buzzer actions

// =====================================================
// SETUP FUNCTION
// Initializes PWM channel and attaches buzzer pin
// =====================================================
void buzzersetup() {
    pinMode(BUZZER_PIN, OUTPUT);

    // Configure PWM for tone generation
    ledcSetup(BUZZER_CHANNEL, TONE_RIGHT, PWM_RESOLUTION);
    ledcAttachPin(BUZZER_PIN, BUZZER_CHANNEL);
}

// =====================================================
// HELPER FUNCTION
// Plays a tone at a given frequency for a duration
// =====================================================
static void playTone(int frequency, int duration_ms) {
    ledcWriteTone(BUZZER_CHANNEL, frequency); // Set frequency
    ledcWrite(BUZZER_CHANNEL, PWM_MAX);       // Enable output

    delay(duration_ms);                      // Play duration

    ledcWrite(BUZZER_CHANNEL, 0);            // Stop sound
}

// =====================================================
// SOUND PATTERNS
// =====================================================

// Correct action sound
void playRight() {
    playTone(TONE_RIGHT, 200);
}

// Incorrect action sound
void playWrong() {
    playTone(TONE_WRONG, 400);
}

// =====================================================
// LOOP FUNCTION
// Executes buzzer commands based on shared variable
// =====================================================
void buzzerloop() {
    switch (buzzerCommand) {

        case BUZZ_RIGHT:
            playRight();
            buzzerCommand = BUZZ_NONE;  // Reset command
            break;

        case BUZZ_WRONG:
            playWrong();
            buzzerCommand = BUZZ_NONE;  // Reset command
            break;

        default:
            // No action
            break;
    }
}