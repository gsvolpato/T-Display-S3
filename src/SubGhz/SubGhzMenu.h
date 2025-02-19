#ifndef SUBGHZ_MENU_H
#define SUBGHZ_MENU_H
#define DISABLE_ALL_LIBRARY_WARNINGS

#include <TFT_eSPI.h>
#include "../UI/DisplayConfig.h"
#include "../UI/UISettings.h"

extern const char* SubGhzMenuItems[];
void handleSubGhzMenuSelection(int selectedOption);
void SubGhzMenu();

#endif // SUBGHZ_MENU_H