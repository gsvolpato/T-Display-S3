#define DISABLE_ALL_LIBRARY_WARNINGS
#include <Arduino.h>
#include <TFT_eSPI.h>
#include <FS.h>
#include "StartupSettings.h"
#include "UISettings.h"
#include "DisplayConfig.h"
#include "GPIOS.h"
#include "images.h"
#include "MenuLists.h"

extern TFT_eSPI tft;

void loadUserConfig() {
    if (!SPIFFS.begin(true)) {
        return;
    }
    
    if (SPIFFS.exists("/user_config.json")) {
        fs::File file = SPIFFS.open("/user_config.json", "r");
        if (!file) {
            return;
        }
        
        uint8_t rotation = file.read();
        if (rotation <= 3) {
            tft.setRotation(rotation);
        }
        
        file.read((uint8_t*)&highlightColor, sizeof(highlightColor));
        file.read((uint8_t*)&backgroundColor, sizeof(backgroundColor));
        file.close();
    }
}

void displayStartupLogo() {
    // Save current text datum and color settings
    uint8_t previousDatum = tft.getTextDatum();
    
    // Clear the entire screen
    tft.fillScreen(backgroundColor);

    const uint16_t* currentLogo;
    uint8_t rotation = tft.getRotation();
    int logoWidth, logoHeight;
    
    if (rotation == 1 || rotation == 3) {  // Landscape (90° or 270°)
        currentLogo = skull_170x320_Landscape;
        logoWidth = 320;
        logoHeight = 170;
    } else {  // Portrait (0° or 180°)
        currentLogo = skull_170x320_portrait;
        logoWidth = 170;
        logoHeight = 320;
    }

    // Disable temperature updates temporarily
    bool tempUpdateState = staticItemsDrawn;
    staticItemsDrawn = false;

    // Draw from top-left corner (0,0)
    for (int y = 0; y < logoHeight; y++) {
        for (int x = 0; x < logoWidth; x++) {
            uint16_t pixel = currentLogo[y * logoWidth + x];
            if (pixel != 0) {
                tft.drawPixel(x, y, highlightColor);
            }
        }
    }
    
    delay(2000);
    
    // Restore previous states
    tft.setTextDatum(previousDatum);
    staticItemsDrawn = tempUpdateState;
}

void startUp() {
    setupGPIOPins();
    if (SPIFFS.begin(true)) {
        loadUserConfig();
    }
    tft.begin();
    
    displayStartupLogo();
    mainMenu();
}
