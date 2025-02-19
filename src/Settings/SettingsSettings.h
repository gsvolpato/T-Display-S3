#ifndef SETTINGS_SETTINGS_H
#define SETTINGS_SETTINGS_H
#define DISABLE_ALL_LIBRARY_WARNINGS
#include <TFT_eSPI.h>
#include "../UI/DisplayConfig.h"
#include "../UI/UISettings.h"

void handleOrientationSelection(int selectedOption);
void orientationMenu();
void saveColors();
void loadColors();
void uiColorMenu();
void restartDevice();
void turnOffDevice();
void saveBrightness();
void loadBrightness();

extern const char* orientationItems[];
extern uint16_t highlightColor;
extern uint16_t backgroundColor;
extern uint8_t currentBrightness;

#endif
