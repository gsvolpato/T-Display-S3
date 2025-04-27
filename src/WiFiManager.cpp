#include "WiFiManager.h"

WiFiManager::WiFiManager(const String &ssid, const String &password, bool debug, unsigned long timeout) {
    this->ssid = ssid;
    this->password = password;
    this->debug = debug;
    this->timeout = timeout;
}

bool WiFiManager::connect() {
    if (WiFi.status() == WL_CONNECTED) {
        if (debug) Serial.println("WiFi already connected");
        return true;
    }
    
    if (debug) {
        Serial.print("Connecting to WiFi ");
        Serial.print(ssid);
    }
    
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), password.c_str());
    
    unsigned long startAttemptTime = millis();
    
    while (WiFi.status() != WL_CONNECTED && 
           millis() - startAttemptTime < timeout) {
        if (debug) Serial.print(".");
        delay(500);
    }
    
    if (WiFi.status() != WL_CONNECTED) {
        if (debug) Serial.println("\nFailed to connect to WiFi");
        return false;
    }
    
    if (debug) {
        Serial.println("\nWiFi connected");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
    }
    
    return true;
}

void WiFiManager::disconnect() {
    WiFi.disconnect();
    if (debug) Serial.println("WiFi disconnected");
}

bool WiFiManager::isConnected() {
    return WiFi.status() == WL_CONNECTED;
}

IPAddress WiFiManager::getIP() {
    return WiFi.localIP();
}

String WiFiManager::getSSID() {
    return WiFi.SSID();
}

int WiFiManager::getRSSI() {
    return WiFi.RSSI();
} 