#include <Arduino.h>
#include "DisplayConfig.h"

void setup() {
    Serial.begin(115200);
    
    display_init();

    // Create a red square
    lv_obj_t *square = lv_obj_create(lv_scr_act());
    lv_obj_set_size(square, 100, 100);
    lv_obj_set_style_bg_color(square, lv_color_make(255, 0, 0), LV_PART_MAIN);
    lv_obj_center(square);
}

void loop() {
    lv_timer_handler();
    delay(5);
}
