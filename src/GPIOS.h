#define DISABLE_ALL_LIBRARY_WARNINGS
#ifndef GPIOS_H
#define GPIOS_H

//| Power
#define PIN_POWER_ON 15
#define PIN_BAT_VOLT 4

//| Switches
#define PIN_WAKEUP_SWITCH 3
#define PIN_ENCODER_SWITCH 0

//| Rotary Encoder
#define PIN_ENCODER_CLK 12
#define PIN_ENCODER_DT 11

//| RF Modules SPI Pins (to avoid conflict with TFT SPI pins)
#define PIN_RF_MISO 10  // Shared by CC1101, nRF24L01, RC522
#define PIN_RF_MOSI 13   // Shared by CC1101, nRF24L01, RC522
#define PIN_RF_SCK 43   // Shared by CC1101, nRF24L01, RC522

//| RFID RC522 specific pins
#define PIN_RFID_RST RST_PIN
#define PIN_RFID_SDA SS_PIN
#define PIN_RFID_MISO PIN_RF_MISO
#define PIN_RFID_MOSI PIN_RF_MOSI
#define PIN_RFID_SCK PIN_RF_SCK

//| CC1101
#define PIN_CC1101_CSN 1
#define PIN_CC1101_GD00 2

//| nRF24L01
#define PIN_NRF_CE 16
#define PIN_NRF_CNS 17
#define PIN_NRF_IRQ 18

//| RC522
#define RST_PIN 18      // RC522 RST pin
#define SS_PIN 21       // RC522 SDA (SS) pin
#define IRQ_PIN 44      // RC522 IRQ pin

#endif // GPIOS_H