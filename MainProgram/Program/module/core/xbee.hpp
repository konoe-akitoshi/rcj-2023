#ifndef MODULE_CORE_XBEE_HPP
#define MODULE_CORE_XBEE_HPP

#ifdef WITHOUT_ARDUINO_ENVIRONMENT
#include "deps/arduino.hpp"
#else
#include <Arduino.h>
#endif

namespace module
{
class XBee
{
  public:
    /**
     * @param speed transfer speed rate (in bits per second).
     */
    explicit constexpr XBee(const int speed) : SPEED_(speed) {
    }

    void setup(void) const {
        do {
            Serial1.begin(SPEED_);
            delay(100);
        } while (!Serial1);
    }

    /**
     * Check the data is reached at the serial buffer.
     *
     * @return true: exist data in the serial buffer.
     */
    bool hasData(void) const {
        return Serial1.available() > 0;
    }

    /**
     * Read the top of the data which is reached at the serial buffer.
     * If there are no data at the buffer, returns 0.
     */
    uint8_t readData(void) const {
        uint8_t ret = 0;
        while (Serial1.available() > 0) {
            ret = Serial1.read();
        }
        return ret;
    }

    /**
     * Send specified data.
     *
     * @param data the transfer data
     */
    void sendData(const uint8_t data) const {
        Serial1.write(data);
    }

  private:
    const int SPEED_;
};
}  // namespace module

#endif
