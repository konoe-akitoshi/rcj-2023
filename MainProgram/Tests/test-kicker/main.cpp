#ifdef PREVENT_SORCING_FROM_ARDUINO

#include "utils/serial_reader.hpp"
#include "components/setup_handler.hpp"
#include "components/kicker.hpp"
#include "pin.hpp"

component::SetupHandler Handler;
const util::SerialReader Reader;
const component::Kicker Kicker(Handler, raspberry_pi_pico::PIN16_GP12);

void setup() {
    Serial.begin(9600);
    Handler.Setup();
}

void loop() {
    Kicker.PushFront();
    delay(200);
    Kicker.PullBack();
    for (int i = 0; i < 10; ++i) {
        Serial.print("Waiting... ");
        Serial.print(10 - i);
        Serial.println("s");
        delay(1000);
    }
}

#endif
