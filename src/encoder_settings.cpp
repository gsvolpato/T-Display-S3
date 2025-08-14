#include "encoder_settings.h"
#include "Arduino.h"
#include "GPIOS.h"
#include "menu_squares.h"

extern int current_square;
extern lv_obj_t* squares[];
extern volatile bool encoderUp;
extern volatile bool encoderDown;
extern const square_config_t square_configs[];

void update_squares_position(bool animate);

void IRAM_ATTR updateEncoder() {
    static uint8_t lastState = 0;
    uint8_t currentState = (digitalRead(PIN_ENCODER_CLK) << 1) | digitalRead(PIN_ENCODER_DT);
    
    if (lastState != currentState) {
        if ((lastState == 0b00 && currentState == 0b01) ||
            (lastState == 0b01 && currentState == 0b11) ||
            (lastState == 0b11 && currentState == 0b10) ||
            (lastState == 0b10 && currentState == 0b00)) {
            encoderDown = true;
        } else if (
            (lastState == 0b00 && currentState == 0b10) ||
            (lastState == 0b10 && currentState == 0b11) ||
            (lastState == 0b11 && currentState == 0b01) ||
            (lastState == 0b01 && currentState == 0b00)) {
            encoderUp = true;
        }
        lastState = currentState;
    }
}

void setupEncoder() {
    pinMode(PIN_ENCODER_CLK, INPUT_PULLUP);
    pinMode(PIN_ENCODER_DT, INPUT_PULLUP);
    
    attachInterrupt(digitalPinToInterrupt(PIN_ENCODER_CLK), updateEncoder, CHANGE);
    attachInterrupt(digitalPinToInterrupt(PIN_ENCODER_DT), updateEncoder, CHANGE);
    
    Serial.println("Encoder setup complete");
}

void handle_encoder() {
    if (encoderUp) {
        Serial.println("Counter-clockwise");
        current_square = (current_square - 1 + 9) % 9;
        update_squares_position(true);
        encoderUp = false;
    }
    
    if (encoderDown) {
        Serial.println("Clockwise");
        current_square = (current_square + 1) % 9;
        update_squares_position(true);
        encoderDown = false;
    }
}

void handle_encoder_switch() {
    lv_obj_set_style_bg_opa(squares[current_square], LV_OPA_50, 0);
    delay(100);
    lv_obj_set_style_bg_opa(squares[current_square], LV_OPA_COVER, 0);
    
    if (square_configs[current_square].action) {
        square_configs[current_square].action();
    }
}
