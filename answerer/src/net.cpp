#include <Arduino.h>
#include <WebSocketsClient.h>
#include <WiFi.h>
#include "net.h"
#include "stepper.h"
#include "4digit.h"
#include "buzzer.h"
#include "led.h"

// WebSocket client instance
WebSocketsClient webSocket;

// Timer helper (currently unused in logic)
unsigned long lastSecondTick = 0;

// Callback for stage data
void (*stageDataCallback)(int stage, int correctIndex, String image) = nullptr;

// ---------------- WebSocket Event Handler ----------------

void websocketEvent(WStype_t type, uint8_t* payload, size_t length) {
    switch (type) {

        case WStype_CONNECTED:
            Serial.println("Connected to WebSocket server");
            break;

        case WStype_DISCONNECTED:
            Serial.println("Disconnected");
            break;

        case WStype_TEXT: {
            String msg = String((char*)payload);
            Serial.println("WS Received: " + msg);

            // ---------------- Motor Trigger ----------------
            if (msg.indexOf("\"type\":\"motor\"") >= 0) {
                triggerMotor = true;
                Serial.println("Motor triggered from server!");
            }

            // ---------------- Status Update ----------------
            if (msg.indexOf("\"type\":\"status\"") >= 0) {

                int tStart = msg.indexOf("\"timeLeft\":") + 11;
                int tEnd   = msg.indexOf(",", tStart);
                int newTime = msg.substring(tStart, tEnd).toInt();

                timeLeft = newTime;

                Serial.printf("Updated timeLeft from server: %d\n", timeLeft);

                // OPTIONAL: chancesLeft
                int cStart = msg.indexOf("\"chancesLeft\":") + 14;
                int cEnd   = msg.indexOf(",", cStart);
            }

            // ---------------- Stage Result ----------------
            if (msg.indexOf("\"type\":\"stage\"") >= 0) {

                int stage = msg.substring(msg.indexOf("\"stage\":") + 8).toInt();

                bool pass = false;
                if (msg.indexOf("\"pass\":true") >= 0) {
                    pass = true;
                }

                if (pass) {
                    ledCommand = LED_GREEN;
                    buzzerCommand = BUZZ_RIGHT;
                } else {
                    ledCommand = LED_RED;
                    buzzerCommand = BUZZ_WRONG;
                }

                delay(1000);
                ledCommand = LED_NONE;

                Serial.printf("Stage %d result: %s\n", stage, pass ? "PASS" : "FAIL");

                // Trigger motor on stage 5 pass
                if (stage == 5 && pass) {
                    triggerMotor = true;
                    Serial.println("Motor Triggered!");
                }
            }

            // ---------------- Stage Image Data ----------------
            if (msg.indexOf("\"type\":\"stage_data\"") >= 0) {

                int stage = msg.substring(msg.indexOf("\"stage\":") + 8).toInt();
                int correctIndex = msg.substring(msg.indexOf("\"correctIndex\":") + 15).toInt();

                int imgStart = msg.indexOf("\"image\":\"") + 9;
                int imgEnd   = msg.indexOf("\"", imgStart);
                String image = msg.substring(imgStart, imgEnd);

                if (stageDataCallback != nullptr) {
                    stageDataCallback(stage, correctIndex, image);
                }
            }

            break;
        }

        case WStype_ERROR:
            Serial.println("WebSocket error");
            break;

        default:
            break;
    }
}

// ---------------- Setup ----------------

void netsetup() {

    Serial.println("Connecting to WebSocket server...");

    // Wait for WiFi connection
    while (WiFi.status() != WL_CONNECTED) {
        Serial.println("Waiting for WiFi...");
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }

    // Initialize WebSocket
    webSocket.begin("192.168.137.100", 81, "/");
    webSocket.setReconnectInterval(500);
    webSocket.onEvent(websocketEvent);

    Serial.println("WebSocket setup done");
}

// ---------------- Loop ----------------

void netloop() {
    webSocket.loop();
}

// ---------------- Send Key ----------------

void sendKey(char key, int stage) {
    String json = "{\"type\":\"key\",\"stage\":" + String(stage) +
                  ",\"value\":\"" + String(key) + "\"}";
    webSocket.sendTXT(json);
}

// ---------------- Send Stage Answer ----------------

void sendStageAnswer(int stage, int idx) {
    String json = "{\"type\":\"answer\",\"stage\":" + String(stage) +
                  ",\"answer\":" + String(idx) + "}";
    webSocket.sendTXT(json);
}

// ---------------- Callback Setter ----------------

void setStageDataCallback(void (*cb)(int stage, int correctIndex, String image)) {
    stageDataCallback = cb;
}