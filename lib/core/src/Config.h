#pragma once

#include <Arduino.h>

// Build-time behavior (can be overridden via build_flags)
#ifndef TARGET_FPS
  #define TARGET_FPS 40
#endif

#ifndef PPR_DEFAULT
  #define PPR_DEFAULT 1
#endif

// Basic data model for the tractor state
struct TractorState {
  uint32_t rpm = 0;       // engine RPM
  float    fuel_pct = 0;  // 0..100
  float    temp_c = 20;   // coolant temp °C
  bool     oil_ok = true;
  bool     bl_left = false;
  bool     bl_right = false;
  bool     headlights = false;
  bool     glow_on = false;     // For the glow plug indicator
  bool     battery_ok = true;   // For the battery charge indicator
};
