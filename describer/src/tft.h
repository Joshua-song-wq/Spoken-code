#ifndef TFT_H
#define TFT_H

#include <Arduino.h>
#include <TFT_eSPI.h>

// =====================================================
// EXTERNAL GAME STATE
// =====================================================

// Current image used by stage rendering (defined in tft.cpp / genstage system)
extern String currentImage;

// =====================================================
// TFT SYSTEM FUNCTIONS
// =====================================================

// Main lifecycle functions
void tftsetup();
void tftloop();

// =====================================================
// IMAGE RENDERING FUNCTIONS
// =====================================================

// Draw raw 8x8 image
void drawImage(String imgStr, int xOffset=0, int yOffset=0, int scale=20);

// Draw current stage image (centered rendering)
void drawCurrentStageImage();

// =====================================================
// GAME FEEDBACK SCREENS
// =====================================================

// Game result screens
void success();          // Stage completion success message
void fail();             // Failure screen
void win();              // Final win screen

// Network status screens
void waitForNetwork();   // Waiting for WiFi/network connection
void connected();        // Connected confirmation screen

#endif