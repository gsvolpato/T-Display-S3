#define DISABLE_ALL_LIBRARY_WARNINGS

#include <Arduino.h>
#include <TFT_eSPI.h>
#include "RFID_NFC_Menu.h"
#include "../UI/StartupSettings.h"
#include "../UI/UISettings.h"
#include "../UI/DisplayConfig.h"
#include "../UI/GPIOS.h"
#include "RFID_NFC_Settings.h"

extern TFT_eSPI tft;

const char* RFIDMenuItems[] = {
    "Read ID",
    "Clone Tag",
    "Impersonate",
    "Format ID",
    "Read Dump",
    "Write Dump",
    "Stored IDs",
    "Back",
    nullptr
};

void handleRFIDMenuSelection(int selectedOption) {
    uint8_t rotation = tft.getRotation();
    int textX = (rotation == 1 || rotation == 3) ? SCREEN_WIDTH/2 : SCREEN_WIDTH/4;
    tft.setTextDatum(MC_DATUM);

    switch (selectedOption) {
        case 0:
            rfidScanContinuous();
            break;
        case 1:
            rfidClone();
            break;
        case 2:
            staticItems();
            tft.drawString("Impersonate", textX, SCREEN_HEIGHT/2);
            Serial.println("RFID Impersonate selected");
            delay(1000);
            break;
        case 3:
            formatRFIDCard();
            break;
        case 4:
            staticItems();
            tft.drawString("Read Dump", textX, SCREEN_HEIGHT/2);
            Serial.println("RFID Read Dump selected");
            delay(1000);
            break;
        case 5:
            staticItems();
            tft.drawString("Write Dump", textX, SCREEN_HEIGHT/2);
            Serial.println("RFID Write Dump selected");
            delay(1000);
            break;
        case 6:
            staticItems();
            tft.drawString("Stored IDs", textX, SCREEN_HEIGHT/2);
            Serial.println("RFID Stored IDs selected");
            delay(1000);
            break;
        default:
            staticItems();
            tft.drawString("Invalid selection", textX, SCREEN_HEIGHT/2);
            Serial.println("Invalid RFID menu selection");
            delay(1000);
            break;
    }
}

void RFIDMenu() {
    Serial.println("Parsed RFID Menu Items:");
    for (int i = 0; RFIDMenuItems[i] != nullptr; i++) {
        Serial.println(RFIDMenuItems[i]);
    }
    
    // Use "RFID" for landscape and "RF" for portrait to save space
    uint8_t rotation = tft.getRotation();
    const char* headerText = (rotation == 1 || rotation == 3) ? "RFID and NFC" : "RFID";
    
    userInterface(headerText, RFIDMenuItems, handleRFIDMenuSelection);
}