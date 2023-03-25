#include "module/switch.hpp"

// #define USE_LED_LIGHT

#ifdef USE_LED_LIGHT
#include "module/led_light.hpp"
#endif

void setup() {
#ifndef USE_LED_LIGHT
    do {
#endif
        Serial.begin(9600);
#ifndef USE_LED_LIGHT
    } while(!Serial);
#endif
    Serial.println("Open Serial(9600)");

    StartSwitch.setup();
    GoalSwitch.setup();
    AttackerSwitch.setup();
    KeeperSwitch.setup();

#ifdef USE_LED_LIGHT
    LedB.setup();
    LedG.setup();
    LedR.setup();
    LedY.setup();
#endif
}

void loop() {
    Serial.print("StartSwitch: ");
    Serial.println(StartSwitch.isHigh() ? "HIGH" : "LOW");

    Serial.print("GoalSwitch: ");
    Serial.println(GoalSwitch.isHigh() ? "HIGH" : "LOW");

    Serial.print("AttackerSwitch: ");
    Serial.println(AttackerSwitch.isHigh() ? "HIGH" : "LOW");

    Serial.print("KeeperSwitch: ");
    Serial.println(KeeperSwitch.isHigh() ? "HIGH" : "LOW");

#ifdef USE_LED_LIGHT
    if (StartSwitch.isHigh()) {
        LedB.turnOn();
    } else {
        LedB.turnOff();
    }
    if (GoalSwitch.isHigh()) {
        LedG.turnOn();
    } else {
        LedG.turnOff();
    }
    if (AttackerSwitch.isHigh()) {
        LedR.turnOn();
    } else {
        LedR.turnOff();
    }
    if (KeeperSwitch.isHigh()) {
        LedY.turnOn();
    } else {
        LedY.turnOff();
    }
#endif

    Serial.println("=====");
    delay(500);
}
