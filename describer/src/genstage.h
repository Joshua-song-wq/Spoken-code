#ifndef GENSTAGE_H
#define GENSTAGE_H

#include <Arduino.h>

// =====================================================
// CONSTANTS
// =====================================================

#define IMG_SIZE 8

// =====================================================
// GAME STATE
// =====================================================

extern volatile int currentStage;   // Current game stage (1–5)

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

extern int correctIndex;       // Correct answer index (0–3)
extern String currentImage;    // 8x8 image encoded as string

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

#endif