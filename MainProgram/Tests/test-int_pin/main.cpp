#ifdef PREVENT_SORCING_FROM_ARDUINO

#include "pin.hpp"
#include "components/setup_handler.hpp"
#include "components/led_light.hpp"

void interruptHandler();

constexpr int INTERRUPT_PIN = raspberry_pi_pico::PIN34_GP28;
component::SetupHandler Handler;
const component::LedLight LineSensorLed(Handler, raspberry_pi_pico::PIN27_GP21);

void setup() {
    Serial.begin(9600);
    Handler.Setup();
    pinMode(INTERRUPT_PIN, INPUT_PULLUP);
    attachInterrupt(INTERRUPT_PIN, interruptHandler, RISING);
    LineSensorLed.TernOn();
}

void loop() {
}


void interruptHandler() {
    while (digitalRead(INTERRUPT_PIN) == HIGH) {
        Serial.println("interrupt");
    }
}

#endif
