#ifndef GOOGLE_FORMS_H
#define GOOGLE_FORMS_H

#include <Arduino.h>
#include <WiFi.h>
#include <GoogleFormPost.h>

class GoogleFormsHandler {
private:
    GoogleFormPost googleForm;
    const char* ssid;
    const char* password;
    const char* formURL;
    const char* uidFieldName;
    const char* typeFieldName;
    const char* timestampFieldName;
    bool wifiConnected;

public:
    GoogleFormsHandler(const char* wifiSSID, const char* wifiPassword, 
                      const char* googleFormURL, const char* uidField, 
                      const char* typeField, const char* timestampField);
    
    bool connectToWiFi();
    bool isWiFiConnected();
    bool sendCardData(String uid, String cardType);
    void checkWiFiConnection();
    String getLocalIP();
};

#endif 