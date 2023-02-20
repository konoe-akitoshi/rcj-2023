#ifndef COMPONENTS_SETUP_HANDLER_HPP
#define COMPONENTS_SETUP_HANDLER_HPP

#ifdef LOCAL_INCLUDE
#include "../../local/arduino_deps.hpp"
#endif

#include <functional>
#include <utility>
#include <vector>

namespace component
{
class SetupHandler
{
  public:
    explicit SetupHandler();

    void SetPinMode(const int pin, const int mode);
    void SetCallback(const std::function<void(void)> callback);
    void SetDigitalPinDefault(const int pin, const int value);
    void Setup() const;

  private:
    // first: pin, second: mode
    std::vector<std::pair<int, int>> pinmodes_;
    // first: pin, second: value
    std::vector<std::pair<int, int>> init_d_pins_;
    std::vector<std::function<void(void)>> callbacks_;
};

inline SetupHandler::SetupHandler() {
    pinmodes_.resize(0);
    callbacks_.resize(0);
}

inline void SetupHandler::SetPinMode(const int pin, const int mode) {
    pinmodes_.emplace_back(pin, mode);
}

inline void SetupHandler::SetCallback(const std::function<void(void)> callback) {
    callbacks_.emplace_back(callback);
}

inline void SetupHandler::SetDigitalPinDefault(const int pin, const int value) {
    init_d_pins_.emplace_back(pin, value);
}

inline void SetupHandler::Setup() const {
    for (const auto& pm : pinmodes_) {
        pinMode(pm.first, pm.second);
    }
    for (const auto& dpv : init_d_pins_) {
        digitalWrite(dpv.first, dpv.second);
    }
    for (const auto& callback : callbacks_) {
        callback();
    }
}
}  // namespace component

#endif
