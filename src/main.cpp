#include <Arduino.h>
#include "Dashboard.h"
#include "TractorInputs.h"
#include "RemoteServer.h" // <-- Include our new class

// --- Global Objects ---
Dashboard *dashboard = nullptr;
ITractorInputs *inputs = nullptr;
RemoteServer *remoteServer = nullptr;

// In src/main.cpp

void setup()
{
    Serial.begin(115200);

    // --- REVISED INITIALIZATION ORDER ---

    // 1. Initialize hardware and local objects first.
    inputs = createTractorInputs();
    inputs->begin();

    TFT_eSPI *tft = new TFT_eSPI();
    dashboard = new Dashboard(*tft);
    dashboard->begin();

    // 3. Initialize Remote Server LAST, passing it the inputs pointer.
    remoteServer = new RemoteServer();
    remoteServer->begin(inputs); // <-- PASS THE POINTER HERE


    // Now it is safe to draw the splash screen to the display.
    dashboard->drawSplash();
    delay(1000);
}

void loop()
{
    // Read the latest state from the tractor
    TractorState s = inputs->read();

    // Send the state to the TFT dashboard
    dashboard->draw(s);

    // Broadcast the state to any connected web clients
    static unsigned long lastWsSend = 0;
    if (millis() - lastWsSend > 100)
    { // Send data 10 times per second
        lastWsSend = millis();
        // Debug: print the TractorState before sending
        Serial.printf(
            "WS Send -> RPM: %u, Fuel: %.1f%%, Temp: %.1f°C, Oil OK: %d, BL Left: %d, BL Right: %d, Lights: %d, Glow: %d, Batt OK: %d\n",
            s.rpm,
            s.fuel_pct,
            s.temp_c,
            s.oil_ok,
            s.bl_left,
            s.bl_right,
            s.headlights,
            s.glow_on,
            s.battery_ok);
        remoteServer->broadcastState(s);
    }

    delay(1000 / TARGET_FPS);
}