#pragma once
#include <Arduino.h>

// =====================================================
// CONSTANTS
// =====================================================

#define IMG_SIZE 8

// =====================================================
// GAME STATE
// =====================================================

extern volatile int currentStage;   // Current stage (1–5)

// =====================================================
// STAGE 1 STATE
// =====================================================

extern String stage1Answer;
extern int stage1Index;

// =====================================================
// STAGE 2 STATE
// =====================================================

extern String stage2Answer;

// =====================================================
// IMAGE STAGES STATE (STAGE 3–5)
// =====================================================

extern int correctIndex;       // Correct option index
extern String currentImage;    // 8x8 image data

// =====================================================
// GAME CONTROL STATE
// =====================================================

extern volatile bool gameEnded;     // True when game is finished
extern bool stagesPassed;           // True when stage 5 is cleared
extern unsigned long lastSecondTick; // Timer tracking

// =====================================================
// FUNCTION DECLARATIONS
// =====================================================

// Initialization
void genStageSetup();

// Stage generators
void generateStage1();
void generateStage2();
void generateImageStage();

// Input handlers
bool handleStage1Input(char key);
bool handleStage2Input(char key);