// TractorInputsReal.cpp (or wherever this class lives)
// Complete, drop-in implementation using ESP32 PCNT for RPM measurement.

#include "TractorInputs.h"
#include <Arduino.h>
#include <math.h>
#include "driver/pcnt.h"

#ifndef ENABLE_SW_EMULATOR
#define ENABLE_SW_EMULATOR 1
#endif

// ========================= Tunables =========================
static constexpr int   PULSES_PER_REV = 2;      // Set per your source (coil/alt/crank)
static constexpr int   WINDOW_MS      = 100;    // Window for frequency over time
static constexpr float EMA_ALPHA      = 0.20f;  // Smoothing for display stability
// PCNT glitch filter: APB clock cycles (classic ESP32 APB ~80 MHz).
// 1000 cycles ≈ 12.5 µs. Tune to reject ignition noise without clipping legit pulses.
static constexpr uint16_t PCNT_GLITCH = 800;    // start ~8–12 µs; adjust after testing

// PCNT counter limits (int16 range on classic PCNT)
static constexpr int PCNT_H_LIM = 32767;
static constexpr int PCNT_L_LIM = -32768;

// ========================= Implementation =========================
class TractorInputsReal : public ITractorInputs {
public:
  void begin() override {
    // Discrete inputs (adjust polarity in read())
    pinMode(PIN_OIL,  INPUT_PULLUP);
    pinMode(PIN_BL_L, INPUT_PULLUP);
    pinMode(PIN_BL_R, INPUT_PULLUP);
    pinMode(PIN_HEAD, INPUT_PULLUP);
    pinMode(PIN_GLOW, INPUT_PULLUP);
    pinMode(PIN_BATT, INPUT_PULLUP);

    // RPM input — explicit is good practice even though PCNT will mux it
    pinMode(PIN_RPM, INPUT_PULLUP);

    setup_pcnt_();

    // Prime first update so the first read() computes immediately (avoids an initial 0)
    last_tick_ms_ = millis() - WINDOW_MS;
  }

  TractorState read() override {
    TractorState s{};

    // --------- RPM via PCNT (non-blocking windowed count) ---------
    if (pcnt_ok_) {
      const uint32_t now = millis();
      const uint32_t elapsed = now - last_tick_ms_;

      if (elapsed >= (uint32_t)WINDOW_MS) {
        // Commit a proper window sample: read & clear the hardware counter
        int16_t count = 0;
        pcnt_get_counter_value(PCNT_UNIT_0, &count);
        pcnt_counter_clear(PCNT_UNIT_0);
        last_tick_ms_ = now;

        // pulses/sec = count / (WINDOW_MS/1000)  ⇒  RPM = (pps * 60) / PPR
        const float pps = static_cast<float>(count) * (1000.0f / static_cast<float>(WINDOW_MS));
        const float rpm = (pps * 60.0f) / static_cast<float>(PULSES_PER_REV);

        // EMA smoothing for display/gauge stability
        rpm_filtered_ += EMA_ALPHA * (rpm - rpm_filtered_);
      } else if (elapsed >= 10) {
        // Optional: provisional estimate between windows (light touch)
        int16_t count_now = 0;
        pcnt_get_counter_value(PCNT_UNIT_0, &count_now);
        const float pps = static_cast<float>(count_now) * (1000.0f / static_cast<float>(elapsed));
        const float rpm = (pps * 60.0f) / static_cast<float>(PULSES_PER_REV);
        const float beta = 0.05f;  // gentle nudge; keeps non-blocking freshness
        rpm_filtered_ += beta * (rpm - rpm_filtered_);
      }

      s.rpm = static_cast<int>(roundf(rpm_filtered_));
    } else {
      // PCNT init failed; provide a safe default (or keep last)
      s.rpm = 0;
    }

    // --------- ADCs (placeholder scaling) ---------
    // NOTE: tune these scale factors to your actual sender curves.
    s.fuel_pct = constrain((analogRead(PIN_FUEL_ADC) / 4095.0f) * 100.0f, 0.0f, 100.0f);
    s.temp_c   = (analogRead(PIN_TEMP_ADC) / 4095.0f) * 120.0f;

    // --------- Digital inputs (adjust polarity to your wiring) ---------
    s.oil_ok     = (digitalRead(PIN_OIL)  == HIGH);
    s.bl_left    = (digitalRead(PIN_BL_L) == LOW);
    s.bl_right   = (digitalRead(PIN_BL_R) == LOW);
    s.headlights = (digitalRead(PIN_HEAD) == LOW);

    // Assumptions: glow indicator ON when signal LOW; battery OK when HIGH
    s.glow_on    = (digitalRead(PIN_GLOW) == LOW);
    s.battery_ok = (digitalRead(PIN_BATT) == HIGH);

    return s;
  }

private:
  void setup_pcnt_() {
    // Configure PCNT to count rising edges on PIN_RPM
    pcnt_config_t cfg{};
    cfg.pulse_gpio_num = static_cast<gpio_num_t>(PIN_RPM);
    cfg.ctrl_gpio_num  = PCNT_PIN_NOT_USED;
    cfg.unit           = PCNT_UNIT_0;
    cfg.channel        = PCNT_CHANNEL_0;
    cfg.pos_mode       = PCNT_COUNT_INC;   // count rising edges
    cfg.neg_mode       = PCNT_COUNT_DIS;   // ignore falling edges
    cfg.lctrl_mode     = PCNT_MODE_KEEP;
    cfg.hctrl_mode     = PCNT_MODE_KEEP;
    cfg.counter_h_lim  = PCNT_H_LIM;
    cfg.counter_l_lim  = PCNT_L_LIM;

    esp_err_t err = pcnt_unit_config(&cfg);
    if (err != ESP_OK) {
      pcnt_ok_ = false;
      return;
    }

    // Optional glitch filter to reject coil ringing / EMI
    if (PCNT_GLITCH > 0) {
      pcnt_set_filter_value(PCNT_UNIT_0, PCNT_GLITCH);
      pcnt_filter_enable(PCNT_UNIT_0);
    } else {
      pcnt_filter_disable(PCNT_UNIT_0);
    }

    pcnt_counter_clear(PCNT_UNIT_0);
    pcnt_counter_resume(PCNT_UNIT_0);
    pcnt_ok_ = true;
  }

  // State
  uint32_t last_tick_ms_ = 0;
  float    rpm_filtered_ = 0.0f;
  bool     pcnt_ok_      = false;
};

// ========================= Factory =========================
#if !ENABLE_SW_EMULATOR
ITractorInputs* createTractorInputs() {
  static TractorInputsReal inst;
  return &inst;
}
#endif