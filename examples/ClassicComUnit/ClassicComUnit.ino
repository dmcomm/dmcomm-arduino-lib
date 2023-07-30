// This file is part of the DMComm project by BladeSabre. License: MIT.

// Program for 5V AVR Arduino boards.
// Aiming for same behaviour as non-library version of DMComm.

#include <DMComm.h>

using namespace DMComm;

DComOutput output = DComOutput(A2, A1);
AnalogProngInput input = AnalogProngInput(A3, 5000, 10);
ClassicCommunicator comm = ClassicCommunicator(output, input);

Controller controller = Controller();
SerialFollower serial_follower = SerialFollower(controller, Serial);

void setup () {
    Serial.begin(9600);
    controller.add(comm);
}

void loop() {
    serial_follower.loop();
}
