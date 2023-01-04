#ifndef COMPONENTS_XBEE_HPP
#define COMPONENTS_XBEE_HPP

#ifdef LOCAL_INCLUDE
#include "../../local/arduino_deps.h"
#endif

#include "setup_handler.hpp"

namespace component
{
class XBee
{
  public:
    /**
     * @param speed transfer speed rate (in bits per second).
     */
    explicit XBee(SetupHandler& handler, const int speed);

    /**
     * Check the data is reached at the serial port.
     *
     * @return true: exist data in the serial port.
     */
    bool HasData() const;

    /**
     * Read the top of the data which is reached at the serial port.
     * If there are no data at the port, returns -1.
     */
    int ReadData() const;

    /**
     * Send specified data.
     *
     * @param data the transfer data
     */
    void SendData(const int data) const;
};

inline XBee::XBee(SetupHandler& handler, const int speed) {
    handler.SetSerialSpeed(1, speed);
}

inline bool XBee::HasData() const {
    return Serial1.available() > 0;
}

inline int XBee::ReadData() const {
    int ret = 0;
    while (Serial1.available() > 0) {
        ret = Serial1.read();
    }
    return ret;
}

inline void XBee::SendData(const int data) const {
    Serial.write(data);
}
}  // namespace component

#endif
