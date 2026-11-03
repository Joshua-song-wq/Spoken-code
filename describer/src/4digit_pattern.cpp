#include <Arduino.h>
#include <TM1637Display.h>
#include "genstage.h"

// =====================================================
// PIN DEFINITIONS (TM1637 DISPLAY)
// =====================================================
#define CLK 2
#define DIO 42

// =====================================================
// DISPLAY OBJECT
// =====================================================
TM1637Display display2(CLK, DIO);

// =====================================================
// SETUP FUNCTION
// Initializes display brightness
// =====================================================
void fourdigitpatternsetup() {
  display2.setBrightness(3);  // Brightness range: 0 (min) to 7 (max)
}

// =====================================================
// LOOP FUNCTION
// Displays Stage 1 pattern on 4-digit display
// =====================================================
void fourdigitpatternloop() {

  // Only active during Stage 1
  if (currentStage != 1) {
    display2.clear();  // Clear display when not in Stage 1
    vTaskDelay(pdMS_TO_TICKS(100));
    return;
  }

  uint8_t displayData[4];  // Stores encoded segment data

  // Convert stage1Answer characters into segment data
  for (int i = 0; i < 4; i++) {
    char c = stage1Answer[i];

    // If numeric character (0–9)
    if (c >= '0' && c <= '9') {
      displayData[i] = display2.encodeDigit(c - '0');
    } 
    // If alphabetic character (A–Z)
    else {
      displayData[i] = display2.encodeDigit(10 + (c - 'A'));
    }
  }

  // Update display with generated pattern
  display2.setSegments(displayData);

  // Small delay for task scheduling (FreeRTOS)
  vTaskDelay(pdMS_TO_TICKS(100));
}