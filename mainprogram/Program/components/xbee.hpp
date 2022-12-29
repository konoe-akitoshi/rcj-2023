#ifndef COMPONENT_XBEE_HPP
#define COMPONENT_XBEE_HPP

#ifdef LOCAL_INCLUDE
#include "../../local/arduino_deps.h"
#endif

namespace component
{
class XBee
{
  public:
    explicit XBee(const int speed);
    bool HasData() const;
    int ReadData() const;
    void SendData(const int data) const;
};

inline XBee::XBee(const int speed) {
    Serial1.begin(speed);
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
