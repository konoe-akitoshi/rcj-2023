#include "module/gyro.hpp"

void setup() {
    do {
        Serial.begin(9600);
        delay(100);
    } while(!Serial);
    Serial.println("Open Serial(9600)");

    Gyro.setup();
    Serial.println("DONE setup Gyro");
}

void loop() {
    const int rotate = Gyro.getRotation();
    Serial.println(rotate);
}
