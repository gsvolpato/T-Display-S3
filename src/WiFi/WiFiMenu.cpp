#define DISABLE_ALL_LIBRARY_WARNINGS

#include <Arduino.h>
#include <TFT_eSPI.h>
#include "WiFiMenu.h"
#include "../UI/StartupSettings.h"
#include "../UI/UISettings.h"
#include "../UI/DisplayConfig.h"
#include "../UI/GPIOS.h"

extern TFT_eSPI tft;

const char* wifiMenuItems[] = {
    "Scan WiFi",
    "WiFi Serial Monitor",
    "03- WiFi", 
    "04- WiFi",
    "05- WiFi",
    "06- WiFi",
    "07- Back",
    nullptr
};

void handleWiFiMenuSelection(int selectedOption) {
    tft.setTextDatum(TL_DATUM);

    switch (selectedOption) {
        case 0:
            //staticItems();
            wifiScan();
            break;
        case 1:
            staticItems();
            tft.setTextDatum(TL_DATUM);
            tft.drawString("WiFi Serial Monitor", 15, 45);
            Serial.println("WiFi Serial Monitor");
            break;
        case 2:
            staticItems();
            tft.drawString("WiFi option 3 selected", 15, 45);
            Serial.println("WiFi option 3 selected");
            break;
        case 3:
            staticItems();
            tft.drawString("WiFi option 4 selected", 15, 45);
            Serial.println("WiFi option 4 selected");
            break;
        case 4:
            staticItems();
            tft.drawString("WiFi option 5 selected", 15, 45);
            Serial.println("WiFi option 5 selected");
            break;
        case 5:
            staticItems();
            tft.drawString("WiFi option 6 selected", 15, 45);
            Serial.println("WiFi option 6 selected");
            break;
        default:
            staticItems();
            tft.drawString("Invalid WiFi menu selection", 15, 45);
            Serial.println("Invalid WiFi menu selection");
            break;
    }
}

void WiFiMenu() {
    Serial.println("Parsed WiFi Menu Items:");
    for (int i = 0; wifiMenuItems[i] != nullptr; i++) {
        Serial.println(wifiMenuItems[i]);
    }
    userInterface("WiFi", wifiMenuItems, handleWiFiMenuSelection);
}