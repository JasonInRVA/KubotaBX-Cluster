#pragma once
#include <TFT_eSPI.h>
#include "Config.h"

class Dashboard {
public:
    explicit Dashboard(TFT_eSPI& tft) : tft_(tft) {}
    void begin();
    void drawSplash();
    void draw(const TractorState& s);

private:
    // Added a boolean to reverse the color zones for the fuel gauge
    void drawArcGauge(int x, int y, int radius, float value, float minVal, float maxVal, const char* label, const char* unit, bool reverseZones = false);

    TFT_eSPI& tft_;
    TFT_eSprite* sprite_ = nullptr;
};