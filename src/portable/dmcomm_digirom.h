// This file is part of the DMComm project by BladeSabre. License: MIT.

#ifndef DMCOMM_DIGIROM_H_
#define DMCOMM_DIGIROM_H_

#include "dmcomm_printonly.h"

#define DMCOMM_CLASSIC_RESULT_SIZE 21
#define DMCOMM_WORDS_RESULT_SIZE 5
#define DMCOMM_WORDS_RESULT_SEGMENT_SIZE 8

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

class BaseCore {
public:
    virtual ~BaseCore() {}
    uint16_t length();
    void printResult(Print& dest);
protected:
    virtual void printResultSegmentData(Print& dest, uint16_t index) = 0;
    ResultSegmentType result_types_[DMCOMM_CLASSIC_RESULT_SIZE];
    uint16_t length_ = 0;
    ReceiveOutcome last_outcome_;
};

class ClassicCore : public BaseCore {
public:
    void prepare();
    uint16_t send(uint16_t bits,
        uint16_t copy_mask=0, uint16_t invert_mask=0,
        int8_t checksum_target=-1, uint8_t check_digit_LSB_pos=12);
    void receive(uint16_t data[], ReceiveOutcome outcome);
    //ClassicResultSegment result(uint16_t i);
protected:
    void printResultSegmentData(Print& dest, uint16_t index);
private:
    uint16_t result_data_[DMCOMM_CLASSIC_RESULT_SIZE];
    uint16_t checksum_;
    void result_append(ResultSegmentType type, uint16_t data);
};

struct WordsResultSegment {
    uint16_t length;
    uint16_t data[DMCOMM_WORDS_RESULT_SEGMENT_SIZE];
};

class WordsCore : public BaseCore {
public:
    void prepare();
    void send(uint16_t data[], uint16_t length);
    void receive(uint16_t data[], ReceiveOutcome outcome);
    //result?
protected:
    void printResultSegmentData(Print& dest, uint16_t index);
private:
    WordsResultSegment result_segments_[DMCOMM_WORDS_RESULT_SIZE];
    void result_append(ResultSegmentType type, uint16_t data[], uint16_t segment_length);
};

class BaseTextDigiROM : public BaseDigiROM {
public:
    SignalType signal_type();
    uint8_t turn();
protected:
    SignalType signal_type_;
    uint8_t turn_;
};

class ClassicDigiROM : public BaseTextDigiROM {
public:
    ClassicDigiROM(const char * digirom);
    void prepare();
    int16_t send(uint16_t buffer[], uint16_t buffer_size);
    void receive(uint16_t data[], ReceiveOutcome outcome);
    void printResult(Print& dest);
private:
    const char * digirom_;
    ClassicCore core_;
    uint8_t data_start_;
    const char * cursor_;
};

class WordsDigiROM : public BaseTextDigiROM {
public:
    WordsDigiROM(const char * digirom);
    void prepare();
    int16_t send(uint16_t buffer[], uint16_t buffer_size);
    void receive(uint16_t data[], ReceiveOutcome outcome);
    void printResult(Print& dest);
private:
    const char * digirom_;
    WordsCore core_;
    uint8_t data_start_;
    const char * cursor_;
};

}  // namespace DMComm

#endif  // DMCOMM_DIGIROM_H_
