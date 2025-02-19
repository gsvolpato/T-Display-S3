#define DISABLE_ALL_LIBRARY_WARNINGS

#include <Arduino.h>
#include <TFT_eSPI.h>
#include "SettingsMenu.h"
#include "../UI/StartupSettings.h"
#include "../UI/UISettings.h"
#include "../UI/DisplayConfig.h"
#include "../UI/GPIOS.h"

extern TFT_eSPI tft;

const char* settingsMenuItems[] = {
    "Restart",
    "Turn Off", 
    "UI Color",
    "Orientation",
    "Clock",
    "Startup Logo",
    "Startup WiFi",
    "Sound",
    "About",
    "Back",
    nullptr // Null-terminated to count items
};

void handleSettingsMenuSelection(int selectedOption) {
    
    tft.setTextDatum(MC_DATUM);

    switch (selectedOption) {
        case 0: // Restart
            restartDevice();
            break;
        case 1: // Turn Off
            turnOffDevice();
            break;
        case 2: // UI Color
            uiColorMenu();
            break;
        case 3: // Orientation
            orientationMenu();
            break;
        case 4: // Clock
            staticItems();
            tft.drawString("Clock selected", SCREEN_WIDTH/2, SCREEN_HEIGHT / 2);
            Serial.println("Clock selected");
            break;
        case 5: // Startup Logo
            displayStartupLogo();
            backOption();
            break;
        case 6: // Startup WiFi
            staticItems();
            tft.drawString("Startup WiFi selected", SCREEN_WIDTH/2, SCREEN_HEIGHT / 2);
            Serial.println("Startup WiFi selected");
            delay(1000);
            break;
            backOption();
        case 7: // Sound
            staticItems();
            tft.drawString("Sound selected", SCREEN_WIDTH/2, SCREEN_HEIGHT / 2);
            Serial.println("Sound selected");
            delay(1000);
            backOption();
            break;
        case 8: // About
            AboutMenu();
            break;
        case 9: // Back
            backOption();
            break;
        default:
            staticItems();
            tft.drawString("Invalid settings menu selection", SCREEN_WIDTH/2, SCREEN_HEIGHT / 2);
            Serial.println("Invalid settings menu selection");
            delay(1000);
            backOption();
            break;
    }
}

void SettingsMenu() {
    Serial.println("Parsed Settings Menu Items:");
    for (int i = 0; settingsMenuItems[i] != nullptr; i++) {
        Serial.println(settingsMenuItems[i]);
    }
    userInterface("Settings", settingsMenuItems, handleSettingsMenuSelection);
}