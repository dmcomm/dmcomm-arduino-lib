// This file is part of the DMComm project by BladeSabre. License: MIT.

#ifndef DMCOMM_DIGIROM_H_
#define DMCOMM_DIGIROM_H_

#include "dmcomm_printonly.h"

#define DMCOMM_CLASSIC_RESULT_SIZE 21

namespace DMComm {

/**
  * Print `value` as hex, with specified number of digits up to 4 (with leading zeros).
  * If too few digits to display that number, will take the least significant.
  */
void printHex(Print& dest, uint16_t value, uint8_t num_digits);

void printReceiveOutcome(Print& dest, ReceiveOutcome outcome);

class BaseDigiROM {
public:
    virtual ~BaseDigiROM() {}
    virtual SignalType signal_type() = 0;
    virtual uint8_t turn() = 0;
    virtual void prepare() = 0;
    virtual int16_t send(uint16_t buffer[], uint16_t buffer_size) = 0;
    virtual void receive(uint16_t data[], ReceiveOutcome outcome) = 0;
    virtual void printResult(Print& dest) = 0;
};

class ClassicCore {
public:
    void prepare();
    uint16_t send(uint16_t bits,
        uint16_t copy_mask=0, uint16_t invert_mask=0,
        int8_t checksum_target=-1, uint8_t check_digit_LSB_pos=12);
    void receive(uint16_t data[], ReceiveOutcome outcome);
    uint16_t length();
    ClassicResultSegment result(uint16_t i);
    void printResult(Print& dest);
private:
    ClassicResultSegment result_[DMCOMM_CLASSIC_RESULT_SIZE];
    uint16_t length_ = 0;
    uint16_t checksum_;
    ReceiveOutcome last_outcome_;
    void result_append(ClassicResultSegment item);
};

class ClassicDigiROM : public BaseDigiROM {
public:
    ClassicDigiROM(const char * digirom);
    SignalType signal_type();
    uint8_t turn();
    void prepare();
    int16_t send(uint16_t buffer[], uint16_t buffer_size);
    void receive(uint16_t data[], ReceiveOutcome outcome);
    void printResult(Print& dest);
private:
    const char * digirom_;
    ClassicCore core_;
    SignalType signal_type_;
    uint8_t turn_;
    uint8_t data_start_;
    const char * cursor_;
};

}  // namespace DMComm

#endif  // DMCOMM_DIGIROM_H_
