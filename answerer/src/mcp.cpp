#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MCP23X17.h>

// MCP23017 object
Adafruit_MCP23X17 mcp;

// WebSocket function from netclient.cpp
extern void sendKey(char key, int stage);

// ---------------- Key Maps ----------------

// Keypad 1 (Port A)
const char keys1[4][4] = {
    {'1','2','3','A'},
    {'4','5','6','B'},
    {'7','8','9','C'},
    {'*','0','#','D'}
};

// Keypad 2 (Port B)
const char keys2[4][4] = {
    {'Q','W','E','R'},
    {'T','Y','U','I'},
    {'O','P','L','K'},
    {'Z','X','V','S'}
};

// ---------------- Setup ----------------

void mcpsetup() {

    Serial.begin(115200);

    // Custom I2C pins for ESP32-S3
    Wire.begin(11, 12);

    // Initialize MCP23X17
    if (!mcp.begin_I2C(0x20)) {
        Serial.println("MCP23X17 not found!");
        while (1); // Halt if not found
    }

    Serial.println("MCP23X17 Ready");

    // -------- Keypad 1 (Port A) --------

    // Rows (outputs)
    for (int i = 0; i < 4; i++) {
        mcp.pinMode(i, OUTPUT);
        mcp.digitalWrite(i, HIGH);
    }

    // Columns (inputs with pull-up)
    for (int i = 4; i < 8; i++) {
        mcp.pinMode(i, INPUT_PULLUP);
    }

    // -------- Keypad 2 (Port B) --------

    // Rows (outputs)
    for (int i = 8; i < 12; i++) {
        mcp.pinMode(i, OUTPUT);
        mcp.digitalWrite(i, HIGH);
    }

    // Columns (inputs with pull-up)
    for (int i = 12; i < 16; i++) {
        mcp.pinMode(i, INPUT_PULLUP);
    }
}

// ---------------- Loop ----------------

void mcploop() {

    // ===== Scan Keypad 1 =====

    for (int row = 0; row < 4; row++) {

        // Reset all rows HIGH
        for (int i = 0; i < 4; i++) {
            mcp.digitalWrite(i, HIGH);
        }

        // Activate current row
        mcp.digitalWrite(row, LOW);

        // Check columns
        for (int col = 0; col < 4; col++) {

            if (mcp.digitalRead(col + 4) == LOW) {

                char key = keys1[row][col];

                Serial.print("Keypad1: ");
                Serial.println(key);

                sendKey(key, 1);

                delay(200); // Debounce
            }
        }
    }

    // ===== Scan Keypad 2 =====

    for (int row = 0; row < 4; row++) {

        // Reset all rows HIGH
        for (int i = 8; i < 12; i++) {
            mcp.digitalWrite(i, HIGH);
        }

        // Activate current row
        mcp.digitalWrite(row + 8, LOW);

        // Check columns
        for (int col = 0; col < 4; col++) {

            if (mcp.digitalRead(col + 12) == LOW) {

                char key = keys2[3 - row][col];

                Serial.print("Keypad2: ");
                Serial.println(key);

                sendKey(key, 2);

                delay(200); // Debounce
            }
        }
    }
}