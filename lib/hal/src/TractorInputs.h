#pragma once
#include <Arduino.h>
#include "Config.h"

// Abstract-ish interface for inputs
class ITractorInputs {
public:
  virtual ~ITractorInputs() = default;
  virtual void begin() = 0;
  virtual TractorState read() = 0;
};

// Factory selected by ENABLE_SW_EMULATOR
ITractorInputs* createTractorInputs();
