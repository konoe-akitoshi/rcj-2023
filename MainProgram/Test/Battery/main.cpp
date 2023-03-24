#include "module/battery.hpp"
#include "module/led_light.hpp"

void setup() {
    Serial.begin(9600);
    SwitchLedG.setup();
    SwitchLedR.setup();
}

void loop() {
    Serial.println(Battery.voltage());
    if (Battery.isEmergency()) {
        SwitchLedR.turnOff();
        SwitchLedG.turnOn();
        delay(500);
        SwitchLedR.turnOn();
        SwitchLedG.turnOff();
        delay(500);
    } else {
        SwitchLedR.turnOff();
        SwitchLedG.turnOff();
        delay(200);
    }
}
