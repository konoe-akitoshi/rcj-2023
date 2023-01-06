#ifndef COMPONENTS_KICKER_HPP
#define COMPONENTS_KICKER_HPP

#ifdef LOCAL_INCLUDE
#include "../../local/arduino_deps.hpp"
#endif

#include "setup_handler.hpp"

namespace component
{
class Kicker
{
  public:
    /**
     * @param pin pin number.
     */
    explicit Kicker(SetupHandler& handler, const int pin);

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

inline Kicker::Kicker(SetupHandler& handler, const int pin) : PIN_(pin) {
    handler.SetPinMode(PIN_, OUTPUT);
}

inline void Kicker::PushFront() const {
    digitalWrite(PIN_, HIGH);
}
inline void Kicker::PullBack() const {
    digitalWrite(PIN_, LOW);
}
}  // namespace component

#endif
