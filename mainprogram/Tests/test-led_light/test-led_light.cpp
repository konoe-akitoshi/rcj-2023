#ifdef PREVENT_SORCING_FROM_ARDUINO
#ifdef LOCAL_INCLUDE
#define LED_BUILTIN 1
#else
#endif

#include "components/led_light.hpp"
#include "components/setup_handler.hpp"

component::SetupHandler Handler;

const component::LedLight Light(Handler, LED_BUILTIN);

void setup() {
    Handler.Setup();
}

void loop() {
    Light.TernOn();
    delay(1000);
    Light.TernOff();
    delay(1000);
}

#endif
