#define DISABLE_ALL_LIBRARY_WARNINGS
#ifndef MENULISTS_H
#define MENULISTS_H
#include <TFT_eSPI.h>
#include "DisplayConfig.h"
#include "UISettings.h"

//| MAIN MENU ###############################################################
extern const char* mainMenuItems[];
extern const int numMenuItems;
extern int selectedMenuItem;
void handleMainMenuSelection(int selectedOption);
void mainMenu();

//| 1 - WIFI MENU ##########################################################
extern const char* WiFiMenuItems[];
void handleWiFiMenuSelection(int selectedOption);
void WiFiMenu();
void wifiScan();

//| 2 - BLUETOOTH MENU ##########################################################
void BluetoothMenu();

//| 3 - IR MENU ##########################################################
void IRMenu();

//| 4 - RFID AND NFC MENU ##########################################################
extern const char* RFIDMenuItems[];
void handleRFIDMenuSelection(int selectedOption);
void RFIDMenu();

//| 5 - NRF24 MENU ##########################################################
void nRF24Menu();
void handleNRF24MenuSelection(int selectedOption);
extern const char* nRF24MenuItems[];

//| 6 - SUBGHZ MENU ##########################################################
extern const char* SubGhzMenuItems[];
void handleSubGhzMenuSelection(int selectedOption);
void SubGhzMenu();

//| 7 - USB MENU ##########################################################
extern const char* USBMenuItems[];
void handleUSBMenuSelection(int selectedOption);
void USBMenu();

//| 8 - TOOLS MENU ##########################################################
extern const char* ToolsMenuItems[];
void handleToolsMenuSelection(int selectedOption);
void ToolsMenu();

//| 9 - SETTINGS MENU ##########################################################
void SettingsMenu();

#endif // MENULISTS_H