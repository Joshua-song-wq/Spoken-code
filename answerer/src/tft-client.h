#ifndef TFT_CLIENT_H
#define TFT_CLIENT_H

#include <Arduino.h>

// ---------------- UI CONFIGURATION ----------------

#define NUM_OPTIONS 5
#define IMG_SIZE 8

// ---------------- Shared UI State ----------------

extern String images[NUM_OPTIONS];
extern int currentOption;
extern int currentStage;
extern int stageCorrectIndex;
extern volatile bool uiNeedsUpdate;

// ---------------- Function Declarations ----------------

// UI rendering
void drawUI();
void drawImage(const String &img);

// Data handling
void generateFakeOptions(int correctIndex, const String &correctImage);
void onStageData(int stage, int correctIndex, String image);

// TFT touch system
void tfttouchsetup();
void tfttouchloop();

#endif