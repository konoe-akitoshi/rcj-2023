#ifndef MODULE_CORE_BATTERY_HPP
#define MODULE_CORE_BATTERY_HPP

#ifdef WITHOUT_ARDUINO_ENVIRONMENT
#include "deps/arduino.hpp"
#else
#include <Arduino.h>
#endif

namespace module
{
class Battery
{
  public:
    /**
     * @param pin pin number.
     * @param limit the limit of voltage (unit is V)
     */
    explicit constexpr Battery(const pin_size_t pin, const float limit) : PIN_(pin), LIMIT_(limit) {
    }

    void setup(void) const {
        // empty
    }

    /**
     * Check whether the battery voltage is low (under the limit).
     *
     * @return true: the battery voltage is low.
     */
    bool isEmergency() const {
        bool emergency = true;
        for (int i = 0; i < 5; ++i) {
            const int limit = LIMIT_ / kCOUNT_;
            const int voltage = analogRead(PIN_);
            emergency = (voltage < limit);
            if (!emergency) {
                break;
            }
            delay(10);
        }
        return emergency;
    }

    /**
     * Get the current battery voltage.
     *
     * @return the voltage of the battery (unit: V).
     */
    float voltage() const {
        int read_voltage = analogRead(PIN_);
        return read_voltage * kCOUNT_;
    }

  private:
    const pin_size_t PIN_;
    const float LIMIT_;
    static constexpr float kCOUNT_ = 0.01811;  // 0.01811V/count
};
}  // namespace module

#endif
