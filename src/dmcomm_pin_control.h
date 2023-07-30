// This file is part of the DMComm project by BladeSabre. License: MIT.

#ifndef DMCOMM_PIN_CONTROL_H_
#define DMCOMM_PIN_CONTROL_H_

namespace DMComm {

class BaseProngOutput {
public:
    virtual ~BaseProngOutput() {}
    virtual void begin() = 0;
    virtual void end() = 0;
    virtual void driveActive() = 0;
    virtual void driveIdle() = 0;
    virtual void release() = 0;
    void setActiveLevel(uint8_t level);
protected:
    uint8_t idle_level_;
    uint8_t active_level_;
};

class DComOutput : public BaseProngOutput {
public:
    DComOutput(uint8_t pin_out, uint8_t pin_notOE=DMCOMM_NO_PIN);
    ~DComOutput();
    void begin();
    void end();
    void driveActive();
    void driveIdle();
    void release();
private:
    uint8_t pin_out_;
    uint8_t pin_notOE_;
};

class BaseProngInput {
public:
    virtual ~BaseProngInput() {}
    virtual void begin() = 0;
    virtual void end() = 0;
    virtual bool isActive() = 0;
    virtual void setThreshold(uint16_t threshold_mV) = 0;
    void setActiveLevel(uint8_t level);
    uint32_t waitForActive(uint32_t timeout);
    uint32_t waitForIdle(uint32_t timeout);
    uint32_t waitFor(bool active, uint32_t timeout);
protected:
    uint8_t idle_level_;
    uint8_t active_level_;
};

class AnalogProngInput : public BaseProngInput {
public:
    AnalogProngInput(uint8_t pin_in, uint16_t board_voltage_mV, uint8_t read_resolution);
    ~AnalogProngInput();
    void begin();
    void end();
    bool isActive();
    void setThreshold(uint16_t threshold_mV);
    uint16_t voltage();
private:
    uint8_t pin_in_;
    uint16_t board_voltage_mV_;
    uint8_t read_resolution_;
    uint16_t threshold_mV_;
    uint16_t threshold_units_;
};

}  // namespace DMComm

#endif  // DMCOMM_PIN_CONTROL_H_
