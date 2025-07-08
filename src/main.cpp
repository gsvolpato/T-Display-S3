/**
 * RFID Card Reader with Google Sheets Logging
 * 
 * Main application file for T-Display S3 + RC522 RFID system
 * See README.md for complete setup instructions
 */

#include <Arduino.h>
#include <TFT_eSPI.h>
#include "skull_logo_s3.h"
#include "GPIOS.h"
#include "config.h"
#include "rfid_handler.h"
#include "google_forms.h"

const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;

const char* formURL = GOOGLE_FORM_URL;
const char* uidFieldName = UID_FIELD_ID;
const char* typeFieldName = TYPE_FIELD_ID;
const char* timestampFieldName = TIMESTAMP_FIELD_ID;

TFT_eSPI tft = TFT_eSPI();
RFIDHandler rfidHandler;
GoogleFormsHandler googleForms(ssid, password, formURL, uidFieldName, typeFieldName, timestampFieldName);

unsigned long lastCardTime = 0;
bool showingCard = false;

void displayWiFiStatus(bool connecting = false) {
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_CYAN, TFT_BLACK);
    tft.setTextSize(2);
    tft.setCursor(10, 50);
    
    if (connecting) {
        tft.println("Connecting to WiFi...");
    } else if (googleForms.isWiFiConnected()) {
        tft.setTextColor(TFT_GREEN, TFT_BLACK);
        tft.println("WiFi Connected!");
        tft.setCursor(10, 80);
        tft.print("IP: ");
        tft.println(googleForms.getLocalIP());
        delay(2000);
    } else {
        tft.setTextColor(TFT_RED, TFT_BLACK);
        tft.println("WiFi Failed!");
        tft.setCursor(10, 80);
        tft.println("Check credentials");
        delay(3000);
    }
}

void displayCardInfo(String uid, String type) {
    showingCard = true;
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    
    tft.setTextSize(3);
    tft.setCursor(10, 20);
    tft.println("Card Detected!");
    
    tft.setTextSize(2);
    tft.setCursor(10, 70);
    tft.print("UID: ");
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.println(uid);
    
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.setCursor(10, 100);
    tft.print("Type: ");
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.println(type);
    
    tft.setTextSize(1);
    tft.setCursor(10, 130);
    if (googleForms.isWiFiConnected()) {
        tft.setTextColor(TFT_GREEN, TFT_BLACK);
        tft.print("WiFi: Connected | ");
        tft.setTextColor(TFT_CYAN, TFT_BLACK);
        tft.println("Logging to Google Sheets");
    } else {
        tft.setTextColor(TFT_RED, TFT_BLACK);
        tft.println("WiFi: Disconnected | Local only");
    }
    
    tft.setTextColor(TFT_CYAN, TFT_BLACK);
    tft.setCursor(10, SCREEN_HEIGHT - 20);
    tft.println("Check Serial Monitor for details...");
    
    lastCardTime = millis();
}

void displayWelcomeMessage() {
    showingCard = false;
    tft.pushImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, skull_logo_s3);
}

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    pinMode(PIN_POWER_ON, OUTPUT);
    digitalWrite(PIN_POWER_ON, HIGH);
    
    tft.init();
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);
    
    if (!rfidHandler.initialize()) {
        tft.setTextColor(TFT_RED, TFT_BLACK);
        tft.setTextSize(2);
        tft.setCursor(10, SCREEN_HEIGHT/2);
        tft.println("RFID Reader Error!");
        delay(5000);
    }
    
    displayWiFiStatus(true);
    googleForms.connectToWiFi();
    displayWiFiStatus(false);
    
    Serial.println(F("Scan a card to begin..."));
    
    displayWelcomeMessage();
}

void loop() {
    if (showingCard && (millis() - lastCardTime > DISPLAY_TIMEOUT)) {
        displayWelcomeMessage();
    }
    
    CardInfo cardInfo = rfidHandler.readCard();
    
    if (cardInfo.isValid) {
        displayCardInfo(cardInfo.uid, cardInfo.type);
        rfidHandler.readAllBlocks();
        googleForms.sendCardData(cardInfo.uid, cardInfo.type);
    }
    
    delay(50);
}
