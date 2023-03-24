#include "module/button.hpp"

void setup() {
    do {
        Serial.begin(9600);
        delay(100);
    } while (!Serial);
    Serial.println("Open Serial(9600)");

    ButtonLeft.setup();
    ButtonCenter.setup();
    ButtonRight.setup();
    StickUp.setup();
    StickDown.setup();
    StickLeft.setup();
    StickRight.setup();
    StickPress.setup();
    Serial.println("DONE: setup buttons");
}

void loop() {
    if (ButtonLeft.isPressed()) {
        Serial.println("Button Left");
    }
    if (ButtonCenter.isPressed()) {
        Serial.println("Button Center");
    }
    if (ButtonRight.isPressed()) {
        Serial.println("Button Right");
    }
    if (StickUp.isPressed()) {
        Serial.println("Stick Up");
    }
    if (StickDown.isPressed()) {
        Serial.println("Stick Down");
    }
    if (StickLeft.isPressed()) {
        Serial.println("Stick Left");
    }
    if (StickRight.isPressed()) {
        Serial.println("Stick Right");
    }
    if (StickPress.isPressed()) {
        Serial.println("Stick Press");
    }
}
