#include <Wire.h>
#include <VL6180X.h>

VL6180X ToFSensor;

void setup() {
    do {
        Serial.begin(9600);
    } while (!Serial);
    Serial.println("Open Serial(9600)");

    Wire.setSDA(4);
    Wire.setSCL(5);
    Wire.begin();

    ToFSensor.init();
    ToFSensor.configureDefault();
    ToFSensor.setScaling(1);
    ToFSensor.setAddress(0x29);
    ToFSensor.setTimeout(100);
    Serial.println("DONE setup ToFSensor");
}

void loop() {
    uint16_t data = ToFSensor.readRangeSingleMillimeters();
    Serial.println(data);
}
