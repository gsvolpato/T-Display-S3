#ifndef RFID_NFC_MENU_H
#define RFID_NFC_MENU_H
#define DISABLE_ALL_LIBRARY_WARNINGS

#include <TFT_eSPI.h>
#include "../UI/DisplayConfig.h"
#include "../UI/UISettings.h"

extern const char* RFIDMenuItems[];
void handleRFIDMenuSelection(int selectedOption);
void RFIDMenu();

#endif // RFID_NFC_MENU_H