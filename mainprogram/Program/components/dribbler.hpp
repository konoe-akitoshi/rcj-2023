#ifndef COMPONENT_DRIBBLER_HPP
#define COMPONENT_DRIBBLER_HPP

#ifdef LOCAL_INCLUDE
#include "../../local/arduino_deps.h"
#endif

// std::min のために include してるけど、Arduino.h とかで include or define されてるかも
#include <algorithm>

namespace component
{
class Dribbler
{
  public:
    explicit Dribbler(const int pin_pwm);
    void Start(const int power) const;
    void Stop() const;

  private:
    const int PIN_;
};

inline Dribbler::Dribbler(const int pin_pwm) : PIN_(pin_pwm) {
    pinMode(PIN_, OUTPUT);
    // ↓なんかコメントアウトされてた、よくわからないのでコメントアウトしておく。
    // analogWriteFrequency(DR1PWM, 37000);
}

inline void Dribbler::Start(const int power) const {
    if (power <= 0) {
        Stop();
        return;
    }
    int p = std::min(power, 100);
    p = (p << 1) + (p >> 1);  // p *= 2.5
    analogWrite(PIN_, p);
}

inline void Dribbler::Stop() const {
    analogWrite(PIN_, 0);
}
}  // namespace component

#endif
