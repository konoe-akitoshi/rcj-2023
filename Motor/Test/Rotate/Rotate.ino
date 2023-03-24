#include <Wire.h>

#define DATA_LENGTH (4)

constexpr int I2C_ADDRESS = 0x0a;
constexpr int I2C_SDA = 10;
constexpr int I2C_SCL = 11;

byte power[DATA_LENGTH] = {0};

void setup() {
    Wire1.setSDA(I2C_SDA);
    Wire1.setSCL(I2C_SCL);
    Wire1.begin();
    Serial.begin(9600);
}

void transmit() {
    Wire1.beginTransmission(I2C_ADDRESS);
    Wire1.write(power, DATA_LENGTH);
    Wire1.endTransmission();
}

byte fmt(int p) {
    if (p > 0) {
        return p;
    } else {
        return (abs(p) | 0b10000000);
    }
}

void setPower(byte p) {
    for (int i = 0; i < DATA_LENGTH; ++i) {
        power[i] = p;
    }
}

void loop() {
    static long count = 0;

    int id = count % 4;
    switch (id) {
        case 0:
            setPower(0);
            Serial.println("Free");
            break;
        case 1:
            setPower(fmt(30));
            Serial.println("Rotate counter clockwise");
            break;
        case 2:
            setPower(fmt(-30));
            Serial.println("Rotate clockwise");
            break;
        case 3:
            setPower(0b10000000);
            Serial.println("Brake");
            break;
    }
    transmit();

    delay(3000);
    count += 1;
}
