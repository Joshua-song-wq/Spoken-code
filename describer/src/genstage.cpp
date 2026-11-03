#include <Arduino.h>
#include "genstage.h"
#include "1digit.h"
#include "buzzer.h"
#include "led.h"

// =====================================================
// CONSTANTS
// =====================================================
#define IMG_SIZE 8   // Image size (8x8 grid)

// =====================================================
// GLOBAL STATE VARIABLES
// =====================================================
volatile int currentStage = 1;   // Tracks current game stage

// =====================================================
// STAGE 1 VARIABLES
// =====================================================
String stage1Answer = "";   // Stores generated 4-digit answer
int stage1Index = 0;        // Tracks user progress

// =====================================================
// STAGE 2 VARIABLES
// =====================================================
String stage2Answer = "";   // Stores unique 4-letter answer

// =====================================================
// IMAGE STAGES (STAGE 3 ~ 5)
// =====================================================
int correctIndex = -1;      // Correct option index (0~3)
String currentImage = "";   // Stores generated 8x8 image data

// =====================================================
// SETUP FUNCTION
// =====================================================
void genStageSetup() {
    randomSeed(millis());   // Initialize random seed
}

// =====================================================
// STAGE 1 GENERATION (4 RANDOM DIGITS)
// =====================================================
void generateStage1() {
    const char charset[] = "123456789";

    stage1Answer = "";
    for (int i = 0; i < 4; i++) {
        stage1Answer += charset[random(0, sizeof(charset) - 1)];
    }

    stage1Index = 0;

    // Debug output
    Serial.print("Stage 1 Answer: ");
    Serial.println(stage1Answer);
}

// =====================================================
// STAGE 2 GENERATION (4 UNIQUE LETTERS)
// =====================================================
void generateStage2() {
    const char charset[] = "QWERTYUIOPLKZXVS";

    stage2Answer = "";
    for (int i = 0; i < 4; i++) {
        char c = charset[random(0, sizeof(charset) - 1)];

        // Ensure uniqueness
        if (stage2Answer.indexOf(c) < 0)
            stage2Answer += c;
        else
            i--;  // Retry if duplicate
    }

    // Debug output
    Serial.print("Stage 2 Letters: ");
    Serial.println(stage2Answer);
}

// =====================================================
// IMAGE STAGE GENERATION (STAGES 3 ~ 5)
// =====================================================
void generateImageStage() {
    correctIndex = random(0, 4);  // Select correct option (0~3)
    currentImage = "";

    // Generate 8x8 binary image (40% chance of '1')
    for (int i = 0; i < IMG_SIZE * IMG_SIZE; i++) {
        currentImage += (random(0, 100) < 40) ? '1' : '0';
    }

    // Debug output
    Serial.print("Stage ");
    Serial.print(currentStage);
    Serial.println(" image generated");

    Serial.print("Correct index: ");
    Serial.println(correctIndex);

    Serial.println(currentImage);
}

// =====================================================
// HANDLE INPUT - STAGE 1
// =====================================================
bool handleStage1Input(char key) {
    if (currentStage != 1) return false;

    // Correct key input
    if (key == stage1Answer[stage1Index]) {
        stage1Index++;

        // Stage completed
        if (stage1Index >= stage1Answer.length()) {
            Serial.println("Stage1 PASSED");

            ledCommand = LED_GREEN;
            buzzerCommand = BUZZ_RIGHT;

            delay(1000);
            ledCommand = LED_NONE;

            currentStage = 2;
            generateStage2();
            return true;
        }
    } 
    // Incorrect input
    else {
        Serial.println("Stage1 FAILED - RESET");

        ledCommand = LED_RED;
        buzzerCommand = BUZZ_WRONG;

        delay(1000);
        ledCommand = LED_NONE;

        chancesLeft--;
        generateStage1();
        return false;
    }

    return false;
}

// =====================================================
// HANDLE INPUT - STAGE 2
// =====================================================
bool handleStage2Input(char key) {
    if (currentStage != 2) return false;

    int pos = stage2Answer.indexOf(key);

    // Incorrect input
    if (pos < 0) {
        Serial.println("Stage2 FAILED - RESET");

        ledCommand = LED_RED;
        buzzerCommand = BUZZ_WRONG;

        delay(1000);
        ledCommand = LED_NONE;

        generateStage2();
        chancesLeft--;
        return false;
    }

    // Remove correct letter
    stage2Answer.remove(pos, 1);

    // Stage completed
    if (stage2Answer.length() == 0) {
        Serial.println("Stage2 PASSED");

        ledCommand = LED_GREEN;
        buzzerCommand = BUZZ_RIGHT;

        delay(1000);
        ledCommand = LED_NONE;

        currentStage = 3;
        generateImageStage();
        return true;
    }

    return false;
}