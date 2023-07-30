// This file is part of the DMComm project by BladeSabre. License: MIT.

#ifndef DMCOMM_H_
#define DMCOMM_H_

#include <Arduino.h>

#define DMCOMM_NO_PIN 0xFF
#define DMCOMM_TICK_MICROS 200
#define DMCOMM_COMMAND_BUFFER_SIZE 64
#define DMCOMM_SERIAL_TIMEOUT_MILLIS 6000
#define DMCOMM_GOFIRST_REPEAT_MILLIS 5000
#define DMCOMM_LISTEN_TIMEOUT_MILLIS 5000
#define DMCOMM_INACTIVE_DELAY_MILLIS 3000
#define DMCOMM_TIMED_OUT 0xFFFFFFFF

namespace DMComm {

/**
 * Call pinMode unless pin is DMCOMM_NO_PIN.
 */
void pinModeMaybe(uint8_t pin, uint8_t mode);

/**
 * Call digitalWrite unless pin is DMCOMM_NO_PIN.
 */
void digitalWriteMaybe(uint8_t pin, uint8_t val);

}  // namespace DMComm

#include "portable/dmcomm_printonly.h"
#include "dmcomm_pin_control.h"
#include "dmcomm_base_communicator.h"
#include "dmcomm_classic_communicator.h"
#include "dmcomm_controller.h"

#endif  // DMCOMM_H_
