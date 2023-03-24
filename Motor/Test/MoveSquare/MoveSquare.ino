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

void loop() {
    static long count = 0;

    int id = count % 4;
    switch (id) {
        case 0:
            power[0] = fmt(30);
            power[1] = fmt(30);
            power[2] = fmt(-30);
            power[3] = fmt(-30);
            Serial.println("Forward");
            break;
        case 1:
            power[0] = fmt(30);
            power[1] = fmt(-30);
            power[2] = fmt(-30);
            power[3] = fmt(30);
            Serial.println("Left");
            break;
        case 2:
            power[0] = fmt(-30);
            power[1] = fmt(-30);
            power[2] = fmt(30);
            power[3] = fmt(30);
            Serial.println("Back");
            break;
        case 3:
            power[0] = fmt(-30);
            power[1] = fmt(30);
            power[2] = fmt(30);
            power[3] = fmt(-30);
            Serial.println("Right");
            break;
    }
    transmit();

    delay(1000);
    count += 1;
}
