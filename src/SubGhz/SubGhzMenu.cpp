#define DISABLE_ALL_LIBRARY_WARNINGS

#include <Arduino.h>
#include <TFT_eSPI.h>
#include "SubGhzMenu.h"
#include "../UI/StartupSettings.h"
#include "../UI/UISettings.h"
#include "../UI/DisplayConfig.h"
#include "../UI/GPIOS.h"

extern TFT_eSPI tft;

const char* SubGhzMenuItems[] = {
    "01- SubGhz",
    "02- SubGhz",
    "03- SubGhz",
    "04- SubGhz",
    "05- SubGhz",
    "06- SubGhz",
    "07- Back",
    nullptr
};

void handleSubGhzMenuSelection(int selectedOption) {
    int leftPadding = 15;
    tft.setTextDatum(MC_DATUM);

    switch (selectedOption) {
        case 0:
            staticItems();
            tft.drawString("SubGhz option 1 selected", leftPadding, SCREEN_HEIGHT / 2);
            Serial.println("SubGhz option 1 selected");
            break;
        case 1:
            staticItems();
            tft.drawString("SubGhz option 2 selected", leftPadding, SCREEN_HEIGHT / 2);
            Serial.println("SubGhz option 2 selected");
            break;
        case 2:
            staticItems();
            tft.drawString("SubGhz option 3 selected", leftPadding, SCREEN_HEIGHT / 2);
            Serial.println("SubGhz option 3 selected");
            break;
        case 3:
            staticItems();
            tft.drawString("SubGhz option 4 selected", leftPadding, SCREEN_HEIGHT / 2);
            Serial.println("SubGhz option 4 selected");
            break;
        case 4:
            staticItems();
            tft.drawString("SubGhz option 5 selected", leftPadding, SCREEN_HEIGHT / 2);
            Serial.println("SubGhz option 5 selected");
            break;
        case 5:
            staticItems();
            tft.drawString("SubGhz option 6 selected", leftPadding, SCREEN_HEIGHT / 2);
            Serial.println("SubGhz option 6 selected");
            break;
        default:
            staticItems();
            tft.drawString("Invalid SubGhz menu selection", leftPadding, SCREEN_HEIGHT / 2);
            Serial.println("Invalid SubGhz menu selection");
            break;
    }
}

void SubGhzMenu() {
    Serial.println("Parsed SubGhz Menu Items:");
    for (int i = 0; SubGhzMenuItems[i] != nullptr; i++) {
        Serial.println(SubGhzMenuItems[i]);
    }
    userInterface("SubGhz", SubGhzMenuItems, handleSubGhzMenuSelection);
}