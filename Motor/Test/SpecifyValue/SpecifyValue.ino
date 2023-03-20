#include <Wire.h>

#define DATA_LENGTH (6)

constexpr int I2C_ADDRESS = 0x0a;
constexpr int I2C_SDA = 10;
constexpr int I2C_SCL = 11;

uint8_t power[DATA_LENGTH] = {0};

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

void loop() {
    power[0] = 128;
    power[1] = 128;
    power[2] = 80;
    power[3] = -30;
    transmit();
    delay(1000);
}
