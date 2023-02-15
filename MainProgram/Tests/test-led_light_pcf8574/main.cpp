#ifdef PREVENT_SORCING_FROM_ARDUINO
#ifdef LOCAL_INCLUDE
#include "../local/Adafruit_PCF8574.hpp"
#else
#include <Adafruit_PCF8574.h>
#endif

#include "components/led_light.hpp"
#include "components/setup_handler.hpp"
#include "pin.hpp"

constexpr int TargetLedPin1 = pcf8574::EX02;
constexpr int TargetLedPin2 = pcf8574::EX03;
Adafruit_PCF8574 PCF8574;
component::SetupHandler Handler;

const component::LedLightPCF8574 Light1(Handler, PCF8574, TargetLedPin1);
const component::LedLightPCF8574 Light2(Handler, PCF8574, TargetLedPin2);

void setup() {
    Serial.begin(9600);
    const bool pcf8574_status = PCF8574.begin(0x27, &Wire);
    if (pcf8574_status == false) {
        Serial.println("ERROR not find PCF8574");
        while (true);
    }
    Handler.Setup();
}

void loop() {
    Light1.TernOn();
    Light2.TernOff();
    delay(1000);
    Light1.TernOff();
    Light2.TernOn();
    delay(1000);
}

#endif
