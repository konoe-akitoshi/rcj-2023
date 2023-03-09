#include "module/line_sensors.hpp"
#include "module/led_light.hpp"

void setup() {
    Serial.begin(9600);

    LineSensors.Setup();
    LineSensorLed.Setup();
}

void loop() {
    static long count = 0;
    LineSensorLed.TernOn();
    count += 1;
    const bool state = LineSensors.IsLineDetected();
    // const bool state = LineSensors[0].IsHigh();
    if (count % 200) {
        Serial.print(state ? "#" : ".");
    } else {
        Serial.println(state ? "#" : ".");
    }
}
