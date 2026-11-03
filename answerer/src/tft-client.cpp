#include <TFT_eSPI.h>
#include "net.h"
#include "tft-client.h"
#include "esp_system.h"

// TFT display instance
TFT_eSPI tft = TFT_eSPI();

// ---------------- UI State ----------------

String images[NUM_OPTIONS];
int currentOption = 0;
int currentStage = 3;
int stageCorrectIndex = -1;
volatile bool uiNeedsUpdate = false;

// ---------------- Draw Image ----------------

void drawImage(const String &img) {

    int size = 20;
    int xOffset = (tft.width() - IMG_SIZE * size) / 2;
    int yOffset = (tft.height() - IMG_SIZE * size) / 2 - 20;

    // Clear drawing area
    tft.fillRect(0, 40, tft.width(), tft.height() - 40, TFT_BLACK);

    // Draw pixel grid
    for (int y = 0; y < IMG_SIZE; y++) {
        for (int x = 0; x < IMG_SIZE; x++) {

            char c = img[y * IMG_SIZE + x];

            tft.fillRect(
                xOffset + x * size,
                yOffset + y * size,
                size,
                size,
                (c == '1') ? TFT_WHITE : TFT_BLACK
            );
        }
    }
}

// ---------------- Draw UI ----------------

void drawUI() {

    tft.fillScreen(TFT_BLACK);
    drawImage(images[currentOption]);

    int btnWidth = 60;
    int btnHeight = 40;
    int spacing = 20;

    int totalWidth = btnWidth * 4 + spacing * 3;
    int startX = (tft.width() - totalWidth) / 2;
    int y = tft.height() - 50;

    // Draw buttons: < OK > R
    tft.fillRect(startX, y, btnWidth, btnHeight, TFT_BLUE);
    tft.fillRect(startX + btnWidth + spacing, y, btnWidth, btnHeight, TFT_GREEN);
    tft.fillRect(startX + (btnWidth + spacing) * 2, y, btnWidth, btnHeight, TFT_BLUE);
    tft.fillRect(startX + (btnWidth + spacing) * 3, y, btnWidth, btnHeight, TFT_RED);

    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(2);

    tft.drawString("<", startX + 20, y + 10);
    tft.drawString("OK", startX + btnWidth + spacing + 15, y + 10);
    tft.drawString(">", startX + (btnWidth + spacing) * 2 + 20, y + 10);
    tft.drawString("R", startX + (btnWidth + spacing) * 3 + 20, y + 10);
}

// ---------------- Generate Fake Options ----------------

void generateFakeOptions(int correctIndex, const String &correctImage) {

    stageCorrectIndex = correctIndex;
    images[correctIndex] = correctImage;

    for (int i = 0; i < NUM_OPTIONS; i++) {

        if (i == correctIndex) continue;

        images[i] = "";

        for (int j = 0; j < IMG_SIZE * IMG_SIZE; j++) {
            images[i] += (random(0, 100) < 40) ? '1' : '0';
        }
    }

    currentOption = 0;
    uiNeedsUpdate = true;
}

// ---------------- Stage Callback ----------------

void onStageData(int stage, int correctIndex, String image) {
    currentStage = stage;
    generateFakeOptions(correctIndex, image);
}

// ---------------- Setup ----------------

void tfttouchsetup() {
    tft.init();
    tft.setRotation(1);

    drawUI();

    // Register callback for stage data
    setStageDataCallback(onStageData);
}

// ---------------- Loop ----------------

void tfttouchloop() {

    uint16_t x, y;

    // Update UI if needed
    if (uiNeedsUpdate) {
        uiNeedsUpdate = false;
        drawUI();
    }

    // Handle touch input
    if (tft.getTouch(&x, &y)) {

        // Align touch with display
        x = tft.width() - x;

        int btnWidth = 60;
        int btnHeight = 40;
        int spacing = 20;

        int totalWidth = btnWidth * 4 + spacing * 3;
        int startX = (tft.width() - totalWidth) / 2;
        int btnY = tft.height() - 50;

        // -------- "<" Button --------
        if (x > startX && x < startX + btnWidth &&
            y > btnY && y < btnY + btnHeight) {

            currentOption = (currentOption - 1 + NUM_OPTIONS) % NUM_OPTIONS;
            drawUI();
            delay(200);
            return;
        }

        // -------- "OK" Button --------
        if (x > startX + btnWidth + spacing &&
            x < startX + btnWidth * 2 + spacing &&
            y > btnY && y < btnY + btnHeight) {

            sendStageAnswer(currentStage, currentOption);
            delay(300);
            return;
        }

        // -------- ">" Button --------
        if (x > startX + (btnWidth + spacing) * 2 &&
            x < startX + (btnWidth + spacing) * 2 + btnWidth &&
            y > btnY && y < btnY + btnHeight) {

            currentOption = (currentOption + 1) % NUM_OPTIONS;
            drawUI();
            delay(200);
            return;
        }

        // -------- "Reset" Button --------
        if (x > startX + (btnWidth + spacing) * 3 &&
            x < startX + (btnWidth + spacing) * 3 + btnWidth &&
            y > btnY && y < btnY + btnHeight) {

            ESP.restart();
            delay(300);
        }
    }
}