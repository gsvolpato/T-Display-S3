#define DISABLE_ALL_LIBRARY_WARNINGS

#include "ColorPicker.h"
#include "../UI/UISettings.h"
#include "../UI/DisplayConfig.h"
#include "SettingsSettings.h"

extern TFT_eSPI tft;

uint16_t currentR = 0;  // Red component in RGB565 format
uint16_t currentG = 0;  // Green component in RGB565 format
uint16_t currentB = 0;  // Blue component in RGB565 format
uint8_t currentBrightness = 128;  // Add this line to define the brightness variable
int8_t selectedItem = 0;  // 0 = red, 1 = green, 2 = blue, 3 = brightness, 4 = back, 5 = store
bool isItemSelected = false;

void drawColorRect(int x, int y, int width, int height) {
    uint16_t color = currentR | currentG | currentB;  // Combine RGB565 components
    tft.fillRect(x, y, width, height, color);
}

void updateColorPickerDisplay() {
    staticItems();
    
    // Draw result rectangle on the right side
    int rectX = SCREEN_WIDTH - RESULT_RECT_WIDTH - 20;
    int rectY = (SCREEN_HEIGHT - RESULT_RECT_HEIGHT) / 2;
    drawColorRect(rectX, rectY, RESULT_RECT_WIDTH, RESULT_RECT_HEIGHT);
    
    // Clear the previous values area
    tft.fillRect(10, 39, 100, 140, backgroundColor);
    
    // Draw RGB values and brightness vertically on the left
    tft.setTextDatum(TL_DATUM);
    
    // Brightness value (new)
    if (selectedItem == 3) {
        tft.setFreeFont(&OpenSans_Bold10pt7b);
        tft.setTextColor(highlightColor);
    } else {
        tft.setFreeFont(&OpenSans_Regular8pt7b);
        tft.setTextColor(highlightColor, backgroundColor);
    }
    int brightnessPercent = map(currentBrightness, BRIGHTNESS_MIN, BRIGHTNESS_MAX, 0, 100);
    tft.drawString("BL:" + String(brightnessPercent) + "%", COLOR_VALUE_X, COLOR_VALUE_Y_START);
    
    // Blue value
    if (selectedItem == 2) {
        tft.setFreeFont(&OpenSans_Bold10pt7b);
        tft.setTextColor(highlightColor);
    } else {
        tft.setFreeFont(&OpenSans_Regular8pt7b);
        tft.setTextColor(highlightColor, backgroundColor);
    }
    tft.drawString("B:" + String(currentB & B_MAX), COLOR_VALUE_X, COLOR_VALUE_Y_START - COLOR_VALUE_SPACING);
    
    // Green value
    if (selectedItem == 1) {
        tft.setFreeFont(&OpenSans_Bold10pt7b);
        tft.setTextColor(highlightColor);
    } else {
        tft.setFreeFont(&OpenSans_Regular8pt7b);
        tft.setTextColor(highlightColor, backgroundColor);
    }
    tft.drawString("G:" + String((currentG & G_MAX) >> 5), COLOR_VALUE_X, COLOR_VALUE_Y_START - (COLOR_VALUE_SPACING * 2));
    
    // Red value
    if (selectedItem == 0) {
        tft.setFreeFont(&OpenSans_Bold10pt7b);
        tft.setTextColor(highlightColor);
    } else {
        tft.setFreeFont(&OpenSans_Regular8pt7b);
        tft.setTextColor(highlightColor, backgroundColor);
    }
    tft.drawString("R:" + String((currentR & R_MAX) >> 11), COLOR_VALUE_X, COLOR_VALUE_Y_START - (COLOR_VALUE_SPACING * 3));
    
    // Draw BACK and STORE buttons
    if (selectedItem == 4) {  // Changed from 3 to 4
        tft.setFreeFont(&OpenSans_Bold10pt7b);
        tft.setTextColor(highlightColor);
    } else {
        tft.setFreeFont(&OpenSans_Regular8pt7b);
        tft.setTextColor(highlightColor, backgroundColor);
    }
    tft.drawString("BACK", 20, BUTTON_Y);
    
    if (selectedItem == 5) {  // Changed from 4 to 5
        tft.setFreeFont(&OpenSans_Bold10pt7b);
        tft.setTextColor(highlightColor);
    } else {
        tft.setFreeFont(&OpenSans_Regular8pt7b);
        tft.setTextColor(highlightColor, backgroundColor);
    }
    tft.drawString("STORE", SCREEN_WIDTH - 80, BUTTON_Y);
}

bool handleColorPicker() {
    if (encoderUp || encoderDown) {
        if (isItemSelected) {
            if (selectedItem <= 2) {  // RGB values
                // Adjust RGB565 values when selected
                if (encoderUp) {
                    switch(selectedItem) {
                        case 0: // Red (0-31 in position 11-15)
                            if(currentR < R_MAX) currentR += (1 << 11);
                            break;
                        case 1: // Green (0-63 in position 5-10)
                            if(currentG < G_MAX) currentG += (1 << 5);
                            break;
                        case 2: // Blue (0-31 in position 0-4)
                            if(currentB < B_MAX) currentB += 1;
                            break;
                    }
                } else {
                    switch(selectedItem) {
                        case 0: // Red
                            if(currentR > 0) currentR -= (1 << 11);
                            break;
                        case 1: // Green
                            if(currentG > 0) currentG -= (1 << 5);
                            break;
                        case 2: // Blue
                            if(currentB > 0) currentB -= 1;
                            break;
                    }
                }
                // Update highlight color immediately to see changes
                highlightColor = currentR | currentG | currentB;
            } else if (selectedItem == 3) {  // Brightness control
                if (encoderUp) {
                    if (currentBrightness < BRIGHTNESS_MAX) {
                        currentBrightness = min(currentBrightness + BRIGHTNESS_STEP, BRIGHTNESS_MAX);
                    }
                } else {
                    if (currentBrightness > BRIGHTNESS_MIN) {
                        currentBrightness = max(currentBrightness - BRIGHTNESS_STEP, BRIGHTNESS_MIN);
                    }
                }
                ledcWrite(0, currentBrightness);  // Update backlight brightness
            }
        } else {
            // Navigate between options
            if (encoderUp) {
                selectedItem = (selectedItem > 0) ? selectedItem - 1 : 5;  // Changed from 4 to 5
            } else {
                selectedItem = (selectedItem < 5) ? selectedItem + 1 : 0;  // Changed from 4 to 5
            }
        }
        encoderUp = false;
        encoderDown = false;
        updateColorPickerDisplay();
    }

    // Handle encoder button press
    int buttonState = digitalRead(PIN_ENCODER_SWITCH);
    static bool buttonPressed = false;
    
    if (buttonState == LOW && !buttonPressed) {
        buttonPressed = true;
        delayMicroseconds(10000);
    } 
    else if (buttonState == HIGH && buttonPressed) {
        buttonPressed = false;
        delayMicroseconds(10000);
        
        if (selectedItem <= 2) {
            isItemSelected = !isItemSelected;
            updateColorPickerDisplay();
        } else if (selectedItem == 3) { // BACK
            if (!isItemSelected) {
                return false; // Exit color picker without saving
            }
        } else if (selectedItem == 4) { // STORE
            if (!isItemSelected) {
                highlightColor = currentR | currentG | currentB;
                saveColors();
                return false; // Exit color picker after saving
            }
        }
    }
    return true; // Continue color picker
}

void initColorPicker() {
    // Setup PWM channel for backlight control
    ledcSetup(0, 5000, 8);  // Channel 0, 5000 Hz, 8-bit resolution
    ledcAttachPin(BACKLIGHT_PIN, 0);
    
    selectedItem = 0;
    isItemSelected = false;
    
    // Get current backlight value or set default
    currentBrightness = 128;  // Default to 50%
    ledcWrite(0, currentBrightness);
    
    // Split current highlightColor into RGB565 components
    currentR = highlightColor & R_MAX;
    currentG = highlightColor & G_MAX;
    currentB = highlightColor & B_MAX;
    
    updateColorPickerDisplay();
    
    while(handleColorPicker()) {
        yield();
        delayMicroseconds(5000);
    }
} 