#include "buzzer.h"

// ---------------- Pin Configuration ----------------

// Buzzer output pin
const int BUZZER_PIN = 14;

// ---------------- Tone Frequencies ----------------

// Frequency for correct answer sound
const int TONE_RIGHT = 1000;

// Frequency for wrong answer sound
const int TONE_WRONG = 400;

// ---------------- PWM Configuration ----------------

// PWM channel used for buzzer
const int BUZZER_CHANNEL = 0;

// PWM resolution (bits)
const int PWM_RESOLUTION = 8;

// Maximum duty cycle value
const int PWM_MAX = 127;

// ---------------- Shared Variable ----------------

// Command flag to control buzzer behavior
volatile int buzzerCommand = 0;

// ---------------- Setup Function ----------------

void buzzersetup() {
    pinMode(BUZZER_PIN, OUTPUT);

    // Initialize PWM for buzzer
    ledcSetup(BUZZER_CHANNEL, TONE_RIGHT, PWM_RESOLUTION);
    ledcAttachPin(BUZZER_PIN, BUZZER_CHANNEL);
}

// ---------------- Tone Helper ----------------

/**
 * @brief Plays a tone at a given frequency for a specified duration
 */
static void playTone(int frequency, int duration_ms) {
    ledcWriteTone(BUZZER_CHANNEL, frequency); // Set frequency
    ledcWrite(BUZZER_CHANNEL, PWM_MAX);       // Enable sound
    delay(duration_ms);                       // Hold tone
    ledcWrite(BUZZER_CHANNEL, 0);             // Stop sound
}

// ---------------- Sound Effects ----------------

/**
 * @brief Plays "correct" sound
 */
void playRight() {
    playTone(TONE_RIGHT, 200);
}

/**
 * @brief Plays "wrong" sound
 */
void playWrong() {
    playTone(TONE_WRONG, 400);
}

// ---------------- Main Loop ----------------

void buzzerloop() {
    switch (buzzerCommand) {

        case BUZZ_RIGHT:
            playRight();
            buzzerCommand = BUZZ_NONE;
            break;

        case BUZZ_WRONG:
            playWrong();
            buzzerCommand = BUZZ_NONE;
            break;

        default:
            // No action
            break;
    }
}