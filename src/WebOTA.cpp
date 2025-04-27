#include "WebOTA.h"

WebOTAUpdater::WebOTAUpdater(const String &hostname, bool debug) {
    this->hostname = hostname;
    this->debug = debug;
    this->server = new WebServer(80);
}

WebOTAUpdater::~WebOTAUpdater() {
    if (server) {
        delete server;
        server = nullptr;
    }
}

void WebOTAUpdater::begin() {
    // Index page
    server->on("/", HTTP_GET, [this]() {
        server->sendHeader("Connection", "close");
        server->send(200, "text/html", updateIndex);
    });
    
    // Handle firmware update
    server->on("/update", HTTP_POST, [this]() {
        server->sendHeader("Connection", "close");
        server->send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
        delay(1000);
        ESP.restart();
    }, [this]() {
        HTTPUpload& upload = server->upload();
        
        if (upload.status == UPLOAD_FILE_START) {
            if (debug) {
                Serial.printf("Update: %s\n", upload.filename.c_str());
            }
            if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { // Start with max available size
                if (debug) {
                    Update.printError(Serial);
                }
            }
        } else if (upload.status == UPLOAD_FILE_WRITE) {
            // Write received bytes to flash
            if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
                if (debug) {
                    Update.printError(Serial);
                }
            }
        } else if (upload.status == UPLOAD_FILE_END) {
            // Finish update
            if (Update.end(true)) {
                if (debug) {
                    Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
                }
            } else {
                if (debug) {
                    Update.printError(Serial);
                }
            }
        }
    });
    
    server->begin();
    
    if (debug) {
        Serial.println("Web OTA server started");
        Serial.println("Open http://" + WiFi.localIP().toString() + " in your browser");
    }
}

void WebOTAUpdater::handle() {
    server->handleClient();
} 