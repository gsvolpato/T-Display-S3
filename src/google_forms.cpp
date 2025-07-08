#include "google_forms.h"

GoogleFormsHandler::GoogleFormsHandler(const char* wifiSSID, const char* wifiPassword, 
                                     const char* googleFormURL, const char* uidField, 
                                     const char* typeField, const char* timestampField) {
    ssid = wifiSSID;
    password = wifiPassword;
    formURL = googleFormURL;
    uidFieldName = uidField;
    typeFieldName = typeField;
    timestampFieldName = timestampField;
    wifiConnected = false;
}

bool GoogleFormsHandler::connectToWiFi() {
    Serial.println("Connecting to WiFi...");
    WiFi.begin(ssid, password);
    
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(500);
        Serial.print(".");
        attempts++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        wifiConnected = true;
        Serial.println("\nWiFi connected!");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
        return true;
    } else {
        wifiConnected = false;
        Serial.println("\nWiFi connection failed!");
        return false;
    }
}

bool GoogleFormsHandler::isWiFiConnected() {
    return wifiConnected && (WiFi.status() == WL_CONNECTED);
}

void GoogleFormsHandler::checkWiFiConnection() {
    if (WiFi.status() != WL_CONNECTED) {
        wifiConnected = false;
        Serial.println("WiFi connection lost. Attempting to reconnect...");
        connectToWiFi();
    }
}

String GoogleFormsHandler::getLocalIP() {
    if (isWiFiConnected()) {
        return WiFi.localIP().toString();
    }
    return "Not Connected";
}

bool GoogleFormsHandler::sendCardData(String uid, String cardType) {
    if (!isWiFiConnected()) {
        Serial.println("WiFi not connected, skipping Google Form submission");
        return false;
    }
    
    checkWiFiConnection();
    
    if (!isWiFiConnected()) {
        return false;
    }
    
    Serial.println("Sending data to Google Form...");
    
    String timestamp = String(millis());
    
    googleForm.setFormUrl(formURL);
    googleForm.addData(uid, uidFieldName);
    googleForm.addData(cardType, typeFieldName);
    googleForm.addData(timestamp, timestampFieldName);
    
    if (googleForm.send()) {
        Serial.println("Data sent to Google Form successfully!");
        return true;
    } else {
        Serial.println("Failed to send data to Google Form");
        return false;
    }
} 