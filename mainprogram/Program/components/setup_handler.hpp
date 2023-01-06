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
    void Setup() const;

  private:
    // first: pin, second: mode
    std::vector<std::pair<int, int>> pinmodes_;
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

inline void SetupHandler::Setup() const {
    for (const auto& pm : pinmodes_) {
        pinMode(pm.first, pm.second);
    }
    for (const auto& callback : callbacks_) {
        callback();
    }
}
}  // namespace component

#endif
