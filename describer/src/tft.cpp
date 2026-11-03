#include <Arduino.h>
#include <TFT_eSPI.h>
#include "genstage.h"
#include "joystick.h"

// =====================================================
// CONSTANTS
// =====================================================
#define IMG_SIZE 8   // 8x8 image grid

// =====================================================
// TFT OBJECT
// =====================================================
TFT_eSPI tft = TFT_eSPI();

// =====================================================
// STATE TRACKING VARIABLES
// =====================================================
String lastImage = "";
String lastMask  = "";

// MUST always be 64 chars (8x8 grid)
String mask = "0000000000000000000000000000000000000000000000000000000000000000";

// Cursor position (controlled by joystick)
int cursorX = 0;
int cursorY = 0;

// =====================================================
// UI SCREENS
// =====================================================

// ---------------- SUCCESS SCREEN ----------------
void success() {
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.setTextSize(3);
    tft.setCursor(10, tft.height() / 2 - 20);
    tft.println("Congratulations! But you are not finished yet!");
    tft.println("Get the rfid card key and tap it to win the game!");
}

// ---------------- FAIL SCREEN ----------------
void fail() {
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_RED, TFT_BLACK);
    tft.setTextSize(3);
    tft.setCursor(10, tft.height() / 2 - 20);
    tft.println("One more try!");
}

// ---------------- WIN SCREEN ----------------
void win() {
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_PURPLE, TFT_BLACK);
    tft.setTextSize(3);
    tft.setCursor(10, tft.height() / 2 - 20);
    tft.println("Congratulations! You win the game!");
}

// ---------------- NETWORK SCREENS ----------------
void waitForNetwork() {
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.setTextSize(2);
    tft.setCursor(10, tft.height() / 2 - 10);
    tft.println("Waiting for network...");
}

void connected() {
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_BLUE, TFT_BLACK);
    tft.setTextSize(2);
    tft.setCursor(10, tft.height() / 2 - 10);
    tft.println("Connected!");
}

// =====================================================
// IMAGE RENDERING FUNCTIONS
// =====================================================

// ---------------- DRAW RAW IMAGE ----------------
void drawImage(String imgStr, int xOffset=0, int yOffset=0, int scale=20){
    for(int y=0; y<IMG_SIZE; y++){
        for(int x=0; x<IMG_SIZE; x++){
            int idx = y*IMG_SIZE + x;
            if(idx >= imgStr.length()) return;

            uint16_t color = (imgStr[idx]=='1') ? TFT_WHITE : TFT_BLACK;

            tft.fillRect(xOffset + x*scale, yOffset + y*scale, scale, scale, color);
        }
    }
}

// ---------------- DRAW IMAGE WITH MASK ----------------
void drawImageWithMask(String imgStr, String maskStr, int xOffset=0, int yOffset=0, int scale=20){
    for(int y=0; y<IMG_SIZE; y++){
        for(int x=0; x<IMG_SIZE; x++){
            int idx = y*IMG_SIZE + x;

            if(idx >= imgStr.length() || idx >= maskStr.length()) return;

            bool visible = (maskStr[idx] == '1');
            bool pixel   = (imgStr[idx] == '1');

            uint16_t color;

            if(!visible){
                color = TFT_DARKGREY;
            } else {
                color = pixel ? TFT_WHITE : TFT_BLACK;
            }

            tft.fillRect(xOffset + x*scale, yOffset + y*scale, scale, scale, color);
        }
    }
}

// ---------------- DRAW CURSOR ----------------
void drawCursor(int x, int y, int xOffset, int yOffset, int scale){
    tft.drawRect(
        xOffset + x * scale,
        yOffset + y * scale,
        scale,
        scale,
        TFT_RED
    );
}

// =====================================================
// STAGE RENDERING
// =====================================================

// ---------------- STAGE IMAGE (NORMAL) ----------------
void drawCurrentStageImage(){
    tft.fillScreen(TFT_BLACK);

    int scale = 20;
    int imgWidth = IMG_SIZE * scale;
    int imgHeight = IMG_SIZE * scale;

    int xOff = (tft.width() - imgWidth) / 2;
    int yOff = (tft.height() - imgHeight) / 2;

    drawImage(currentImage, xOff, yOff, scale);
}

// ---------------- STAGE 4 (MASK + CURSOR) ----------------
void drawStage4(){
    tft.fillScreen(TFT_BLACK);

    int scale = 20;
    int imgWidth = IMG_SIZE * scale;
    int imgHeight = IMG_SIZE * scale;

    int xOff = (tft.width() - imgWidth) / 2;
    int yOff = (tft.height() - imgHeight) / 2;

    drawImageWithMask(currentImage, mask, xOff, yOff, scale);
    drawCursor(cursorX, cursorY, xOff, yOff, scale);
}

// ---------------- STAGE 5 (DUAL IMAGE PUZZLE) ----------------
void drawStage5() {

    tft.fillScreen(TFT_BLACK);

    int scale = 20;
    int imgWidth = IMG_SIZE * scale;
    int imgHeight = IMG_SIZE * scale;

    int spacing = 20;
    int totalWidth = imgWidth * 2 + spacing;

    int xStart = (tft.width() - totalWidth) / 2;
    int yOff = (tft.height() - imgHeight) / 2;

    int leftX = xStart;
    int rightX = xStart + imgWidth + spacing;

    String corrupted = "";
    String combined = "";

    // ---------------- GENERATE IMAGES ----------------
    for (int i = 0; i < IMG_SIZE * IMG_SIZE; i++) {

        char orig = currentImage[i];
        char corr = '0';

        if (orig == '0') {
            corr = '1';

            // Random noise removal
            if (random(0, 100) < 30) {
                corr = '0';
            }
        }

        corrupted += corr;

        if (orig == '1' || corr == '1') {
            combined += '1';
        } else {
            combined += '0';
        }
    }

    // ---------------- DRAW BOTH IMAGES ----------------
    drawImage(corrupted, leftX, yOff, scale);
    drawImage(combined, rightX, yOff, scale);
}

// =====================================================
// SETUP FUNCTION
// =====================================================
void tftsetup(){
    tft.init();
    tft.setRotation(3);
    tft.fillScreen(TFT_BLACK);
}

// =====================================================
// MAIN LOOP
// =====================================================
void tftloop(){

    // ---------------- STAGE 4 ----------------
    if(currentStage == 4){
        if(currentImage != lastImage || mask != lastMask){
            drawStage4();
            lastImage = currentImage;
            lastMask  = mask;
        }
    }

    // ---------------- STAGE 5 ----------------
    else if (currentStage == 5) {
        if(currentImage != lastImage){
            drawStage5();
            lastImage = currentImage;
        }
    }

    // ---------------- OTHER STAGES ----------------
    else {
        if(currentImage != lastImage){
            drawCurrentStageImage();
            lastImage = currentImage;
        }
    }

    vTaskDelay(pdMS_TO_TICKS(50));
}