#include <Wire.h>

#define DATA_LENGTH (4)

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

uint8_t fmt(int p) {
    if (p > 0) {
        return p;
    } else {
        return (abs(p) | 0b10000000);
    }
}

void setPower(const uint8_t p, const int index) {
    for (int i = 0; i < DATA_LENGTH; ++i) {
        if (i == index) {
            power[i] = p;
        } else {
            power[i] = 0;
        }
    }
}

void loop() {
    static long count = 0;

    int id = count % DATA_LENGTH;
    setPower(fmt(30), id );
    transmit();
    Serial.print("Motor");
    Serial.println(id + 1);

    delay(1000);
    count += 1;
}
