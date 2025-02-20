#define DISABLE_ALL_LIBRARY_WARNINGS
#include <Arduino.h>
#include <TFT_eSPI.h>
#include "GPIOS.h"
#include <SPI.h>

bool rfidActive = false;
bool nrfActive = false;

void setupGPIOPins() {
    Serial.println("setupGPIOPins()");
//| Power
    pinMode(PIN_POWER_ON, OUTPUT);
    digitalWrite(PIN_POWER_ON, HIGH);
    Serial.println("Power Pins OK");
//| Rotary Encoder
    pinMode(PIN_ENCODER_CLK, INPUT_PULLUP);
    pinMode(PIN_ENCODER_DT, INPUT_PULLUP);
    Serial.println("Encoder Pins OK");
//| Switches
    pinMode(PIN_ENCODER_SWITCH, INPUT_PULLUP);
    pinMode(PIN_WAKEUP_SWITCH, INPUT_PULLUP);
    Serial.println("Switch Pins OK");
//| RF Modules SPI Pins
    pinMode(PIN_RF_MISO, INPUT);
    pinMode(PIN_RF_MOSI, OUTPUT);
    pinMode(PIN_RF_SCK, OUTPUT);
    Serial.println("RF Modules SPI Pins OK");
//| CC1101 Specific
    pinMode(PIN_CC1101_CSN, OUTPUT);
    digitalWrite(PIN_CC1101_CSN, HIGH);
    pinMode(PIN_CC1101_GD00, INPUT);
    Serial.println("CC1101 Pins OK");
//| nRF24L01 Specific
    pinMode(PIN_NRF_CE, OUTPUT);
    digitalWrite(PIN_NRF_CE, LOW);
    pinMode(PIN_NRF_CNS, OUTPUT);
    digitalWrite(PIN_NRF_CNS, HIGH);
    pinMode(PIN_NRF_IRQ, INPUT);
    Serial.println("nRF24L01 Pins OK");
//| RC522 RFID Specific
    pinMode(RST_PIN, OUTPUT);
    digitalWrite(RST_PIN, HIGH);
    pinMode(IRQ_PIN, INPUT);
    pinMode(SS_PIN, OUTPUT);
    digitalWrite(SS_PIN, HIGH);
    Serial.println("RFID Pins OK");
//| TFT Backlight
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);
    Serial.println("TFT Backlight OK");
    Serial.println("All GPIOs OK");
}

void deinitRFID() {
    if (rfidActive) {
        digitalWrite(RST_PIN, LOW);
        digitalWrite(SS_PIN, HIGH);
        rfidActive = false;
        Serial.println("RFID deinitialized");
    }
}

void deinitNRF24() {
    if (nrfActive) {
        digitalWrite(PIN_NRF_CE, LOW);    // Put nRF in standby
        digitalWrite(PIN_NRF_CNS, HIGH);  // Deselect nRF
        nrfActive = false;
        Serial.println("nRF24 deinitialized");
    }
}
