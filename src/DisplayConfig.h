#ifndef DISPLAY_CONFIG_H
#define DISPLAY_CONFIG_H

#include <Arduino.h>
#include <lvgl.h>
#include "GPIOS.h"

void display_init();
void my_disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map);

#endif
