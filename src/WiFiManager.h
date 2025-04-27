#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>

class WiFiManager {
private:
    String ssid;
    String password;
    bool debug;
    unsigned long timeout;
    
public:
    WiFiManager(const String &ssid, const String &password, bool debug = false, unsigned long timeout = 30000);
    bool connect();
    void disconnect();
    bool isConnected();
    IPAddress getIP();
    String getSSID();
    int getRSSI();
};

#endif // WIFI_MANAGER_H 