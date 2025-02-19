#ifndef ABOUT_MENU_H
#define ABOUT_MENU_H

#define DISABLE_ALL_LIBRARY_WARNINGS
#include <TFT_eSPI.h>
#include "../UI/DisplayConfig.h"
#include "../UI/UISettings.h"

extern const uint16_t about[];
extern const uint16_t about_portrait[];

void AboutMenu();
void displayAboutScreen();

#endif // ABOUT_MENU_H