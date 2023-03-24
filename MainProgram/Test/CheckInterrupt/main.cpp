#include "module/line_sensors.hpp"
#include "module/led_light.hpp"

void setup() {
    Serial.begin(9600);

    LineSensors.setup();
    LineSensorLed.setup();
}

void loop() {
    static long count = 0;
    LineSensorLed.turnOn();
    count += 1;
    const bool state = LineSensors.isLineDetected();
    // const bool state = LineSensors[0].IsHigh();
    if (count % 200) {
        Serial.print(state ? "#" : ".");
    } else {
        Serial.println(state ? "#" : ".");
    }
}
