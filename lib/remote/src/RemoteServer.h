#pragma once
#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include "Config.h"
#include "TractorInputs.h"
#include "secrets.h"



class RemoteServer {
public:
    // Constructor
    RemoteServer();

    // Initializes WiFi and starts the servers
    void begin(ITractorInputs* inputs);

    // Broadcasts the latest tractor state to all web clients
    void broadcastState(const TractorState& s);

private:
    // The web server object
    AsyncWebServer server_;

    // The WebSocket object
    AsyncWebSocket ws_;

    // A member to hold a pointer to the inputs object
    ITractorInputs* inputs_ = nullptr; 
};