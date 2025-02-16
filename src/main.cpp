#include <Arduino.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include <MFRC522.h>
#include "skull_logo_s3.h"  // Include the background image

// Pin definitions
#define PIN_POWER_ON 15
#define PIN_RFID_RST 18
#define PIN_RFID_SDA 21  // SDA = SS pin
#define PIN_RFID_MISO 13 // Changed from 10
#define PIN_RFID_MOSI 11 // Changed from 3
#define PIN_RFID_SCK 12  // Changed from 43

// Screen dimensions
#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 170

TFT_eSPI tft = TFT_eSPI();
MFRC522 rfid(PIN_RFID_SDA, PIN_RFID_RST);

unsigned long lastCardTime = 0;
const unsigned long DISPLAY_TIMEOUT = 10000; // 10 seconds timeout
bool showingCard = false;

void dump_byte_array(byte *buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
}

void readRFIDBlocks() {
    MFRC522::MIFARE_Key key;
    for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;  // Factory default key

    Serial.println(F("\n===================="));
    Serial.println(F("Card Detected:"));
    Serial.print(F("Card UID: "));
    for (byte i = 0; i < rfid.uid.size; i++) {
        Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
        Serial.print(rfid.uid.uidByte[i], HEX);
    }
    Serial.println();
    
    MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
    Serial.print(F("PICC Type: "));
    Serial.println(rfid.PICC_GetTypeName(piccType));

    // Try to authenticate and read each sector
    byte sector = 1;
    byte trailerBlock = sector * 4 + 3;
    byte status;

    // Authenticate using key A
    Serial.println(F("\nAuthenticating using key A..."));
    status = rfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(rfid.uid));
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("PCD_Authenticate() failed: "));
        Serial.println(rfid.GetStatusCodeName(status));
        return;
    }

    // Read sector blocks
    Serial.println(F("\nData from sector:"));
    for (byte blockAddr = sector * 4; blockAddr < (sector * 4 + 3); blockAddr++) {
        // Read block
        byte buffer[18];
        byte size = sizeof(buffer);
        status = rfid.MIFARE_Read(blockAddr, buffer, &size);
        if (status != MFRC522::STATUS_OK) {
            Serial.print(F("MIFARE_Read() failed: "));
            Serial.println(rfid.GetStatusCodeName(status));
            continue;
        }

        // Print block contents
        Serial.print(F("Block ")); Serial.print(blockAddr); Serial.print(F(": "));
        for (byte i = 0; i < 16; i++) {
            Serial.print(buffer[i] < 0x10 ? " 0" : " ");
            Serial.print(buffer[i], HEX);
        }
        Serial.println();
    }

    Serial.println(F("\nStopping crypto..."));
    rfid.PICC_HaltA(); // Halt PICC
    rfid.PCD_StopCrypto1(); // Stop encryption on PCD
}

void displayCardInfo(String uid, String type) {
    showingCard = true;
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    
    // Display header
    tft.setTextSize(3);
    tft.setCursor(10, 20);
    tft.println("Card Detected!");
    
    // Display UID
    tft.setTextSize(2);
    tft.setCursor(10, 70);
    tft.print("UID: ");
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.println(uid);
    
    // Display card type
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.setCursor(10, 100);
    tft.print("Type: ");
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.println(type);
    
    // Display instruction
    tft.setTextColor(TFT_CYAN, TFT_BLACK);
    tft.setCursor(10, SCREEN_HEIGHT - 30);
    tft.println("Check Serial Monitor...");
    
    lastCardTime = millis();
}

String getCardType(MFRC522::PICC_Type piccType) {
    switch (piccType) {
        case MFRC522::PICC_TYPE_MIFARE_MINI:  return "MIFARE Mini";
        case MFRC522::PICC_TYPE_MIFARE_1K:    return "MIFARE 1K";
        case MFRC522::PICC_TYPE_MIFARE_4K:    return "MIFARE 4K";
        case MFRC522::PICC_TYPE_MIFARE_UL:    return "MIFARE Ultralight";
        case MFRC522::PICC_TYPE_ISO_14443_4:  return "ISO 14443-4";
        case MFRC522::PICC_TYPE_ISO_18092:    return "ISO 18092";
        default:                               return "Unknown";
    }
}

void displayWelcomeMessage() {
    showingCard = false;
    tft.pushImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, skull_logo_s3);
}

void setup() {
    Serial.begin(115200);
    while (!Serial); // Wait for serial port to connect
    
    // Setup display power
    pinMode(PIN_POWER_ON, OUTPUT);
    digitalWrite(PIN_POWER_ON, HIGH);
    
    // Initialize display
    tft.init();
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);
    
    // Initialize RFID
    SPI.begin(PIN_RFID_SCK, PIN_RFID_MISO, PIN_RFID_MOSI, PIN_RFID_SDA);
    rfid.PCD_Init();
    delay(4);  // Optional, but recommended delay
    
    // Test if RFID module is responding
    byte v = rfid.PCD_ReadRegister(rfid.VersionReg);
    Serial.print(F("MFRC522 Software Version: 0x"));
    Serial.println(v, HEX);
    if (v == 0x00 || v == 0xFF) {
        Serial.println(F("Warning: Communication failure, is the MFRC522 properly connected?"));
        tft.setTextColor(TFT_RED, TFT_BLACK);
        tft.setTextSize(2);
        tft.setCursor(10, SCREEN_HEIGHT/2);
        tft.println("RFID Reader Error!");
        while (1); // Don't continue if RFID reader is not responding
    }
    
    rfid.PCD_SetAntennaGain(rfid.RxGain_max); // Set maximum antenna gain
    
    Serial.println(F("RFID Reader initialized successfully."));
    Serial.println(F("Scan a card to begin..."));
    
    // Display welcome message
    displayWelcomeMessage();
}

void loop() {
    // Check if we need to return to welcome screen after timeout
    if (showingCard && (millis() - lastCardTime > DISPLAY_TIMEOUT)) {
        displayWelcomeMessage();
    }
    
    // Check for new cards
    if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
        delay(50);
        return;
    }
    
    // Get card UID
    String uid = "";
    for (byte i = 0; i < rfid.uid.size; i++) {
        uid += (rfid.uid.uidByte[i] < 0x10 ? "0" : "");
        uid += String(rfid.uid.uidByte[i], HEX);
        if (i < rfid.uid.size - 1) {
            uid += ":";
        }
    }
    uid.toUpperCase();
    
    // Get card type
    String type = getCardType(rfid.PICC_GetType(rfid.uid.sak));
    
    // Display card information on screen
    displayCardInfo(uid, type);
    
    // Read and display all blocks
    readRFIDBlocks();
}
