#define DISABLE_ALL_LIBRARY_WARNINGS

#include <Arduino.h>
#include <TFT_eSPI.h>
#include "AboutMenu.h"
#include "../UI/StartupSettings.h"
#include "../UI/UISettings.h"
#include "../UI/DisplayConfig.h"
#include "../UI/GPIOS.h"
#include "../UI/images.h"

extern TFT_eSPI tft;

void displayAboutScreen() {
    staticItems();
    
    const uint16_t* currentAbout;
    uint8_t rotation = tft.getRotation();
    int width, height;
    
    // Set about image based on rotation
    if (rotation == 1 || rotation == 3) {  // Landscape
        currentAbout = about_landscape;
        width = 320;
        height = 170;
    } else {  // Portrait
        currentAbout = about_portrait;
        width = 170;
        height = 320;
    }

    // Draw about image with correct dimensions
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            uint16_t pixel = currentAbout[y * width + x];
            if (pixel != 0) {
                tft.drawPixel(x, y, highlightColor);
            }
        }
        if (y % 10 == 0) yield(); // Allow watchdog timer to reset
    }

    // Non-blocking button check
    bool buttonPressed = false;
    while (!buttonPressed) {
        if (digitalRead(PIN_ENCODER_SWITCH) == LOW) {
            delay(50); // Debounce
            if (digitalRead(PIN_ENCODER_SWITCH) == LOW) {
                buttonPressed = true;
            }
        }
        yield(); // Allow other tasks to run
        delay(10); // Small delay to prevent tight loop
    }
    
    delay(200); // Final debounce
}

void AboutMenu() {
    Serial.println("Entering About Menu");
    displayAboutScreen();
}