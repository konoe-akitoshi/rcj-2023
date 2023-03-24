#ifndef MODULE_CORE_DRIBBLER_HPP
#define MODULE_CORE_DRIBBLER_HPP

#ifdef WITHOUT_ARDUINO_ENVIRONMENT
#include "deps/arduino.hpp"
#else
#include <Arduino.h>
#endif

namespace module
{
class Dribbler
{
  public:
    /**
     * @param pin_pwm PWM pin number.
     */
    explicit constexpr Dribbler(const pin_size_t pin_pwm) : PIN_PWM_(pin_pwm) {
    }

    void setup(void) const {
        pinMode(PIN_PWM_, OUTPUT);
        // NOTE: プログラム全体のPWMの周波数を変えるみたい。複数のPWM周波数を使うときはその度に変更すること
        analogWriteFreq(37000);
    }

    /**
     * Start dribbler with specified power. If power is zero, it meens Stop().
     */
    void start(const uint8_t power) const {
        if (power == 0) {
            stop();
            return;
        }
        uint8_t p = min(power, 100);
        p = (p << 1) + (p >> 1);  // p *= 2.5
        analogWrite(PIN_PWM_, p);
    }

    /**
     * Stop dribbler.
     */
    void stop(void) const {
        analogWrite(PIN_PWM_, 0);
    }

  private:
    const pin_size_t PIN_PWM_;
};
}  // namespace module

#endif
