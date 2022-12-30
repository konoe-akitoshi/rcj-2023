#ifndef COMPONENT_MOTOR_HPP
#define COMPONENT_MOTOR_HPP

// std::min のために include してるけど、Arduino.h とかで include or define されてるかも
#include <algorithm>

#ifdef LOCAL_INCLUDE
#include "../../local/arduino_deps.h"
#endif

namespace component
{
class Motor
{
  public:
    explicit Motor(const int pin_forward_brake, const int pin_reverse_brake, const int pin_pwm, const int frequency);
    void Start(const int power) const;
    void Stop() const;
    void Free() const;

  private:
    const int PIN_FORWARD_BRAKE_;
    const int PIN_REVERSE_BRAKE_;
    const int PIN_PWM_;
};

class MotorContoroler
{
  public:
    explicit MotorContoroler(const Motor& motor1, const Motor& motor2, const Motor& motor3, const Motor& motor4);

    /**
     * Contorol 4 motors.
     *
     * @param azimuth radian
     * @param rotation [-100, 100]
     */
    void Drive(float azimuth, int rotation) const;
    /**
     * Contorol 4 motors.
     *
     * @param azimuth radian
     * @param power [-100, 100]
     * @param rotation [-100, 100]
     */
    void Drive(float azimuth, int rotation, int power) const;
    void SetPower(const int power);
    void StopAll() const;
    void FreeAll() const;

  private:
    const Motor MOTOR1, MOTOR2, MOTOR3, MOTOR4;
    int power_ = 100;
};

// Impl for Motor

inline Motor::Motor(const int pin_forward_brake, const int pin_reverse_brake, const int pin_pwm, const int frequency)
    : PIN_FORWARD_BRAKE_(pin_forward_brake), PIN_REVERSE_BRAKE_(pin_reverse_brake), PIN_PWM_(pin_pwm) {
    pinMode(PIN_FORWARD_BRAKE_, OUTPUT);
    pinMode(PIN_REVERSE_BRAKE_, OUTPUT);
    pinMode(PIN_PWM_, OUTPUT);
    analogWriteFrequency(PIN_PWM_, frequency);
}

inline void Motor::Start(const int power) const {
    if (power == 0) {  // brake
        Stop();
        return;
    }
    if (power > 0) {  // move forward
        digitalWrite(PIN_FORWARD_BRAKE_, LOW);
        digitalWrite(PIN_REVERSE_BRAKE_, HIGH);
    } else {  // move backward
        digitalWrite(PIN_FORWARD_BRAKE_, HIGH);
        digitalWrite(PIN_REVERSE_BRAKE_, LOW);
    }
    // power -> 0 < p(power) < 250
    int p = std::min(abs(power), 100);
    p = (p << 1) + (p >> 1);  // p *= 2.5;
    analogWrite(PIN_PWM_, p);
}

inline void Motor::Stop() const {
    analogWrite(PIN_PWM_, 0);
    digitalWrite(PIN_FORWARD_BRAKE_, HIGH);
    digitalWrite(PIN_REVERSE_BRAKE_, HIGH);
}

inline void Motor::Free() const {
    analogWrite(PIN_PWM_, HIGH);  // よくわからないけど、HIGH だと power = 0 なんだと。(LOWじゃないんだ...)
    digitalWrite(PIN_FORWARD_BRAKE_, LOW);
    digitalWrite(PIN_REVERSE_BRAKE_, LOW);
}

// Impl for MotorContoroler

inline MotorContoroler::MotorContoroler(const Motor& motor1, const Motor& motor2, const Motor& motor3, const Motor& motor4)
    : MOTOR1(motor1), MOTOR2(motor2), MOTOR3(motor3), MOTOR4(motor4) {
    MOTOR1.Stop();
    MOTOR2.Stop();
    MOTOR3.Stop();
    MOTOR4.Stop();
}

inline void MotorContoroler::Drive(float azimuth, int rotation) const {
    Drive(azimuth, rotation, power_);
}

inline void MotorContoroler::Drive(float azimuth, int rotation, int power) const {
    // clamp(rotation, -100, 100)
    rotation = rotation > 100 ? 100 : rotation < -100 ? -100 : rotation;
    power = power > 100 ? 100 : power < -100 ? -100 : power;

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
    for (int i = 0; i < 4; ++i) {
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

    MOTOR1.Start(x[0] * power);
    MOTOR2.Start(x[1] * power);
    MOTOR3.Start(x[2] * power);
    MOTOR4.Start(x[3] * power);
}

inline void MotorContoroler::SetPower(const int power) {
    power_ = power > 100 ? 100 : power < -100 ? -100 : power;
}

inline void MotorContoroler::StopAll() const {
    MOTOR1.Stop();
    MOTOR2.Stop();
    MOTOR3.Stop();
    MOTOR4.Stop();
}

inline void MotorContoroler::FreeAll() const {
    MOTOR1.Free();
    MOTOR2.Free();
    MOTOR3.Free();
    MOTOR4.Free();
}

}  // namespace component
#endif
