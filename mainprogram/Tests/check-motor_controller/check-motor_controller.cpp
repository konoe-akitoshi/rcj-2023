#ifdef PREVENT_SORCING_FROM_ARDUINO
#ifdef LOCAL_INCLUDE
#include "../local/arduino_deps.hpp"
#else
#endif

#include "components/motor_controller.hpp"
#include "components/setup_handler.hpp"
#include "pin.hpp"
#include "utils/serial_reader.hpp"

component::SetupHandler Handler;

const component::MotorController MotorController(
    component::Motor(Handler, PIN_MOTOR1_FORWARD_BRAKE, PIN_MOTOR1_REVERSE_BRAKE, PIN_MOTOR1_PWM, 37000),
    component::Motor(Handler, PIN_MOTOR2_FORWARD_BRAKE, PIN_MOTOR2_REVERSE_BRAKE, PIN_MOTOR2_PWM, 37000),
    component::Motor(Handler, PIN_MOTOR3_FORWARD_BRAKE, PIN_MOTOR3_REVERSE_BRAKE, PIN_MOTOR3_PWM, 37000),
    component::Motor(Handler, PIN_MOTOR4_FORWARD_BRAKE, PIN_MOTOR4_REVERSE_BRAKE, PIN_MOTOR4_PWM, 37000));

const util::SerialReader Reader;

void setup() {
    Serial.begin(9600);
    Handler.Setup();
}

void loop() {
    float az = Reader.ReadFloat();
    // NEXT: Serial.printlnがないとコンパイルエラーになる。原因を調査する
    Serial.println(az);
    MotorController.Drive(az, 30, 0);
}

#endif
