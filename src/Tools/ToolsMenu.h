#ifndef TOOLS_MENU_H
#define TOOLS_MENU_H

#define DISABLE_ALL_LIBRARY_WARNINGS

#include <TFT_eSPI.h>
#include "../UI/DisplayConfig.h"
#include "../UI/UISettings.h"

extern const char* ToolsMenuItems[];
void handleToolsMenuSelection(int selectedOption);
void ToolsMenu();

#endif // TOOLS_MENU_H