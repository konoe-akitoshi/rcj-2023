#ifndef MODULE_CORE_CAMERA_HPP
#define MODULE_CORE_CAMERA_HPP

#include "../vector2.hpp"
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
struct ObjectData
{
    bool is_exist;
    Vector2 position;
    uint8_t width;

    explicit constexpr ObjectData() : is_exist(false), position(0, 0), width(0) {
    }

    explicit constexpr ObjectData(const bool _is_exist, const Vector2 _position, const uint8_t _width)
        : is_exist(_is_exist), position(_position), width(_width) {
    }
};

class Camera
{
  public:
    /**
     * @param speed transfer speed rate (in bits per second).
     */
    explicit constexpr Camera(const I2CManager& i2c_manager, const uint8_t address)
        : I2CManager_(i2c_manager), ADDRESS_(address) {
    }

    void setup(void) const {
        I2CManager_.beginWire();
    }

    bool available() const {
        Wire.beginTransmission(ADDRESS_);
        const uint8_t status = Wire.endTransmission();
        return status == 0;
    }

    /**
     * Get the ball existance, position.
     *
     * @return the existance and coordinate of the ball. the field of width is always zero.
     */
    ObjectData getBallData(void) const {
        uint8_t data[DATA_LENGTH_] = {0};
        request_(1, data);
        const int x = (int8_t)data[1];
        const int y = (int8_t)data[2];
        return ObjectData(data[0], {x, y}, 0);
    }

    /**
     * Get the yellow goal existance,  position and width.
     *
     * @return the existance, coordinate and width of yellow goal.
     */
    ObjectData getYellowGoalData(void) const {
        uint8_t data[DATA_LENGTH_] = {0};
        request_(2, data);
        const int x = (int8_t)data[1];
        const int y = (int8_t)data[2];
        const int w = (uint8_t)data[3];
        return ObjectData(data[0], {x, y}, w);
    }

    /**
     * Get the yellow goal existance,  position and width.
     *
     * @return the existance, coordinate and width of blue goal.
     */
    ObjectData getBlueGoalData(void) const {
        uint8_t data[DATA_LENGTH_] = {0};
        request_(3, data);
        const int x = (int8_t)data[1];
        const int y = (int8_t)data[2];;
        const int w = (uint8_t)data[3];;
        return ObjectData(data[0], {x, y}, w);
    }

  private:
    const I2CManager& I2CManager_;
    const uint8_t ADDRESS_;

    static constexpr int8_t DATA_LENGTH_ = 4;

    inline void request_(const uint8_t id, uint8_t (&data)[DATA_LENGTH_]) const {
        Wire.beginTransmission(ADDRESS_);
        Wire.write(id);
        Wire.endTransmission();
        while (Wire.available()) {
            // clear buffer
            Wire.read();
        }
        Wire.requestFrom(ADDRESS_, DATA_LENGTH_);
        for (int8_t time = 0; !Wire.available(); ++time) {
            if (time < 5) {
                delay(1);
            }
            return;
        }
        for (int8_t i = 0; i < DATA_LENGTH_; ++i) {
            data[i] = Wire.read();
        }
    }
};
}  // namespace module

#endif
