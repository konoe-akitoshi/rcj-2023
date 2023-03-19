#ifndef MODULE_CORE_CAMERA_CONTROLLER_HPP
#define MODULE_CORE_CAMERA_CONTROLLER_HPP

#ifdef WITHOUT_ARDUINO_ENVIRONMENT
#include "deps/arduino.hpp"
#include "deps/wire.hpp"
#else
#include "Arduino.h"
#include "Wire.h"
#endif

#include "../types.hpp"

namespace module
{
struct Image
{
    static constexpr uint16_t width = 320;
    static constexpr uint16_t height = 240;
    uint16_t pixel[height][width] = {{0}};
    explicit Image() {}

    void set(const uint16_t x, const uint16_t y, const uint16_t rgb565) {
        pixel[y][x] = rgb565;
    }

    uint16_t get(const uint16_t x, const uint16_t y) const {
        return pixel[y][x];
    }
};

class CameraController
{
  public:
    explicit constexpr CameraController(const pin_size_t sda, const pin_size_t scl, const uint8_t (&addresses)[4])
        : SDA_(sda), SCL_(scl), ADDRESSES_{addresses[0], addresses[1], addresses[2], addresses[3]} {}

    void setup() const {
        Wire.setSDA(SDA_);
        Wire.setSCL(SCL_);
        Wire.setClock(400000);
        Wire.begin();
    }

    CameraFieldData getFieldData(const int index) const {
        const uint8_t addr = getAddress_(index);
        const Vector2Int ball_pos = {getCoordinateOne_(addr, 100), getCoordinateOne_(addr, 101)};
        const Vector2Int yellow_goal_pos = {getCoordinateOne_(addr, 102), getCoordinateOne_(addr, 103)};
        const Vector2Int blue_goal_pos = {getCoordinateOne_(addr, 105), getCoordinateOne_(addr, 106)};
        const int yellow_goal_width = getCoordinateOne_(addr, 104);
        const int blue_goal_width = getCoordinateOne_(addr, 107);
        return CameraFieldData(
            ball_pos, yellow_goal_pos, blue_goal_pos,                             // Position
            (ball_pos.x >= 0), (yellow_goal_pos.x >= 0), (blue_goal_pos.x >= 0),  // is exist
            yellow_goal_width, blue_goal_width                                    // width
        );
    }

    void scanImage(const int index, Image& image) const {
        const uint8_t addr = getAddress_(index);
        requestSet_(addr, 2);
        for (int y = 0; y < image.height; ++y) {
            for (int x = 0; x < image.width; ++x) {
                uint16_t top = requestGet_(addr);
                uint16_t low = requestGet_(addr);
                image.set(x, y, (top << 8) | low);
            }
        }
    }

    void setWhiteBlance(const int index) const {
        const uint8_t addr = getAddress_(index);
        requestSet_(addr, 1);
        do {
            delay(100);
        } while (requestGet_(addr) != 1);
    }

  private:
    const pin_size_t SDA_;
    const pin_size_t SCL_;
    const uint8_t ADDRESSES_[4];

    uint8_t getAddress_(const int index) const {
        return ADDRESSES_[index % 4];
    }

    void requestSet_(const uint8_t address, const uint8_t id) const {
        Wire.beginTransmission(address);
        Wire.write(id);
        Wire.endTransmission();
    }

    int requestGet_(const uint8_t address) const {
        Wire.requestFrom(address, 1);
        for (int time = 0; !Wire.available(); ++time) {
            if (time < 5) {
                delay(1);
                continue;
            }
            Serial.println("timeout");
            return -1;
        }
        return (int)((uint8_t)Wire.read());
    }

    int getCoordinateOne_(const uint8_t address, const uint8_t id) const {
        requestSet_(address, id);
        int t1 = requestGet_(address);
        int t2 = requestGet_(address);
        if (t2 > t1 || t1 < 0 || t2 < 0) {
            return -1;
        }
        return t1 + t2;
    }
};
}  // namespace module

#endif
