#include <Arduino.h>

// =====================================================
// FUNCTION DECLARATIONS (MODULE INTERFACES)
// =====================================================

// Setup functions
void fourdigitsetup();
void iotsetup();
void tftsetup();
void ledsetup();
void joysticksetup();
void rfidsetup();
void onedigitsetup();
void ledpatternsetup();
void fourdigitpatternsetup();
void buzzersetup();

// Loop functions
void fourdigitloop();
void iotloop();
void tftloop();
void ledloop();
void joystickloop();
void rfidloop();
void onedigitloop();
void ledpatternloop();
void fourdigitpatternloop();
void buzzerloop();

// =====================================================
// FREE RTOS TASK WRAPPERS
// Each module runs as a separate task
// =====================================================

// ---------------- 4-DIGIT DISPLAY TASK ----------------
void taskFourDigit(void *pvParameters) {
    fourdigitsetup();
    for (;;) {
        fourdigitloop();
        vTaskDelay(1);  // Yield CPU
    }
}

// ---------------- IOT / WEB TASK ----------------
void taskIot(void *pvParameters) {
    iotsetup();
    for (;;) {
        iotloop();
        vTaskDelay(1);
    }
}

// ---------------- TFT DISPLAY TASK ----------------
void taskTft(void *pvParameters) {
    tftsetup();
    for (;;) {
        tftloop();
        vTaskDelay(1);
    }
}

// ---------------- LED CONTROL TASK ----------------
void taskLed(void *pvParameters) {
    ledsetup();
    for (;;) {
        ledloop();
        vTaskDelay(1);
    }
}

// ---------------- LED MATRIX PATTERN TASK ----------------
void taskLedPattern(void *pvParameters) {
    ledpatternsetup();
    for (;;) {
        ledpatternloop();
        vTaskDelay(1);
    }
}

// ---------------- BUZZER TASK ----------------
void taskBuzzer(void *pvParameters) {
    buzzersetup();
    for (;;) {
        buzzerloop();
        vTaskDelay(1);
    }
}

// ---------------- JOYSTICK TASK ----------------
void taskJoystick(void *pvParameters) {
    joysticksetup();
    for (;;) {
        joystickloop();
        vTaskDelay(1);
    }
}

// ---------------- RFID TASK ----------------
void taskRFID(void *pvParameters) {
    rfidsetup();
    for (;;) {
        rfidloop();
        vTaskDelay(1);
    }
}

// ---------------- 1-DIGIT DISPLAY TASK ----------------
void taskOnedigit(void *pvParameters) {
    onedigitsetup();
    for (;;) {
        onedigitloop();
        vTaskDelay(1);
    }
}

// ---------------- 4-DIGIT PATTERN TASK ----------------
void taskFourDigitPattern(void *pvParameters) {
    fourdigitpatternsetup();
    for (;;) {
        fourdigitpatternloop();
        vTaskDelay(1);
    }
}

// =====================================================
// SYSTEM SETUP (CREATE ALL TASKS)
// =====================================================
void setup() {

    // ---------------- CORE 0 TASKS ----------------
    xTaskCreatePinnedToCore(taskFourDigit, "FourDigit", 4096, NULL, 1, NULL, 0);
    xTaskCreatePinnedToCore(taskLedPattern,"LEDPATTERN",4096, NULL, 1, NULL, 0);
    xTaskCreatePinnedToCore(taskFourDigitPattern,"FOURDIGITPATTERN",4096, NULL, 1, NULL, 0);

    // ---------------- CORE 1 TASKS ----------------
    xTaskCreatePinnedToCore(taskIot, "IOT", 8192, NULL, 1, NULL, 1);
    xTaskCreatePinnedToCore(taskTft, "TFT", 8192, NULL, 1, NULL, 1);
    xTaskCreatePinnedToCore(taskLed, "LED", 4096, NULL, 1, NULL, 1);
    xTaskCreatePinnedToCore(taskJoystick, "JOYSTICK", 4096, NULL, 1, NULL, 1);
    xTaskCreatePinnedToCore(taskRFID, "RFID", 4096, NULL, 1, NULL, 1);
    xTaskCreatePinnedToCore(taskOnedigit, "ONEDIGIT", 4096, NULL, 1, NULL, 1);
    xTaskCreatePinnedToCore(taskBuzzer, "BUZZER", 2048, NULL, 1, NULL, 1);
}

// =====================================================
// MAIN LOOP (NOT USED - FREE RTOS HANDLES EVERYTHING)
// =====================================================
void loop() {
    // Intentionally empty
}