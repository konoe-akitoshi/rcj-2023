#ifndef COMPONENT_BATTERY_HPP
#define COMPONENT_BATTERY_HPP

#ifdef LOCAL_INCLUDE
#include "../../local/arduino_deps.h"
#endif

namespace component
{
class Battery
{
  public:
    /**
     * @param pin pin number.
     * @param limit the limit of voltage (unit is V)
     */
    explicit Battery(const int pin, const float limit);

    /**
     * Check whether the battery voltage is low (under the limit).
     *
     * @return true: the battery voltage is low.
     */
    bool IsEmergency() const;

    /**
     * Get the current battery voltage.
     *
     * @return the voltage of the battery (unit: V).
     */
    float Voltage() const;

  private:
    const int PIN_;
    const float LIMIT_;
    static constexpr float kCOUNT_ = 0.01811;  // 0.01811V/count
};

inline Battery::Battery(const int pin, const float limit) : PIN_(pin), LIMIT_(limit) {
}

inline bool Battery::IsEmergency() const {
    int limit = LIMIT_ / kCOUNT_;
    int voltage = analogRead(PIN_);
    return voltage < limit;
}

inline float Battery::Voltage() const {
    int read_voltage = analogRead(PIN_);
    return read_voltage * kCOUNT_;
}
}  // namespace component

#endif
