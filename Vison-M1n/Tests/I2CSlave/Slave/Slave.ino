#include <Wire.h>

constexpr uint8_t I2C_ADDRESS = 0x10;
constexpr uint8_t I2C_SDA = 6;
constexpr uint8_t I2C_SCL = 7;

volatile uint8_t recieved_id = 0;

/*
|     ID | operation                   |
| -----: | :-------------------------- |
|      1 | Return {1}                  |
|      2 | Return {1, 2}               |
|      3 | Return {1, 2, 3}            |
|      4 | Return {1, 2, 3, 4}         |
|     2n | Return recieved value       |
| 2n + 1 | Return recieved value twice |
*/

void onReceive(int count) {
    if (!Wire1.available()) {
        Serial.println("Wire1 is not available!!");
        Serial.print("  count = ");
        Serial.println(count);
        return;
    }

    for (int i = 0; i < count; i++) {
        recieved_id = Wire1.read();
    }
    Serial.print("onReceive(");
    Serial.print(count);
    Serial.print("): ");
    Serial.println(recieved_id);
}

void onRequest() {
    static constexpr uint8_t ret1[] = {1};
    static constexpr uint8_t ret2[] = {1, 2};
    static constexpr uint8_t ret3[] = {1, 2, 3};
    static constexpr uint8_t ret4[] = {1, 2, 3, 4};
    if (recieved_id == 1) {
        Wire1.write(ret1, 1);
    } else if (recieved_id == 2) {
        Wire1.write(ret2, 2);
    } else if (recieved_id == 3) {
        Wire1.write(ret3, 3);
    } else if (recieved_id == 4) {
        Wire1.write(ret4, 4);
    } else if (recieved_id % 2 == 0) {
        Wire1.write(recieved_id);
    } else { // recieved_id % 2 == 1
        Wire1.write(recieved_id);
        Wire1.write(recieved_id);
    }
}

void setup() {
    do {
        Serial.begin(9600);
        delay(10);
    } while(!Serial);
    Serial.println("Open Serial(9600)");

    Wire1.setSDA(I2C_SDA);
    Serial.println("1");
    Wire1.setSCL(I2C_SCL);
    Serial.println("2");
    Wire1.begin(I2C_ADDRESS);
    Serial.println("3");
    Wire1.onReceive(onReceive);
    Serial.println("4");
    Wire1.onRequest(onRequest);
    Serial.println("Done Wire1 setup");
}

void loop() {}
