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
     * @param on  the value(HIGH or LOW) to Write to the specified pin if you want to tern on the LED.
     * @param off  the value(HIGH or LOW) to Write to the specified pin if you want to tern off the LED.
     */
    explicit LedLight(SetupHandler& handler, const int pin, const int on = HIGH, const int off = LOW);

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
    const int MODE_ON_;
    const int MODE_OFF_;
};

class LedLightPCF8574
{
  public:
    /**
     * @param pin pin number.
     * @param on  the value(HIGH or LOW) to Write to the specified pin if you want to tern on the LED.
     * @param off  the value(HIGH or LOW) to Write to the specified pin if you want to tern off the LED.
     */
    explicit LedLightPCF8574(SetupHandler& handler, Adafruit_PCF8574& pcf8574, const int pin, const int on = HIGH, const int off = LOW);

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
    const int MODE_ON_;
    const int MODE_OFF_;
};

inline LedLight::LedLight(SetupHandler& handler, const int pin, const int on, const int off)
    : PIN_(pin), MODE_ON_(on), MODE_OFF_(off) {
    handler.SetPinMode(PIN_, OUTPUT);
    handler.SetDigitalPinDefault(PIN_, MODE_OFF_);
}

inline void LedLight::TernOn() const {
    digitalWrite(PIN_, MODE_ON_);
}

inline void LedLight::TernOff() const {
    digitalWrite(PIN_, MODE_OFF_);
}

inline LedLightPCF8574::LedLightPCF8574(SetupHandler& handler, Adafruit_PCF8574& pcf8574, const int pin, const int on, const int off)
    : pcf8574_(pcf8574), PIN_(pin), MODE_ON_(on), MODE_OFF_(off) {
    handler.SetCallback([this]() {
        pcf8574_.pinMode(PIN_, OUTPUT);
        pcf8574_.digitalWrite(PIN_, MODE_OFF_);
    });
}

inline void LedLightPCF8574::TernOn() const {
    pcf8574_.digitalWrite(PIN_, MODE_ON_);
}

inline void LedLightPCF8574::TernOff() const {
    pcf8574_.digitalWrite(PIN_, MODE_OFF_);
}
}  // namespace component

#endif
