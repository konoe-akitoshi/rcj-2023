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
    /**
     * @param pin pin number.
     */
    explicit LedLight(const int pin);

    /**
     * Tern on the LED light.
     */
    void TernOn() const;

    /**
     * Tern off the LED light.
     */
    void TernOff() const;

  private:
    const int PIN_;
};

inline LedLight::LedLight(const int pin) : PIN_(pin) {
    pinMode(PIN_, OUTPUT);
    digitalWrite(PIN_, LOW);
}

inline void LedLight::TernOn() const {
    digitalWrite(PIN_, HIGH);
}

inline void LedLight::TernOff() const {
    digitalWrite(PIN_, LOW);
}

}  // namespace component

#endif
