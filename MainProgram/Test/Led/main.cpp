#include "module/led_light.hpp"

void setup() {
    SwitchLedG.setup();
    SwitchLedR.setup();
    LedR.setup();
    LedY.setup();
    LedG.setup();
    LedB.setup();
    LineSensorLed.setup();
}

void nextSwitchLed() {
    static int flag = 0;
    if (flag == 0) {
        SwitchLedG.ternOn();
        SwitchLedR.ternOff();
    } else if (flag == 1) {
        SwitchLedG.ternOff();
        SwitchLedR.ternOn();
    } else {
        SwitchLedG.ternOff();
        SwitchLedR.ternOff();
    }
    flag = (flag + 1) % 3;
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

void nextLineSensorLed() {
    static int flag = 0;
    if (flag) {
        LineSensorLed.ternOff();
    } else {
        LineSensorLed.ternOn();
    }
    flag = (flag + 1) % 3;
}

void loop() {
    nextSwitchLed();
    nextRYGBLed();
    nextLineSensorLed();
    delay(500);
}
