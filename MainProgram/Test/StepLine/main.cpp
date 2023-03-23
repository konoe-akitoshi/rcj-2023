#include "module/line_sensors.hpp"
#include "module/motor_controller.hpp"
#include "module/switch.hpp"
#include "module/led_light.hpp"

volatile float azimuth = 1 * PI / 2;

void interruptHandler() {
    if (StartSwitch.isHigh()) {
        return;
    }
    while (LineSensors.isLineDetected()) {
        const float az = (azimuth + LineSensors.calculateLineAzimut()) / 2;
        MotorController.drive(az + PI, 50, 0);
        delayMicroseconds(500);
    }
    MotorController.freeAll();
}

void setup() {
    Serial.begin(9600);

    StartSwitch.setup();
    LineSensors.setup();
    LineSensorLed.setup();
    MotorController.setup();

    attachInterrupt(LineSensors.getInterruptPin(), interruptHandler, RISING);
}

void loop() {
    LineSensorLed.ternOn();
    if (StartSwitch.isHigh()) {
        MotorController.freeAll();
        return;
    }
    MotorController.drive(azimuth, 70, 0);
}
