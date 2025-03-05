#include <Arduino.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include <MFRC522.h>
#include "skull_logo_s3.h"
#include "GPIOS.h"

#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 170

TFT_eSPI tft = TFT_eSPI();
MFRC522 rfid(PIN_RFID_SDA, PIN_RFID_RST);

unsigned long lastCardTime = 0;
const unsigned long DISPLAY_TIMEOUT = 10000;
bool showingCard = false;

const char* RFIDMenuItems[] = {
    "Read ID",
    "Clone Tag", 
    "Format ID",
    "Read Dump",
    "Write Dump",
    "Stored IDs"
};
const int NUM_MENU_ITEMS = sizeof(RFIDMenuItems) / sizeof(RFIDMenuItems[0]);
int selectedMenuItem = 0;


volatile int encoderPos = 0;
volatile bool encoderChanged = false;

bool inSubMenu = false;
volatile bool encoderSwitchPressed = false;
volatile bool wakeupSwitchPressed = false;

void IRAM_ATTR handleEncoder() {
    static uint8_t old_AB = 3;  
    static int8_t encval = 0;
    static const int8_t enc_states[] = {0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0};
    
    old_AB <<= 2;
    old_AB |= (digitalRead(PIN_ENCODER_CLK) << 1) | digitalRead(PIN_ENCODER_DT);
    encval += enc_states[old_AB & 0x0f];

    // 4 steps = 1 detent
    if (encval > 3) {        // Clockwise
        encoderPos++;
        encval = 0;
        encoderChanged = true;
    }
    else if (encval < -3) {  // Counter-clockwise
        encoderPos--;
        encval = 0;
        encoderChanged = true;
    }
}

void IRAM_ATTR handleEncoderSwitch() {
    static unsigned long lastDebounceTime = 0;
    unsigned long currentTime = millis();
    
    if (currentTime - lastDebounceTime > 50) {  // 50ms debounce
        encoderSwitchPressed = true;
        lastDebounceTime = currentTime;
    }
}

void IRAM_ATTR handleWakeupSwitch() {
    static unsigned long lastDebounceTime = 0;
    unsigned long currentTime = millis();
    
    if (currentTime - lastDebounceTime > 50) {  // 50ms debounce
        wakeupSwitchPressed = true;
        lastDebounceTime = currentTime;
    }
}

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
        Serial.println(rfid.GetStatusCodeName((MFRC522::StatusCode)status));
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
            Serial.println(rfid.GetStatusCodeName((MFRC522::StatusCode)status));
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
    delay(3000);
}

void displayMenu() {
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(2);
    
    for (int i = 0; i < NUM_MENU_ITEMS; i++) {
        if (i == selectedMenuItem) {
            tft.setTextColor(TFT_GREEN, TFT_BLACK);
        } else {
            tft.setTextColor(TFT_WHITE, TFT_BLACK);
        }
        tft.setCursor(10, 20 + i * 25);
        tft.println(RFIDMenuItems[i]);
    }
}

void handleRFIDMenuSelection(int selectedOption) {
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.setTextSize(2);
    tft.setTextDatum(MC_DATUM);
    int textX = SCREEN_WIDTH/2;
    int textY = SCREEN_HEIGHT/2;

    inSubMenu = true;

    switch (selectedOption) {
        case 0: // Read ID
            while (inSubMenu) {
                tft.fillScreen(TFT_BLACK);
                tft.drawString("Scanning for card...", textX, textY);
                
                // Check for card
                if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
                    readRFIDBlocks();
                    delay(2000);  // Show results for 2 seconds
                }
                
                // Check for return button
                if (wakeupSwitchPressed) {
                    while(digitalRead(PIN_WAKEUP_SWITCH) == LOW);  // Wait for release
                    wakeupSwitchPressed = false;
                    inSubMenu = false;
                }
                
                delay(100);  // Small delay between scans
            }
            break;
            
        case 1: // Clone Tag
            tft.drawString("Clone Tag Mode", textX, textY);
            tft.drawString("Press WAKE to return", textX, textY + 30);
            while (inSubMenu) {
                if (wakeupSwitchPressed) {
                    while(digitalRead(PIN_WAKEUP_SWITCH) == LOW);  // Wait for release
                    wakeupSwitchPressed = false;
                    inSubMenu = false;
                }
                delay(100);
            }
            break;
            
        case 2: // Format ID
            tft.drawString("Format - Coming soon", textX, textY);
            delay(2000);
            break;
            
        case 3: // Read Dump
            tft.drawString("Read Dump - Coming soon", textX, textY);
            delay(2000);
            break;
            
        case 4: // Write Dump
            tft.drawString("Write Dump - Coming soon", textX, textY);
            delay(2000);
            break;
            
        case 5: // Stored IDs
            tft.drawString("Stored IDs - Coming soon", textX, textY);
            delay(2000);
            break;
            
        default:
            tft.drawString("Invalid selection", textX, textY);
            delay(2000);
            inSubMenu = false;
            break;
    }
    
    displayMenu();
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
    
    // Display welcome message
    displayWelcomeMessage();
    
    displayMenu();
    
    // Setup encoder pins
    pinMode(PIN_ENCODER_CLK, INPUT_PULLUP);
    pinMode(PIN_ENCODER_DT, INPUT_PULLUP);
    pinMode(PIN_ENCODER_SWITCH, INPUT_PULLUP);
    
    // Setup switches
    pinMode(PIN_WAKEUP_SWITCH, INPUT_PULLUP);
    
    // Attach interrupts for encoder
    attachInterrupt(digitalPinToInterrupt(PIN_ENCODER_CLK), handleEncoder, CHANGE);
    attachInterrupt(digitalPinToInterrupt(PIN_ENCODER_DT), handleEncoder, CHANGE);
    attachInterrupt(digitalPinToInterrupt(PIN_ENCODER_SWITCH), handleEncoderSwitch, FALLING);
    attachInterrupt(digitalPinToInterrupt(PIN_WAKEUP_SWITCH), handleWakeupSwitch, FALLING);
}

void loop() {
    // Handle encoder switch release
    if (encoderSwitchPressed && digitalRead(PIN_ENCODER_SWITCH) == HIGH) {  // Switch released
        encoderSwitchPressed = false;
        if (!inSubMenu) {  // Only handle menu selection if not in submenu
            handleRFIDMenuSelection(selectedMenuItem);
        }
    }
    
    // Handle menu navigation only when not in submenu
    if (!inSubMenu && encoderChanged) {
        selectedMenuItem = encoderPos % NUM_MENU_ITEMS;
        if (selectedMenuItem < 0) {
            selectedMenuItem += NUM_MENU_ITEMS;
        }
        displayMenu();
        encoderChanged = false;
    }
    
    delay(1);  // Small delay to prevent CPU hogging
}
