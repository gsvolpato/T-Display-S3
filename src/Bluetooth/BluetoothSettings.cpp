#define DISABLE_ALL_LIBRARY_WARNINGS

#include <Arduino.h>
#include <TFT_eSPI.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include "BluetoothSettings.h"
#include "../UI/StartupSettings.h"
#include "../UI/UISettings.h"
#include "../UI/DisplayConfig.h"
#include "../UI/GPIOS.h"
#include "../UI/fonts/OpenSans_Bold10pt7b.h"
#include "../UI/fonts/OpenSans_Regular8pt7b.h"

extern TFT_eSPI tft;

#define SCAN_TIME 5 // Scan time in seconds
#define MAX_DEVICES 10 // Maximum number of devices to display
#define MAX_MONITOR_LINES 5 // Maximum lines to show on screen
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

BLEScan* pBLEScan;
BLEServer* pServer = nullptr;
BLEService* pService = nullptr;
BLECharacteristic* pCharacteristic = nullptr;
std::vector<BLEAdvertisedDevice> devices;
bool monitorActive = false;
bool deviceConnected = false;
int8_t selectedButton = 0;  // 0 = Connect/Disconnect, 1 = Back

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
        // Store only the first MAX_DEVICES devices
        if (devices.size() < MAX_DEVICES) {
            devices.push_back(advertisedDevice);
        }
    }
};

// New callback class for BLE Serial Monitor
class BLEMonitorCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
        String deviceInfo = "";
        
        // Get device name or address
        if (advertisedDevice.haveName()) {
            deviceInfo += "Name: " + String(advertisedDevice.getName().c_str());
        } else {
            deviceInfo += "Addr: " + String(advertisedDevice.getAddress().toString().c_str());
        }
        
        // Add RSSI
        deviceInfo += " RSSI: " + String(advertisedDevice.getRSSI());
        
        // Add manufacturer data if available
        if (advertisedDevice.haveManufacturerData()) {
            std::string manufacturerData = advertisedDevice.getManufacturerData();
            deviceInfo += " Data: ";
            char hex[3];
            for (int i = 0; i < manufacturerData.length() && i < 8; i++) {
                sprintf(hex, "%02X", (uint8_t)manufacturerData[i]);
                deviceInfo += String(hex);
            }
            if (manufacturerData.length() > 8) deviceInfo += "...";
        }
        
        // Print to serial
        Serial.println(deviceInfo);
        
        // Update display
        if (monitorActive) {
            staticItems();
            tft.fillRoundRect(10, 39, 300, 125, 8, backgroundColor);
            tft.setFreeFont(&OpenSans_Regular8pt7b);
            tft.setTextDatum(TL_DATUM);
            
            // Show monitoring status
            tft.drawString("BLE Monitor Active", 15, 45);
            tft.drawString("Press button to stop", 15, 65);
            
            // Show latest device info
            String displayInfo = deviceInfo;
            if (displayInfo.length() > 40) {
                displayInfo = displayInfo.substring(0, 37) + "...";
            }
            tft.drawString(displayInfo, 15, 85);
        }
    }
};

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
        deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
        deviceConnected = false;
    }
};

void initBluetooth() {
    BLEDevice::init("T Display S3");
    
    // Create the BLE Server
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());

    // Create the BLE Service
    pService = pServer->createService(SERVICE_UUID);

    // Create a BLE Characteristic
    pCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ |
        BLECharacteristic::PROPERTY_WRITE |
        BLECharacteristic::PROPERTY_NOTIFY
    );

    // Start the service
    pService->start();

    // Start advertising
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
    pAdvertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();

    // Initialize scanner
    pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    pBLEScan->setActiveScan(true);
    pBLEScan->setInterval(100);
    pBLEScan->setWindow(99);
}

void drawBottomButtons(bool isConnected) {
    // Draw bottom bar background
    tft.fillRect(0, SCREEN_HEIGHT - 35, SCREEN_WIDTH, 35, backgroundColor);
    
    // Draw buttons
    int buttonWidth = SCREEN_WIDTH / 2;
    
    // Draw connect/disconnect button
    tft.drawRoundRect(5, SCREEN_HEIGHT - 30, buttonWidth - 10, 25, 4, highlightColor);
    tft.setTextDatum(MC_DATUM);
    
    // Set font based on selection
    if (selectedButton == 0) {
        tft.setFreeFont(&OpenSans_Bold10pt7b);
    } else {
        tft.setFreeFont(&OpenSans_Regular8pt7b);
    }
    tft.drawString(isConnected ? "Disconnect" : "Connect", buttonWidth/2, SCREEN_HEIGHT - 17);
    
    // Draw back button
    tft.drawRoundRect(buttonWidth + 5, SCREEN_HEIGHT - 30, buttonWidth - 10, 25, 4, highlightColor);
    if (selectedButton == 1) {
        tft.setFreeFont(&OpenSans_Bold10pt7b);
    } else {
        tft.setFreeFont(&OpenSans_Regular8pt7b);
    }
    tft.drawString("Back", buttonWidth + buttonWidth/2, SCREEN_HEIGHT - 17);
}

void startBluetoothScan() {
    devices.clear(); // Clear previous results
    staticItems(); // Clear screen and draw menu frame
    
    // Show scanning message
    tft.setTextDatum(MC_DATUM);
    tft.drawString("Scanning for devices...", SCREEN_WIDTH/2, SCREEN_HEIGHT/2);
    
    // Start scan
    BLEScanResults foundDevices = pBLEScan->start(SCAN_TIME, false);
    pBLEScan->clearResults();
    
    displayBluetoothDevices();
}

void displayBluetoothDevices() {
    staticItems(); // Clear screen and draw menu frame
    
    tft.setTextDatum(TL_DATUM); // Set text alignment to top-left
    int yPos = 45; // Starting Y position for device list
    
    if (devices.empty()) {
        tft.drawString("No devices found", 15, SCREEN_HEIGHT/2);
        return;
    }

    // Display device count
    String countText = "Found " + String(devices.size()) + " devices:";
    tft.drawString(countText, 15, yPos);
    yPos += 20;

    // Display each device
    for (auto& device : devices) {
        if (yPos > SCREEN_HEIGHT - 20) break; // Prevent drawing outside screen
        
        // Get device name or address
        std::string stdName = device.getName();
        String deviceName = String(stdName.c_str());
        if (deviceName.length() == 0) {
            BLEAddress address = device.getAddress();
            deviceName = String(address.toString().c_str());
        }
        
        // Get RSSI
        String rssi = String(device.getRSSI());
        
        // Truncate name if too long
        if (deviceName.length() > 25) {
            deviceName = deviceName.substring(0, 22) + "...";
        }
        
        // Display device info
        String deviceInfo = deviceName + " (" + rssi + "dBm)";
        tft.drawString(deviceInfo, 15, yPos);
        yPos += 20;
    }
}

void stopBluetoothScan() {
    pBLEScan->stop();
    devices.clear();
}

void startBLESerialMonitor() {
    Serial.println("\nStarting BLE Serial Monitor...");
    Serial.println("------------------------");
    
    // Stop scanning completely
    pBLEScan->stop();
    
    // Make sure advertising is started
    BLEDevice::startAdvertising();
    
    monitorActive = true;
    selectedButton = 0;
    
    // Show initial screen
    staticItems();
    tft.fillRoundRect(10, 39, 300, 125, 8, backgroundColor);
    tft.setFreeFont(&OpenSans_Regular8pt7b);
    tft.setTextDatum(TL_DATUM);
    tft.drawString("BLE Monitor Active", 15, 45);
    tft.drawString("Device Name: T Display S3", 15, 65);
    tft.drawString("Status: " + String(deviceConnected ? "Connected" : "Waiting..."), 15, 85);
    
    // Draw bottom buttons
    drawBottomButtons(deviceConnected);
    
    // Monitor loop
    while (monitorActive) {
        // Handle encoder rotation
        if (encoderUp || encoderDown) {
            if (encoderUp) {
                selectedButton = (selectedButton > 0) ? selectedButton - 1 : 1;
            } else {
                selectedButton = (selectedButton < 1) ? selectedButton + 1 : 0;
            }
            encoderUp = false;
            encoderDown = false;
            drawBottomButtons(deviceConnected);
        }
        
        // Handle button press
        if (digitalRead(PIN_ENCODER_SWITCH) == LOW) {
            delay(50); // Debounce
            if (digitalRead(PIN_ENCODER_SWITCH) == LOW) {
                if (selectedButton == 0) {
                    // Toggle connection
                    if (deviceConnected) {
                        pServer->disconnect(0);
                        deviceConnected = false;
                        BLEDevice::stopAdvertising();
                    } else {
                        BLEDevice::startAdvertising();
                    }
                    
                    // Update status display
                    tft.fillRoundRect(10, 39, 300, 125, 8, backgroundColor);
                    tft.setFreeFont(&OpenSans_Regular8pt7b);
                    tft.setTextDatum(TL_DATUM);
                    tft.drawString("BLE Monitor Active", 15, 45);
                    tft.drawString("Device Name: T Display S3", 15, 65);
                    tft.drawString("Status: " + String(deviceConnected ? "Connected" : "Waiting..."), 15, 85);
                } else {
                    // Back button pressed
                    monitorActive = false;
                    BLEDevice::stopAdvertising();
                    Serial.println("\nBLE Monitor stopped");
                    Serial.println("------------------------\n");
                    break;
                }
                drawBottomButtons(deviceConnected);
            }
            while (digitalRead(PIN_ENCODER_SWITCH) == LOW) delay(10); // Wait for button release
        }
        
        // Update status if connection state changes
        static bool lastConnectedState = !deviceConnected;
        if (lastConnectedState != deviceConnected) {
            lastConnectedState = deviceConnected;
            tft.fillRoundRect(10, 39, 300, 125, 8, backgroundColor);
            tft.setFreeFont(&OpenSans_Regular8pt7b);
            tft.setTextDatum(TL_DATUM);
            tft.drawString("BLE Monitor Active", 15, 45);
            tft.drawString("Device Name: T Display S3", 15, 65);
            tft.drawString("Status: " + String(deviceConnected ? "Connected" : "Waiting..."), 15, 85);
            drawBottomButtons(deviceConnected);
        }
        
        delay(100); // Small delay between operations
    }
}
