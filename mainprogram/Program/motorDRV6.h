#ifndef MOTOR_DRV6_H
#define MOTOR_DRV6_H

// FourWheel Motor control module
// for Teensy 3.5
// for Faulharber Motor PWM=37KHz
// モーターを動かすサブプログラム

#ifdef LOCAL_INCLUDE
#include "../local/arduino_deps.h"
#else
#include "Servo.h"
#endif

#include "pin.hpp"

constexpr int PIN_DR1_PWM = 6;
constexpr int PIN_DR2_PWM = 6;

void motorCh1(int data) {  // CH1のモーターを動かすプログラム
    int power;
    if (data == 0) {                  // data == 0 なら停止
        analogWrite(PIN_MOTOR1_PWM, 0);       // power = 0
        digitalWrite(PIN_MOTOR1_REVERSE_BRAKE, HIGH);  // Brake
        digitalWrite(PIN_MOTOR1_FORWARD_BRAKE, HIGH);
        return;
    }
    if (data > 0) {                   //  data > 0 なら正転、data < 0 なら逆転
        digitalWrite(PIN_MOTOR1_REVERSE_BRAKE, HIGH);  // Forward
        digitalWrite(PIN_MOTOR1_FORWARD_BRAKE, LOW);
    } else {
        digitalWrite(PIN_MOTOR1_REVERSE_BRAKE, LOW);  // Reverse
        digitalWrite(PIN_MOTOR1_FORWARD_BRAKE, HIGH);
    }
    power = abs(data);  //  -100 ~ +100 のデータを 0～250 に変換する
    if (power > 100) {  //  data > 100 の場合は 100 にする
        power = 100;
    }
    power = (power << 1) + (power >> 1);  // power *= 2.5
    analogWrite(PIN_MOTOR1_PWM, power);           // 0 < power < 250
}

void motorCh2(int data) {  // CH2のモーターを動かすプログラム
    int power;
    if (data == 0) {                  // if Brake
        analogWrite(PIN_MOTOR2_PWM, 0);       // power = 0
        digitalWrite(PIN_MOTOR2_REVERSE_BRAKE, HIGH);  // Brake
        digitalWrite(PIN_MOTOR2_FORWARD_BRAKE, HIGH);
        return;
    }
    if (data > 0) {
        digitalWrite(PIN_MOTOR2_REVERSE_BRAKE, HIGH);  // Forward
        digitalWrite(PIN_MOTOR2_FORWARD_BRAKE, LOW);
    } else {
        digitalWrite(PIN_MOTOR2_REVERSE_BRAKE, LOW);  // Reverse
        digitalWrite(PIN_MOTOR2_FORWARD_BRAKE, HIGH);
    }
    power = abs(data);
    if (power > 100) {
        power = 100;
    }
    power = (power << 1) + (power >> 1);  // power *= 2.5
    analogWrite(PIN_MOTOR2_PWM, power);           // 0 < power < 250
}

void motorCh3(int data) {  // CH3のモーターを動かすプログラム
    int power;
    if (data == 0) {                  // if Brake
        analogWrite(PIN_MOTOR3_PWM, 0);       // power = 0
        digitalWrite(PIN_MOTOR3_REVERSE_BRAKE, HIGH);  // Brake
        digitalWrite(PIN_MOTOR3_FORWARD_BRAKE, HIGH);
        return;
    }
    if (data > 0) {
        digitalWrite(PIN_MOTOR3_REVERSE_BRAKE, HIGH);  // Forward
        digitalWrite(PIN_MOTOR3_FORWARD_BRAKE, LOW);
    } else {
        digitalWrite(PIN_MOTOR3_REVERSE_BRAKE, LOW);  // Reverse
        digitalWrite(PIN_MOTOR3_FORWARD_BRAKE, HIGH);
    }
    power = abs(data);
    if (power > 100) {
        power = 100;
    }
    power = (power << 1) + (power >> 1);  // power *= 2.5
    analogWrite(PIN_MOTOR3_PWM, power);           // 0 < power < 250
}

void motorCh4(int data) {  // CH4のモーターを動かすプログラム
    int power;
    if (data == 0) {                  // if Brake
        analogWrite(PIN_MOTOR4_PWM, 0);       // power = 0
        digitalWrite(PIN_MOTOR4_REVERSE_BRAKE, HIGH);  // Brake
        digitalWrite(PIN_MOTOR4_FORWARD_BRAKE, HIGH);
        return;
    }
    if (data > 0) {
        digitalWrite(PIN_MOTOR4_REVERSE_BRAKE, HIGH);  // Forward
        digitalWrite(PIN_MOTOR4_FORWARD_BRAKE, LOW);
    } else {
        digitalWrite(PIN_MOTOR4_REVERSE_BRAKE, LOW);  // Reverse
        digitalWrite(PIN_MOTOR4_FORWARD_BRAKE, HIGH);
    }
    power = abs(data);
    if (power > 100) {
        power = 100;
    }
    power = (power << 1) + (power >> 1);  // power *= 2.5
    analogWrite(PIN_MOTOR4_PWM, power);           // 0 < power < 250
}

void motorInit() {  // Arduinoのモーター制御を初期化する
    pinMode(PIN_MOTOR1_REVERSE_BRAKE, OUTPUT);
    pinMode(PIN_MOTOR1_FORWARD_BRAKE, OUTPUT);
    pinMode(PIN_MOTOR2_REVERSE_BRAKE, OUTPUT);
    pinMode(PIN_MOTOR2_FORWARD_BRAKE, OUTPUT);
    pinMode(PIN_MOTOR3_REVERSE_BRAKE, OUTPUT);
    pinMode(PIN_MOTOR3_FORWARD_BRAKE, OUTPUT);
    pinMode(PIN_MOTOR4_REVERSE_BRAKE, OUTPUT);
    pinMode(PIN_MOTOR4_FORWARD_BRAKE, OUTPUT);

    pinMode(PIN_MOTOR1_PWM, OUTPUT);
    pinMode(PIN_MOTOR2_PWM, OUTPUT);
    pinMode(PIN_MOTOR3_PWM, OUTPUT);
    pinMode(PIN_MOTOR4_PWM, OUTPUT);

    analogWriteFrequency(PIN_MOTOR1_PWM, 37000);  // set PWM = 37KHz
    analogWriteFrequency(PIN_MOTOR2_PWM, 37000);
    analogWriteFrequency(PIN_MOTOR3_PWM, 37000);
    analogWriteFrequency(PIN_MOTOR4_PWM, 37000);

    motorCh1(0);  // Set All motors STOP
    motorCh2(0);
    motorCh3(0);
    motorCh4(0);
}

void dribInit() {  // ドリブラモーターの PW Mピンを初期化する

    pinMode(PIN_DR1_PWM, OUTPUT);
    pinMode(PIN_DR2_PWM, OUTPUT);
    // analogWriteFrequency(DR1PWM, 37000);
    // analogWriteFrequency(DR2PWM, 37000);
    delay(500);
}

// motor control function
// azimuth(radian)
// power(-100:100)
// rotation(-100:100)
void motorfunction(float azimuth, int power, int rotation) {
    // clamp(power|rotation, -100, 100)
    power = power > 100 ? 100 : power < -100 ? -100 : power;
    rotation = rotation > 100 ? 100 : rotation < -100 ? -100 : rotation;

    float x[4];
    x[0] = -sin(azimuth - (1 * PI / 4));
    x[1] = -sin(azimuth - (3 * PI / 4));
    x[2] = -sin(azimuth - (5 * PI / 4));
    x[3] = -sin(azimuth - (7 * PI / 4));

    float x_max = 0;
    for (int i = 0; i < 4; ++i) {
        if (x_max < abs(x[i])) {
            x_max = abs(x[i]);
        }
    }
    const float w = -(rotation / 100.0);
    for(int i = 0; i < 4; ++i) {
        x[i] = (x[i] / x_max) + w;
    }

    x_max = 0;
    for (int i = 0; i < 4; ++i) {
        if (x_max < abs(x[i])) {
            x_max = abs(x[i]);
        }
    }
    for (int i = 0; i < 4; ++i) {
        x[i] /= x_max;
    }

    motorCh1(x[0] * power);
    motorCh2(x[1] * power);
    motorCh3(x[2] * power);
    motorCh4(x[3] * power);
}

//  時計回りに回転する

void turnCW(int power) {
    motorCh1(-power);
    motorCh2(-power);
    motorCh3(-power);
    motorCh4(-power);
}

//  反時計回りに回転する

void turnCCW(int power) {
    motorCh1(power);
    motorCh2(power);
    motorCh3(power);
    motorCh4(power);
}

//  モーターを止める(Brake)

void motorStop() {
    motorCh1(0);
    motorCh2(0);
    motorCh3(0);
    motorCh4(0);
}

//  モーターを止める(Free)

void motorFree() {
    digitalWrite(PIN_MOTOR1_PWM, HIGH);  // power = 0
    digitalWrite(PIN_MOTOR1_REVERSE_BRAKE, LOW);  // Off
    digitalWrite(PIN_MOTOR1_FORWARD_BRAKE, LOW);

    digitalWrite(PIN_MOTOR2_PWM, HIGH);  // power = 0
    digitalWrite(PIN_MOTOR2_REVERSE_BRAKE, LOW);  // Off
    digitalWrite(PIN_MOTOR2_FORWARD_BRAKE, LOW);

    digitalWrite(PIN_MOTOR3_PWM, HIGH);  // power = 0
    digitalWrite(PIN_MOTOR3_REVERSE_BRAKE, LOW);  // Off
    digitalWrite(PIN_MOTOR3_FORWARD_BRAKE, LOW);

    digitalWrite(PIN_MOTOR4_PWM, HIGH);  // power = 0
    digitalWrite(PIN_MOTOR4_REVERSE_BRAKE, LOW);  // Off
    digitalWrite(PIN_MOTOR4_FORWARD_BRAKE, LOW);
}

void dribbler1(int power) {  // ドリブラ１を回す. 0 < power <100
    if (power < 0 || power > 100) {
        return;
    }
    if (power == 0) {
        analogWrite(PIN_DR1_PWM, 0);
    } else {
        analogWrite(PIN_DR1_PWM, power * 2.5);
    }
    return;
}

void dribbler2(int power) {  // ドリブラ１を回す. 0 < power <100
    if (power < 0 || power > 100) {
        return;
    }
    if (power == 0) {
        analogWrite(PIN_DR2_PWM, 0);
    } else {
        analogWrite(PIN_DR2_PWM, power * 2.5);
    }
    return;
}

/*
  void dribbler1(int power) { // ドリブラ１を回す. 0 < power <100
  if ( power < 0 || power > 100)
    return;
  power = power * 2.5;
  analogWrite(DR2PWM, power);
  return;
  }

  void dribbler2(int power) { // ドリブラ２を回す. 0 < power <100
  if ( power < 0 || power > 100)
    return;
  power = power * 2.5;
  analogWrite(DR2PWM, power);
  return;
  }
*/

void dribbler(int power) {
    dribbler1(power);
    return;
}

#endif
