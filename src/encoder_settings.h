#ifndef ENCODER_SETTINGS_H
#define ENCODER_SETTINGS_H

#include "Arduino.h"

void IRAM_ATTR updateEncoder();
void setupEncoder();
void handle_encoder();
void handle_encoder_switch();

// Forward declaration for main.cpp function
void update_squares_position(bool animate);

#endif // ENCODER_SETTINGS_H