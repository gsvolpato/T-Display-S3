#include <Arduino.h>
#include "WiFiManager.h"
#include "OTA.h"
#include "WebOTA.h"

// WiFi credentials
const char* ssid = "AP Bridge";
const char* password = "batatafrita";

// Network configuration - static IP
IPAddress staticIP(192, 168, 50, 248);  // Your desired static IP
IPAddress gateway(192, 168, 50, 1);     // Your gateway
IPAddress subnet(255, 255, 255, 0);     // Your subnet mask
IPAddress dns(8, 8, 8, 8);              // DNS server

// OTA configuration
const char* hostname = "T-DISPLAY-S3";  // Device hostname for OTA
const char* otaPassword = "admin";      // Password for OTA updates (optional)
const bool debugMode = true;            // Enable debug messages

// Create instances
WiFiManager wifiManager(ssid, password, debugMode);
OTAUpdater otaUpdater(hostname, otaPassword, debugMode);
WebOTAUpdater webOtaUpdater(hostname, debugMode);

bool waitForSerial(unsigned long timeout = 5000) {
  unsigned long start = millis();
  
  // Wait for serial connection
  while (!Serial && (millis() - start < timeout)) {
    delay(100);
  }
  
  return Serial;
}

bool setupWifi() {
  // Configure static IP
  if(!WiFi.config(staticIP, gateway, subnet, dns)) {
    Serial.println("STA Failed to configure static IP");
    return false;
  }
  
  // Connect to WiFi
  if (!wifiManager.connect()) {
    Serial.println("Failed to connect to WiFi. OTA updates will not be available.");
    return false;
  }
  
  return true;
}

void setup() {
  Serial.begin(115200);
  
  // Wait for serial monitor to open
  waitForSerial();
  
  Serial.println("\nStarting...");
  Serial.println("Waiting for Serial Monitor... Press any key to continue");
  
  // Wait for input before proceeding
  while (!Serial.available()) {
    delay(100);
  }
  
  Serial.println("Setting up WiFi and OTA...");
  
  // Setup WiFi with static IP
  if (setupWifi()) {
    // Initialize both OTA methods
    otaUpdater.begin();     // ArduinoOTA for IDE updates
    webOtaUpdater.begin();  // Web server for browser updates
    
    Serial.println("OTA ready!");
    Serial.print("Device IP: ");
    Serial.println(WiFi.localIP());
    Serial.println("You can now upload firmware via:");
    Serial.println("1. Arduino IDE/PlatformIO (ArduinoOTA)");
    Serial.println("2. Web browser: http://" + WiFi.localIP().toString());
  }
  
  // Your other setup code here
}

void loop() {
  // Handle both OTA methods
  otaUpdater.handle();
  webOtaUpdater.handle();
  
  // Your other loop code here
}
