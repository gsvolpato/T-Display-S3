# T-Display-S3
LilyGo T-Display-S3 Projects. Each branch a different project.

# ESP32 OTA Update Module

This project provides a modular OTA (Over-The-Air) firmware update solution for ESP32 devices, specifically for the LILYGO T-Display-S3 board.

## Components

The solution consists of three main modules:

1. **OTAUpdater** - Handles Arduino OTA updates via IDE/PlatformIO
2. **WebOTAUpdater** - Provides a web interface for browser-based OTA updates
3. **WiFiManager** - Manages WiFi connections with static IP support

## How to Use

### 1. Copy the Module Files

Copy these files to your project:
- `src/OTA.h` and `src/OTA.cpp` - Arduino OTA updates
- `src/WebOTA.h` and `src/WebOTA.cpp` - Web browser OTA updates
- `src/WiFiManager.h` and `src/WiFiManager.cpp` - WiFi connection management

### 2. Include in Your Project

```cpp
#include "WiFiManager.h"
#include "OTA.h"
#include "WebOTA.h"

// WiFi credentials
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// Optional: Static IP configuration
IPAddress staticIP(192, 168, 1, 100);  // Your desired static IP
IPAddress gateway(192, 168, 1, 1);     // Your gateway
IPAddress subnet(255, 255, 255, 0);    // Your subnet mask
IPAddress dns(8, 8, 8, 8);             // DNS server

// OTA configuration
const char* hostname = "esp32-device";  // Device hostname for OTA
const char* otaPassword = "admin";      // Password for OTA updates
const bool debugMode = true;            // Enable debug messages

// Create instances
WiFiManager wifiManager(ssid, password, debugMode);
OTAUpdater otaUpdater(hostname, otaPassword, debugMode);
WebOTAUpdater webOtaUpdater(hostname, debugMode);

void setup() {
  Serial.begin(115200);
  
  // Wait for serial monitoring if needed
  while (!Serial) {
    delay(100);
  }
  
  // Configure static IP (optional)
  WiFi.config(staticIP, gateway, subnet, dns);
  
  // Connect to WiFi
  if (wifiManager.connect()) {
    // Initialize both OTA methods
    otaUpdater.begin();     // Arduino OTA updates
    webOtaUpdater.begin();  // Web browser updates
    
    Serial.println("OTA ready!");
    Serial.print("Device IP: ");
    Serial.println(WiFi.localIP());
  }
}

void loop() {
  // Handle both OTA methods
  otaUpdater.handle();
  webOtaUpdater.handle();
  
  // Your other loop code here
}
```

### 3. Update Methods

#### IDE/PlatformIO Updates (ArduinoOTA)
Configure platformio.ini to enable OTA uploads when ready:

```ini
upload_protocol = espota
upload_port = esp32-device.local  ; or IP address
upload_flags =
    --auth=admin
```

#### Web Browser Updates
Access the device's web interface by navigating to:
```
http://device-ip-address/
```

The interface will allow you to upload firmware binaries directly from your browser.

### 4. Initial Upload

For the first time, upload via USB. Subsequent uploads can be done over OTA using either method.

## Customization

### WiFiManager

```cpp
// Constructor
WiFiManager(
    const String &ssid,       // WiFi SSID
    const String &password,   // WiFi password
    bool debug = false,       // Enable debug output
    unsigned long timeout = 30000  // Connection timeout in ms
);
```

### OTAUpdater (ArduinoOTA)

```cpp
// Constructor
OTAUpdater(
    const String &hostname = "esp32-ota",  // Device hostname
    const String &password = "",           // OTA password (optional)
    bool debug = false                     // Enable debug output
);
```

### WebOTAUpdater

```cpp
// Constructor
WebOTAUpdater(
    const String &hostname = "esp32-ota",  // Device hostname
    bool debug = false                     // Enable debug output
);
```

## Troubleshooting

- Ensure your device is connected to the same network as your computer
- Verify the hostname or IP address in platformio.ini is correct if using ArduinoOTA
- Check that the OTA password matches what's in your code
- If using hostname resolution, ensure mDNS is working on your network
- For web uploads, try accessing the device IP directly in your browser
- Make sure your browser allows JavaScript for the web interface to function
