#define DISABLE_ALL_LIBRARY_WARNINGS

#include <Arduino.h>
#include <SPIFFS.h>
#include <TFT_eSPI.h>
#include "SettingsSettings.h"
#include "../UI/UISettings.h"
#include "../UI/DisplayConfig.h"
#include "ColorPicker.h"
#include "../UI/StartupSettings.h"

#ifndef TFT_SLPIN
#define TFT_SLPIN 0x10
#endif

extern TFT_eSPI tft;

const char* orientationItems[] = {
    "0° (Portrait)",
    "90° (Landscape)",
    "180° (Portrait)",
    "270° (Landscape)",
    "Back",
    nullptr
};

uint16_t highlightColor = 0x6B6D;  // Default color (white)
uint16_t backgroundColor = TFT_BLACK; // Default color

void handleOrientationSelection(int selectedOption) {
    if (selectedOption >= 0 && selectedOption <= 3) {
        tft.setRotation(selectedOption);
        staticItems();
        tft.setTextDatum(MC_DATUM);
        tft.drawString("Rotation set to " + String(selectedOption * 90) + "°", SCREEN_WIDTH/2, SCREEN_HEIGHT/2);
        
        if (!SPIFFS.begin(true)) {
            return;
        }
        
        fs::File file = SPIFFS.open("/user_config.json", "w");
        if (!file) {
            return;
        }
        
        file.write(selectedOption);
        file.write((uint8_t*)&highlightColor, sizeof(highlightColor));
        file.write((uint8_t*)&backgroundColor, sizeof(backgroundColor));
        file.close();
        
        delay(1500);
    }
}

void orientationMenu() {
    userInterface("Rotation", orientationItems, handleOrientationSelection);
}

void saveColors() {
    if (!SPIFFS.begin(true)) {
        return;
    }
    
    fs::File file = SPIFFS.open("/user_config.json", "w");
    if (!file) {
        return;
    }
    
    uint8_t rotation = tft.getRotation();
    file.write(rotation);
    file.write((uint8_t*)&highlightColor, sizeof(highlightColor));
    file.write((uint8_t*)&backgroundColor, sizeof(backgroundColor));
    file.write((uint8_t*)&currentBrightness, sizeof(currentBrightness));
    file.close();
}

void loadColors() {
    if (!SPIFFS.begin(true)) {
        return;
    }
    
    if (SPIFFS.exists("/user_config.json")) {
        fs::File file = SPIFFS.open("/user_config.json", "r");
        if (!file) {
            return;
        }
        
        file.seek(1);
        file.read((uint8_t*)&highlightColor, sizeof(highlightColor));
        file.read((uint8_t*)&backgroundColor, sizeof(backgroundColor));
        file.read((uint8_t*)&currentBrightness, sizeof(currentBrightness));
        file.close();
        
        ledcWrite(0, currentBrightness);
    }
}

void uiColorMenu() {
    initColorPicker();
}

void restartDevice() {
    staticItems();
    tft.drawString("Restarting...", SCREEN_WIDTH/2, SCREEN_HEIGHT/2);
    delay(500);
    displayStartupLogo();
    delay(500);
    ESP.restart();
}

void turnOffDevice() {
    staticItems();
    tft.drawString("Turning off...", SCREEN_WIDTH/2, SCREEN_HEIGHT/2);
    delay(1000);
    displayStartupLogo();
    delay(500);
    tft.fillScreen(TFT_BLACK);
    tft.writecommand(TFT_DISPOFF); // Turn off display
    tft.writecommand(0x10);   // 0x10 is the standard command for Sleep In Mode
    
    // Configure wake up on wakeup switch press using the correct pin from GPIOS.h
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_3, 0); // PIN_WAKEUP_SWITCH is GPIO3, wake on LOW
    
    // Enter deep sleep
    esp_deep_sleep_start();
}
