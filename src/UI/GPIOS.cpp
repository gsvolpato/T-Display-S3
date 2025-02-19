#define DISABLE_ALL_LIBRARY_WARNINGS
#include <Arduino.h>
#include "GPIOS.h"
#include <SPI.h>

bool rfidActive = false;
bool nrfActive = false;

void setupGPIOPins() {
//| Encoder & Power
    pinMode(PIN_ENCODER_SWITCH, INPUT_PULLUP);
    pinMode(PIN_WAKEUP_SWITCH, INPUT_PULLUP);
    pinMode(PIN_ENCODER_CLK, INPUT_PULLUP);
    pinMode(PIN_ENCODER_DT, INPUT_PULLUP);
    pinMode(PIN_POWER_ON, OUTPUT);
    digitalWrite(PIN_POWER_ON, HIGH);

//| RF Modules SPI Pins
    pinMode(PIN_RF_MISO, INPUT);
    pinMode(PIN_RF_MOSI, OUTPUT);
    pinMode(PIN_RF_SCK, OUTPUT);

//| CC1101 Specific
    pinMode(PIN_CC1101_CSN, OUTPUT);
    digitalWrite(PIN_CC1101_CSN, HIGH);
    pinMode(PIN_CC1101_GD00, INPUT);

//| nRF24L01 Specific
    pinMode(PIN_NRF_CE, OUTPUT);
    digitalWrite(PIN_NRF_CE, LOW);
    pinMode(PIN_NRF_CNS, OUTPUT);
    digitalWrite(PIN_NRF_CNS, HIGH);
    pinMode(PIN_NRF_IRQ, INPUT);

//| RC522 RFID Specific
    pinMode(RST_PIN, OUTPUT);
    digitalWrite(RST_PIN, HIGH);
    pinMode(IRQ_PIN, INPUT);
    pinMode(SS_PIN, OUTPUT);
    digitalWrite(SS_PIN, HIGH);
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
