#define DISABLE_ALL_LIBRARY_WARNINGS

#include <Arduino.h>
#include "MainMenu.h"
#include "StartupSettings.h"
#include "UISettings.h"
#include "DisplayConfig.h"


extern TFT_eSPI tft;

const char* mainMenuItems[] = {
    "WiFi",
    "Bluetooth",
    "IR",
    "RFID and NFC",
    "nRF24",
    "SubGhz",
    "USB",
    "Tools",
    "Settings",
    " ",     // Added Back option to maintain menu system consistency
    nullptr
};

void handleMainMenuSelection(int selectedOption) {
    switch (selectedOption) {
        case 0: // WiFi
            WiFiMenu();
            break;
        case 1: // Bluetooth
            BluetoothMenu();
            break;
        case 2: // IR
            IRMenu();
            break;
        case 3: // RFID and NFC
            RFIDMenu();
            //NFCMenu(); // Temporarily disabled until implemented
            break;
        case 4: // nRF24
            //nRF24Menu(); // Temporarily disabled until implemented
            staticItems();
            tft.setTextDatum(MC_DATUM);
            tft.drawString("nRF24 Menu - Coming soon", SCREEN_WIDTH/2, SCREEN_HEIGHT/2);
            delay(1500);
            break;
        case 5: // SubGhz
            SubGhzMenu();
            break;
        case 6: // USB
            USBMenu();
            break;
        case 7: // Tools
            ToolsMenu();
            break;
        case 8: // Settings
            SettingsMenu();
            break;
        case 9: // Back (will do nothing in main menu)
            // Do nothing since we're in the main menu
            break;
        default:
            Serial.println("Invalid menu selection");
            break;
    }
}

void mainMenu() {
    Serial.println("Parsed Menu Items:");
    for (int i = 0; mainMenuItems[i] != nullptr; i++) {
        Serial.println(mainMenuItems[i]);
    }
    userInterface("Main", mainMenuItems, handleMainMenuSelection);
}