#ifdef PREVENT_SORCING_FROM_ARDUINO
#ifdef LOCAL_INCLUDE
#include "../local/arduino_deps.hpp"
#else
#endif

void setup() {
    while(Serial== false) {
        Serial.begin(9600);
        delay(100);
    }
    Serial.println("setup");
}

void loop() {
    Serial.println("loop");
}

#endif
