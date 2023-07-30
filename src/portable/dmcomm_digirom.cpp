// This file is part of the DMComm project by BladeSabre. License: MIT.

#include "dmcomm_digirom.h"

namespace DMComm {

void printHex(Print& dest, uint16_t value, uint8_t num_digits) {
    const uint8_t max_digits = 4;
    int8_t i;
    uint8_t digits[max_digits];
    if (num_digits > max_digits) {
        num_digits = max_digits;
    }
    for (i = 0; i < num_digits; i ++) {
        digits[i] = val2hex((uint8_t)value);
        value /= 0x10;
    }
    for (i = num_digits - 1; i >= 0; i --) {
        dest.write(digits[i]);
    }
}

void printReceiveOutcome(Print& dest, ReceiveOutcome outcome) {
    switch (outcome.status) {
    case kStatusReceived:
        dest.print(F("[Received]"));
        break;
    case kStatusNothing:
        dest.print(F("[Nothing]"));
        break;
    case kErrorBufferFull:
        dest.print(F("[Buffer full]"));
        break;
    case kErrorTooShort:
    case kErrorTimeout:
        dest.print(F("[Bit "));
        dest.print(outcome.current_bit);
        if (outcome.current_bit_active) {
            dest.print(F("active "));
        } else {
            dest.print(F("idle "));
        }
        if (outcome.status == kErrorTooShort) {
            dest.print(F("too short "));
            dest.print(outcome.last_duration);
            dest.write(']');
        } else {
            dest.print(F("timed out]"));
        }
        break;
    }
}


void ClassicCore::prepare() {
    length_ = 0;
    checksum_ = 0;
}

uint16_t ClassicCore::send(uint16_t bits,
        uint16_t copy_mask, uint16_t invert_mask,
        int8_t checksum_target, uint8_t check_digit_LSB_pos) {
    uint16_t bits_received = 0;
    if (length_ > 0) {
        bits_received = result_[length_ - 1].data;
    }
    bits &= ~copy_mask;
    bits |= copy_mask & bits_received;
    bits &= ~invert_mask;
    bits |= invert_mask & ~bits_received;
    if (checksum_target >= 0) {
        bits &= ~(0xF << check_digit_LSB_pos);
    }
    for (int8_t i = 0; i < 4; i ++) {
        checksum_ += bits >> (4 * i);
    }
    checksum_ %= 16;
    if (checksum_target >= 0) {
        uint16_t check_digit = (checksum_target - checksum_) % 16;
        bits |= check_digit << check_digit_LSB_pos;
        checksum_ = checksum_target;
    }
    result_append({kDataSent, bits});
    return bits;
}

void ClassicCore::receive(uint16_t data[], ReceiveOutcome outcome) {
    last_outcome_ = outcome;
    if (outcome.status == kStatusReceived) {
        result_append({kDataReceived, data[0]});
    } else if (outcome.status == kStatusNothing) {
        result_append({kNoData, 0});
    }
}

uint16_t ClassicCore::length() {
    return length_;
}

ClassicResultSegment ClassicCore::result(uint16_t i) {
    return result_[i];
}

void ClassicCore::printResult(Print& dest) {
    for (uint16_t i = 0; i < length_; i ++) {
        ResultSegmentType seg_type = result_[i].type;
        if (seg_type == kNoData) {
            dest.write('t');
        } else {
            if (seg_type == kDataSent) {
                dest.write('s');
            } else {
                dest.write('r');
            }
            dest.write(':');
            printHex(dest, result_[i].data, 4);
        }
        dest.write(' ');
    }
    switch (last_outcome_.status) {
    case kStatusReceived:
    case kStatusNothing:
        break;
    default:
        printReceiveOutcome(dest, last_outcome_);
    }
}

void ClassicCore::result_append(ClassicResultSegment item) {
    if (length_ < DMCOMM_CLASSIC_RESULT_SIZE) {
        result_[length_] = item;
        length_ ++;
    }
}


ClassicDigiROM::ClassicDigiROM(const char * digirom) : digirom_(digirom) {}

SignalType ClassicDigiROM::signal_type() {
    return signal_type_;
}

uint8_t ClassicDigiROM::turn() {
    return turn_;
}

void ClassicDigiROM::prepare() {
    //TODO
    signal_type_ = kSignalTypeX;
    turn_ = 2;
    cursor_ = 0;
    core_.prepare();
}

uint16_t ClassicDigiROM::send(uint16_t buffer[], uint16_t buffer_size) {
    //TODO stub function
    uint16_t bits;
    if (buffer_size < 1) {
        return 0;
    }
    switch(cursor_) {
    case 0:
        bits = core_.send(0x0069);
        break;
    case 1:
        bits = core_.send(0x2169);
        break;
    case 2:
        bits = core_.send(0x8009);
        break;
    case 3:
        bits = core_.send(0x0009, 0x0C00, 0x03F0, 4);  // for turn 2
        //bits = core_.send(0x9009);  // for turn 1
        break;
    default:
        return 0;
    }
    buffer[0] = bits;
    cursor_ ++;
    return 1;
}

void ClassicDigiROM::receive(uint16_t data[], ReceiveOutcome outcome) {
    core_.receive(data, outcome);
}

void ClassicDigiROM::printResult(Print& dest) {
    core_.printResult(dest);
}

}  // namespace DMComm
