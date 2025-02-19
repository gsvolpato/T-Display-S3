#define DISABLE_ALL_LIBRARY_WARNINGS

#include <Arduino.h>
#include <TFT_eSPI.h>
#include "nRF24Menu.h"
#include "../UI/StartupSettings.h"
#include "../UI/UISettings.h"
#include "../UI/DisplayConfig.h"
#include "../UI/GPIOS.h"
#include "nRF24Settings.h"

extern TFT_eSPI tft;

const char* nRF24MenuItems[] = {
    "01- Scan",
    "02- Send",
    "03- Receive",
    "04- Clone",
    "05- Sniff",
    "06- Back",
    nullptr // Null-terminated to count items
};

void handlenRF24MenuSelection(int selectedOption) {
    tft.setTextDatum(TL_DATUM);

    // Initialize nRF24 when entering any nRF24 menu option
    setupNRF24();

    switch (selectedOption) {
        case 0:
            staticItems();
            tft.drawString("nRF24 Scan selected", 15, 45);
            Serial.println("nRF24 Scan selected");
            // Add your nRF24 scan code here
            delay(1000);
            break;
        case 1:
            staticItems();
            tft.drawString("nRF24 Send selected", 15, 45);
            Serial.println("nRF24 Send selected");
            delay(1000);
            break;
        case 2:
            staticItems();
            tft.drawString("nRF24 Receive selected", 15, 45);
            Serial.println("nRF24 Receive selected");
            delay(1000);
            break;
        case 3:
            staticItems();
            tft.drawString("nRF24 Clone selected", 15, 45);
            Serial.println("nRF24 Clone selected");
            delay(1000);
            break;
        case 4:
            staticItems();
            tft.drawString("nRF24 Sniff selected", 15, 45);
            Serial.println("nRF24 Sniff selected");
            delay(1000);
            break;
        default:
            staticItems();
            tft.drawString("Invalid nRF24 menu selection", 15, 45);
            Serial.println("Invalid nRF24 menu selection");
            delay(1000);
            break;
    }
}

void nRF24Menu() {
    Serial.println("Parsed nRF24 Menu Items:");
    for (int i = 0; nRF24MenuItems[i] != nullptr; i++) {
        Serial.println(nRF24MenuItems[i]);
    }
    userInterface("nRF24", nRF24MenuItems, handlenRF24MenuSelection);
} 