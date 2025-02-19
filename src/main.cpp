#define DISABLE_ALL_LIBRARY_WARNINGS
#include <Arduino.h>
#include <esp_sleep.h>
#include <TFT_eSPI.h>
#include "UI/UISettings.h"
#include "UI/MainMenu.h"
#include "UI/GPIOS.h"
#include "UI/StartupSettings.h"
#include "WiFi/WiFiMenu.h"
#include "Bluetooth/BluetoothMenu.h"
#include "IR/IRMenu.h"
#include "RFID_NFC/RFID_NFC_Menu.h"
#include "nRF24/nRF24Menu.h"
#include "SubGhz/SubGhzMenu.h"
#include "USB/USBMenu.h"
#include "Tools/ToolsMenu.h"
#include "Settings/SettingsMenu.h"
#include "Settings/SettingsSettings.h"
#include "About/AboutMenu.h"

TFT_eSPI tft = TFT_eSPI();  // Initialize TFT object

void setup() {
    Serial.begin(115200);
    Serial.println("booting up");
    startUp();
}

void loop() {
    
}
