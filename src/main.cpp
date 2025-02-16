#include "lvgl.h"
#include "Arduino.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_vendor.h"
#include "pin_config.h"

esp_lcd_panel_io_handle_t io_handle = NULL;
static lv_disp_draw_buf_t disp_buf;
static lv_disp_drv_t disp_drv;
static lv_color_t *lv_disp_buf;
static bool is_initialized_lvgl = false;

// Single slider for brightness control
static lv_obj_t* brightness_slider = NULL;
static lv_obj_t* brightness_label = NULL;
static volatile long encoderValue = 0;
static const int ENCODER_STEPS_PER_DETENT = 4;
static volatile bool encoderUp = false;
static volatile bool encoderDown = false;

// Function declarations
void IRAM_ATTR updateEncoder();
void setupEncoder();
void handle_encoder();
void create_brightness_slider();
void update_brightness(int32_t value);
void update_brightness_label(int32_t value);

// Encoder interrupt handler
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

void setupEncoder() {
    pinMode(PIN_ENCODER_CLK, INPUT_PULLUP);
    pinMode(PIN_ENCODER_DT, INPUT_PULLUP);
    
    detachInterrupt(digitalPinToInterrupt(PIN_ENCODER_CLK));
    detachInterrupt(digitalPinToInterrupt(PIN_ENCODER_DT));
    attachInterrupt(digitalPinToInterrupt(PIN_ENCODER_CLK), updateEncoder, CHANGE);
    attachInterrupt(digitalPinToInterrupt(PIN_ENCODER_DT), updateEncoder, CHANGE);
    Serial.println("Encoder setup complete");
}

void create_brightness_slider() {
    // Create the brightness slider
    brightness_slider = lv_slider_create(lv_scr_act());
    
    // Create label for brightness value
    brightness_label = lv_label_create(lv_scr_act());
    lv_obj_set_style_text_font(brightness_label, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(brightness_label, lv_color_hex(0xFFB000), 0);
    
    // Set size and position
    lv_obj_set_size(brightness_slider, 240, 20);
    lv_obj_align(brightness_slider, LV_ALIGN_CENTER, 0, 0);
    
    // Position label above slider
    lv_obj_align_to(brightness_label, brightness_slider, LV_ALIGN_OUT_TOP_MID, 0, -10);
    
    // Set range (0-255 for brightness)
    lv_slider_set_range(brightness_slider, 0, 255);
    lv_slider_set_value(brightness_slider, 64, LV_ANIM_OFF);
    update_brightness_label(64);

    // Style for main slider part
    lv_obj_set_style_bg_color(brightness_slider, lv_color_darken(lv_color_hex(0xFFB000), LV_OPA_30), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(brightness_slider, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_radius(brightness_slider, 10, LV_PART_MAIN);
    
    // Add shadow for better visibility
    lv_obj_set_style_shadow_width(brightness_slider, 15, LV_PART_MAIN);
    lv_obj_set_style_shadow_color(brightness_slider, lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_set_style_shadow_opa(brightness_slider, LV_OPA_30, LV_PART_MAIN);

    // Style for indicator part
    lv_obj_set_style_bg_color(brightness_slider, lv_color_hex(0xFFB000), LV_PART_INDICATOR);
    lv_obj_set_style_bg_opa(brightness_slider, LV_OPA_COVER, LV_PART_INDICATOR);

    // Style for knob part
    lv_obj_set_style_bg_color(brightness_slider, lv_color_hex(0xFFB000), LV_PART_KNOB);
    lv_obj_set_style_bg_opa(brightness_slider, LV_OPA_COVER, LV_PART_KNOB);
    lv_obj_set_style_radius(brightness_slider, 10, LV_PART_KNOB);
    lv_obj_set_style_pad_all(brightness_slider, 6, LV_PART_KNOB);
}

void update_brightness_label(int32_t value) {
    static char buf[8];
    lv_snprintf(buf, sizeof(buf), "%d%%", (value * 100) / 255);
    lv_label_set_text(brightness_label, buf);
}

void update_brightness(int32_t value) {
    // Update the actual screen brightness
    ledcWrite(0, value);
    
    // Update the label
    update_brightness_label(value);
    
    // Print value to Serial for debugging
    Serial.printf("Brightness: %d\n", value);
}

void handle_encoder() {
    if (encoderUp || encoderDown) {
        // Get current brightness value
        int32_t value = lv_slider_get_value(brightness_slider);
        
        // Update value based on encoder direction (inverted and multiplied by 3)
        if (encoderUp) {
            value = (value < 255 - 3) ? value + 3 : 255;  // Inverted: up increases value
            encoderUp = false;
        }
        if (encoderDown) {
            value = (value > 3) ? value - 3 : 0;  // Inverted: down decreases value
            encoderDown = false;
        }
        
        // Set new value with animation
        lv_slider_set_value(brightness_slider, value, LV_ANIM_ON);
        
        // Update the actual brightness
        update_brightness(value);
    }
}

// LVGL display callbacks
static bool example_notify_lvgl_flush_ready(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_io_event_data_t *edata, void *user_ctx) {
    if (is_initialized_lvgl) {
        lv_disp_drv_t *disp_driver = (lv_disp_drv_t *)user_ctx;
        lv_disp_flush_ready(disp_driver);
    }
    return false;
}

static void example_lvgl_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map) {
    esp_lcd_panel_handle_t panel_handle = (esp_lcd_panel_handle_t)drv->user_data;
    int offsetx1 = area->x1;
    int offsetx2 = area->x2;
    int offsety1 = area->y1;
    int offsety2 = area->y2;
    esp_lcd_panel_draw_bitmap(panel_handle, offsetx1, offsety1, offsetx2 + 1, offsety2 + 1, color_map);
}

void setup() {
    Serial.begin(115200);

    // Initialize power and display pins
    pinMode(PIN_POWER_ON, OUTPUT);
    digitalWrite(PIN_POWER_ON, HIGH);
    pinMode(PIN_LCD_RD, OUTPUT);
    digitalWrite(PIN_LCD_RD, HIGH);

    // Initialize LCD
    esp_lcd_i80_bus_handle_t i80_bus = NULL;
    esp_lcd_i80_bus_config_t bus_config = {
        .dc_gpio_num = PIN_LCD_DC,
        .wr_gpio_num = PIN_LCD_WR,
        .clk_src = LCD_CLK_SRC_PLL160M,
        .data_gpio_nums = {
            PIN_LCD_D0,
            PIN_LCD_D1,
            PIN_LCD_D2,
            PIN_LCD_D3,
            PIN_LCD_D4,
            PIN_LCD_D5,
            PIN_LCD_D6,
            PIN_LCD_D7,
        },
        .bus_width = 8,
        .max_transfer_bytes = LVGL_LCD_BUF_SIZE * sizeof(uint16_t),
        .psram_trans_align = 0,
        .sram_trans_align = 0
    };
    esp_lcd_new_i80_bus(&bus_config, &i80_bus);

    esp_lcd_panel_io_i80_config_t io_config = {
        .cs_gpio_num = PIN_LCD_CS,
        .pclk_hz = EXAMPLE_LCD_PIXEL_CLOCK_HZ,
        .trans_queue_depth = 20,
        .on_color_trans_done = example_notify_lvgl_flush_ready,
        .user_ctx = &disp_drv,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
        .dc_levels = {
            .dc_idle_level = 0,
            .dc_cmd_level = 0,
            .dc_dummy_level = 0,
            .dc_data_level = 1,
        },
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_i80(i80_bus, &io_config, &io_handle));

    esp_lcd_panel_handle_t panel_handle = NULL;
    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = PIN_LCD_RES,
        .color_space = ESP_LCD_COLOR_SPACE_RGB,
        .bits_per_pixel = 16,
        .vendor_config = NULL
    };
    esp_lcd_new_panel_st7789(io_handle, &panel_config, &panel_handle);
    esp_lcd_panel_reset(panel_handle);
    esp_lcd_panel_init(panel_handle);
    esp_lcd_panel_invert_color(panel_handle, true);
    esp_lcd_panel_swap_xy(panel_handle, true);
    esp_lcd_panel_mirror(panel_handle, true, false);
    esp_lcd_panel_set_gap(panel_handle, 0, 35);

    // Initialize backlight PWM
    ledcSetup(0, 10000, 8);
    ledcAttachPin(PIN_LCD_BL, 0);
    ledcWrite(0, 64);  // Start at 25% brightness

    // Initialize LVGL
    lv_init();
    lv_disp_buf = (lv_color_t *)heap_caps_malloc(LVGL_LCD_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    lv_disp_draw_buf_init(&disp_buf, lv_disp_buf, NULL, LVGL_LCD_BUF_SIZE);
    
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = EXAMPLE_LCD_H_RES;
    disp_drv.ver_res = EXAMPLE_LCD_V_RES;
    disp_drv.flush_cb = example_lvgl_flush_cb;
    disp_drv.draw_buf = &disp_buf;
    disp_drv.user_data = panel_handle;
    lv_disp_drv_register(&disp_drv);

    is_initialized_lvgl = true;

    // Initialize encoder pins
    pinMode(PIN_ENCODER_CLK, INPUT_PULLUP);
    pinMode(PIN_ENCODER_DT, INPUT_PULLUP);
    pinMode(PIN_ENCODER_SWITCH, INPUT_PULLUP);
    pinMode(PIN_WAKEUP_SWITCH, INPUT_PULLUP);
    pinMode(PIN_POWER_ON, OUTPUT);
    digitalWrite(PIN_POWER_ON, HIGH);
    
    setupEncoder();

    // Create the brightness slider
    create_brightness_slider();
}

void loop() {
    lv_timer_handler();
    
    static uint32_t last_encoder_check = 0;
    static bool last_wakeup_switch = HIGH;
    
    // Handle encoder with debounce
    if (millis() - last_encoder_check > 5) {  // Check every 5ms
        handle_encoder();
        last_encoder_check = millis();
    }

    // Handle wakeup switch for power control with debounce
    bool current_wakeup_switch = digitalRead(PIN_WAKEUP_SWITCH);
    if (current_wakeup_switch == LOW && last_wakeup_switch == HIGH) {
        delayMicroseconds(10000); // 10ms debounce
        if (digitalRead(PIN_WAKEUP_SWITCH) == LOW) {
            digitalWrite(PIN_POWER_ON, LOW);
        }
    }
    last_wakeup_switch = current_wakeup_switch;

    delayMicroseconds(5000); // 5ms delay
}