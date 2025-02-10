#ifndef GPIOS_H
#define GPIOS_H

//| LCD pins
#define PIN_LCD_BL                  38
#define PIN_LCD_DC                  7
#define PIN_LCD_RES                 5
#define PIN_LCD_CS                  6
#define PIN_LCD_WR                  8
#define PIN_LCD_D0                  39
#define PIN_LCD_D1                  40
#define PIN_LCD_D2                  41
#define PIN_LCD_D3                  42
#define PIN_LCD_D4                  45
#define PIN_LCD_D5                  46
#define PIN_LCD_D6                  47
#define PIN_LCD_D7                  48
#define PIN_LCD_RD                  9

//| LCD CONFIG
#define EXAMPLE_LCD_H_RES           170
#define EXAMPLE_LCD_V_RES           320
#define LVGL_LCD_BUF_SIZE          (EXAMPLE_LCD_H_RES * EXAMPLE_LCD_V_RES)
#define EXAMPLE_LCD_PIXEL_CLOCK_HZ  (16 * 1000 * 1000)
#define EXAMPLE_PSRAM_DATA_ALIGNMENT 64

//| ENCODER PINS
#define PIN_ENCODER_CLK             12
#define PIN_ENCODER_DT              11

//| SWITCHES    
#define PIN_ENCODER_SWITCH           0
#define PIN_WAKEUP_SWITCH           3  // Changed from 13 to 3

//| POWER AND BATTERY
#define PIN_POWER_ON                15
#define PIN_BAT_VOLT                4
#define BATTERY_CHECK_INTERVAL      5000

//| RF Modules SPI Pins
#define PIN_RF_MISO                 10  // Shared by CC1101, nRF24L01, RC522
#define PIN_RF_MOSI                 13  // Shared by CC1101, nRF24L01, RC522
#define PIN_RF_SCK                  43  // Shared by CC1101, nRF24L01, RC522

//| CC1101 Specific
#define PIN_CC1101_CSN              1
#define PIN_CC1101_GD00             2

//| nRF24L01 Specific
#define PIN_NRF_CE                  16
#define PIN_NRF_CNS                 17
#define PIN_NRF_IRQ                 18

//| RC522 RFID Specific
#define RST_PIN                     18  // RC522 RST pin
#define SS_PIN                      21  // RC522 SDA (SS) pin
#define IRQ_PIN                     44  // RC522 IRQ pin

#endif