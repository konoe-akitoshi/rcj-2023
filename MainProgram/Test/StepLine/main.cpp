#include "module/line_sensors.hpp"
#include "module/motor_controller.hpp"
#include "module/switch.hpp"
#include "module/led_light.hpp"

// #include <utility>

volatile bool stepped_line = false;
volatile bool no_interrupt = false;
volatile std::pair<float, module::LineSensorsDirectionIndex> stepped_line_sensor_data = module::NOT_FOUND_LINE_AZIMUT;

void interruptHandler() {
    if (no_interrupt) {
        return;
    }
    stepped_line = true;

    // pair が volatile な = オペレータ用意してないっぽいので
    const auto data = LineSensors.calculateLineAzimut();
    stepped_line_sensor_data.first = data.first;
    stepped_line_sensor_data.second = data.second;
    return;
}

void setup() {
    Serial.begin(9600);

    StartSwitch.setup();
    LineSensors.setup();
    LineSensorLed.setup();
    MotorController.setup();

    attachInterrupt(LineSensors.getInterruptPin(), interruptHandler, RISING);
}

// std::pair<float, module::LineSensorsDirectionIndex> calculateLineAzimut()

void stop() {
    MotorController.stopAll();
    Serial.println("stepped_line = true");
    stepped_line = false;
    delay(2000);
}

void back() {
    while (LineSensors.isLineDetected()) {
        if (stepped_line_sensor_data.first == module::NOT_FOUND_LINE_AZIMUT.first) {
            const auto data = LineSensors.calculateLineAzimut();
            if (data.first == module::NOT_FOUND_LINE_AZIMUT.first) {
                break;
            }
            stepped_line_sensor_data.first = data.first;
            stepped_line_sensor_data.second = data.second;
        }
        float az = stepped_line_sensor_data.first;
        auto index = stepped_line_sensor_data.second;

        Serial.println("---");
        Serial.println(index.front);
        Serial.println(index.right);
        Serial.println(index.back);
        Serial.println(index.left);
        Serial.println(index.center);
        Serial.println("---");

        // ラインを大きく超えているので一旦下がる
        while (LineSensors[index.front].isLow() || LineSensors[index.center].isHigh()) {
            MotorController.drive(az + PI, 30, 0);
            Serial.println("back");
            delay(70);
        }

        // ラインと反対方向へ移動する
        while (LineSensors[index.front].isHigh() || LineSensors[index.center].isHigh() || LineSensors[index.back].isHigh()) {
            if (LineSensors[index.left].isHigh()) {
                az += -3 * PI / 4;
            } else if (LineSensors[index.right].isHigh()) {
                az += 3 * PI / 4;
            } else {
                az += PI;
            }
            MotorController.drive(az + PI, 30, 0);
            delay(70);
        }

        // reset
        stepped_line_sensor_data.first = module::NOT_FOUND_LINE_AZIMUT.first;
        stepped_line_sensor_data.second = module::NOT_FOUND_LINE_AZIMUT.second;
    }
    stepped_line = false;
}

void loop() {
    LineSensorLed.ternOn();
    if (StartSwitch.isHigh()) {
        MotorController.freeAll();
        if (stepped_line) {
            stepped_line = false;
        }
        return;
    }

    if (stepped_line) {
        Serial.println("noInterrupts");
        no_interrupt = true;
        back();
        no_interrupt = false;
        Serial.println("interrupts");
    } else {
        MotorController.drive(1 * PI / 2, 50, 0);
    }
}
