#define DISABLE_ALL_LIBRARY_WARNINGS

#include <Arduino.h>
#include <TFT_eSPI.h>
#include "USBMenu.h"
#include "../UI/StartupSettings.h"
#include "../UI/UISettings.h"
#include "../UI/DisplayConfig.h"
#include "../UI/GPIOS.h"

extern TFT_eSPI tft;

const char* usbMenuItems[] = {
    "01- USB",
    "02- USB",
    "03- USB",
    "04- USB",
    "05- USB",
    "06- USB",
    "07- Back",
    nullptr
};

void handleUSBMenuSelection(int selectedOption) {
    int leftPadding = 15;
    tft.setTextDatum(MC_DATUM);

    switch (selectedOption) {
        case 0:
            staticItems();
            tft.drawString("USB option 1 selected", leftPadding, SCREEN_HEIGHT / 2);
            Serial.println("USB option 1 selected");
            break;
        case 1:
            staticItems();
            tft.drawString("USB option 2 selected", leftPadding, SCREEN_HEIGHT / 2);
            Serial.println("USB option 2 selected");
            break;
        case 2:
            staticItems();
            tft.drawString("USB option 3 selected", leftPadding, SCREEN_HEIGHT / 2);
            Serial.println("USB option 3 selected");
            break;
        case 3:
            staticItems();
            tft.drawString("USB option 4 selected", leftPadding, SCREEN_HEIGHT / 2);
            Serial.println("USB option 4 selected");
            break;
        case 4:
            staticItems();
            tft.drawString("USB option 5 selected", leftPadding, SCREEN_HEIGHT / 2);
            Serial.println("USB option 5 selected");
            break;
        case 5:
            staticItems();
            tft.drawString("USB option 6 selected", leftPadding, SCREEN_HEIGHT / 2);
            Serial.println("USB option 6 selected");
            break;
        default:
            staticItems();
            tft.drawString("Invalid USB menu selection", leftPadding, SCREEN_HEIGHT / 2);
            Serial.println("Invalid USB menu selection");
            break;
    }
}

void USBMenu() {
    Serial.println("Parsed USB Menu Items:");
    for (int i = 0; usbMenuItems[i] != nullptr; i++) {
        Serial.println(usbMenuItems[i]);
    }
    userInterface("USB", usbMenuItems, handleUSBMenuSelection);
}