#include "TractorInputs.h"

class TractorInputsEmu : public ITractorInputs {
public:
  void begin() override {}
  TractorState read() override {
    static uint32_t t = 0;
    t += 1;
    TractorState s;
    s.rpm = 800 + (uint32_t)(2000.0 * (0.5 + 0.5 * sinf(t * 0.05f)));
    s.fuel_pct = 60.0f + 40.0f * sinf(t * 0.01f);
    s.temp_c = 70.0f + 20.0f * sinf(t * 0.007f);
    s.oil_ok = true;
    s.bl_left = (t / 20) % 20 < 10;
    s.bl_right = (t / 20) % 24 < 12;
    s.headlights = (t / 200) % 2;

    // --- ADD THESE NEW LINES ---

    // 1. Simulate the glow plug being on for the first 200 ticks (like starting the tractor)
    s.glow_on = (t < 200);

    // 2. Simulate a battery warning light that flashes every ~10 seconds
    s.battery_ok = (t / 400) % 2;

    return s;
  }
};

ITractorInputs* createTractorInputs() {
#if ENABLE_SW_EMULATOR
  static TractorInputsEmu inst;
  return &inst;
#else
  return nullptr; // Real impl compiled in other TU
#endif
}