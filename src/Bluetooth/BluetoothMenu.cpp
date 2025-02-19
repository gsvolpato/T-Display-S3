#define DISABLE_ALL_LIBRARY_WARNINGS

#include <Arduino.h>
#include <TFT_eSPI.h>
#include "BluetoothMenu.h"
#include "../UI/StartupSettings.h"
#include "../UI/UISettings.h"
#include "../UI/DisplayConfig.h"
#include "../UI/GPIOS.h"
#include "BluetoothSettings.h"

extern TFT_eSPI tft;

const char* BluetoothMenuItems[] = {
    "Bluetooth Scan",
    "BLE Serial Monitor",
    "03- Bluetooth",
    "04- Bluetooth",
    "05- Bluetooth",
    "Back",
    nullptr
};

void handleBluetoothMenuSelection(int selectedOption) {
    switch (selectedOption) {
        case 0:
            staticItems();
            startBluetoothScan();
            break;
        case 1:
            staticItems();
            startBLESerialMonitor();
            break;
        case 2:
            staticItems();
            tft.drawString("Bluetooth option 3 selected", SCREEN_WIDTH/2, SCREEN_HEIGHT/2);
            Serial.println("Bluetooth option 3 selected");
            break;
        case 3:
            staticItems();
            tft.drawString("Bluetooth option 4 selected", SCREEN_WIDTH/2, SCREEN_HEIGHT/2);
            Serial.println("Bluetooth option 4 selected");
            break;
        case 4:
            staticItems();
            tft.drawString("Bluetooth option 5 selected", SCREEN_WIDTH/2, SCREEN_HEIGHT/2);
            Serial.println("Bluetooth option 5 selected");
            break;
        default:
            staticItems();
            tft.drawString("Invalid Bluetooth menu selection", SCREEN_WIDTH/2, SCREEN_HEIGHT/2);
            Serial.println("Invalid Bluetooth menu selection");
            break;
    }
}

void BluetoothMenu() {
    // Initialize Bluetooth if not already initialized
    static bool initialized = false;
    if (!initialized) {
        initBluetooth();
        initialized = true;
    }

    // Get current rotation to determine orientation
    uint8_t rotation = tft.getRotation();
    const char* headerText = (rotation == 1 || rotation == 3) ? "Bluetooth" : "BLE";

    Serial.println("Parsed Bluetooth Menu Items:");
    for (int i = 0; BluetoothMenuItems[i] != nullptr; i++) {
        Serial.println(BluetoothMenuItems[i]);
    }
    
    userInterface(headerText, BluetoothMenuItems, handleBluetoothMenuSelection);
}