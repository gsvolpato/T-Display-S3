#include "battery_settings.h"
#include "Arduino.h"
#include "GPIOS.h"
#include "esp_adc_cal.h"

extern lv_obj_t* battery_label;
extern esp_adc_cal_characteristics_t adc_chars;
extern uint32_t last_battery_check;

static const uint32_t BATTERY_CHECK_INTERVAL = 30000;

void update_battery_percentage(int percentage) {
    if (battery_label != NULL) {
        char buffer[5];
        snprintf(buffer, sizeof(buffer), "%d%%", percentage);
        lv_label_set_text(battery_label, buffer);
    }
}

void check_battery_level() {
    uint32_t current_time = millis();
    if (current_time - last_battery_check >= BATTERY_CHECK_INTERVAL) {
        uint32_t raw = 0;
        const int samples = 5;
        for(int i = 0; i < samples; i++) {
            raw += analogRead(PIN_BAT_VOLT);
            delay(1);
        }
        raw /= samples;
        
        uint32_t voltage = esp_adc_cal_raw_to_voltage(raw, &adc_chars);
        voltage *= 2;
        
        int percentage;
        if (voltage >= 4200) {
            percentage = 100;
        } else if (voltage <= 3300) {
            percentage = 0;
        } else {
            percentage = ((voltage - 3300) * 100) / 900;
        }

        Serial.print("Raw ADC: ");
        Serial.print(raw);
        Serial.print(" Voltage: ");
        Serial.print(voltage);
        Serial.print("mV Battery: ");
        Serial.print(percentage);
        Serial.println("%");
        
        update_battery_percentage(percentage);
        last_battery_check = current_time;
    }
}
