#include "module/camera_controller.hpp"

void setup() {
    do {
        Serial.begin(9600);
        delay(100);
    } while (!Serial);
    Serial.println("Open Serial(9600)");

    CameraController.setup();
    Serial.println("DONE setup CameraController");
}

void loop() {
    Serial.println("===");
    const auto data = CameraController.getFieldData(1);
    CameraFieldData::dumpToSerial(data);
    delay(100);
}
