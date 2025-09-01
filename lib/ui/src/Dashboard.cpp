#include "Dashboard.h"
#include <math.h>


// Define a custom color for the "ghost" outlines
#define TFT_DARKGREY 0x4A49

#define DEG2RAD 0.0174532925

// --- HELPER FUNCTIONS FOR INDICATOR ICONS ---

void drawGlowIndicator(TFT_eSprite* sprite, int x, int y, bool on) {
    uint16_t color = on ? TFT_ORANGE : TFT_DARKGREY;
    if (on) {
        sprite->fillCircle(x, y, 10, color);
        sprite->fillCircle(x, y, 7, TFT_BLACK);
    }
    sprite->drawCircle(x, y, 10, color);
    sprite->fillRect(x - 2, y - 14, 4, 5, on ? color : TFT_BLACK);
    if (!on) sprite->drawRect(x - 2, y - 14, 4, 5, color);
}

void drawBatteryIndicator(TFT_eSprite* sprite, int x, int y, bool ok) {
    uint16_t color = ok ? TFT_DARKGREY : TFT_RED;
    if (!ok) {
        sprite->fillRect(x - 12, y - 8, 24, 16, color);
        sprite->fillRect(x + 12, y - 4, 4, 8, color);
    }
    sprite->drawRect(x - 12, y - 8, 24, 16, color);
    sprite->drawRect(x + 12, y - 4, 4, 8, color);
    sprite->setTextColor(!ok ? TFT_BLACK : TFT_DARKGREY);
    sprite->setTextDatum(MC_DATUM);
    sprite->setFreeFont(&FreeMonoBold9pt7b);
    sprite->drawString("+", x - 5, y-2);
    sprite->drawString("-", x + 6, y-2);
    sprite->setFreeFont(nullptr);
}

void drawOilIndicator(TFT_eSprite* sprite, int x, int y, bool ok) {
    uint16_t color = ok ? TFT_DARKGREY : TFT_RED;
    if (!ok) {
       sprite->fillRect(x - 10, y - 10, 20, 14, color);
       sprite->fillRect(x - 12, y - 13, 5, 3, color);
       sprite->fillTriangle(x + 12, y + 4, x + 16, y + 4, x + 14, y + 9, color);
    }
    sprite->drawRect(x - 10, y - 10, 20, 14, color);
    sprite->drawRect(x - 12, y - 13, 5, 3, color);
    sprite->drawTriangle(x + 12, y + 4, x + 16, y + 4, x + 14, y + 9, color);
}

void drawHeadlightIndicator(TFT_eSprite* sprite, int x, int y, bool on) {
    uint16_t color = on ? TFT_BLUE : TFT_DARKGREY;
    if (on) {
        sprite->fillCircle(x, y, 10, color);
        sprite->fillRect(x - 10, y - 10, 10, 20, TFT_BLACK);
    }
    sprite->drawCircle(x, y, 10, color);
    sprite->drawRect(x - 10, y - 10, 10, 20, color);
}

void drawLeftBlinker(TFT_eSprite* sprite, bool on) {
    uint16_t color = on ? TFT_GREEN : TFT_DARKGREY;
    if (on) {
        sprite->fillTriangle(20, 20, 40, 10, 40, 30, color);
        sprite->fillRect(40, 15, 10, 10, color);
    }
    sprite->drawTriangle(20, 20, 40, 10, 40, 30, color);
    sprite->drawRect(40, 15, 10, 10, color);
}

void drawRightBlinker(TFT_eSprite* sprite, bool on) {
    uint16_t color = on ? TFT_GREEN : TFT_DARKGREY;
    if (on) {
        sprite->fillTriangle(460, 20, 440, 10, 440, 30, color);
        sprite->fillRect(430, 15, 10, 10, color);
    }
    sprite->drawTriangle(460, 20, 440, 10, 440, 30, color);
    sprite->drawRect(430, 15, 10, 10, color);
}

// --- Main Dashboard Class ---

void Dashboard::begin() {
    tft_.init();
    tft_.setRotation(1);
    tft_.fillScreen(TFT_BLACK);

    sprite_ = new TFT_eSprite(&tft_);
    sprite_->setColorDepth(16);
    sprite_->createSprite(480, 320);
    bool ok = sprite_->created();
    Serial.printf("createSprite(480,320) ok? %s\n", ok ? "YES" : "NO");
    Serial.printf("heap=%u, psram=%u\n", ESP.getFreeHeap(), ESP.getPsramSize());
    sprite_->fillSprite(TFT_BLACK);
}

void Dashboard::drawSplash() {
    sprite_->fillSprite(TFT_BLACK);
    sprite_->setTextColor(TFT_WHITE, TFT_BLACK);

    sprite_->setTextFont(1);      // built-in font
    sprite_->setTextSize(1);
    sprite_->setTextDatum(MC_DATUM);  // ← change from TL_DATUM to MC_DATUM

    Serial.println("About to draw centered");
    sprite_->drawString("BX Cluster", 240, 160);          // keep built-in font
    sprite_->setFreeFont(&FreeSans9pt7b);                 // switch to FreeFont for test
    sprite_->drawString("Initializing...", 240, 190);     // FreeFont-drawn string
    sprite_->setFreeFont(nullptr);                        // restore
    Serial.println("After draw centered");

    sprite_->pushSprite(0, 0);
}

void Dashboard::drawArcGauge(int x, int y, int radius, float value, float minVal, float maxVal, const char* label, const char* unit, bool reverseZones) {
    int start_angle = 90;
    int end_angle = 270;

    if (reverseZones) {
        int red_end_angle = map(maxVal * 0.15, minVal, maxVal, start_angle, end_angle);
        int yellow_end_angle = map(maxVal * 0.35, minVal, maxVal, start_angle, end_angle);
        sprite_->drawArc(x, y, radius, radius - 12, start_angle, red_end_angle, TFT_RED, TFT_BLACK);
        sprite_->drawArc(x, y, radius, radius - 12, red_end_angle, yellow_end_angle, TFT_YELLOW, TFT_BLACK);
        sprite_->drawArc(x, y, radius, radius - 12, yellow_end_angle, end_angle, TFT_GREEN, TFT_BLACK);
    } else {
        int green_end_angle = map(maxVal * 0.65, minVal, maxVal, start_angle, end_angle);
        int yellow_end_angle = map(maxVal * 0.85, minVal, maxVal, start_angle, end_angle);
        sprite_->drawArc(x, y, radius, radius - 12, start_angle, green_end_angle, TFT_GREEN, TFT_BLACK);
        sprite_->drawArc(x, y, radius, radius - 12, green_end_angle, yellow_end_angle, TFT_YELLOW, TFT_BLACK);
        sprite_->drawArc(x, y, radius, radius - 12, yellow_end_angle, end_angle, TFT_RED, TFT_BLACK);
    }

    float tft_angle = map(value, minVal, maxVal, start_angle, end_angle);
    if (value < minVal) tft_angle = start_angle;
    if (value > maxVal) tft_angle = end_angle;
    float math_angle_rad = (-tft_angle + 270.0) * DEG2RAD;
    int needle_tip_x = x + (radius - 5) * cos(math_angle_rad);
    int needle_tip_y = y - (radius - 5) * sin(math_angle_rad);
    sprite_->fillTriangle(
        x + 2 * cos(math_angle_rad + M_PI_2), y - 2 * sin(math_angle_rad + M_PI_2),
        needle_tip_x, needle_tip_y,
        x + 2 * cos(math_angle_rad - M_PI_2), y - 2 * sin(math_angle_rad - M_PI_2),
        TFT_WHITE
    );
    sprite_->fillCircle(x, y, 5, TFT_WHITE);

    sprite_->setTextColor(TFT_WHITE, TFT_BLACK);
    sprite_->setTextDatum(MC_DATUM);
    sprite_->setFreeFont(&FreeSans9pt7b);
    sprite_->drawString(String((int)value), x, y - 20);
    sprite_->setFreeFont(&FreeSans9pt7b);
    sprite_->drawString(label, x, y + 5);
    sprite_->setFreeFont(nullptr);
}

void Dashboard::draw(const TractorState& s) {
    sprite_->fillSprite(TFT_BLACK);

    // --- Indicator Icons - Top Row ---
    drawLeftBlinker(sprite_, s.bl_left);
    drawOilIndicator(sprite_, 120, 25, s.oil_ok);
    drawGlowIndicator(sprite_, 180, 25, s.glow_on);
    drawBatteryIndicator(sprite_, 300, 25, s.battery_ok);
    drawHeadlightIndicator(sprite_, 360, 25, s.headlights);
    drawRightBlinker(sprite_, s.bl_right);

    // --- Gauges ---
    drawArcGauge(240, 280, 120, s.rpm, 0, 4000, "RPM", "", false);
    drawArcGauge(100, 120, 70, s.temp_c, 40, 120, "Coolant", "C", false);
    drawArcGauge(380, 120, 70, s.fuel_pct, 0, 100, "Fuel", "%", true);

    sprite_->pushSprite(0, 0);
}