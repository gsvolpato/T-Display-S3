#ifndef GPIOS_H
#define GPIOS_H

// Encoder & Power
#define PIN_ENCODER_SWITCH 0
#define PIN_WAKEUP_SWITCH 3
#define PIN_ENCODER_CLK 12
#define PIN_ENCODER_DT 11
#define PIN_POWER_ON 15
#define PIN_BAT_VOLT 4

// RF Modules SPI Pins (to avoid conflict with TFT SPI pins)
#define PIN_RF_MISO 10  // Shared by CC1101, nRF24L01, RC522
#define PIN_RF_MOSI 13   // Shared by CC1101, nRF24L01, RC522
#define PIN_RF_SCK 43   // Shared by CC1101, nRF24L01, RC522

// CC1101 Specific
#define PIN_CC1101_CSN 1
#define PIN_CC1101_GD00 2

// nRF24L01 Specific
#define PIN_NRF_CE 16
#define PIN_NRF_CNS 17
#define PIN_NRF_IRQ 18

// RC522 RFID Specific
#define RST_PIN 18      // RC522 RST pin
#define SS_PIN 21       // RC522 SDA (SS) pin
#define IRQ_PIN 44      // RC522 IRQ pin

extern bool rfidActive;
extern bool nrfActive;
void deinitRFID();
void deinitNRF24();

void setupGPIOPins();

#endif // GPIOS_H