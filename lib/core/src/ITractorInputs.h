#pragma once
#include "Config.h" // For TractorState

// This is the Interface. It's a contract for any class that provides tractor data.
class ITractorInputs {
public:
    // A virtual destructor is important for base classes
    virtual ~ITractorInputs() {} 

    // The "= 0" means any class that inherits from this one MUST provide its own
    // version of these functions.
    virtual void begin() = 0;
    virtual TractorState getState() = 0;
};