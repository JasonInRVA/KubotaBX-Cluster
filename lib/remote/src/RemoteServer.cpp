#include "RemoteServer.h"
#include <WiFi.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include <secrets.h>


// --- WiFi Credentials ---
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;

RemoteServer::RemoteServer() : server_(80), ws_("/ws") {
}


void RemoteServer::begin(ITractorInputs* inputs) {
    this->inputs_ = inputs;

    if (!SPIFFS.begin(true)) {
        Serial.println("An Error has occurred while mounting SPIFFS");
        return;
    }

Serial.print("Connecting to WiFi (STA)...");
WiFi.mode(WIFI_STA);
WiFi.begin(ssid, password);

// Optional: set a hostname so your router shows a nice name
WiFi.setHostname("TractorDash");

unsigned long t0 = millis();
while (WiFi.status() != WL_CONNECTED && millis() - t0 < 10000) {
    delay(250);
    Serial.print(".");
}

if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.print("Connected. IP: ");
    Serial.println(WiFi.localIP());
} else {
    Serial.println();
    Serial.println("STA connect failed. Starting AP fallback...");
    WiFi.mode(WIFI_AP);
    WiFi.softAP("TractorDash-AP", "password");
    Serial.print("AP IP address: ");
    Serial.println(WiFi.softAPIP());
}

    ws_.onEvent([this](AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
        if (type == WS_EVT_CONNECT) {
            Serial.printf("WS client #%u connected\n", client->id());
            
            // --- DEFENSIVE PROGRAMMING IN ACTION ---
            // Let's send the current state to the new client, but SAFELY.
            if (this->inputs_ != nullptr) {
                // If the pointer is valid, get the state and broadcast it.
                TractorState currentState = this->inputs_->read();                
                this->broadcastState(currentState); 
            } else {
                // If the pointer is not valid, do nothing. NO CRASH.
                Serial.println("Warning: WS connected but inputs not ready.");
            }

        } else if (type == WS_EVT_DISCONNECT) {
            Serial.printf("WS client #%u disconnected\n", client->id());
        }
    });    
    
    server_.addHandler(&ws_);

    // This handler serves the main index.html page
    server_.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(SPIFFS, "/index.html", "text/html");
    });

    // This "catch-all" handler will try to serve any other requested file from SPIFFS
    server_.onNotFound([](AsyncWebServerRequest *request) {
        if (SPIFFS.exists(request->url())) {
            request->send(SPIFFS, request->url(), "text/javascript");
        } else {
            request->send(404, "text/plain", "Not Found");
        }
    });

    server_.begin();
    Serial.println("Remote server started.");
}

void RemoteServer::broadcastState(const TractorState& s) {
    // Clean up any disconnected clients
    ws_.cleanupClients();
    
    // Don't bother serializing if no one is listening
    if (ws_.count() == 0) {
        return;
    }

    JsonDocument doc;
    doc["rpm"] = s.rpm;
    doc["fuel_pct"] = s.fuel_pct;
    doc["temp_c"] = s.temp_c;
    doc["oil_ok"] = s.oil_ok;
    doc["bl_left"] = s.bl_left;
    doc["bl_right"] = s.bl_right;
    doc["headlights"] = s.headlights;
    doc["glow_on"] = s.glow_on;
    doc["battery_ok"] = s.battery_ok;

    String jsonString;
    serializeJson(doc, jsonString);
    
    ws_.textAll(jsonString);
}

