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

    void Setup(void) {
        do {
            Serial1.begin(SPEED_);
            delay(100);
        } while (Serial1 == false);
    }

    /**
     * Check the data is reached at the serial port.
     *
     * @return true: exist data in the serial port.
     */
    bool HasData() const {
        return Serial1.available() > 0;
    }

    /**
     * Read the top of the data which is reached at the serial port.
     * If there are no data at the port, returns -1.
     */
    int ReadData() const {
        int ret = 0;
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
    void SendData(const int data) const {
        Serial.write(data);
    }

  private:
    const int SPEED_;
};
}  // namespace module

#endif
