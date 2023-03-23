#ifndef MODULE_CORE_TERMINAL_HPP
#define MODULE_CORE_TERMINAL_HPP

#include "camera.hpp"
#include "i2c_manager.hpp"

#ifdef WITHOUT_ARDUINO_ENVIRONMENT
#include "deps/arduino.hpp"
#include "deps/wire.hpp"
#else
#include "Arduino.h"
#include "Wire.h"
#endif

namespace module
{
class Terminal
{
  public:
    explicit constexpr Terminal(const I2CManager& i2c_manager, const uint8_t address) : I2CManager_(i2c_manager), ADDRESS_(address) {
    }

    void setup(void) const {
        I2CManager_.beginWire();
    }

    void sendFieldObjectData(const ObjectData& ball, const ObjectData& yellow_goal, const ObjectData& blue_goal, const int8_t ball_front_dist) const {
        const uint8_t existance = (ball.is_exist ? 0b001 : 0) | (yellow_goal.is_exist ? 0b010 : 0) | (blue_goal.is_exist ? 0b100 : 0);
        const uint8_t send_data[10] = {
            existance, (uint8_t)(int8_t)ball.position.x, (uint8_t)(int8_t)ball.position.y,
            (uint8_t)(int8_t)yellow_goal.position.x, (uint8_t)(int8_t)yellow_goal.position.y, (uint8_t)(yellow_goal.width / 2),
            (uint8_t)(int8_t)blue_goal.position.x, (uint8_t)(int8_t)blue_goal.position.y, (uint8_t)(blue_goal.width / 2),
            (uint8_t)ball_front_dist,
        };
        Wire.beginTransmission(ADDRESS_);
        Wire.write(send_data, 10);
        Wire.endTransmission();
    }

    void sendMachineStatus(const float voltage, const int8_t rotation) const {
        const uint8_t send_data[2] = { (uint8_t)(voltage * 10), (uint8_t(rotation)) };
        Wire.beginTransmission(ADDRESS_);
        Wire.write(send_data, 2);
        Wire.endTransmission();
    }

  private:
    const I2CManager& I2CManager_;
    const uint8_t ADDRESS_;
};
}  // namespace module

#endif
