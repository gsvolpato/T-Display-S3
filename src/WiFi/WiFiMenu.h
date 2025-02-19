#ifndef WIFI_MENU_H
#define WIFI_MENU_H
#define DISABLE_ALL_LIBRARY_WARNINGS

#include <TFT_eSPI.h>
#include "../UI/DisplayConfig.h"
#include "../UI/UISettings.h"

extern const char* WiFiMenuItems[];
void handleWiFiMenuSelection(int selectedOption);
void WiFiMenu();
void wifiScan();

#endif // WIFI_MENU_H