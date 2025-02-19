#ifndef WIFI_SETTINGS_H
#define WIFI_SETTINGS_H
#define DISABLE_ALL_LIBRARY_WARNINGS

#include <TFT_eSPI.h>
#include "../UI/DisplayConfig.h"
#include "../UI/UISettings.h"

void initWiFi();
void wifiScan();
void startWiFiMonitor();

#endif // WIFI_SETTINGS_H
