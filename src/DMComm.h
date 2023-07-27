// This file is part of the DMComm project by BladeSabre. License: MIT.

#ifndef DMCOMM_H_
#define DMCOMM_H_

#include <Arduino.h>

#define DMCOMM_NO_PIN 0xFF
#define DMCOMM_TICK_MICROS 200
#define DMCOMM_COMMAND_BUFFER_SIZE 64
#define DMCOMM_SERIAL_TIMEOUT_MILLIS 6000
#define DMCOMM_GOFIRST_REPEAT_MILLIS 5000
#define DMCOMM_INACTIVE_DELAY_MILLIS 3000
#define DMCOMM_TIMED_OUT 0xFFFFFFFF

namespace DMComm {

/**
 * The signal type for communicating with the toy.
 * V for 2-prong, X for 3-prong, Y for Xros Mini.
 */
enum SignalType {kSignalTypeV = 0, kSignalTypeX = 1, kSignalTypeY = 2};

/**
 * Call pinMode unless pin is DMCOMM_NO_PIN.
 */
void pinModeMaybe(uint8_t pin, uint8_t mode);

/**
 * Call digitalWrite unless pin is DMCOMM_NO_PIN.
 */
void digitalWriteMaybe(uint8_t pin, uint8_t val);

/**
 * Return integer value of hex digit character, or -1 if not a hex digit.
 */
int8_t hex2val(int8_t hexdigit);

/**
 * Return uppercase hex digit character for lowest 4 bits of input byte.
 */
int8_t val2hex(int8_t value);

}  // namespace DMComm

#include "dmcomm_pin_control.h"
#include "dmcomm_base_communicator.h"
#include "dmcomm_classic_communicator.h"

#endif  // DMCOMM_H_
