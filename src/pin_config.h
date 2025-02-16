#pragma once

/* Encoder pins */
#define PIN_ENCODER_SWITCH           0
#define PIN_WAKEUP_SWITCH           13
#define PIN_ENCODER_CLK             12
#define PIN_ENCODER_DT              11

/* LCD CONFIG */
#define EXAMPLE_LCD_PIXEL_CLOCK_HZ   (16 * 1000 * 1000)
#define EXAMPLE_LCD_H_RES            320
#define EXAMPLE_LCD_V_RES            170
#define LVGL_LCD_BUF_SIZE           (EXAMPLE_LCD_H_RES * EXAMPLE_LCD_V_RES)
#define EXAMPLE_PSRAM_DATA_ALIGNMENT 64

/*ESP32S3 LCD Pins*/
#define PIN_LCD_BL                   38

#define PIN_LCD_D0                   39
#define PIN_LCD_D1                   40
#define PIN_LCD_D2                   41
#define PIN_LCD_D3                   42
#define PIN_LCD_D4                   45
#define PIN_LCD_D5                   46
#define PIN_LCD_D6                   47
#define PIN_LCD_D7                   48

#define PIN_POWER_ON                 15

#define PIN_LCD_RES                  5
#define PIN_LCD_CS                   6
#define PIN_LCD_DC                   7
#define PIN_LCD_WR                   8
#define PIN_LCD_RD                   9

#define PIN_IIC_SCL                  17
#define PIN_IIC_SDA                  18

#define PIN_TOUCH_INT                16
#define PIN_TOUCH_RES                21
