#ifndef COMPONENTS_KICKER_HPP
#define COMPONENTS_KICKER_HPP

#ifdef LOCAL_INCLUDE
#include "../../local/arduino_deps.h"
#endif

namespace component
{
class Kicker
{
  public:
    /**
     * @param pin pin number.
     */
    explicit Kicker(const int pin);

    /**
     * Push kicker to front.
     */
    void PushFront() const;

    /**
     * Pull kicker to the first place.
     */
    void PullBack() const;

  private:
    const int PIN_;
};

inline Kicker::Kicker(const int pin) : PIN_(pin) {
    pinMode(PIN_, OUTPUT);
    digitalWrite(PIN_, LOW);
}

inline void Kicker::PushFront() const {
    digitalWrite(PIN_, HIGH);
}
inline void Kicker::PullBack() const {
    digitalWrite(PIN_, LOW);
}
}  // namespace component

#endif
