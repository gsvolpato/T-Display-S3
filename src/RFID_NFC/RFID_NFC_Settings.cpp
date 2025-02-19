#define DISABLE_ALL_LIBRARY_WARNINGS
#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <TFT_eSPI.h>
#include "../UI/DisplayConfig.h"
#include "../UI/UISettings.h"
#include "../UI/GPIOS.h"
#include "../UI/fonts/OpenSans_Bold10pt7b.h"
#include "../UI/fonts/OpenSans_Regular8pt7b.h"
#include "RFID_NFC_Settings.h"

extern TFT_eSPI tft;
MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

void setupRFID() {
    // First, deinitialize nRF24 if it's active
    deinitNRF24();
    
    // Now initialize RFID
    SPI.begin(PIN_RF_SCK, PIN_RF_MISO, PIN_RF_MOSI, SS_PIN);
    mfrc522.PCD_Init();
    delay(4);
    mfrc522.PCD_DumpVersionToSerial();
    rfidActive = true;
    Serial.println("RFID initialized");
}

void rfidScanContinuous() {
    Serial.println("\nStarting continuous RFID scan...");
    Serial.println("------------------------");
    
    staticItems();
    tft.setTextDatum(TL_DATUM);
    tft.setFreeFont(&OpenSans_Bold10pt7b);
    tft.drawString("Waiting for RFID card...", 15, 45);
    tft.setFreeFont(&OpenSans_Regular8pt7b);
    tft.drawString("Press encoder switch to exit", 15, 70);

    setupRFID();
    bool scanning = true;
    MFRC522::MIFARE_Key key;
    
    // Default keys array
    const byte knownKeys[][MFRC522::MF_KEY_SIZE] = {
        {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}, // Factory default
        {0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5}, // Known key A
        {0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5}, // Known key B
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}  // Null key
    };

    while (scanning) {
        if (digitalRead(PIN_ENCODER_SWITCH) == LOW) {
            delay(50);
            if (digitalRead(PIN_ENCODER_SWITCH) == LOW) {
                staticItems();
                tft.drawString("Scan ended", 15, 45);
                Serial.println("Scan ended by user");
                delay(1000);
                return;
            }
        }

        if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
            delay(50);
            continue;
        }

        // Get card details
        String uid = "";
        for (byte i = 0; i < mfrc522.uid.size; i++) {
            if (mfrc522.uid.uidByte[i] < 0x10) uid += "0";
            uid += String(mfrc522.uid.uidByte[i], HEX);
            if (i < mfrc522.uid.size - 1) uid += ":";
        }
        uid.toUpperCase();

        // Get card type and size
        MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
        String cardType = String(mfrc522.PICC_GetTypeName(piccType));
        
        String sizeInfo;
        switch (piccType) {
            case MFRC522::PICC_TYPE_MIFARE_MINI:
                sizeInfo = "320 bytes";
                break;
            case MFRC522::PICC_TYPE_MIFARE_1K:
                sizeInfo = "1K bytes";
                break;
            case MFRC522::PICC_TYPE_MIFARE_4K:
                sizeInfo = "4K bytes";
                break;
            default:
                sizeInfo = "Unknown";
                break;
        }

        // Print card details header
        Serial.println("\nCard Details:");
        Serial.println("------------------------");
        Serial.println("UID: " + uid);
        Serial.println("Type: " + cardType);
        Serial.println("Size: " + sizeInfo);
        Serial.println("------------------------");

        // Print dump header
        Serial.println("\nDump Editor (UID: " + uid + ")\n");

        // Read sectors
        for (byte sector = 0; sector < 16; sector++) {
            Serial.printf("\nSector: %d\n", sector);
            bool authenticated = false;

            // Try authentication with all known keys
            for (byte k = 0; k < 4 && !authenticated; k++) {
                for (byte i = 0; i < MFRC522::MF_KEY_SIZE; i++) {
                    key.keyByte[i] = knownKeys[k][i];
                }

                // Try both Key A and Key B
                if (mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, sector * 4 + 3, &key, &(mfrc522.uid)) == MFRC522::STATUS_OK ||
                    mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, sector * 4 + 3, &key, &(mfrc522.uid)) == MFRC522::STATUS_OK) {
                    authenticated = true;
                    break;
                }
            }

            if (authenticated) {
                // Read all blocks in sector
                for (byte block = 0; block < 4; block++) {
                    byte buffer[18];
                    byte size = sizeof(buffer);
                    byte blockAddr = sector * 4 + block;

                    if (mfrc522.MIFARE_Read(blockAddr, buffer, &size) == MFRC522::STATUS_OK) {
                        for (byte i = 0; i < 16; i++) {
                            if (buffer[i] < 0x10) Serial.print("0");
                            Serial.print(String(buffer[i], HEX));
                        }
                        Serial.println();
                    }
                }
                break;
            }

            mfrc522.PCD_StopCrypto1();
        }

        Serial.println("\n------------------------");
        
        // Display on TFT
        staticItems();
        tft.setTextDatum(TL_DATUM);
        tft.setFreeFont(&OpenSans_Bold10pt7b);
        tft.drawString("Card Detected!", 15, 45);
        tft.setFreeFont(&OpenSans_Regular8pt7b);
        tft.drawString("UID: " + uid, 15, 75);
        tft.drawString("Type: " + cardType, 15, 95);
        tft.drawString("Size: " + sizeInfo, 15, 115);

        mfrc522.PICC_HaltA();
        mfrc522.PCD_StopCrypto1();

        while (mfrc522.PICC_IsNewCardPresent()) {
            delay(50);
        }
        
        delay(200);
    }
}

void rfidScan() {
    Serial.println("\nStarting RFID scan...");
    Serial.println("------------------------");
    
    staticItems();
    tft.setTextDatum(TL_DATUM);
    tft.fillRoundRect(10, 39, 300, 125, 8, backgroundColor);
    tft.setFreeFont(&OpenSans_Bold10pt7b);
    tft.drawString("Waiting for RFID card...", 15, 45);
    tft.setFreeFont(&OpenSans_Regular8pt7b);
    tft.drawString("Press wakeup switch to cancel", 15, 70);

    setupRFID();
    bool scanning = true;

    while (scanning) {
        // Check for wakeup switch press
        if (digitalRead(PIN_WAKEUP_SWITCH) == LOW) {
            delay(50);  // Simple debounce
            if (digitalRead(PIN_WAKEUP_SWITCH) == LOW) {
                tft.fillRoundRect(10, 39, 300, 125, 8, backgroundColor);
                tft.setFreeFont(&OpenSans_Bold10pt7b);
                tft.drawString("Scan cancelled", 15, 45);
                Serial.println("Scan cancelled by user");
                delay(1500);
                return;
            }
        }

        // Look for new cards
        if (!mfrc522.PICC_IsNewCardPresent()) {
            delay(50);
            continue;
        }

        // Select one of the cards
        if (!mfrc522.PICC_ReadCardSerial()) {
            delay(50);
            continue;
        }

        // Card detected - display info
        tft.fillRoundRect(10, 39, 300, 125, 8, backgroundColor);
        tft.setFreeFont(&OpenSans_Bold10pt7b);
        tft.drawString("Card Detected!", 15, 45);

        // Display UID
        String uid = "";
        for (byte i = 0; i < mfrc522.uid.size; i++) {
            uid += (mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
            uid += String(mfrc522.uid.uidByte[i], HEX);
            if (i < mfrc522.uid.size - 1) uid += ":";
        }
        uid.toUpperCase();

        tft.setFreeFont(&OpenSans_Regular8pt7b);
        tft.drawString("UID: " + uid, 15, 75);

        // Display card type
        MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
        String cardType = "Type: " + String(mfrc522.PICC_GetTypeName(piccType));
        tft.drawString(cardType, 15, 100);

        // Display size info
        String sizeInfo;
        switch (piccType) {
            case MFRC522::PICC_TYPE_MIFARE_MINI:
                sizeInfo = "320 bytes";
                break;
            case MFRC522::PICC_TYPE_MIFARE_1K:
                sizeInfo = "1K bytes";
                break;
            case MFRC522::PICC_TYPE_MIFARE_4K:
                sizeInfo = "4K bytes";
                break;
            default:
                sizeInfo = "Unknown";
                break;
        }
        tft.drawString("Size: " + sizeInfo, 15, 125);

        // Log to serial
        Serial.println("\nCard Details:");
        Serial.println("------------------------");
        Serial.println("UID: " + uid);
        Serial.println(cardType);
        Serial.println("Size: " + sizeInfo);
        Serial.println("------------------------\n");

        // Stop PICC
        mfrc522.PICC_HaltA();
        // Stop encryption on PCD
        mfrc522.PCD_StopCrypto1();

        delay(3000); // Display for 3 seconds
        
        // Clear and show waiting message again
        tft.fillRoundRect(10, 39, 300, 125, 8, backgroundColor);
        tft.setFreeFont(&OpenSans_Bold10pt7b);
        tft.drawString("Waiting for RFID card...", 15, 45);
        tft.setFreeFont(&OpenSans_Regular8pt7b);
        tft.drawString("Press wakeup switch to cancel", 15, 70);
    }
}

void rfidClone() {
    Serial.println("\nStarting RFID cloning...");
    Serial.println("------------------------");
    
    // Buffer for storing source card data
    byte sourceData[64][16];  // 64 blocks of 16 bytes each
    byte sourceUID[10];       // UID can be up to 10 bytes
    byte sourceUIDLength = 0;
    MFRC522::PICC_Type sourceType;
    
    staticItems();
    tft.setTextDatum(TL_DATUM);
    tft.setFreeFont(&OpenSans_Bold10pt7b);
    tft.drawString("Place source card", 15, 45);
    tft.setFreeFont(&OpenSans_Regular8pt7b);
    tft.drawString("Press wakeup switch to cancel", 15, 70);

    setupRFID();
    bool scanning = true;
    bool sourceRead = false;

    // Read source card
    while (scanning && !sourceRead) {
        // Check for wakeup switch press to cancel
        if (digitalRead(PIN_WAKEUP_SWITCH) == LOW) {
            delay(50);  // Simple debounce
            if (digitalRead(PIN_WAKEUP_SWITCH) == LOW) {
                backOption();
                return;
            }
        }

        // Look for cards
        if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
            delay(50);
            continue;
        }

        // Store source card type and UID
        sourceType = mfrc522.PICC_GetType(mfrc522.uid.sak);
        sourceUIDLength = mfrc522.uid.size;
        memcpy(sourceUID, mfrc522.uid.uidByte, sourceUIDLength);

        // Display source card info
        staticItems();
        tft.setTextDatum(TL_DATUM);
        tft.setFreeFont(&OpenSans_Bold10pt7b);
        tft.drawString("Source card detected!", 15, 45);
        
        String uid = "";
        for (byte i = 0; i < sourceUIDLength; i++) {
            uid += (sourceUID[i] < 0x10 ? "0" : "");
            uid += String(sourceUID[i], HEX);
            if (i < sourceUIDLength - 1) uid += ":";
        }
        uid.toUpperCase();

        tft.setTextDatum(TL_DATUM);
        tft.setFreeFont(&OpenSans_Regular8pt7b);
        tft.drawString("UID: " + uid, 15, 75);
        tft.drawString("Type: " + String(mfrc522.PICC_GetTypeName(sourceType)), 15, 100);

        // Try to read card data
        bool readSuccess = true;
        MFRC522::StatusCode status;
        byte trailerBlock;
        MFRC522::MIFARE_Key keyA;
        MFRC522::MIFARE_Key keyB;
        
        // Default keys
        for (byte i = 0; i < 6; i++) {
            keyA.keyByte[i] = 0xFF;  // Factory default key A
            keyB.keyByte[i] = 0xFF;  // Factory default key B
        }

        for (byte sector = 0; sector < 16; sector++) {
            trailerBlock = sector * 4 + 3;
            
            // First try with key A
            status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &keyA, &(mfrc522.uid));
            if (status != MFRC522::STATUS_OK) {
                // If key A fails, try key B
                status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, trailerBlock, &keyB, &(mfrc522.uid));
                if (status != MFRC522::STATUS_OK) {
                    Serial.printf("Authentication failed for sector %d\n", sector);
                    continue;
                }
            }

            // Read only data blocks (0,1,2) in the sector, skip trailer block (3)
            for (byte blockOffset = 0; blockOffset < 3; blockOffset++) {
                byte blockAddr = sector * 4 + blockOffset;  // Calculate absolute block address
                
                status = mfrc522.MIFARE_Read(blockAddr, sourceData[blockAddr], &blockAddr);
                if (status != MFRC522::STATUS_OK) {
                    Serial.printf("Read failed for block %d\n", blockAddr);
                    readSuccess = false;
                } else {
                    Serial.printf("Block %d read successfully\n", blockAddr);
                }
            }

            // Re-authenticate if needed (some cards require this)
            mfrc522.PCD_StopCrypto1();
            if (sector < 15) { // Don't need to re-authenticate after last sector
                status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &keyA, &(mfrc522.uid));
            }
        }

        mfrc522.PICC_HaltA();
        mfrc522.PCD_StopCrypto1();

        if (!readSuccess) {
            tft.setTextDatum(TL_DATUM);
            tft.drawString("Error reading some blocks", 15, 125);
            Serial.println("Some blocks failed to read");
            delay(2000);
            return;
        }

        sourceRead = true;
        tft.setTextDatum(TL_DATUM);
        tft.drawString("Read successful!", 15, 125);
        delay(2000);

        // Prompt for target card
        staticItems();
        tft.setTextDatum(TL_DATUM);
        tft.setFreeFont(&OpenSans_Bold10pt7b);
        tft.drawString("Place target card", 15, 45);
        tft.setFreeFont(&OpenSans_Regular8pt7b);
        tft.drawString("Press wakeup switch to cancel", 15, 70);

        // Wait for target card
        while (scanning) {
            // Check for cancel
            if (digitalRead(PIN_WAKEUP_SWITCH) == LOW) {
                delay(50);  // Simple debounce
                if (digitalRead(PIN_WAKEUP_SWITCH) == LOW) {
                    backOption();
                    return;
                }
            }

            if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
                delay(50);
                continue;
            }

            // Check if it's not the same card
            bool isSameCard = true;
            if (mfrc522.uid.size == sourceUIDLength) {
                for (byte i = 0; i < sourceUIDLength && isSameCard; i++) {
                    if (mfrc522.uid.uidByte[i] != sourceUID[i]) {
                        isSameCard = false;
                    }
                }
            } else {
                isSameCard = false;
            }

            if (isSameCard) {
                staticItems();
                tft.setTextDatum(TL_DATUM);
                tft.setFreeFont(&OpenSans_Bold10pt7b);
                tft.drawString("Please use a different card", 15, 45);
                delay(2000);
                staticItems();
                tft.setTextDatum(TL_DATUM);
                tft.setFreeFont(&OpenSans_Bold10pt7b);
                tft.drawString("Place target card", 15, 45);
                tft.setFreeFont(&OpenSans_Regular8pt7b);
                tft.drawString("Press wakeup switch to cancel", 15, 70);
                continue;
            }

            // Write data to target card
            bool writeSuccess = true;
            
            for (byte sector = 0; sector < 16; sector++) {
                trailerBlock = sector * 4 + 3;
                
                // Authenticate with key A
                status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &keyA, &(mfrc522.uid));
                if (status != MFRC522::STATUS_OK) {
                    Serial.printf("Authentication failed for sector %d\n", sector);
                    writeSuccess = false;
                    continue;
                }

                // Write only data blocks (0,1,2), skip trailer block (3)
                for (byte blockOffset = 0; blockOffset < 3; blockOffset++) {
                    byte blockAddr = sector * 4 + blockOffset;
                    
                    status = mfrc522.MIFARE_Write(blockAddr, sourceData[blockAddr], 16);
                    if (status != MFRC522::STATUS_OK) {
                        Serial.printf("Write failed for block %d\n", blockAddr);
                        writeSuccess = false;
                    } else {
                        Serial.printf("Block %d written successfully\n", blockAddr);
                    }
                }

                // Re-authenticate if needed
                mfrc522.PCD_StopCrypto1();
                if (sector < 15) { // Don't need to re-authenticate after last sector
                    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &keyA, &(mfrc522.uid));
                }
            }

            mfrc522.PICC_HaltA();
            mfrc522.PCD_StopCrypto1();

            // Show result
            staticItems();
            tft.setTextDatum(TL_DATUM);
            tft.setFreeFont(&OpenSans_Bold10pt7b);
            if (writeSuccess) {
                tft.drawString("Cloning successful!", 15, 45);
                Serial.println("Card cloned successfully");
            } else {
                tft.drawString("Cloning partially failed", 15, 45);
                tft.setTextDatum(TL_DATUM);
                tft.setFreeFont(&OpenSans_Regular8pt7b);
                tft.drawString("Some blocks could not be written", 15, 75);
                Serial.println("Some blocks failed to write");
            }
            
            delay(2000);
            return;
        }
    }
}

void formatRFIDCard() {
    Serial.println("\nStarting RFID format...");
    Serial.println("------------------------");
    
    staticItems();
    tft.setTextDatum(TL_DATUM);
    tft.setFreeFont(&OpenSans_Bold10pt7b);
    tft.drawString("Place card to format", 15, 45);
    tft.setFreeFont(&OpenSans_Regular8pt7b);
    tft.drawString("Press wakeup switch to cancel", 15, 70);

    setupRFID();
    bool formatting = true;
    MFRC522::MIFARE_Key keyA;
    
    // Default key
    for (byte i = 0; i < 6; i++) {
        keyA.keyByte[i] = 0xFF;  // Factory default key
    }

    // Prepare clean data block (all zeros)
    byte cleanBlock[16] = {0};

    while (formatting) {
        // Check for wakeup switch press to cancel
        if (digitalRead(PIN_WAKEUP_SWITCH) == LOW) {
            delay(50);  // Simple debounce
            if (digitalRead(PIN_WAKEUP_SWITCH) == LOW) {
                backOption();
                return;
            }
        }

        // Look for new cards
        if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
            delay(50);
            continue;
        }

        // Card detected - start formatting
        staticItems();
        tft.setTextDatum(TL_DATUM);
        tft.setFreeFont(&OpenSans_Bold10pt7b);
        tft.drawString("Formatting card...", 15, 45);
        
        bool formatSuccess = true;
        MFRC522::StatusCode status;

        // Format each sector
        for (byte sector = 0; sector < 16; sector++) {
            byte trailerBlock = sector * 4 + 3;
            
            // Authenticate sector
            status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &keyA, &(mfrc522.uid));
            if (status != MFRC522::STATUS_OK) {
                Serial.printf("Authentication failed for sector %d\n", sector);
                formatSuccess = false;
                continue;
            }

            // Write zeros to data blocks (0,1,2) in sector, skip trailer block (3)
            for (byte blockOffset = 0; blockOffset < 3; blockOffset++) {
                byte blockAddr = sector * 4 + blockOffset;
                
                status = mfrc522.MIFARE_Write(blockAddr, cleanBlock, 16);
                if (status != MFRC522::STATUS_OK) {
                    Serial.printf("Format failed for block %d\n", blockAddr);
                    formatSuccess = false;
                } else {
                    Serial.printf("Block %d formatted successfully\n", blockAddr);
                }
            }

            // Re-authenticate if needed
            mfrc522.PCD_StopCrypto1();
            if (sector < 15) { // Don't need to re-authenticate after last sector
                status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &keyA, &(mfrc522.uid));
            }
        }

        mfrc522.PICC_HaltA();
        mfrc522.PCD_StopCrypto1();

        // Show result
        staticItems();
        tft.setTextDatum(TL_DATUM);
        tft.setFreeFont(&OpenSans_Bold10pt7b);
        if (formatSuccess) {
            tft.drawString("Format successful!", 15, 45);
            Serial.println("Card formatted successfully");
        } else {
            tft.drawString("Format partially failed", 15, 45);
            tft.setFreeFont(&OpenSans_Regular8pt7b);
            tft.drawString("Some blocks could not be formatted", 15, 75);
            Serial.println("Some blocks failed to format");
        }
        
        delay(2000);
        return;
    }
}
