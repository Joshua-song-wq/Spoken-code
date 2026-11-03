#include <Arduino.h>

// ---------------- Function Declarations ----------------

// Setup functions
void steppersetup();
void tfttouchsetup();
void mcpsetup();
void fourdigitsetup();
void netsetup();
void wifisetup();
void buzzersetup();
void ledsetup();

// Loop functions
void stepperloop();
void tfttouchloop();
void mcploop();
void fourdigitloop();
void netloop();
void wifiloop();
void buzzerloop();
void ledloop();

// ---------------- Task Wrappers ----------------

// Stepper task
void taskStepper(void *pvParameters) {
    steppersetup();
    for (;;) {
        stepperloop();
        vTaskDelay(1);
    }
}

// TFT Touch task
void taskTFTTouch(void *pvParameters) {
    tfttouchsetup();
    for (;;) {
        tfttouchloop();
        vTaskDelay(1);
    }
}

// MCP task
void taskMCP(void *pvParameters) {
    mcpsetup();
    for (;;) {
        mcploop();
        vTaskDelay(1);
    }
}

// 4-digit display task
void taskFourDigit(void *pvParameters) {
    fourdigitsetup();
    for (;;) {
        fourdigitloop();
        vTaskDelay(1);
    }
}

// Network task
void taskNet(void *pvParameters) {
    netsetup();
    for (;;) {
        netloop();
        vTaskDelay(1);
    }
}

// WiFi task
void taskWifi(void *pvParameters) {
    wifisetup();
    for (;;) {
        wifiloop();
        vTaskDelay(1);
    }
}

// Buzzer task
void taskBuzzer(void *pvParameters) {
    buzzersetup();
    for (;;) {
        buzzerloop();
        vTaskDelay(1);
    }
}

// LED task
void taskLed(void *pvParameters) {
    ledsetup();
    for (;;) {
        ledloop();
        vTaskDelay(1);
    }
}

// ---------------- Setup ----------------

void setup() {
    // Create tasks pinned to specific cores

    xTaskCreatePinnedToCore(taskStepper,   "Stepper",   4096, NULL, 1, NULL, 0);
    xTaskCreatePinnedToCore(taskTFTTouch,  "Touch",     8192, NULL, 1, NULL, 1);
    xTaskCreatePinnedToCore(taskMCP,       "MCP",       8192, NULL, 1, NULL, 1);
    xTaskCreatePinnedToCore(taskFourDigit, "FourDigit", 4096, NULL, 1, NULL, 0);
    xTaskCreatePinnedToCore(taskNet,       "Net",       8192, NULL, 1, NULL, 0);
    xTaskCreatePinnedToCore(taskWifi,      "Wifi",      4096, NULL, 1, NULL, 0);
    xTaskCreatePinnedToCore(taskLed,       "LED",       4096, NULL, 1, NULL, 1);
    xTaskCreatePinnedToCore(taskBuzzer,    "Buzzer",    2048, NULL, 1, NULL, 1);
}

// ---------------- Main Loop ----------------

void loop() {
    // Not used (FreeRTOS handles execution)
}