#ifndef COMPONENTS_LED_HPP
#define COMPONENTS_LED_HPP

#ifdef LOCAL_INCLUDE
#include "../../local/arduino_deps.hpp"
#include "../../local/Adafruit_PCF8574.hpp"
#else
#include <Adafruit_PCF8574.h>
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

class LedLightPCF8574
{
  public:
    /**
     * @param pin pin number.
     */
    explicit LedLightPCF8574(SetupHandler& handler, Adafruit_PCF8574& pcf8574, const int pin);

    /**
     * Tern on the LED light.
     */
    void TernOn() const;

    /**
     * Tern off the LED light.
     */
    void TernOff() const;

  private:
    Adafruit_PCF8574& pcf8574_;
    const int PIN_;
};

inline LedLight::LedLight(SetupHandler& handler, const int pin) : PIN_(pin) {
    handler.SetPinMode(PIN_, OUTPUT);
    handler.SetDigitalPinDefault(PIN_, LOW);
}

inline void LedLight::TernOn() const {
    digitalWrite(PIN_, HIGH);
}

inline void LedLight::TernOff() const {
    digitalWrite(PIN_, LOW);
}

inline LedLightPCF8574::LedLightPCF8574(SetupHandler& handler, Adafruit_PCF8574& pcf8574, const int pin)
    : pcf8574_(pcf8574), PIN_(pin) {
    handler.SetCallback([this]() {
        pcf8574_.pinMode(PIN_, OUTPUT);
        pcf8574_.digitalWrite(PIN_, LOW);
    });
}

inline void LedLightPCF8574::TernOn() const {
    pcf8574_.digitalWrite(PIN_, HIGH);
}

inline void LedLightPCF8574::TernOff() const {
    pcf8574_.digitalWrite(PIN_, LOW);
}
}  // namespace component

#endif
