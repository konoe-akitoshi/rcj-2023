#include "components/setup_handler.hpp"
#include "components/dribbler.hpp"
#include "pin.hpp"

component::SetupHandler Handler;
const component::Dribbler Dribbler(Handler, raspberry_pi_pico::PIN17_GP13);

void setup() {
    Serial.begin(9600);
    Handler.Setup();
}

void loop() {
    Dribbler.Stop();
    delay(1000);
    for (int i = 1; i < 10; ++i) {
        const int power = 10 * i;
        Dribbler.Start(power);
        delay(1000);
    }
    for (int i = 10; i > 0; --i) {
        const int power = 10 * i;
        Dribbler.Start(power);
        delay(1000);
    }
}
