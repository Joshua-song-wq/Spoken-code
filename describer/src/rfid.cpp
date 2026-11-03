#include <SPI.h>
#include <MFRC522.h>
#include <Arduino.h>
#include "4digit.h"    // shared timeLeft
#include "1digit.h"    // shared chancesLeft
#include "iot_proto.h"
#include "rfid.h"

// =====================================================
// RFID MODULE OBJECT
// =====================================================
MFRC522 mfrc522(SS_PIN, RST_PIN);

// =====================================================
// AUTHORIZED CARD UID
// =====================================================
const byte AUTH_UID[4] = {0x33, 0xFC, 0x5B, 0x19};

// =====================================================
// SHARED STATE VARIABLES
// =====================================================
String lastUID = "None";           // Last scanned UID string
String lastAccess = "No card";     // Last access status message

volatile bool rfidUpdated = false;  // Flag for UI/update sync
volatile bool winGame = false;      // Game win flag (RFID trigger)

// =====================================================
// SETUP FUNCTION
// Initializes RFID reader and SPI bus
// =====================================================
void rfidsetup() {

    SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);
    mfrc522.PCD_Init();

    Serial.println("RFID Ready - Scan a card...");

    // Read RFID module version for debugging
    byte v = mfrc522.PCD_ReadRegister(mfrc522.VersionReg);
    Serial.print("MFRC522 Version: 0x");
    Serial.println(v, HEX);
}

// =====================================================
// MAIN LOOP
// Handles RFID card detection and validation
// =====================================================
void rfidloop() {

    // Wait for a new card
    if (!mfrc522.PICC_IsNewCardPresent()) return;
    if (!mfrc522.PICC_ReadCardSerial()) return;

    // =====================================================
    // READ UID FROM CARD
    // =====================================================
    String uidStr = "";

    for (byte i = 0; i < mfrc522.uid.size; i++) {

        if (mfrc522.uid.uidByte[i] < 0x10)
            uidStr += "0";

        uidStr += String(mfrc522.uid.uidByte[i], HEX);

        if (i < mfrc522.uid.size - 1)
            uidStr += ":";
    }

    uidStr.toUpperCase();
    lastUID = uidStr;

    // =====================================================
    // CHECK AUTHORIZATION
    // =====================================================
    bool authorized = true;

    for (byte i = 0; i < 4; i++) {
        if (mfrc522.uid.uidByte[i] != AUTH_UID[i]) {
            authorized = false;
            break;
        }
    }

    // =====================================================
    // HANDLE ACCESS RESULT
    // =====================================================
    if (authorized) {

        // Only allow win if game conditions are met
        if (stagesPassed && timeLeft > 0 && chancesLeft > 0) {
            lastAccess = "ACCESS GRANTED - GAME WIN";
            winGame = true;   // trigger win state
        } else {
            lastAccess = "ACCESS DENIED - INCOMPLETE";
        }

    } else {
        lastAccess = "ACCESS DENIED - WRONG CARD";
    }

    // =====================================================
    // UPDATE FLAGS + DEBUG OUTPUT
    // =====================================================
    rfidUpdated = true;

    Serial.print("RFID: ");
    Serial.print(lastUID);
    Serial.print(" -> ");
    Serial.println(lastAccess);

    // Stop communication with card
    mfrc522.PICC_HaltA();
}