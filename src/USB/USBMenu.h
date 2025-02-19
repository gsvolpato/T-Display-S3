#ifndef USB_MENU_H
#define USB_MENU_H
#define DISABLE_ALL_LIBRARY_WARNINGS

#include <TFT_eSPI.h>
#include "../UI/DisplayConfig.h"
#include "../UI/UISettings.h"

extern const char* USBMenuItems[];
void handleUSBMenuSelection(int selectedOption);
void USBMenu();

#endif // USB_MENU_H