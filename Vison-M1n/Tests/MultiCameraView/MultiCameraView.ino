#include <Wire.h>

#define ADDRESS_COUNT (4)
constexpr uint8_t I2C_ADDRESS[ADDRESS_COUNT] = {0x21, 0x22, 0x23, 0x24};

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
    Serial.println();
    bool find_ball = false;

    for (int i = 0; i < ADDRESS_COUNT; ++i) {
        Wire.requestFrom(I2C_ADDRESS[i], 1);
        bool timeout = false;
        for (int time = 0; !Wire.available(); ++time) {
            if (time < 5) {
                delay(1);
            } else {
                timeout = true;
                break;
            }
        }

        Serial.print("camera[");
        Serial.print(i);
        Serial.print("] = ");
        if (timeout) {
            Serial.println("Timeout");
        } else {
            uint8_t find = Wire.read();
            find_ball |= find ? true : false;
            Serial.println(find ? "True" : "False");
        }
    }
    Serial.println(find_ball ? "Exist ball" : "Not found");

    delay(500);
}
