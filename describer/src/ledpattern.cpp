#include <Arduino.h>
#include <FastLED.h>
#include "genstage.h"

// =====================================================
// LED MATRIX CONFIGURATION
// =====================================================
#define DATA_PIN 18
#define NUM_LEDS 16
#define BRIGHTNESS 10
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB

CRGB leds[NUM_LEDS];

// =====================================================
// INDEX MAPPING FUNCTION
// Maps 4x4 matrix coordinates to physical LED strip order
// Some rows are reversed to match wiring layout
// =====================================================
int getIndex(int row, int col) {

    // Rows 0 and 2 are normal order (left → right)
    if (row == 0 || row == 2) {
        return row * 4 + col;
    }

    // Rows 1 and 3 are reversed (right → left)
    else {
        return row * 4 + (3 - col);
    }
}

// =====================================================
// STAGE 2 KEY LAYOUT (4x4 GRID)
// =====================================================
const char stage2Keys[4][4] = {
    {'Q','W','E','R'},
    {'T','Y','U','I'},
    {'O','P','L','K'},
    {'Z','X','V','S'}
};

// =====================================================
// SETUP FUNCTION
// Initializes FastLED strip
// =====================================================
void ledpatternsetup() {
    FastLED.setMaxPowerInVoltsAndMilliamps(5, 500);
    FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);
    FastLED.setBrightness(BRIGHTNESS);
}

// =====================================================
// LOOP FUNCTION
// Displays Stage 2 pattern based on remaining keys
// =====================================================
void ledpatternloop() {

    // Only active in Stage 2
    if (currentStage != 2) {
        FastLED.clear();
        FastLED.show();
        vTaskDelay(pdMS_TO_TICKS(100));
        return;
    }

    // Update LED matrix based on remaining answer string
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {

            int idx = getIndex(row, col);
            char key = stage2Keys[row][col];

            // Light up green if key still exists in answer
            if (stage2Answer.indexOf(key) >= 0) {
                leds[idx] = CRGB::Green;
            } else {
                leds[idx] = CRGB::Black;
            }
        }
    }

    FastLED.show();
    vTaskDelay(pdMS_TO_TICKS(100));
}