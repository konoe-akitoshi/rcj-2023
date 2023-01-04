#ifndef COMPONENTS_LED_HPP
#define COMPONENTS_LED_HPP

#ifdef LOCAL_INCLUDE
#include "../../local/arduino_deps.h"
#endif

#include "setup_handler.hpp"

namespace component
{

class LedLight
{
  public:
    /**
     * @param pin pin number.
     */
    explicit LedLight(SetupHandler& handler, const int pin);

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

inline LedLight::LedLight(SetupHandler& handler, const int pin) : PIN_(pin) {
    handler.SetPinMode(PIN_, OUTPUT);
}

inline void LedLight::TernOn() const {
    digitalWrite(PIN_, HIGH);
}

inline void LedLight::TernOff() const {
    digitalWrite(PIN_, LOW);
}

}  // namespace component

#endif
