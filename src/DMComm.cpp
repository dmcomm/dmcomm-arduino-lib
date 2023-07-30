/* This file is part of the DMComm project by BladeSabre. License: MIT. */

#include "DMComm.h"

namespace DMComm {

void pinModeMaybe(uint8_t pin, uint8_t mode) {
    if (pin != DMCOMM_NO_PIN) {
        pinMode(pin, mode);
    }
}

void digitalWriteMaybe(uint8_t pin, uint8_t val) {
    if (pin != DMCOMM_NO_PIN) {
        digitalWrite(pin, val);
    }
}

} /* namespace DMComm */
