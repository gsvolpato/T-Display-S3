#define DISABLE_ALL_LIBRARY_WARNINGS
#include <Arduino.h>
#include <SPI.h>
#include "nRF24Settings.h"
#include "../UI/GPIOS.h"

void setupNRF24() {
    // First, deinitialize RFID if it's active
    deinitRFID();
    
    // Now initialize nRF24
    SPI.begin(PIN_RF_SCK, PIN_RF_MISO, PIN_RF_MOSI, PIN_NRF_CNS);
    digitalWrite(PIN_NRF_CE, LOW);    // Start in standby mode
    digitalWrite(PIN_NRF_CNS, HIGH);  // Deselect nRF
    nrfActive = true;
    Serial.println("nRF24 initialized");
}