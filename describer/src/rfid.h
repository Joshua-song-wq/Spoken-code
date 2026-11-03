#ifndef RFID_H
#define RFID_H

#include <Arduino.h>
#include <MFRC522.h>

// =====================================================
// PIN CONFIGURATION (RFID MODULE)
// =====================================================
#define SS_PIN    20
#define RST_PIN   35
#define SCK_PIN   21
#define MOSI_PIN  47
#define MISO_PIN  45

// =====================================================
// EXTERNAL CONTROL VARIABLES (BUZZER / LED)
// =====================================================
extern volatile int buzzerCommand;
extern volatile int ledCommand;

// Buzzer states
#define BUZZ_RIGHT 1
#define BUZZ_WRONG 2

// LED states
#define LED_GREEN 1
#define LED_RED   2

// =====================================================
// SHARED RFID STATE VARIABLES
// =====================================================
extern String lastUID;            // Last scanned card UID
extern String lastAccess;         // Last access status message
extern volatile bool rfidUpdated; // UI update flag
extern volatile bool winGame;     // Game win trigger flag

// =====================================================
// FUNCTION DECLARATIONS
// =====================================================
void rfidsetup();
void rfidloop();

#endif