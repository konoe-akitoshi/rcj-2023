#ifdef PREVENT_SORCING_FROM_ARDUINO

#include "components/setup_handler.hpp"
#include "components/gyro.hpp"

component::SetupHandler Handler;
const component::Gyro Gyro(Handler, 115200);

void setup() {
    Serial.begin(9600);
    Handler.Setup();
}

void loop() {
    const auto rot = Gyro.GetRotation();
    Serial.println(rot);
    delay(100);
}

#endif
