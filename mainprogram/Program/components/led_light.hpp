#ifndef COMPONENT_LED_HPP
#define COMPONENT_LED_HPP

#ifdef LOCAL_INCLUDE
#include "../../local/arduino_deps.h"
#endif

namespace component
{

class LedLight
{
  public:
    explicit LedLight(const int pin);
    void TernOn() const;
    void TernOff() const;

  private:
    const int PIN;
};

inline LedLight::LedLight(const int pin) : PIN(pin) {
    pinMode(PIN, OUTPUT);
    digitalWrite(PIN, LOW);
}

inline void LedLight::TernOn() const {
    digitalWrite(PIN, HIGH);
}

inline void LedLight::TernOff() const {
    digitalWrite(PIN, LOW);
}

}  // namespace component

#endif
