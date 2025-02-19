#ifndef UI_SETTINGS_H
#define UI_SETTINGS_H

#define DISABLE_ALL_LIBRARY_WARNINGS
#include <TFT_eSPI.h>
#include <Arduino.h>
#include "images.h"

struct Menu {
    const char* name;
    const char** items;
    Menu* previousMenu;
    void (*selectionHandler)(int);
};

// Function declarations
void handleEncoder();
void defaultLayout(const char* items[]);
void staticItems();
void buttonSelection();
void encoderRotation();
void userInterface(const char* menuName, const char** items, void (*selectionHandler)(int));
void setupEncoder();
void IRAM_ATTR updateEncoder();
void WiFiMenu();
void BluetoothMenu();
void IRMenu();
void NFCMenu();
void RFIDMenu();
void SubGhzMenu();
void USBMenu();
void ToolsMenu();
void SettingsMenu();
void AboutMenu();
void uiColorMenu();
void handleColorSelection(const char* element, uint16_t& colorVariable);
void drawUpArrow();
void drawDownArrow();
void backOption();

// Global variables declarations
extern int selectedMenuItem;
extern const int numMenuItems;
extern bool staticItemsDrawn;
extern const int itemsPerPage;
extern int currentPage;
extern int lastSelectedMenuItem;
extern volatile int lastEncoded;
extern volatile long encoderValue;
extern long lastencoderValue;
extern int valueCount;
extern Menu* currentMenu;
extern const char* settingsMenuItems[];
extern const char* uiColorItems[];
extern const char* colorList[];
extern uint16_t highlightColor;
extern uint16_t backgroundColor;
extern int lastPage;
extern volatile bool encoderUp;
extern volatile bool encoderDown;
extern volatile bool encoderPressed;
extern volatile bool encoderButtonHandled;

int countMenuItems(const char* items[]);

#define ENCODER_STEPS_PER_DETENT 4

#endif // UI_SETTINGS_H
