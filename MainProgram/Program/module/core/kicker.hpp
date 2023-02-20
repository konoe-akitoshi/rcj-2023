#ifndef MODULE_CORE_KICKER_HPP
#define MODULE_CORE_KICKER_HPP

#ifdef WITHOUT_ARDUINO_ENVIRONMENT
#include "deps/arduino.hpp"
#else
#include <Arduino.h>
#endif

namespace module
{
class Kicker
{
  public:
    /**
     * @param pin pin number.
     */
    explicit constexpr Kicker(const int pin) : PIN_(pin) {
    }

    void Setup(void) const {
        pinMode(PIN_, OUTPUT);
        digitalWrite(PIN_, LOW);
    }

    /**
     * Push kicker to front.
     */
    void PushFront() const {
        digitalWrite(PIN_, HIGH);
    }

    /**
     * Pull kicker to the first place.
     */
    void PullBack() const {
        digitalWrite(PIN_, LOW);
    }

  private:
    const int PIN_;
};
}  // namespace module

#endif
