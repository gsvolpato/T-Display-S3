#include "lvgl.h"
#include "Arduino.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_vendor.h"
#include "GPIOS.h"
#include "menu_squares.h"
#include "battery_settings.h"
#include "encoder_settings.h"
#include <math.h>
#include "esp_adc_cal.h"

esp_lcd_panel_io_handle_t io_handle = NULL;
static lv_disp_draw_buf_t disp_buf;
static lv_disp_drv_t disp_drv;
static lv_color_t *lv_disp_buf;
static bool is_initialized_lvgl = false;

// Menu state
int current_square = 0;
lv_obj_t* squares[9] = {NULL};  // Changed to 9 squares
static volatile long encoderValue = 0;
static const int ENCODER_STEPS_PER_DETENT = 2;
static const int ENCODER_DEBOUNCE_MS = 0;
volatile bool encoderUp = false;
volatile bool encoderDown = false;


static const int SQUARE_WIDTH = 110;
static const int SQUARE_HEIGHT = 90;
static const int SQUARE_SPACING = 25;
static const int SELECTED_SQUARE_WIDTH = SQUARE_WIDTH + 30;
static const int SELECTED_SQUARE_HEIGHT = SQUARE_HEIGHT + 30;
static const int VERTICAL_OFFSET = 10;

// Add these near the top with other static constants
static const int TOP_BAR_HEIGHT = 20;
static const int TOP_BAR_MARGIN = 4;
static lv_obj_t* top_bar = NULL;
lv_obj_t* battery_label = NULL;

esp_adc_cal_characteristics_t adc_chars;
uint32_t last_battery_check = 0;

void create_squares();
void update_squares_position(bool animate);
void create_top_bar();



void create_top_bar() {
    // Create top rectangle
    top_bar = lv_obj_create(lv_scr_act());
    lv_obj_set_size(top_bar, EXAMPLE_LCD_H_RES - (TOP_BAR_MARGIN * 2), TOP_BAR_HEIGHT);
    lv_obj_set_pos(top_bar, TOP_BAR_MARGIN, TOP_BAR_MARGIN);
    lv_obj_set_style_radius(top_bar, 4, 0);  // Slightly rounded corners
    lv_obj_set_style_bg_color(top_bar, lv_color_hex(0xFFFFFF), 0);  // White background
    lv_obj_set_style_bg_opa(top_bar, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(top_bar, 0, 0);

    // Create battery label
    battery_label = lv_label_create(lv_scr_act());
    lv_obj_set_style_text_color(battery_label, lv_color_hex(0x000000), 0);  // Black text
    lv_label_set_text(battery_label, "100%");  // Initial text
    lv_obj_align(battery_label, LV_ALIGN_TOP_RIGHT, -5, TOP_BAR_MARGIN + (TOP_BAR_HEIGHT - 16)/2);  // Centered vertically in the bar
}

void create_squares() {
    // Change square color to white
    const lv_color_t square_color = lv_color_hex(0xFFFFFF);  // White

    // Create all squares with adjusted vertical position
    for (int i = 0; i < 9; i++) {
        squares[i] = lv_obj_create(lv_scr_act());
        lv_obj_set_size(squares[i], SQUARE_WIDTH, SQUARE_HEIGHT);
        lv_obj_set_style_radius(squares[i], 10, 0);
        lv_obj_set_style_bg_color(squares[i], square_color, 0);
        lv_obj_set_style_bg_opa(squares[i], LV_OPA_COVER, 0);
        lv_obj_set_style_border_width(squares[i], 0, 0);

        // Add initial shadow styles (will be updated in update_squares_position)
        lv_obj_set_style_shadow_width(squares[i], 15, 0);
        lv_obj_set_style_shadow_color(squares[i], lv_color_darken(square_color, LV_OPA_30), 0);
        lv_obj_set_style_shadow_spread(squares[i], 0, 0);
        
        // Create and position the image
        if (square_configs[i].image) {
            lv_obj_t* img = lv_img_create(squares[i]);
            lv_img_set_src(img, (const void*)square_configs[i].image);
            lv_obj_center(img); // Center the image in the square
        }
    }
    
    update_squares_position(false);
}

void update_squares_position(bool animate) {
    int center_x = EXAMPLE_LCD_H_RES / 2;
    // Adjust center_y to be more centered between top bar and bottom
    int available_height = EXAMPLE_LCD_V_RES - (TOP_BAR_HEIGHT + TOP_BAR_MARGIN * 2);
    int center_y = (available_height / 2) + (TOP_BAR_HEIGHT + TOP_BAR_MARGIN * 2) - 2;
    
    for (int i = 0; i < 9; i++) {
        // Calculate position based on index relative to current_square
        int relative_pos = (i - current_square);
        
        // Handle wraparound for smoother transitions
        if (relative_pos > 4) relative_pos -= 9;
        if (relative_pos < -4) relative_pos += 9;
        
        // Calculate target position and size
        int target_width = (i == current_square) ? SELECTED_SQUARE_WIDTH : SQUARE_WIDTH;
        int target_height = (i == current_square) ? SELECTED_SQUARE_HEIGHT : SQUARE_HEIGHT;
        
        int base_x = center_x - (target_width / 2) + (relative_pos * (SQUARE_WIDTH + SQUARE_SPACING));
        int target_y = center_y - (target_height / 2);

        // Determine if square is "behind" based on relative position
        bool is_behind = abs(relative_pos) > 2;

        // Calculate shadow offset based on position
        int shadow_x = -relative_pos * 2;  // More offset for squares further from center
        int shadow_y = abs(relative_pos) * 2 + 3;  // Increase shadow height with distance
        
        // Update shadow style
        lv_obj_set_style_shadow_width(squares[i], 15, 0);
        lv_obj_set_style_shadow_ofs_x(squares[i], shadow_x, 0);
        lv_obj_set_style_shadow_ofs_y(squares[i], shadow_y, 0);
        
        // Adjust shadow opacity based on position
        uint8_t shadow_opa = is_behind ? 0 : (LV_OPA_50 - (abs(relative_pos) * 10));
        lv_obj_set_style_shadow_opa(squares[i], shadow_opa, 0);
        
        if (animate) {
            // Reduce animation time for faster response
            const uint16_t ANIM_TIME = 100;  // Reduced from 150ms to 100ms
            
            lv_anim_t a;
            lv_anim_init(&a);
            lv_anim_set_var(&a, squares[i]);
            lv_anim_set_time(&a, ANIM_TIME);
            lv_anim_set_path_cb(&a, lv_anim_path_ease_out);
            
            // Animate X position
            lv_anim_set_values(&a, lv_obj_get_x(squares[i]), base_x);
            lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_x);
            lv_anim_start(&a);
            
            // Animate Y position
            lv_anim_set_values(&a, lv_obj_get_y(squares[i]), target_y);
            lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_y);
            lv_anim_start(&a);
            
            // Set opacity immediately based on position
            lv_obj_set_style_opa(squares[i], is_behind ? LV_OPA_0 : LV_OPA_COVER, 0);
            
            // Update size animation duration as well
            lv_anim_t size_anim;
            lv_anim_init(&size_anim);
            lv_anim_set_var(&size_anim, squares[i]);
            lv_anim_set_time(&size_anim, ANIM_TIME);
            
            // Animate width and height separately
            lv_anim_set_values(&size_anim, lv_obj_get_width(squares[i]), target_width);
            lv_anim_set_exec_cb(&size_anim, [](void* obj, int32_t v) {
                lv_obj_set_width((lv_obj_t*)obj, v);
            });
            lv_anim_start(&size_anim);

            lv_anim_set_values(&size_anim, lv_obj_get_height(squares[i]), target_height);
            lv_anim_set_exec_cb(&size_anim, [](void* obj, int32_t v) {
                lv_obj_set_height((lv_obj_t*)obj, v);
                // Recenter the icon after size change
                lv_obj_t* img = lv_obj_get_child(((lv_obj_t*)obj), 0);
                if (img) {
                    lv_obj_center(img);
                }
            });
            lv_anim_start(&size_anim);
        } else {
            // Set position and properties immediately
            lv_obj_set_pos(squares[i], base_x, target_y);
            lv_obj_set_size(squares[i], target_width, target_height);
            lv_obj_set_style_opa(squares[i], is_behind ? LV_OPA_0 : LV_OPA_COVER, 0);
        }

        // Update z-index based on position
        if (is_behind) {
            lv_obj_move_background(squares[i]);
        } else {
            lv_obj_move_foreground(squares[i]);
        }
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

#if defined(LCD_MODULE_CMD_1)
    for (uint8_t i = 0; i < (sizeof(lcd_st7789v) / sizeof(lcd_cmd_t)); i++) {
        esp_lcd_panel_io_tx_param(io_handle, lcd_st7789v[i].cmd, lcd_st7789v[i].data, lcd_st7789v[i].len & 0x7f);
        if (lcd_st7789v[i].len & 0x80)
            delay(120);
    }
#endif

    // Initialize backlight
    ledcSetup(0, 10000, 8);
    ledcAttachPin(PIN_LCD_BL, 0);
    const uint8_t BRIGHTNESS = 64;  // 25% of 255 (0xFF)
    ledcWrite(0, BRIGHTNESS);

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

    // Set background color to#e6e6e6
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0xe6e6e6), 0);
    lv_obj_set_style_bg_opa(lv_scr_act(), LV_OPA_COVER, 0);

    // Disable scrollbars
    lv_obj_set_style_pad_all(lv_scr_act(), 0, 0);
    lv_obj_set_scrollbar_mode(lv_scr_act(), LV_SCROLLBAR_MODE_OFF);

    is_initialized_lvgl = true;

    // Initialize encoder pins with pullup resistors
    pinMode(PIN_ENCODER_CLK, INPUT_PULLUP);
    pinMode(PIN_ENCODER_DT, INPUT_PULLUP);
    pinMode(PIN_ENCODER_SWITCH, INPUT_PULLUP);
    pinMode(PIN_WAKEUP_SWITCH, INPUT_PULLUP);
    pinMode(PIN_POWER_ON, OUTPUT);
    digitalWrite(PIN_POWER_ON, HIGH);
    
    
    setupEncoder();

    // Create top bar first
    create_top_bar();

    // Create and show all squares
    create_squares();

    // Initialize ADC with proper configuration
    adcAttachPin(PIN_BAT_VOLT);
    analogReadResolution(12);  // Set ADC resolution to 12 bits
    analogSetAttenuation(ADC_11db);  // Set attenuation for higher voltage range
    
    // Initialize ADC calibration with proper reference voltage
    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, &adc_chars);
    
    // Initial battery reading
    check_battery_level();
}

void loop() {
    lv_timer_handler();
    
    // Handle encoder immediately
    handle_encoder();
    
    // Check battery level (internal timing control)
    check_battery_level();
    
    // Handle switches
    static bool last_encoder_switch = HIGH;
    static bool last_wakeup_switch = HIGH;
    
    bool current_encoder_switch = digitalRead(PIN_ENCODER_SWITCH);
    if (current_encoder_switch == LOW && last_encoder_switch == HIGH) {
        delayMicroseconds(2000);  // Reduced debounce time
        if (digitalRead(PIN_ENCODER_SWITCH) == LOW) {
            handle_encoder_switch();
        }
    }
    last_encoder_switch = current_encoder_switch;
    
    bool current_wakeup_switch = digitalRead(PIN_WAKEUP_SWITCH);
    if (current_wakeup_switch == LOW && last_wakeup_switch == HIGH) {
        delayMicroseconds(2000);  // Reduced debounce time
        if (digitalRead(PIN_WAKEUP_SWITCH) == LOW) {
            digitalWrite(PIN_POWER_ON, LOW);
        }
    }
    last_wakeup_switch = current_wakeup_switch;
    
    // Minimal delay for system tasks
    delayMicroseconds(500);  // Reduced from 1000 to 500
}