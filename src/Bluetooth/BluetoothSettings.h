#ifndef BLUETOOTH_SETTINGS_H
#define BLUETOOTH_SETTINGS_H

#define DISABLE_ALL_LIBRARY_WARNINGS
#include <TFT_eSPI.h>
#include "../UI/DisplayConfig.h"
#include "../UI/UISettings.h"

void initBluetooth();
void startBluetoothScan();
void displayBluetoothDevices();
void startBLESerialMonitor();

#endif // BLUETOOTH_SETTINGS_H
