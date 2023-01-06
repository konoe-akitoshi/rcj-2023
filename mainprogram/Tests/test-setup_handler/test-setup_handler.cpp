#ifdef PREVENT_SORCING_FROM_ARDUINO
#ifdef LOCAL_INCLUDE
#include "../local/arduino_deps.hpp"
#define LED_BUILTIN 1
#else
#endif

#include "components/setup_handler.hpp"

component::SetupHandler handler;

const int speed = 9600;

void setup() {
    handler.SetPinMode(LED_BUILTIN, OUTPUT);
    handler.SetCallback([]() {
        Serial.begin(speed);
    });
    handler.Setup();
}

void loop() {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);
    delay(1000);
    Serial.println(1);
}

#endif
