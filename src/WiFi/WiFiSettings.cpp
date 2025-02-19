#define DISABLE_ALL_LIBRARY_WARNINGS

#include <Arduino.h>
#include <TFT_eSPI.h>
#include <WiFi.h>
#include "WiFiSettings.h"
#include "../UI/StartupSettings.h"
#include "../UI/UISettings.h"
#include "../UI/DisplayConfig.h"
#include "../UI/GPIOS.h"
#include "../UI/fonts/OpenSans_Bold10pt7b.h"
#include "../UI/fonts/OpenSans_Regular8pt7b.h"

extern TFT_eSPI tft;

void wifiScan() {
    Serial.println("\nStarting WiFi scan...");
    Serial.println("------------------------");
    
    staticItems();
    tft.setTextDatum(TL_DATUM);
    tft.fillRoundRect(10, 39, 300, 125, 8, backgroundColor);
    tft.setFreeFont(&OpenSans_Bold10pt7b);
    tft.drawString("Scanning WiFi networks...", 15, 45);
    tft.setFreeFont(&OpenSans_Regular8pt7b);
    tft.drawString("Press BOOT to cancel", 15, 70);
    
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);
    
    // Start async scan
    WiFi.scanNetworks(true);
    bool scanning = true;
    int n = -1;
    
    // Wait for scan to complete or boot button press
    while (scanning) {
        n = WiFi.scanComplete();
        if (n != WIFI_SCAN_RUNNING) {
            scanning = false;
        }
        
        // Check for boot button press (active low)
        if (digitalRead(0) == LOW) {  // GPIO0 is the BOOT button
            delay(50);  // Simple debounce
            if (digitalRead(0) == LOW) {
                WiFi.scanDelete();  // Cancel the scan
                tft.fillRoundRect(10, 39, 300, 125, 8, backgroundColor);
                tft.setFreeFont(&OpenSans_Bold10pt7b);
                tft.drawString("Scan cancelled", 15, 45);
                Serial.println("Scan cancelled by user");
                delay(1500);
                return;
            }
        }
        delay(100);  // Small delay to prevent busy waiting
    }
    
    Serial.printf("\nScan completed. Found %d networks:\n", n);
    Serial.println("------------------------\n");
    
    if (n == 0) {
        tft.fillRoundRect(10, 39, 300, 125, 8, backgroundColor);
        tft.setFreeFont(&OpenSans_Bold10pt7b);
        tft.drawString("No networks found", 15, 45);
        Serial.println("No networks found");
        Serial.println("------------------------\n");
    } else {
        tft.fillRoundRect(10, 39, 300, 125, 8, backgroundColor);
        tft.setFreeFont(&OpenSans_Bold10pt7b);
        // Display network count
        String countText = "Found " + String(n) + " networks";
        tft.drawString(countText, 15, 45);
        delay(1500);

        tft.fillRoundRect(10, 39, 300, 125, 8, backgroundColor);
        // Set text properties for the list
        tft.setFreeFont(&OpenSans_Regular8pt7b);
        tft.setTextDatum(TL_DATUM);
        
        // Use the same positioning as the menu
        int yOffset = 45;  // Same as menu
        int itemSpacing = 25;  // Same as menu
        int leftPadding = 15;  // Same as menu
        
        // Print header in serial monitor
        Serial.println("SSID                             | RSSI | Channel | Encryption");
        Serial.println("--------------------------------|------|---------|------------");
        
        // Display each network
        for (int i = 0; i < n; i++) {
            // Format network info for serial output
            String ssid = WiFi.SSID(i);
            int32_t rssi = WiFi.RSSI(i);
            int32_t channel = WiFi.channel(i);
            String encType;
            switch(WiFi.encryptionType(i)) {
                case WIFI_AUTH_OPEN: encType = "Open"; break;
                case WIFI_AUTH_WEP: encType = "WEP"; break;
                case WIFI_AUTH_WPA_PSK: encType = "WPA_PSK"; break;
                case WIFI_AUTH_WPA2_PSK: encType = "WPA2_PSK"; break;
                case WIFI_AUTH_WPA_WPA2_PSK: encType = "WPA_WPA2_PSK"; break;
                default: encType = "Unknown";
            }
            
            // Print to serial monitor (formatted table)
            String paddedSSID = ssid;
            while (paddedSSID.length() < 32) paddedSSID += " ";
            Serial.printf("%s| %4d | %7d | %s\n", 
                        paddedSSID.c_str(), 
                        rssi,
                        channel,
                        encType.c_str());
        }
        
        Serial.println("------------------------\n");
        
        // Display on screen (only first 5)
        for (int i = 0; i < n && i < 5; i++) {
            int yPos = yOffset + (i * itemSpacing);
            String networkInfo = WiFi.SSID(i) + " (" + WiFi.RSSI(i) + "dBm)";
            
            // Truncate network name if too long
            if (networkInfo.length() > 25) {
                networkInfo = networkInfo.substring(0, 22) + "...";
            }
            
            tft.drawString(networkInfo, leftPadding, yPos);
        }
        
        // Draw navigation arrows if needed
        if (n > 5) {
            // Draw down arrow to indicate more networks
            int triangleYPos = SCREEN_HEIGHT - 20;
            tft.fillTriangle(
                SCREEN_WIDTH - 35, triangleYPos,
                SCREEN_WIDTH - 25, triangleYPos,
                SCREEN_WIDTH - 30, triangleYPos + 5,
                highlightColor
            );
        }
    }
    
    // Clean up scan results
    WiFi.scanDelete();
    delay(5000); // Display results for 5 seconds
}
