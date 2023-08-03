// This file is part of the DMComm project by BladeSabre. License: MIT.

#include "DMComm.h"

namespace DMComm {

SerialFollower::SerialFollower(Controller& controller, Stream& serial) :
    controller_(controller), serial_(serial) {}

SerialFollower::~SerialFollower() {
    delete_digirom();
}

void SerialFollower::loop() {
    uint8_t i = serialRead();
    if (i > 0) {
        serial_.print(F("got "));
        serial_.print(i, DEC);
        serial_.print(F(" bytes: "));
        serial_.write(command_buffer_, i);
        serial_.print(F(" -> "));
        DigiROMType rom_type = digiROMType(command_buffer_);
        delete_digirom();
        switch(rom_type.signal_type) {
            case kSignalTypeV:
            case kSignalTypeX:
            case kSignalTypeY:
                digirom_ = new ClassicDigiROM(command_buffer_);
                break;
            case kSignalTypeC:
                digirom_ = new WordsDigiROM(command_buffer_);
                break;
            default:
                break;
        }
        if (digirom_ != nullptr) {
            serial_.print(F("(new DigiROM)"));
        } else {
            serial_.print(F("(paused)"));
        }
        serial_.println();
        if (digirom_ != nullptr && digirom_->turn() == 1) {
            delay(DMCOMM_GOFIRST_BEFORE_MILLIS);
        }
    }
    if (digirom_ != nullptr) {
        controller_.execute(*digirom_);
        digirom_->printResult(serial_);
        serial_.println();
        if(digirom_->turn() == 1) {
            delay(DMCOMM_GOFIRST_REPEAT_MILLIS);
        }
    } else {
        delay(DMCOMM_INACTIVE_DELAY_MILLIS);
    }
}

uint8_t SerialFollower::serialRead() {
    uint32_t time_start;
    uint32_t time;
    int16_t incoming_int;
    uint8_t incoming_byte;
    uint8_t i = 0;
    if (serial_.available() == 0) {
        return 0;
    }
    time_start = millis();
    do {
        do {
            incoming_int = serial_.read();
            time = millis() - time_start;
            if (time > DMCOMM_SERIAL_TIMEOUT_MILLIS) {
                serial_.println(F("[No EOL received]"));
                return 0;
            }
        } while (incoming_int == -1);
        incoming_byte = incoming_int;
        if (incoming_byte != '\r' && incoming_byte != '\n') {
            command_buffer_[i] = incoming_byte;
            i += 1;
        }
    } while (incoming_byte != '\r' && incoming_byte != '\n' && i < DMCOMM_TEXT_DIGIROM_SIZE - 1);
    if (incoming_byte != '\r' && incoming_byte != '\n') {
        serial_.println(F("[DigiROM too long]"));
        while (serial_.read() != -1);  // empty the buffer
        return 0;
    }
    command_buffer_[i] = '\0';
    return i;
}

void SerialFollower::delete_digirom() {
    if (digirom_ != nullptr) {
        delete digirom_;
        digirom_ = nullptr;
    }
}

}  // namespace DMComm
