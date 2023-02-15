#ifdef PREVENT_SORCING_FROM_ARDUINO

#include <array>
#include "components/setup_handler.hpp"
#include "components/digital_reader.hpp"
#include "components/motor_controller.hpp"
#include "components/led_light.hpp"
#include "pin.hpp"

constexpr int INTERRUPT_PIN = raspberry_pi_pico::PIN34_GP28;
component::SetupHandler SetupHandler;
const component::LedLight LineSensorLed(SetupHandler, raspberry_pi_pico::PIN27_GP21);
std::array<component::DigitalReader, 5> LineSensors = {
    component::DigitalReader(SetupHandler, raspberry_pi_pico::PIN21_GP16, INPUT_PULLUP),
    component::DigitalReader(SetupHandler, raspberry_pi_pico::PIN22_GP17, INPUT_PULLUP),
    component::DigitalReader(SetupHandler, raspberry_pi_pico::PIN24_GP18, INPUT_PULLUP),
    component::DigitalReader(SetupHandler, raspberry_pi_pico::PIN25_GP19, INPUT_PULLUP),
    component::DigitalReader(SetupHandler, raspberry_pi_pico::PIN26_GP20, INPUT_PULLUP)
};
 component::MotorController MotorController(SetupHandler, raspberry_pi_pico::PIN14_GP10, raspberry_pi_pico::PIN15_GP11, 0x0A);

void interruptHandler() {
    Serial.println("!!!");

    // Lineセンサが反応している間は繰り返す
    while (digitalRead(INTERRUPT_PIN) == HIGH) {
        // lineを踏んだセンサーを調べ、Lineセンサと反対方向へ移動する
        float az = 0;
        int front = 0;
        if (LineSensors[0].IsHigh()) {
            az = PI / 2;
            front = 0;
        } else if (LineSensors[1].IsHigh()) {
            az = 0;
            front = 1;
        } else if (LineSensors[2].IsHigh()) {
            az = -1 * PI / 2;
            front = 2;
        } else if (LineSensors[3].IsHigh()) {
            az = PI;
            front = 3;
        } else {
            // LedR.TernOn();
            continue;
        }
        const int back = (front + 2) % 4;
        const int left = (front + 3) % 4;
        const int right = (front + 1) % 4;
        while(LineSensors[front].IsHigh() || LineSensors[4].IsHigh() || LineSensors[back].IsHigh()) {
            if (LineSensors[left].IsHigh()) {
                az += -3 * PI / 4;
            } else if (LineSensors[right].IsHigh()) {
                az += 3 * PI / 4;
            } else {
                az += PI;
            }
            MotorController.Drive(az, 30, 0);
            delay(20);
        }
    }

    // MotorController.StopAll();
    Serial.println("end");
    return;
}

void main_setup() {
}

#include "utils/serial_reader.hpp"
const util::SerialReader Reader;

void setup() {
    while (Serial == false) {
        Serial.begin(9600);
        delay(100);
    }

    Serial.println("DONE open Serial(9600)");
    SetupHandler.Setup();
    Serial.println("DONE setup components");

    pinMode(INTERRUPT_PIN, INPUT_PULLUP);
    attachInterrupt(INTERRUPT_PIN, interruptHandler, RISING);
    Serial.print("DONE attach interrupt to pin(RISING): ");
    Serial.println(INTERRUPT_PIN);

    LineSensorLed.TernOn();
}

void loop() {
    LineSensorLed.TernOn();
    float az = Reader.ReadFloat();
    Serial.println(az);
    MotorController.Drive(az, 30, 0);
}

#endif
