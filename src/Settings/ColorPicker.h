#ifndef COLOR_PICKER_H
#define COLOR_PICKER_H
#define DISABLE_ALL_LIBRARY_WARNINGS
#include <Arduino.h>
#include <TFT_eSPI.h>
#include "../UI/fonts/OpenSans_Bold10pt7b.h"
#include "../UI/fonts/OpenSans_Regular8pt7b.h"
#include "../UI/GPIOS.h"

void initColorPicker();
bool handleColorPicker();
extern uint16_t currentR, currentG, currentB;
extern uint8_t currentBrightness;
extern int8_t selectedItem;
extern bool isItemSelected;

#define COLOR_VALUE_Y_START 100
#define COLOR_VALUE_SPACING 20
#define COLOR_VALUE_X 20
#define BUTTON_Y 160
#define COLOR_STEP 1
#define R_MAX 0xF800
#define G_MAX 0x07E0
#define B_MAX 0x001F
#define RESULT_RECT_WIDTH 120
#define RESULT_RECT_HEIGHT 80

// Brightness control
#define BACKLIGHT_PIN 21
#define BRIGHTNESS_MIN 10
#define BRIGHTNESS_MAX 255
#define BRIGHTNESS_STEP 5

// Padding definitions
#define LEFT_PADDING 15
#define RIGHT_PADDING 15

#endif // COLOR_PICKER_H 