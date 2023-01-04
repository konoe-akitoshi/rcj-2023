#ifdef PREVENT_SORCING_FROM_ARDUINO
#ifdef LOCAL_INCLUDE
#include "../local/arduino_deps.h"
#else
#endif

#include "components/motor_controller.hpp"
#include "components/setup_handler.hpp"
#include "pin.hpp"
#include "utils/serial_reader.hpp"

component::SetupHandler Handler;
const component::Motor Motor(Handler, PIN_MOTOR1_FORWARD_BRAKE, PIN_MOTOR1_REVERSE_BRAKE, PIN_MOTOR1_PWM, 37000);
const util::SerialReader Reader;

void setup() {
    Serial.begin(9600);
    Handler.Setup();
}

void loop() {
    int power = Reader.ReadInt();
    // NEXT: Serial.printlnがないとコンパイルエラーになる。原因を調査する
    Serial.println(power);
    Motor.Start(power);
}

#endif
