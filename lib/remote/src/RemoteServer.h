#pragma once
#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include "Config.h" // To know about TractorState
#include "TractorInputs.h"


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

    // ADDED: A member to hold a pointer to the inputs object
    ITractorInputs* inputs_ = nullptr; 


    // Private helper for handling WebSocket events
    // (No longer needed)
    // static void onWebSocketEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);
};