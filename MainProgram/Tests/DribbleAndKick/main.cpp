#include "module/dribbler.hpp"
#include "module/kicker.hpp"

void setup() {
    Serial.begin(9600);
    delay(100);
    Serial.println("Open Serial(9600)");

    Dribbler.setup();
    Serial.println("DONE setup Dribbler");

    Kicker.setup();
    Serial.println("DONE setup Kicker");
}

void loop() {
    Dribbler.start(100);
    delay(3000);

    Dribbler.stop();
    delay(100);
    Kicker.pushFront();
    delay(100);
    Kicker.pullBack();

    delay(5000);
}
