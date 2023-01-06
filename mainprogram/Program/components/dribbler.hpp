#ifndef COMPONENTS_DRIBBLER_HPP
#define COMPONENTS_DRIBBLER_HPP

#ifdef LOCAL_INCLUDE
#include "../../local/arduino_deps.hpp"
#endif

#include "setup_handler.hpp"

namespace component
{
class Dribbler
{
  public:
    /**
     * @param pin_pwm PWM pin number.
     */
    explicit Dribbler(SetupHandler& handler, const int pin_pwm);

    /**
     * Start dribbler with specified power. If power is zero or negative, it meens Stop().
     */
    void Start(const int power) const;

    /**
     * Stop dribbler.
     */
    void Stop() const;

  private:
    const int PIN_;
};

inline Dribbler::Dribbler(SetupHandler& handler, const int pin_pwm) : PIN_(pin_pwm) {
    handler.SetPinMode(PIN_, OUTPUT);
    // ↓なんかコメントアウトされてた、よくわからないのでコメントアウトしておく。
    // analogWriteFrequency(DR1PWM, 37000);
}

inline void Dribbler::Start(const int power) const {
    if (power <= 0) {
        Stop();
        return;
    }
    int p = min(power, 100);
    p = (p << 1) + (p >> 1);  // p *= 2.5
    analogWrite(PIN_, p);
}

inline void Dribbler::Stop() const {
    analogWrite(PIN_, 0);
}
}  // namespace component

#endif
