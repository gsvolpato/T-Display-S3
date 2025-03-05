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
        Serial.println("Spiffs failed to mount");
        return;
    }
    
    if (SPIFFS.exists("/user_config.json")) {
        Serial.println("user_config.json exists");
        fs::File file = SPIFFS.open("/user_config.json", "r");
        if (!file) {
            Serial.println("Failed to open user_config.json");
            return;
        }
        Serial.println("user_config.json opened");
        uint8_t rotation = file.read();
        if (rotation <= 3) {
            tft.setRotation(rotation);
        }
        Serial.println("Rotation set");
        file.read((uint8_t*)&highlightColor, sizeof(highlightColor));
        Serial.println("Highlight color read");
        file.read((uint8_t*)&backgroundColor, sizeof(backgroundColor));
        Serial.println("Background color read");
        file.close();
    }
}

void displayStartupLogo() {
    Serial.println("displayStartupLogo()");
    uint8_t previousDatum = tft.getTextDatum();
    tft.fillScreen(backgroundColor);

    const uint16_t* currentLogo;
    uint8_t rotation = tft.getRotation();
    int logoWidth, logoHeight;
    
    if (rotation == 1 || rotation == 3) {
        Serial.println("Landscape");
        currentLogo = skull_170x320_Landscape;
        logoWidth = 320;
        logoHeight = 170;
    } else {
        Serial.println("Portrait");
        currentLogo = skull_170x320_portrait;
        logoWidth = 170;
        logoHeight = 320;
    }

    bool tempUpdateState = staticItemsDrawn;
    staticItemsDrawn = false;

    for (int y = 0; y < logoHeight; y++) {
        for (int x = 0; x < logoWidth; x++) {
            uint16_t pixel = currentLogo[y * logoWidth + x];
            if (pixel != 0) {
                tft.drawPixel(x, y, highlightColor);
            }
        }
    }
    
    delay(2000);
    tft.setTextDatum(previousDatum);
    staticItemsDrawn = tempUpdateState;
}

void startUp() {
    Serial.println("startUp()");

    setupGPIOPins();
    if (SPIFFS.begin(true)) {
        loadUserConfig();
        Serial.println("User config loaded");
    }
    tft.begin();
    
    displayStartupLogo();
    mainMenu();
}
