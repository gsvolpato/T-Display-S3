#define DISABLE_ALL_LIBRARY_WARNINGS

#include <Arduino.h>
#include <TFT_eSPI.h>

#include "UISettings.h"

// FONTS
#include "../UI/fonts/OpenSans_Bold10pt7b.h"
#include "../UI/fonts/OpenSans_Regular8pt7b.h"

// IMAGES
#include "images.h"

// config files
#include "GPIOS.h"
#include "DisplayConfig.h"
#include "StartupSettings.h"

// MENU FILES
#include "MenuLists.h"
#include "../About/AboutMenu.h"
#include "../Settings/SettingsSettings.h"

extern TFT_eSPI tft;

Menu* currentMenu = nullptr;
int selectedMenuItem = 0;
const int numMenuItems = 10;
bool staticItemsDrawn = false;

const int LANDSCAPE_ITEMS_PER_PAGE = 5;
const int PORTRAIT_ITEMS_PER_PAGE = 12;

int getItemsPerPage() {
    uint8_t rotation = tft.getRotation();
    return (rotation == 1 || rotation == 3) ? LANDSCAPE_ITEMS_PER_PAGE : PORTRAIT_ITEMS_PER_PAGE;
}

int currentPage = 0;
int lastSelectedMenuItem = -1;
volatile int lastEncoded = 0;
volatile long encoderValue = 0;
long lastencoderValue = 0;
int valueCount = 0;

int yOffset = 43;
int itemSpacing = 25;
int leftPadding = 15;
int rightPadding = 15;

volatile bool encoderUpdated = false;
volatile bool encoderPressed = false;
volatile bool encoderButtonHandled = false;
int lastPage = -1;
volatile bool encoderUp = false;
volatile bool encoderDown = false;

const int HEADER_Y = 10;

void IRAM_ATTR updateEncoder() {
    static uint8_t prevState = 0;
    static const int8_t encoderTable[16] = {
        0, -1, 1, 0,
        1, 0, 0, -1,
        -1, 0, 0, 1,
        0, 1, -1, 0
    };
    
    prevState = (prevState << 2) & 0x0F;
    prevState |= (digitalRead(PIN_ENCODER_CLK) << 1) | digitalRead(PIN_ENCODER_DT);
    
    int8_t increment = encoderTable[prevState];
    if (increment != 0) {
        encoderValue += increment;
        
        if (encoderValue >= ENCODER_STEPS_PER_DETENT) {
            encoderDown = true;
            encoderValue = 0;
        } else if (encoderValue <= -ENCODER_STEPS_PER_DETENT) {
            encoderUp = true;
            encoderValue = 0;
        }
    }
}

void IRAM_ATTR handleButton() {
    static unsigned long lastInterruptTime = 0;
    unsigned long interruptTime = millis();
    
    // Simple debounce - ignore interrupts within 200ms
    if (interruptTime - lastInterruptTime > 200) {
        encoderPressed = true;
        encoderButtonHandled = false;
    }
    lastInterruptTime = interruptTime;
}

void setupEncoder() {
    detachInterrupt(digitalPinToInterrupt(PIN_ENCODER_CLK));
    detachInterrupt(digitalPinToInterrupt(PIN_ENCODER_DT));
    detachInterrupt(digitalPinToInterrupt(PIN_ENCODER_SWITCH));
    
    attachInterrupt(digitalPinToInterrupt(PIN_ENCODER_CLK), updateEncoder, CHANGE);
    attachInterrupt(digitalPinToInterrupt(PIN_ENCODER_DT), updateEncoder, CHANGE);
    attachInterrupt(digitalPinToInterrupt(PIN_ENCODER_SWITCH), handleButton, FALLING);
    
    Serial.println("Encoder setup complete");
}

void staticItems() {
    tft.fillScreen(backgroundColor);
    const uint16_t* currentBg;
    uint8_t rotation = tft.getRotation();
    
    // Adjust parameters based on orientation
    if (rotation == 1 || rotation == 3) {  // Landscape
        yOffset = 43;
        itemSpacing = 25;
        leftPadding = 15;
        rightPadding = 15;
        currentBg = bg_170x320_Landscape;
    } else {  // Portrait
        yOffset = 43;
        itemSpacing = 20; // Reduced spacing for portrait mode
        leftPadding = 10;
        rightPadding = 10;
        currentBg = bg_170x320_portrait;
    }

    // Draw background based on current orientation
    int currentWidth = (rotation == 1 || rotation == 3) ? SCREEN_WIDTH : SCREEN_HEIGHT;
    int currentHeight = (rotation == 1 || rotation == 3) ? SCREEN_HEIGHT : SCREEN_WIDTH;

    for (int y = 0; y < currentHeight; y++) {
        for (int x = 0; x < currentWidth; x++) {
            uint16_t pixel;
            if (rotation == 0 || rotation == 2) { // Portrait
                pixel = currentBg[y * 170 + x]; // Use original dimensions for portrait
            } else {
                pixel = currentBg[y * SCREEN_WIDTH + x];
            }
            if (pixel != 0) {
                tft.drawPixel(x, y, highlightColor);
            }
        }
    }

    // Adjust text positioning based on orientation
    int tempX = (rotation == 1 || rotation == 3) ? SCREEN_WIDTH - rightPadding - 6 : 170 - rightPadding - 6;
    
    tft.setFreeFont(&OpenSans_Bold10pt7b);
    tft.setTextColor(highlightColor);
    tft.setTextDatum(TL_DATUM);
    
    const char* menuName = currentMenu ? currentMenu->name : "Menu";
    tft.drawString(menuName, leftPadding, HEADER_Y);
    
    tft.setTextDatum(TR_DATUM);
    float temperature = temperatureRead();
    String tempString = String(temperature, 1) + " C";
    
    // Draw red rectangle behind temperature
    int tempWidth = tft.textWidth(tempString);
    int tempHeight = 20; // Approximate height of text
    int rectX = tempX - tempWidth - 5; // 5px padding
    int rectY = HEADER_Y - 2; // 2px padding above text
    tft.fillRoundRect(rectX, rectY, tempWidth + 10, tempHeight, 4, TFT_RED); // 10px total padding, 4px radius
    
    tft.drawString(tempString, tempX, HEADER_Y);
    
    tft.setFreeFont(&OpenSans_Regular8pt7b);
    staticItemsDrawn = true;
}

void backOption() {
    if (currentMenu != nullptr && currentMenu->previousMenu != nullptr) {
        Menu* temp = currentMenu;
        currentMenu = currentMenu->previousMenu;
        delete temp;
        
        selectedMenuItem = 0;
        currentPage = 0;
        
        staticItemsDrawn = false;
        staticItems();
             
        // Then redraw the menu items
        int totalItems = countMenuItems(currentMenu->items);
        int startIdx = currentPage * getItemsPerPage();
        int endIdx = min(startIdx + getItemsPerPage(), totalItems);
      
        for (int i = startIdx; i < endIdx; i++) {
            int yPos = yOffset + (i - startIdx) * itemSpacing;
            tft.setFreeFont(i == selectedMenuItem ? &OpenSans_Bold10pt7b : &OpenSans_Regular8pt7b);
            tft.setTextColor(highlightColor);
            tft.setTextDatum(TL_DATUM);
            tft.drawString(currentMenu->items[i], leftPadding, yPos);
        }
        

        if (currentPage > 0) {
            drawUpArrow();
        }
        if ((currentPage + 1) * getItemsPerPage() < totalItems) {
            drawDownArrow();
        }
    }
}

void drawUpArrow() {
    uint8_t rotation = tft.getRotation();
    int triangleYPos = yOffset + 10;
    int arrowX;
    
    if (rotation == 1 || rotation == 3) {  // Landscape
        arrowX = SCREEN_WIDTH - rightPadding - 15;
    } else {  // Portrait
        arrowX = 160;  // 10px from right edge in portrait mode
    }
    
    tft.fillTriangle(
        arrowX - 5, triangleYPos,
        arrowX + 5, triangleYPos,
        arrowX, triangleYPos - 5,
        highlightColor
    );
}

void drawDownArrow() {
    uint8_t rotation = tft.getRotation();
    int triangleYPos;
    int arrowX;
    
    if (rotation == 1 || rotation == 3) {  // Landscape
        triangleYPos = SCREEN_HEIGHT - 20;
        arrowX = SCREEN_WIDTH - rightPadding - 15;
    } else {  // Portrait
        triangleYPos = 280;  // Keep current Y position
        arrowX = 160;  // 10px from right edge in portrait mode
    }
    
    tft.fillTriangle(
        arrowX - 5, triangleYPos,
        arrowX + 5, triangleYPos,
        arrowX, triangleYPos + 5,
        highlightColor
    );
}

void userInterface(const char* menuName, const char** items, void (*selectionHandler)(int)) {
    staticItemsDrawn = false;
    Menu* newMenu = new Menu;
    newMenu->name = menuName;
    newMenu->items = items;
    newMenu->previousMenu = currentMenu;
    newMenu->selectionHandler = selectionHandler;
    currentMenu = newMenu;
    selectedMenuItem = 0;
    currentPage = 0;
    lastSelectedMenuItem = -1;
    lastPage = -1;
    encoderUpdated = true;

    tft.fillScreen(backgroundColor);  
    
    staticItems();
    setupEncoder();

    int totalItems = countMenuItems(items);
    static bool buttonPressed = false;
    unsigned long lastTempUpdate = 0;
    const unsigned long TEMP_UPDATE_INTERVAL = 1000; // Update temperature every 1 second

    int startIdx = currentPage * getItemsPerPage();
    int endIdx = min(startIdx + getItemsPerPage(), totalItems);
    
    uint8_t rotation = tft.getRotation();
    int currentWidth = (rotation == 1 || rotation == 3) ? SCREEN_WIDTH : 170;
    
    for (int i = startIdx; i < endIdx; i++) {
        int yPos = yOffset + (i - startIdx) * itemSpacing;
        tft.setFreeFont(i == selectedMenuItem ? &OpenSans_Bold10pt7b : &OpenSans_Regular8pt7b);
        tft.setTextColor(highlightColor);
        tft.setTextDatum(TL_DATUM);
        
        // Truncate text if it's too long for the current orientation
        String itemText = items[i];
        while (tft.textWidth(itemText) > currentWidth - (leftPadding + rightPadding)) {
            itemText = itemText.substring(0, itemText.length() - 1);
        }
        
        tft.drawString(itemText, leftPadding, yPos);
    }
    
    if ((currentPage + 1) * getItemsPerPage() < totalItems) {
        drawDownArrow();
    }
    
    staticItemsDrawn = true;

    while (true) {
        // Check wakeup switch for back functionality
        static bool wakeupPressed = false;
        int wakeupState = digitalRead(PIN_WAKEUP_SWITCH);
        
        if (wakeupState == LOW && !wakeupPressed) {
            wakeupPressed = true;
            Serial.println("Wakeup switch pressed");
            delay(50); // Simple debounce
        }
        else if (wakeupState == HIGH && wakeupPressed) {
            wakeupPressed = false;
            Serial.println("Wakeup switch released");
            delay(50); // Simple debounce
            backOption();
            return; // Exit the current menu
        }

        unsigned long currentMillis = millis();
        
        // Update temperature every TEMP_UPDATE_INTERVAL
        if (currentMillis - lastTempUpdate >= TEMP_UPDATE_INTERVAL) {
            tft.setFreeFont(&OpenSans_Bold10pt7b);
            tft.setTextColor(highlightColor);
            tft.setTextDatum(TR_DATUM);
            float temperature = temperatureRead();
            String tempString = String(temperature, 1) + " C";
            
            // Clear the old temperature area first - adjust width based on orientation
            uint8_t rotation = tft.getRotation();
            int clearWidth, clearHeight;
            int clearX, clearY;
            
            if (rotation == 1 || rotation == 3) {  // Landscape
                clearWidth = 50;
                clearHeight = 20;
                clearX = SCREEN_WIDTH - rightPadding - clearWidth - 6;
                clearY = 9;
            } else {  // Portrait
                clearWidth = 45;
                clearHeight = 20;
                clearX = 120;
                clearY = 9;
            }
            
            // Clear previous temperature
            tft.fillRect(clearX, clearY, clearWidth, clearHeight, backgroundColor);
            
            // Draw new temperature
            int tempX = (rotation == 1 || rotation == 3) ? 
                SCREEN_WIDTH - rightPadding - 6 : 
                160 - 6;  // Keep the text position the same
            
            // Draw red rectangle behind temperature
            int tempWidth = tft.textWidth(tempString);
            int tempHeight = 20; // Approximate height of text
            int rectX = tempX - tempWidth - 6; // 5px padding
            int rectY = HEADER_Y - 2; // 2px padding above text
            tft.fillRoundRect(rectX, rectY, tempWidth + 10, tempHeight, 4, TFT_RED); // 10px total padding, 4px radius
            
            tft.drawString(tempString, tempX, HEADER_Y);
            lastTempUpdate = currentMillis;
        }

        if (encoderUp || encoderDown) {
            if (encoderUp) {
                if (selectedMenuItem > 0) {
                    selectedMenuItem--;
                }
                encoderUp = false;
            } else {
                if (selectedMenuItem < totalItems - 1) {
                    selectedMenuItem++;
                }
                encoderDown = false;
            }
            
            currentPage = selectedMenuItem / getItemsPerPage();
            staticItemsDrawn = false;
            // Print the actual menu item name instead of just the index
            Serial.println("Selection: " + String(items[selectedMenuItem]));
        }

        // Modified button handling for faster response
        int buttonState = digitalRead(PIN_ENCODER_SWITCH);
        
        if (buttonState == LOW && !buttonPressed) {
            buttonPressed = true;
            Serial.println("Encoder switch pressed");
            delayMicroseconds(10000); // 10ms debounce using microseconds
        } 
        else if (buttonState == HIGH && buttonPressed) {
            buttonPressed = false;
            Serial.println("Encoder switch released");
            delayMicroseconds(10000); // 10ms debounce using microseconds
            
            // Remove the check for last item, just call the selection handler
            selectionHandler(selectedMenuItem);
        }

        if (!staticItemsDrawn) {
            // Adjust black rectangle behind the list based on orientation
            uint8_t rotation = tft.getRotation();
            int rectWidth, rectX;
            int rectHeight = (rotation == 1 || rotation == 3) ? 
                125 :  // Landscape height
                314 - 39;  // Portrait height: 314 (screen height - 6px) minus starting Y position (39)
            
            if (rotation == 1 || rotation == 3) {  // Landscape
                rectX = 10;
                rectWidth = SCREEN_WIDTH - 20;  // 320 - 20 = 300px wide
            } else {  // Portrait
                rectX = 6;  // 6px from left edge
                rectWidth = 158;  // 170 - 12 = 158px wide (6px padding on each side)
            }
            
            tft.fillRoundRect(rectX, 39, rectWidth, rectHeight, 8, backgroundColor);
            
            int startIdx = currentPage * getItemsPerPage();
            int endIdx = min(startIdx + getItemsPerPage(), totalItems);
            
            for (int i = startIdx; i < endIdx; i++) {
                int yPos = yOffset + (i - startIdx) * itemSpacing;
                
                tft.setFreeFont(i == selectedMenuItem ? &OpenSans_Bold10pt7b : &OpenSans_Regular8pt7b);
                tft.setTextColor(highlightColor);
                tft.setTextDatum(TL_DATUM);
                
                // Truncate text if it's too long for the current orientation
                String itemText = items[i];
                while (tft.textWidth(itemText) > currentWidth - (leftPadding + rightPadding)) {
                    itemText = itemText.substring(0, itemText.length() - 1);
                }
                
                tft.drawString(itemText, leftPadding, yPos);
            }
            
            if (currentPage > 0) {
                drawUpArrow();
            }
            if ((currentPage + 1) * getItemsPerPage() < totalItems) {
                drawDownArrow();
            }
            
            staticItemsDrawn = true;
        }

        yield();
        delayMicroseconds(5000); // 5ms delay using microseconds
    
    }
}

void cleanupMenu() {
    while (currentMenu != nullptr) {
        Menu* temp = currentMenu;
        currentMenu = currentMenu->previousMenu;
        delete temp;
    }
}

int countMenuItems(const char* items[]) {
    int count = 0;
    while (items[count] != nullptr) {
        count++;
    }
    return count;
}
