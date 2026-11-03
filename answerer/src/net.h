#ifndef NET_H
#define NET_H

#include <Arduino.h>

// ---------------- WebSocket Network ----------------

// Initialize network + WebSocket connection
void netsetup();

// Main network loop handler
void netloop();

// Send keypad input to server
void sendKey(char key, int stage);

// Send selected answer for a stage
void sendStageAnswer(int stage, int idx);

// ---------------- Callback ----------------

// Callback for receiving stage image data (Stage 3~5)
void setStageDataCallback(void (*cb)(int stage, int correctIndex, String image));

#endif