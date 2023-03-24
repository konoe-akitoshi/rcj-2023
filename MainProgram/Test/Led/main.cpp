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
        SwitchLedG.turnOn();
        SwitchLedR.turnOff();
    } else if (flag == 1) {
        SwitchLedG.turnOff();
        SwitchLedR.turnOn();
    } else {
        SwitchLedG.turnOff();
        SwitchLedR.turnOff();
    }
    flag = (flag + 1) % 3;
}

void nextRYGBLed() {
    static int flag = 0;
    switch (flag) {
        case 0:
            LedR.turnOn();
            LedY.turnOff();
            LedG.turnOff();
            LedB.turnOff();
            break;
        case 1:
            LedR.turnOff();
            LedY.turnOn();
            LedG.turnOff();
            LedB.turnOff();
            break;
        case 2:
            LedR.turnOff();
            LedY.turnOff();
            LedG.turnOn();
            LedB.turnOff();
            break;
        case 3:
            LedR.turnOff();
            LedY.turnOff();
            LedG.turnOff();
            LedB.turnOn();
            break;
    }
    flag = (flag + 1) % 4;
}

void nextLineSensorLed() {
    static int flag = 0;
    if (flag) {
        LineSensorLed.turnOff();
    } else {
        LineSensorLed.turnOn();
    }
    flag = (flag + 1) % 3;
}

void loop() {
    nextSwitchLed();
    nextRYGBLed();
    nextLineSensorLed();
    delay(500);
}
