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
    explicit Battery(const int pin, const float limit);
    bool is_emergency() const;
    float voltage() const;
  private:
    const int PIN;
    const float LIMIT;
    static constexpr float K = 0.01811; // 0.01811V/count
};

inline Battery::Battery(const int pin, const float limit): PIN(pin), LIMIT(limit) {
}

inline bool Battery::is_emergency() const {
    int limit = LIMIT / K;
    int voltage = analogRead(PIN);
    return voltage < limit;
}

inline float Battery::voltage() const {
    int read_voltage = analogRead(PIN);
    return read_voltage * K;
}

}  // namespace component

#endif
