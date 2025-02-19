#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#define DISABLE_ALL_LIBRARY_WARNINGS
#include <TFT_eSPI.h>
#include "DisplayConfig.h"

extern const char* mainMenuItems[];
extern const int numMenuItems;
extern int selectedMenuItem;

void handleMainMenuSelection(int selectedOption);
void mainMenu();


#endif // MAIN_MENU_H