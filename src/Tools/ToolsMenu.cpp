#define DISABLE_ALL_LIBRARY_WARNINGS

#include <Arduino.h>
#include <TFT_eSPI.h>
#include "ToolsMenu.h"
#include "../UI/StartupSettings.h"
#include "../UI/UISettings.h"
#include "../UI/DisplayConfig.h"
#include "../UI/GPIOS.h"

extern TFT_eSPI tft;

const char* ToolsMenuItems[] = {
    "01- Tools",
    "02- Tools",
    "03- Tools",
    "04- Tools",
    "05- Tools",
    "06- Back",
    nullptr
};

void handleToolsMenuSelection(int selectedOption) {
    int leftPadding = 15;
    tft.setTextDatum(MC_DATUM);

    switch (selectedOption) {
        case 0:
            staticItems();
            tft.drawString("Tools option 1 selected", leftPadding, SCREEN_HEIGHT / 2);
            Serial.println("Tools option 1 selected");
            break;
        case 1:
            staticItems();
            tft.drawString("Tools option 2 selected", leftPadding, SCREEN_HEIGHT / 2);
            Serial.println("Tools option 2 selected");
            break;
        case 2:
            staticItems();
            tft.drawString("Tools option 3 selected", leftPadding, SCREEN_HEIGHT / 2);
            Serial.println("Tools option 3 selected");
            break;
        case 3:
            staticItems();
            tft.drawString("Tools option 4 selected", leftPadding, SCREEN_HEIGHT / 2);
            Serial.println("Tools option 4 selected");
            break;
        case 4:
            staticItems();
            tft.drawString("Tools option 5 selected", leftPadding, SCREEN_HEIGHT / 2);
            Serial.println("Tools option 5 selected");
            break;
        default:
            staticItems();
            tft.drawString("Invalid Tools menu selection", leftPadding, SCREEN_HEIGHT / 2);
            Serial.println("Invalid Tools menu selection");
            break;
    }
}

void ToolsMenu() {
    Serial.println("Parsed Tools Menu Items:");
    for (int i = 0; ToolsMenuItems[i] != nullptr; i++) {
        Serial.println(ToolsMenuItems[i]);
    }
    userInterface("Tools", ToolsMenuItems, handleToolsMenuSelection);
}