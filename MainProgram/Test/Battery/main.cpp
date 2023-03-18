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
        SwitchLedR.ternOff();
        SwitchLedG.ternOn();
        delay(500);
        SwitchLedR.ternOn();
        SwitchLedG.ternOff();
        delay(500);
    } else {
        SwitchLedR.ternOff();
        SwitchLedG.ternOff();
        delay(200);
    }
}
