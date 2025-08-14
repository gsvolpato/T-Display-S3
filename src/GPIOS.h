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

//| LCD Configuration
#define EXAMPLE_LCD_PIXEL_CLOCK_HZ  (16 * 1000 * 1000)
#define EXAMPLE_LCD_H_RES           320
#define EXAMPLE_LCD_V_RES           170
#define LVGL_LCD_BUF_SIZE          (EXAMPLE_LCD_H_RES * EXAMPLE_LCD_V_RES)
#define EXAMPLE_PSRAM_DATA_ALIGNMENT 64

//| ESP32S3 LCD Pins
#define PIN_LCD_BL                  38
#define PIN_LCD_D0                  39
#define PIN_LCD_D1                  40
#define PIN_LCD_D2                  41
#define PIN_LCD_D3                  42
#define PIN_LCD_D4                  45
#define PIN_LCD_D5                  46
#define PIN_LCD_D6                  47
#define PIN_LCD_D7                  48
#define PIN_LCD_RES                 5
#define PIN_LCD_CS                  6
#define PIN_LCD_DC                  7
#define PIN_LCD_WR                  8
#define PIN_LCD_RD                  9

//| I2C and Touch
#define PIN_IIC_SCL                 17
#define PIN_IIC_SDA                 18
#define PIN_TOUCH_INT               16
#define PIN_TOUCH_RES               21

//| Function declarations
extern bool rfidActive;
extern bool nrfActive;
void deinitRFID();
void deinitNRF24();
void setupGPIOPins();

#endif // GPIOS_H