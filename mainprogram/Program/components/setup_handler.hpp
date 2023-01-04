#ifndef COMPONENTS_SETUP_HANDLER_HPP
#define COMPONENTS_SETUP_HANDLER_HPP

#ifdef LOCAL_INCLUDE
#include "../../local/arduino_deps.h"
#endif

#include <vector>
#include <utility>
#include <functional>

namespace component
{
class SetupHandler
{
  public:
    explicit SetupHandler();

    void SetPinMode(const int pin, const int mode);
    void SetCallback(const std::function<void(void)> callback);
    void SetSerialSpeed(const int number, const int speed);

    void Setup() const;

  private:
    // first: pin, second: mode
    std::vector<std::pair<int, int>> pinmodes_;
    std::vector<std::function<void(void)>> callbacks_;
    static constexpr int serial_count_ = 3;
    int serials_speed_[serial_count_ + 1];
};

inline SetupHandler::SetupHandler() {}

inline void SetupHandler::SetPinMode(const int pin, const int mode) {
    pinmodes_.emplace_back(pin, mode);
}

inline void SetupHandler::SetCallback(const std::function<void(void)> callback) {
    callbacks_.emplace_back(callback);
}

inline void SetupHandler::SetSerialSpeed(const int number, const int speed) {
    if (number < 0 || serial_count_ < number) {
        // TODO: コンパイル時にエラー
        return;
    }
    serials_speed_[number] = speed;
}

inline void SetupHandler::Setup() const {
    for (const auto& pm : pinmodes_) {
        pinMode(pm.first, pm.second);
    }
    for (const auto& callback : callbacks_) {
        callback();
    }
    for (int i = 0; i <= serial_count_; ++i) {
        const int speed = serials_speed_[i];
        if (speed == 0) {
            return;
        }
        switch (i) {
            case 0:
                Serial.begin(speed);
                break;
            case 1:
                Serial1.begin(speed);
                break;
            case 2:
                Serial2.begin(speed);
                break;
            case 3:
                Serial3.begin(speed);
                break;
        }
    }
}

}  // namespace component

#endif
