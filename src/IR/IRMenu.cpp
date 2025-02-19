#define DISABLE_ALL_LIBRARY_WARNINGS

#include <Arduino.h>
#include <TFT_eSPI.h>
#include "IRMenu.h"
#include "../UI/StartupSettings.h"
#include "../UI/UISettings.h"
#include "../UI/DisplayConfig.h"
#include "../UI/GPIOS.h"

extern TFT_eSPI tft;

const char* IRMenuItems[] = {
    "Receive",
    "Replay",
    "Save",
    "Create Remote",
    "Stored IR",
    "Back",
    nullptr
};

void handleIRMenuSelection(int selectedOption) {
    int leftPadding = 15;
    tft.setTextDatum(MC_DATUM);

    switch (selectedOption) {
        case 0:
            staticItems();
            tft.drawString("IR option 1 selected", leftPadding, SCREEN_HEIGHT / 2);
            Serial.println("IR option 1 selected");
            break;
        case 1:
            staticItems();
            tft.drawString("IR option 2 selected", leftPadding, SCREEN_HEIGHT / 2);
            Serial.println("IR option 2 selected");
            break;
        case 2:
            staticItems();
            tft.drawString("IR option 3 selected", leftPadding, SCREEN_HEIGHT / 2);
            Serial.println("IR option 3 selected");
            break;
        default:
            staticItems();
            tft.drawString("Invalid IR menu selection", leftPadding, SCREEN_HEIGHT / 2);
            Serial.println("Invalid IR menu selection");
            break;
    }
}

void IRMenu() {
    Serial.println("Parsed IR Menu Items:");
    for (int i = 0; IRMenuItems[i] != nullptr; i++) {
        Serial.println(IRMenuItems[i]);
    }
    userInterface("IR", IRMenuItems, handleIRMenuSelection);
}