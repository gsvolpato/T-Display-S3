#define DISABLE_ALL_LIBRARY_WARNING
#include <Arduino.h>
#include <TFT_eSPI.h>

extern TFT_eSPI tft;

//| UI Files
#include "MenuLists.h"
#include "StartupSettings.h"
#include "UISettings.h"
#include "DisplayConfig.h"
#include "GPIOS.h"

//| Settings Files
#include "../WiFi/WiFiSettings.h"
#include "../Bluetooth/BluetoothSettings.h"
#include "../IR/IRSettings.h"   
#include "../RFID_NFC/RFID_NFC_Settings.h"
#include "../nRF24/nRF24Settings.h"
#include "../SubGhz/SubGhzSettings.h"
#include "../USB/USBSettings.h"
#include "../Tools/ToolsSettings.h"
#include "../Settings/SettingsSettings.h"

//| MAIN MENU ###############################################################
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
            break;
        case 4: // nRF24
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
        default:
            Serial.println("Invalid menu selection");
            break;
    }
}
void mainMenu() {
    Serial.println("mainMenu()");
    Serial.println("Parsed Menu Items:");
    for (int i = 0; mainMenuItems[i] != nullptr; i++) {
        Serial.println(mainMenuItems[i]);
    }
    userInterface("Main", mainMenuItems, handleMainMenuSelection);
}

//| 1 - WIFI MENU ##########################################################
const char* wifiMenuItems[] = {
    "Scan WiFi",
    "WiFi Serial Monitor",
    "03- WiFi", 
    "04- WiFi",
    "05- WiFi",
    "06- WiFi",
    nullptr
};
void handleWiFiMenuSelection(int selectedOption) {
    tft.setTextDatum(TL_DATUM);

    switch (selectedOption) {
        case 0:
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

//| 2 - BLUETOOTH MENU ##########################################################
const char* BluetoothMenuItems[] = {
    "Bluetooth Scan",
    "BLE Serial Monitor",
    "03- Bluetooth",
    "04- Bluetooth",
    "05- Bluetooth",
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
    static bool initialized = false;
    if (!initialized) {
        initBluetooth();
        initialized = true;
    }
    uint8_t rotation = tft.getRotation();
    const char* headerText = (rotation == 1 || rotation == 3) ? "Bluetooth" : "BLE";

    Serial.println("Parsed Bluetooth Menu Items:");
    for (int i = 0; BluetoothMenuItems[i] != nullptr; i++) {
        Serial.println(BluetoothMenuItems[i]);
    }
    
    userInterface(headerText, BluetoothMenuItems, handleBluetoothMenuSelection);
}

//| 3 - IR MENU ##########################################################
const char* IRMenuItems[] = {
    "Receive",
    "Replay",
    "Save",
    "Create Remote",
    "Stored IR",
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

//| 4 - RFID AND NFC MENU ##########################################################
const char* RFIDMenuItems[] = {
    "Read ID",
    "Clone Tag",
    "Impersonate",
    "Format ID",
    "Read Dump",
    "Write Dump",
    "Stored IDs",
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

//| 5 - NRF24 MENU ##########################################################
const char* nRF24MenuItems[] = {
    "01- Scan",
    "02- Send",
    "03- Receive",
    "04- Clone",
    "05- Sniff",
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

//| 6 - SUBGHZ MENU ##########################################################
const char* SubGhzMenuItems[] = {
    "01- SubGhz",
    "02- SubGhz",
    "03- SubGhz",
    "04- SubGhz",
    "05- SubGhz",
    "06- SubGhz",
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

//| 7 - USB MENU ##########################################################
const char* usbMenuItems[] = {
    "01- USB",
    "02- USB",
    "03- USB",
    "04- USB",
    "05- USB",
    "06- USB",
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

//| 8 - TOOLS MENU ##########################################################
const char* ToolsMenuItems[] = {
    "01- Tools",
    "02- Tools",
    "03- Tools",
    "04- Tools",
    "05- Tools",
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

//| 9 - SETTINGS MENU ##########################################################
const char* settingsMenuItems[] = {
    "Display",
    "UI Colors",
    "Rotation",
    "Brightness",
    "Power",
    "Storage",
    "Sound",
    "About",
    nullptr  // Terminator
};
void handleSettingsMenuSelection(int selectedOption) {
    switch(selectedOption) {
        case 0: // Display
            // Display handling code
            break;
        case 1: // UI Colors
            uiColorMenu();
            break;
        case 2: // Rotation
            // Rotation handling code
            break;
        case 3: // Brightness
            // Brightness handling code
            break;
        case 4: // Power
            // Power handling code
            break;
        case 5: // Storage
            // Storage handling code
            break;
        case 6: // Sound
            staticItems();
            tft.drawString("Sound selected", SCREEN_WIDTH/2, SCREEN_HEIGHT / 2);
            Serial.println("Sound selected");
            delay(1000);
            break;
        case 7: // About
            AboutMenu();
            break;
        default:
            Serial.println("Invalid settings menu selection");
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
