#define DISABLE_ALL_LIBRARY_WARNINGS
#include <Arduino.h>
#include <esp_sleep.h>
#include <TFT_eSPI.h>
#include "UI/UISettings.h"
#include "UI/GPIOS.h"
#include "UI/StartupSettings.h"
#include "UI/MenuLists.h"

TFT_eSPI tft = TFT_eSPI();  // Initialize TFT object

void setup() {
    delay(5000); 
    Serial.begin(115200);
    Serial.println("booting up");    
    startUp();
}

void loop() {
    
}
