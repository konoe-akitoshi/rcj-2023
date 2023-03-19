#include "module/led_light.hpp"

void setup() {
    SwitchLedG.setup();
    SwitchLedR.setup();
    LedR.setup();
    LedY.setup();
    LedG.setup();
    LedB.setup();
}

void nextSwitchLed() {
    static bool flag = false;
    if (flag) {
        SwitchLedG.ternOn();
        SwitchLedR.ternOff();
    } else {
        SwitchLedG.ternOff();
        SwitchLedR.ternOn();
    }
    flag ^= true;
}

void nextRYGBLed() {
    static int flag = 0;
    switch (flag) {
        case 0:
            LedR.ternOn();
            LedY.ternOff();
            LedG.ternOff();
            LedB.ternOff();
            break;
        case 1:
            LedR.ternOff();
            LedY.ternOn();
            LedG.ternOff();
            LedB.ternOff();
            break;
        case 2:
            LedR.ternOff();
            LedY.ternOff();
            LedG.ternOn();
            LedB.ternOff();
            break;
        case 3:
            LedR.ternOff();
            LedY.ternOff();
            LedG.ternOff();
            LedB.ternOn();
            break;
    }

    flag = (flag + 1) % 4;
}

void loop() {
    nextSwitchLed();
    delay(500);
}
