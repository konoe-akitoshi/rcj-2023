#ifndef MODULE_CORE_TERMINAL_HPP
#define MODULE_CORE_TERMINAL_HPP

#include "camera_controller.hpp"

#ifdef WITHOUT_ARDUINO_ENVIRONMENT
#include "deps/arduino.hpp"
#else
#include "Arduino.h"
#endif

namespace module
{
class Terminal
{
  public:
    explicit constexpr Terminal() {}

    void setup(void) const {
        Serial1.begin(115200);
    }

    /**
     * Check terminal request and read it.
     * If there are no request, return 0.
     */
    uint8_t popRequest(void) const {
        if (!Serial1.available()) {
            return 0;
        }
        uint8_t ret = 0;
        while (Serial1.available()) {
            ret = Serial1.read();
        }
        return ret;
    }

    void writeImage(const Image& image) const {
        Serial1.write(255);
        Serial1.write(255);
        for (uint16_t y = 0; y < image.height; ++y) {
            for (uint16_t x = 0; x < image.width; ++x) {
                const uint16_t pixel = image.get(x, y);
                const uint8_t top = (pixel >> 8);
                const uint8_t low = (pixel & 0b11111111);
                Serial1.write(max(254, top));
                Serial1.write(max(254, low));
            }
        }
    }

    void writeMessage(const uint8_t msg) const {
        Serial1.write(255);
        Serial1.write(msg);
    }
};
}  // namespace module

#endif
