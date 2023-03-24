#include <Wire.h>

#define ADDRESS_COUNT (2)
constexpr uint8_t I2C_ADDRESS[ADDRESS_COUNT] = {0x24, 0x25};

void setup() {
    do {
        Serial.begin(9600);
        delay(100);
    } while (!Serial);
    Serial.println("Open Serial(9600)");

    Wire.setSDA(8);
    Wire.setSCL(9);
    Wire.setClock(400000);
    Wire.begin();
    Serial.println("Wire bigin");
}

void loop() {
    Serial.println("[loop]");
    for (int i = 0; i < ADDRESS_COUNT; ++i) {
        uint8_t rnd = random(256);
        Wire.beginTransmission(I2C_ADDRESS[i]);
        Wire.write(rnd);
        Wire.endTransmission();

        Wire.requestFrom(I2C_ADDRESS[i], 1);
        bool timeout = false;
        for (int time = 0; !Wire.available(); ++time) {
            if (time < 10) {
                delay(1);
            } else {
                timeout = true;
                break;
            }
        }

        Serial.print("data[");
        Serial.print(i);
        Serial.print("] = ");
        if (timeout) {
            Serial.println("timeout");
            continue;
        }
        uint8_t data = Wire.read();
        if (data == rnd) {
            Serial.print(data);
            Serial.println();
        } else {
            Serial.println("error");
        }
    }
    delay(500);
}
