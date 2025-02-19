#ifndef NRF24_MENU_H
#define NRF24_MENU_H

#define DISABLE_ALL_LIBRARY_WARNINGS

#include <TFT_eSPI.h>
#include "../UI/DisplayConfig.h"
#include "../UI/UISettings.h"

void nRF24Menu();
void handleNRF24MenuSelection(int selectedOption);

extern const char* nRF24MenuItems[];

#endif // NRF24_MENU_H 