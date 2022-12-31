#ifndef COMPONENTS_MOTOR_HPP
#define COMPONENTS_MOTOR_HPP

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
    /**
     * @param pin_forward_brake forward-brake pin number.
     * @param pin_reverse_brake reverse-brake pin number.
     * @param pin_pwm PWM pin number.
     * @param frequency PWM frequency.
     */
    explicit Motor(const int pin_forward_brake, const int pin_reverse_brake, const int pin_pwm, const int frequency);

    /**
     * Start motor with specified power. If power is negative, motor move reverse.
     *
     * @param power [-100, 100]
     */
    void Start(const int power) const;

    /**
     * Brake motor. Set PWM to 0.
     */
    void Stop() const;

    /**
     * Free reverse-brake and forward-brake.
     */
    void Free() const;

  private:
    const int PIN_FORWARD_BRAKE_;
    const int PIN_REVERSE_BRAKE_;
    const int PIN_PWM_;
};

class MotorContoroler
{
  public:
    /**
     * @param motor1 right-top motor. forward-direction: ↖︎
     * @param motor2 right-bottom motor. forward-direction: ↗︎
     * @param motor3 left-bottom motor. forward-direction: ↘︎
     * @param motor4 left-top motor. forward-direction: ↙︎
     * @code
     *   ④ ①   ↙︎ ↖︎
     *   ③ ②   ↘︎ ↗︎
     * @endcode
     */
    explicit MotorContoroler(const Motor& motor1, const Motor& motor2, const Motor& motor3, const Motor& motor4);

    /**
     * Contorol 4 motors.
     *
     * Move to the `azimuth` direction and turn to the `rotation` direction with the `power`.
     *
     * @param azimuth radian(clockwise) (forward is 0, right is PI/2, backward is PI).
     * @param power [-100, 100]
     * @param rotation [-100, 100] (positive is right, negative is left)
     */
    void Drive(float azimuth, int power, int rotation) const;

    /**
     * Stop all motors.
     */
    void StopAll() const;

    /**
     * Free all motors.
     */
    void FreeAll() const;

  private:
    const Motor MOTOR1_, MOTOR2_, MOTOR3_, MOTOR4_;
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
    digitalWrite(PIN_PWM_, HIGH);
    digitalWrite(PIN_FORWARD_BRAKE_, LOW);
    digitalWrite(PIN_REVERSE_BRAKE_, LOW);
}

// Impl for MotorContoroler

inline MotorContoroler::MotorContoroler(const Motor& motor1, const Motor& motor2, const Motor& motor3, const Motor& motor4)
    : MOTOR1_(motor1), MOTOR2_(motor2), MOTOR3_(motor3), MOTOR4_(motor4) {
    MOTOR1_.Stop();
    MOTOR2_.Stop();
    MOTOR3_.Stop();
    MOTOR4_.Stop();
}

inline void MotorContoroler::Drive(float azimuth, int power, int rotation) const {
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
    for (int i = 0; i < 4; ++i) {
        x[i] = (x[i] / x_max) - (rotation / 100.0);
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

    MOTOR1_.Start(x[0] * power);
    MOTOR2_.Start(x[1] * power);
    MOTOR3_.Start(x[2] * power);
    MOTOR4_.Start(x[3] * power);
}

inline void MotorContoroler::StopAll() const {
    MOTOR1_.Stop();
    MOTOR2_.Stop();
    MOTOR3_.Stop();
    MOTOR4_.Stop();
}

inline void MotorContoroler::FreeAll() const {
    MOTOR1_.Free();
    MOTOR2_.Free();
    MOTOR3_.Free();
    MOTOR4_.Free();
}

}  // namespace component
#endif
