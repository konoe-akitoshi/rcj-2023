#ifndef MODULE_CORE_CAMERA_HPP
#define MODULE_CORE_CAMERA_HPP

#include "../vector2.hpp"

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
    explicit constexpr Camera(const pin_size_t sda, const pin_size_t scl, const uint8_t address, const uint32_t clock)
        : SDA_(sda), SCL_(scl), ADDRESS_(address), CLOCK_(clock) {
    }

    void setup(void) const {
        Wire.setSDA(SDA_);
        Wire.setSCL(SCL_);
        Wire.setClock(CLOCK_);
        Wire.begin();
    }

    /**
     * Get the ball existance, position.
     *
     * @return the existance and coordinate of the ball. the field of width is always zero.
     */
    ObjectData getBallData(void) const {
        uint8_t data[DATA_LENGTH_] = {0};
        request_(1, data);
        const int x = ((int)data[1] << 8) | data[2];
        const int y = ((int)data[3] << 8) | data[4];
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
        const int x = ((int)data[1] << 8) | data[2];
        const int y = ((int)data[3] << 8) | data[4];
        const int w = ((int)data[5] << 8) | data[6];
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
        const int x = ((int)data[1] << 8) | data[2];
        const int y = ((int)data[3] << 8) | data[4];
        const int w = ((int)data[5] << 8) | data[6];
        return ObjectData(data[0], {x, y}, w);
    }

  private:
    const pin_size_t SDA_;
    const pin_size_t SCL_;
    const uint8_t ADDRESS_;
    const uint32_t CLOCK_;

    static constexpr int8_t DATA_LENGTH_ = 7;

    inline void request_(const uint8_t id, uint8_t (&data)[DATA_LENGTH_]) const {
        while (Wire.available()) {
            // clear buffer
            Wire.read();
        }

        Wire.beginTransmission(ADDRESS_);
        Wire.write(id);
        Wire.endTransmission();

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
