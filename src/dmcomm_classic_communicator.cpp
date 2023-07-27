// This file is part of the DMComm project by BladeSabre. License: MIT.

#include "DMComm.h"

#define DMCOMM_CONF(name) pgm_read_word_near(dmcomm_conf::name + signal_type_)

namespace dmcomm_conf {
static const uint16_t active_level[]       PROGMEM = {LOW, LOW, HIGH};
static const uint16_t invert_bit_read[]    PROGMEM = {false, false, true};
static const uint16_t sensor_threshold[]   PROGMEM = {1900, 1900, 800};
static const uint16_t pre_idle_send[]      PROGMEM = {3000, 3000, 5000};
static const uint16_t pre_active_min_ms[]  PROGMEM = {40, 40, 30};
static const uint16_t pre_active_send_ms[] PROGMEM = {59, 60, 40};
static const uint16_t pre_active_max_ms[]  PROGMEM = {80, 80, 50};
static const uint16_t start_idle_min[]     PROGMEM = {1500, 1500, 9000};
static const uint16_t start_idle_send[]    PROGMEM = {2083, 2200, 11000};
static const uint16_t start_idle_max[]     PROGMEM = {2500, 3500, 13000};
static const uint16_t start_active_min[]   PROGMEM = {400, 1000, 4000};
static const uint16_t start_active_send[]  PROGMEM = {917, 1600, 6000};
static const uint16_t start_active_max[]   PROGMEM = {1400, 2600, 8000};
static const uint16_t bit_idle_min[]       PROGMEM = {500, 800, 1000};
static const uint16_t bit0_idle_send[]     PROGMEM = {1000, 1600, 4000};
static const uint16_t bit_idle_threshold[] PROGMEM = {1800, 2600, 3000};
static const uint16_t bit1_idle_send[]     PROGMEM = {2667, 4000, 1400};
static const uint16_t bit_idle_max[]       PROGMEM = {3400, 5500, 4500};
static const uint16_t bit_active_min[]     PROGMEM = {1000, 1200, 1200};
static const uint16_t bit1_active_send[]   PROGMEM = {1667, 1600, 4400};
static const uint16_t bit0_active_send[]   PROGMEM = {3167, 4000, 1600};
static const uint16_t bit_active_max[]     PROGMEM = {4000, 5500, 5000};
static const uint16_t cooldown_send[]      PROGMEM = {400, 400, 200};
static const uint16_t reply_timeout_ms[]   PROGMEM = {100, 100, 100};
}  // namespace dmcomm_conf

namespace DMComm {

ClassicCommunicator::ClassicCommunicator(BaseProngOutput& output, BaseProngInput& input) {
    output_ = &output;
    input_ = &input;
    signal_type_ = kSignalTypeV;
}

void ClassicCommunicator::prepare(SignalType signal_type, uint16_t timeout_ms) {
    signal_type_ = signal_type;
    initial_timeout_ms_ = timeout_ms;
    initial_receive_ = true;
    uint8_t active_level = (uint8_t) DMCOMM_CONF(active_level);
    output_->begin();
    output_->setActiveLevel(active_level);
    input_->begin();
    input_->setActiveLevel(active_level);
    input_->setThreshold(DMCOMM_CONF(sensor_threshold));
}

void ClassicCommunicator::send(uint16_t data[], uint16_t length) {
    uint16_t bits = data[0];
    initial_receive_ = false;
    output_->driveIdle();
    delayMicroseconds(DMCOMM_CONF(pre_idle_send));
    output_->driveActive();
    delay(DMCOMM_CONF(pre_active_send_ms));
    output_->driveIdle();
    delayMicroseconds(DMCOMM_CONF(start_idle_send));
    output_->driveActive();
    delayMicroseconds(DMCOMM_CONF(start_active_send));
    for (int8_t i = 0; i < 16; i ++) {
        if (bits & 1) {
            output_->driveIdle();
            delayMicroseconds(DMCOMM_CONF(bit1_idle_send));
            output_->driveActive();
            delayMicroseconds(DMCOMM_CONF(bit1_active_send));
        } else {
            output_->driveIdle();
            delayMicroseconds(DMCOMM_CONF(bit0_idle_send));
            output_->driveActive();
            delayMicroseconds(DMCOMM_CONF(bit0_active_send));
        }
        bits >>= 1;
    }
    output_->driveIdle();
    delayMicroseconds(DMCOMM_CONF(cooldown_send));
    output_->release();
}

ReceiveOutcome ClassicCommunicator::receive(uint16_t buffer[], uint16_t buffer_size) {
    ReceiveOutcome outcome;
    uint16_t bits = 0;
    uint32_t start_timeout = initial_receive_ ? initial_timeout_ms_ : DMCOMM_CONF(reply_timeout_ms);
    start_timeout *= 1000;
    if (initial_receive_) {
        outcome = waitFrom(true, 0, start_timeout, -3);
        if (outcome.status == kErrorTimeout) {
            outcome.status = kStatusNothing;
            return outcome;
        }
        start_timeout -= outcome.last_duration;
    }
    outcome = waitFrom(false, 0, start_timeout, -2);
    if (outcome.status == kErrorTimeout) {
        outcome.status = kStatusNothing;
        return outcome;
    }
    initial_receive_ = false;
    outcome = waitFrom(true,
        1000L * DMCOMM_CONF(pre_active_min_ms),
        1000L * DMCOMM_CONF(pre_active_max_ms),
        -2);
    if (outcome.status != kStatusReceived) {
        return outcome;
    }
    outcome = waitFrom(false, DMCOMM_CONF(start_idle_min), DMCOMM_CONF(start_idle_max), -1);
    if (outcome.status != kStatusReceived) {
        return outcome;
    }
    outcome = waitFrom(true, DMCOMM_CONF(start_active_min), DMCOMM_CONF(start_active_max), -1);
    if (outcome.status != kStatusReceived) {
        return outcome;
    }
    for (int8_t i = 0; i < 16; i ++) {
        outcome = waitFrom(false, DMCOMM_CONF(bit_idle_min), DMCOMM_CONF(bit_idle_max), i);
        if (outcome.status != kStatusReceived) {
            return outcome;
        }
        bits >>= 1;
        if (outcome.last_duration > DMCOMM_CONF(bit_idle_threshold)) {
            bits |= 0x8000;
        }
        outcome = waitFrom(true, DMCOMM_CONF(bit_active_min), DMCOMM_CONF(bit_active_max), i);
        if (outcome.status != kStatusReceived) {
            if (outcome.status == kErrorTimeout && signal_type_ == kSignalTypeX && i == 15) {
                // iC bug, ignore
            } else {
                return outcome;
            }
        }
        if (DMCOMM_CONF(invert_bit_read)) {
            bits ^= 0xFFFF;
        }
    }
    buffer[0] = bits;
    outcome.result_length = 1;
    return outcome;
}

ReceiveOutcome ClassicCommunicator::waitFrom(bool active, uint32_t dur_min, uint32_t dur_max, int16_t reached_bit) {
    ReceiveOutcome outcome = {};
    outcome.reached_bit = reached_bit;
    outcome.reached_active = active;
    outcome.last_duration = input_->waitFor(!active, dur_max);
    if (outcome.last_duration == DMCOMM_TIMED_OUT) {
        outcome.status = kErrorTimeout;
        return outcome;
    }
    if (outcome.last_duration < dur_min) {
        outcome.status = kErrorTooShort;
        return outcome;
    }
    outcome.status = kStatusReceived;
    return outcome;
}

}  // namespace DMComm
