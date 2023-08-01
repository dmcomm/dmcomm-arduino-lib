// This file is part of the DMComm project by BladeSabre. License: MIT.

#ifndef DMCOMM_SERIAL_H_
#define DMCOMM_SERIAL_H_

#include "DMComm.h"

namespace DMComm {

class SerialFollower {
public:
    SerialFollower(Controller& controller, Stream& serial);
    ~SerialFollower();
    void loop();
private:
    Controller& controller_;
    Stream& serial_;
    char command_buffer_[DMCOMM_TEXT_DIGIROM_SIZE];
    BaseDigiROM * digirom_ = nullptr;

    /**
     * Try to read from serial into command buffer.
     * Read until end-of-line and replace that with a null terminator.
     * Return 0 on failure, or positive integer for number of characters read.
     */
    uint8_t serialRead();
    void delete_digirom();
};

}  // namespace DMComm

#endif  // DMCOMM_SERIAL_H_
