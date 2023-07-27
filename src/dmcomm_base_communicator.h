// This file is part of the DMComm project by BladeSabre. License: MIT.

#ifndef DMCOMM_BASE_COMMUNICATOR_H_
#define DMCOMM_BASE_COMMUNICATOR_H_

#include "DMComm.h"

namespace DMComm {

enum ReceiveStatus {kStatusReceived, kStatusNothing, kErrorBufferFull, kErrorTooShort, kErrorTimeout};

struct ReceiveOutcome {
    ReceiveStatus status = kStatusNothing;
    uint16_t result_length = 0;
    uint32_t last_duration = 0;
    int16_t reached_bit = 0;
    bool reached_active = false;
};

class BaseCommunicator {
public:
    virtual void prepare(SignalType signal_type, uint16_t timeout_ms) = 0;
    virtual void send(uint16_t data[], uint16_t length) = 0;
    virtual ReceiveOutcome receive(uint16_t buffer[], uint16_t buffer_size) = 0;
};

}  // namespace DMComm

#endif  // DMCOMM_BASE_COMMUNICATOR_H_
