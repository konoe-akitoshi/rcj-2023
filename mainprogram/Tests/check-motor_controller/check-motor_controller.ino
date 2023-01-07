#include "components/motor_controller.hpp"
#include "components/setup_handler.hpp"
#include "pin.hpp"
#include "utils/serial_reader.hpp"

component::SetupHandler Handler;
const component::MotorController MotorController(Handler, raspberry_pi_pico::PIN14_GP10, raspberry_pi_pico::PIN15_GP11, 0x0A);
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
