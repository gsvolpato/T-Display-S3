#include <Arduino.h>
#include <TFT_eSPI.h>
#include "GPIOS.h"

TFT_eSPI tft = TFT_eSPI();

//| Encoder variables
volatile int encoderPos = 50;
volatile bool encoderChanged = false;
volatile bool encoderSwitchPressed = false;

int currentBrightness = 64;  // Start at 25% brightness like the working example
const int MIN_BRIGHTNESS = 0;
const int MAX_BRIGHTNESS = 255;
const int ENCODER_ACCELERATION = 3;  // Reduced to match working example

const int pwmFreq = 10000;    // Changed to 10KHz like working example
const int pwmChannel = 0;
const int pwmResolution = 8;
const int backlightPin = TFT_BL;  // Using the defined pin

bool backlightOn = true;
int lastBrightness = 64;  // Start at 25% brightness

//| Encoder interrupt handler
void IRAM_ATTR handleEncoder() {
    static uint8_t old_AB = 3;  
    static int8_t encval = 0;
    static const int8_t enc_states[] = {0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0};
    
    old_AB <<= 2;
    old_AB |= (digitalRead(PIN_ENCODER_CLK) << 1) | digitalRead(PIN_ENCODER_DT);
    encval += enc_states[old_AB & 0x0f];

    if (encval > 3) {
        encoderPos -= ENCODER_ACCELERATION;
        encval = 0;
        encoderChanged = true;
    }
    else if (encval < -3) {
        encoderPos += ENCODER_ACCELERATION;
        encval = 0;
        encoderChanged = true;
    }
}

// Switch debounce handler
void IRAM_ATTR handleEncoderSwitch() {
    static unsigned long lastDebounceTime = 0;
    unsigned long currentTime = millis();
    
    if (currentTime - lastDebounceTime > 50) {
        encoderSwitchPressed = true;
        lastDebounceTime = currentTime;
    }
}

void updateDisplay(int percentage) {
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawNumber(percentage, tft.width()/2, tft.height()/2);
    tft.drawString("%", tft.width()/2 + 50, tft.height()/2);
    if (!backlightOn) {
        tft.drawString("OFF", tft.width()/2, tft.height()/2 + 40);
    }
}

void setBrightness(int percentage) {
    int pwmValue;
    if (!backlightOn) {
        pwmValue = 0;
    } else {
        // Map percentage (0-100) to PWM range (0-255)
        pwmValue = map(percentage, 0, 100, 0, 255);
    }
    ledcWrite(pwmChannel, pwmValue);
    Serial.printf("Setting PWM to: %d\n", pwmValue);
}

void setup() {
    Serial.begin(115200);
    
    // Initialize power pin like in working example
    pinMode(PIN_POWER_ON, OUTPUT);
    digitalWrite(PIN_POWER_ON, HIGH);
    
    pinMode(PIN_ENCODER_CLK, INPUT_PULLUP);
    pinMode(PIN_ENCODER_DT, INPUT_PULLUP);
    pinMode(PIN_ENCODER_SWITCH, INPUT_PULLUP);
    
    // Initialize PWM for backlight with same settings as working example
    ledcSetup(pwmChannel, pwmFreq, pwmResolution);
    ledcAttachPin(backlightPin, pwmChannel);
    ledcWrite(pwmChannel, currentBrightness);  // Start at 25% brightness
    
    attachInterrupt(digitalPinToInterrupt(PIN_ENCODER_CLK), handleEncoder, CHANGE);
    attachInterrupt(digitalPinToInterrupt(PIN_ENCODER_DT), handleEncoder, CHANGE);
    attachInterrupt(digitalPinToInterrupt(PIN_ENCODER_SWITCH), handleEncoderSwitch, FALLING);
    
    tft.init();
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);
    tft.setTextDatum(MC_DATUM);
    tft.setTextSize(3);
    
    setBrightness(encoderPos);
    updateDisplay(encoderPos);
}

void loop() {
    if (encoderChanged) {
        if (encoderPos > 100) encoderPos = 100;
        if (encoderPos < 0) encoderPos = 0;
        
        setBrightness(encoderPos);
        updateDisplay(encoderPos);
        
        encoderChanged = false;
    }
    
    if (encoderSwitchPressed && digitalRead(PIN_ENCODER_SWITCH) == HIGH) {
        encoderSwitchPressed = false;
        backlightOn = !backlightOn;
        
        setBrightness(encoderPos);
        updateDisplay(encoderPos);
    }
    
    delay(1);
}