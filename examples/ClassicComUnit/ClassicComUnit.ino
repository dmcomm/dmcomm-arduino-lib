// This file is part of the DMComm project by BladeSabre. License: MIT.

// Program for 5V AVR Arduino boards (pins specified for classic D-Com),
// or RP2040 (pins specified for recommended Pi Pico layout).
// Aiming for same behaviour as non-library version of DMComm.

#if defined(ARDUINO_ARCH_AVR) && !defined(USB_PID) && (SERIAL_RX_BUFFER_SIZE < 128)
#error : require SERIAL_RX_BUFFER_SIZE=128
#endif

#include <DMComm.h>

using namespace DMComm;

#ifdef ARDUINO_RASPBERRY_PI_PICO
LED_Indicator led = LED_Indicator(25);
PComOutput output = PComOutput(19, 20, 21);
DigitalProngInput input = DigitalProngInput(22);
AnalogProngInput analog_test_input = AnalogProngInput(26, 3300, 10);
ProngTester prong_tester = ProngTester(output, input, analog_test_input);
#else
LED_Indicator led = LED_Indicator(13);
DComOutput output = DComOutput(A2, A1);
AnalogProngInput input = AnalogProngInput(A3, 5000, 10);
ProngTester prong_tester = ProngTester(output, input, input);
#endif

ClassicCommunicator classic_comm = ClassicCommunicator(output, input);
ColorCommunicator color_comm = ColorCommunicator(output, input);
Controller controller = Controller();
SerialFollower serial_follower = SerialFollower(controller, Serial);

void setup () {
    Serial.begin(9600);
    controller.add(classic_comm);
    controller.add(color_comm);
    serial_follower.setIndicator(led);
    serial_follower.setProngTester(prong_tester);
}

void loop() {
    serial_follower.loop();
}
