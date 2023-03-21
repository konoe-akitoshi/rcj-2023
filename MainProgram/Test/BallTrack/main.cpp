#include "module/camera.hpp"
#include "module/led_light.hpp"
#include "module/motor_controller.hpp"

void setup() {
    Serial.begin(9600);

    SwitchLedG.setup();
    SwitchLedR.setup();

    MotorController.setup();
    Serial.println("DONE: setup MotorController");

    Camera.setup();
    Serial.println("DONE: setup Camera");
}

void loop() {
    if (!Camera.available()) {
        SwitchLedG.ternOff();
        SwitchLedR.ternOn();
        Serial.println("Camera not available");
        delay(1000);
        return;
    }
    SwitchLedG.ternOn();
    SwitchLedR.ternOff();

    const auto ball = Camera.getBallData();
    if (ball.is_exist) {
        MotorController.drive(Vector2::angle(ball.position), 30, 0);
    } else {
        MotorController.stopAll();
    }

    Serial.println("{");
    Serial.print("  is_exist: ");
    Serial.println(ball.is_exist ? "true" : "false");
    Serial.print("  position: ");
    Serial.print(ball.position.x);
    Serial.print(", ");
    Serial.println(ball.position.y);
    Serial.println("}");

    delay(30);
}
