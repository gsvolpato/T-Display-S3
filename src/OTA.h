#ifndef OTA_H
#define OTA_H

#include <Arduino.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

class OTAUpdater {
private:
    String hostname;
    String password;
    bool debug;
    
public:
    OTAUpdater(const String &hostname = "esp32-ota", const String &password = "", bool debug = false);
    void begin();
    void handle();
};

#endif // OTA_H 