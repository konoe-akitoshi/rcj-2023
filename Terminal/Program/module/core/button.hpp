#ifndef MODULE_CORE_BUTTON_HPP
#define MODULE_CORE_BUTTON_HPP

#ifdef WITHOUT_ARDUINO_ENVIRONMENT
#include "deps/arduino.hpp"
#else
#include "Arduino.h"
#endif

namespace module
{
class Button
{
  public:
    /**
     * @param pin pin number.
     * @param mode pin mode. e.g. INPUT
     */
    explicit constexpr Button(const uint8_t pin) : PIN_(pin) {
    }

    /**
     * Set up the digital-reader.
     */
    void setup(void) const {
        pinMode(PIN_, INPUT_PULLUP);
    }

    bool isPressed(void) const {
        return digitalRead(PIN_) == LOW;
    }

  private:
    const uint8_t PIN_;
};
}  // namespace module

#endif
