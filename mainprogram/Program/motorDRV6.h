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

constexpr int PIN_DR1_PWM = 6;
constexpr int PIN_DR2_PWM = 6;

void dribInit() {  // ドリブラモーターの PW Mピンを初期化する

    pinMode(PIN_DR1_PWM, OUTPUT);
    pinMode(PIN_DR2_PWM, OUTPUT);
    // analogWriteFrequency(DR1PWM, 37000);
    // analogWriteFrequency(DR2PWM, 37000);
    delay(500);
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
