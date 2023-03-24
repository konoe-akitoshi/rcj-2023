#include <Wire.h>

constexpr int I2C_ADDRESS = 0x0a;
constexpr int PIN_CH1_INPUT1 = 12;
constexpr int PIN_CH1_INPUT2 = 13;
constexpr int PIN_CH2_INPUT1 = 2;
constexpr int PIN_CH2_INPUT2 = 4;
constexpr int PIN_CH3_INPUT1 = 7;
constexpr int PIN_CH3_INPUT2 = 8;
constexpr int PIN_CH4_INPUT1 = A0;
constexpr int PIN_CH4_INPUT2 = A1;
constexpr int PIN_CH1_ENABLE = 11;
constexpr int PIN_CH2_ENABLE = 3;
constexpr int PIN_CH3_ENABLE = 9;
constexpr int PIN_CH4_ENABLE = 10;

constexpr struct {
    const int INPUT1, INPUT2, ENABLE;
} MOTOR_PIN[4] = {
    {PIN_CH1_INPUT1, PIN_CH1_INPUT2, PIN_CH1_ENABLE},
    {PIN_CH2_INPUT1, PIN_CH2_INPUT2, PIN_CH2_ENABLE},
    {PIN_CH3_INPUT1, PIN_CH3_INPUT2, PIN_CH3_ENABLE},
    {PIN_CH4_INPUT1, PIN_CH4_INPUT2, PIN_CH4_ENABLE},
};

constexpr int STBY = 5;
constexpr int HEARTBEAT = 6;

inline void writeMotorPower(const int index, const byte data) {
    if (data == 0) {
        // Free
        digitalWrite(MOTOR_PIN[index].ENABLE, HIGH);
        digitalWrite(MOTOR_PIN[index].INPUT1, LOW);
        digitalWrite(MOTOR_PIN[index].INPUT2, LOW);
        return;
    }
    if (data == 0b10000000) {
        // Brake
        analogWrite(MOTOR_PIN[index].ENABLE, 0);
        digitalWrite(MOTOR_PIN[index].INPUT1, HIGH);
        digitalWrite(MOTOR_PIN[index].INPUT2, HIGH);
        return;
    }

    // Move
    byte power = data & 0b01111111;
    power = (power << 1) + (power >> 1);  // power *= 2.5 (0 < power < 255)
    if (data & 0b10000000) {
        // Reverse
        digitalWrite(MOTOR_PIN[index].INPUT1, LOW);
        digitalWrite(MOTOR_PIN[index].INPUT2, HIGH);
    } else {
        // Forward
        digitalWrite(MOTOR_PIN[index].INPUT1, HIGH);
        digitalWrite(MOTOR_PIN[index].INPUT2, LOW);
    }
    analogWrite(MOTOR_PIN[index].ENABLE, power);
}

void onReceive(int count) {
    static byte data[4] = {0};

    if (Wire.available()) {
        for (int i = 0; i < count; i++) {
            data[i & 0b11] = Wire.read();  // data[i % 4]
        };
    }

    writeMotorPower(0, data[0]);
    writeMotorPower(1, data[1]);
    writeMotorPower(2, data[2]);
    writeMotorPower(3, data[3]);
}

void setup() {
    for (int i = 0; i < 4; ++i) {
        pinMode(MOTOR_PIN[i].INPUT1, OUTPUT);
        pinMode(MOTOR_PIN[i].INPUT2, OUTPUT);
    }
    pinMode(STBY, OUTPUT);
    pinMode(HEARTBEAT, OUTPUT);

    digitalWrite(STBY, LOW);  // Reset STBY

    TCCR1B &= 0b11111000;
    TCCR1B |= 0b00000001;  // set PWM 15.5kHz
    TCCR2B &= 0b11111000;
    TCCR2B |= 0b00000001;  // set PWM 15.1kHz

    Wire.onReceive(onReceive);
    Wire.begin(I2C_ADDRESS);
}

void loop() {
    digitalWrite(HEARTBEAT, HIGH);
    delay(500);
    digitalWrite(HEARTBEAT, LOW);
    delay(1000);
}
